#include <sioux.h>

#include <iostream>
#include <iomanip>

#include <Utilities.h>

#include <krb5.h>

#include "CCache.h"
ostream& dumpdate (
	ostream& inStream,
	cc_time_t inDate);
	
void dumpbytes (
	std::ostream&		outStream,
	const char*			inPrefix,
	cc_uint32			inSize,
	void*				inData);

void main(void)
{
	SIOUXSettings.columns = 120;
	
	for (;;) {
		char	choice;
		cout << " === CCache Dump === " << endl << endl;
		cout << "Choose one:" << endl;
		cout << "  b) Basic dump (ccaches and basic credentials information)" << endl;
		cout << "  v) Verbose dump (ccaches and all credentials information)" << endl;
		cout << "  c) Complete dump (ccache and all credentials information and data)" << endl << endl;
		do {
			cout << "Choice: ";
			cin >> choice;
		} while ((choice != 'c') && (choice != 'C') &&
			(choice != 'v') && (choice != 'V') &&
			(choice != 'b') && (choice != 'B'));
			
		cc_ccache_t					theCache = nil;
		cc_ccache_iterator_t		ccacheIterator = nil;
		cc_credentials_iterator_t	credsIterator = nil;
		cc_credentials_t			theCreds = nil;
		cc_context_t				context = nil;
		cc_string_t					cacheName = nil, principalName = nil, defaultCCacheName = nil;
		OSStatus		err;
		cc_time_t					collectionChangeTime, ccacheChangeTime;
		
		err = cc_initialize (&context, ccapi_version_3, nil, nil);
		if (err != ccNoError) {
			cout << "***\t cc_initialize returned error " << err << endl;
			goto cleanup;
		}

		// print modification date of the collection
		err = cc_context_get_change_time (context, &collectionChangeTime);
		if (err != ccNoError) {
			cout << "***\t cc_context_get_change_time returned error " << err << endl;
			goto cleanup;
		}
		cout << "\tLast modified: \"";
		dumpdate (cout, collectionChangeTime);
		cout << endl;

		err = cc_context_new_ccache_iterator (context, &ccacheIterator);
		if (err != ccNoError) {
			cout << "***\t cc_context_new_ccache_iterator returned error " << err << endl;
			goto cleanup;
		}
		
		err = cc_context_get_default_ccache_name (context, &defaultCCacheName);
		if (err != ccNoError) {
			cout << "***\t cc_context_default_ccache_name returned error " << err << endl;
			goto cleanup;
		}
		
		while (err == ccNoError) {

			err = cc_ccache_iterator_next (ccacheIterator, &theCache);
			if ((err != ccNoError) && (err != ccIteratorEnd)) {
				cout << "***\t cc_ccache_iterator_next returned error " << err << endl;
				goto cleanup;
			} else if (err == ccIteratorEnd) {
				err = ccNoError;
				break;
			}
			
			// print name of ccache
			err = cc_ccache_get_name (theCache, &cacheName);
			if (err != ccNoError) {
				cout << "***\t cc_ccache_get_name returned error " << err << endl;
				goto cleanup;
			}
			cout << "CCache name: \"" << cacheName -> data << "\"" << endl;
			if (std::string (cacheName -> data) == std::string (defaultCCacheName -> data)) {
				cout << "(Default ccache)" << endl;
			}
			cc_string_release (cacheName);
			cacheName = nil;
			
			// print modification date of ccache
			err = cc_ccache_get_change_time (theCache, &ccacheChangeTime);
			if (err != ccNoError) {
				cout << "***\t cc_ccache_get_change_time returned error " << err << endl;
				goto cleanup;
			}
			cout << "\tLast modified: \"";
			dumpdate (cout, ccacheChangeTime);
			cout << endl;
			
			// print v4 principal of ccache
			err = cc_ccache_get_principal (theCache, cc_credentials_v4, &principalName);
			if ((err != ccNoError) && (err != ccErrBadCredentialsVersion)) {
				cout << "***\t cc_ccache_get_principal returned error " << err << endl;
				goto cleanup;
			} else if (err == ccErrBadCredentialsVersion) {
				cout << "\t(No v4 tickets)" << endl;
				err = ccNoError;
			} else {
				cout << "\tv4 Principal: \"" << principalName -> data << "\"" << endl;
				cc_string_release (principalName);
				principalName = nil;
			}
			
			// print v5 principal of ccache
			err = cc_ccache_get_principal (theCache, cc_credentials_v5, &principalName);
			if ((err != ccNoError) && (err != ccErrBadCredentialsVersion)) {
				cout << "***\t cc_ccache_get_principal returned error " << err << endl;
				goto cleanup;
			} else if (err == ccErrBadCredentialsVersion) {
				cout << "\t(No v5 tickets)" << endl;
				err = ccNoError;
			} else {
				cout << "\tv5 Principal: \"" << principalName -> data << "\"" << endl;
				cc_string_release (principalName);
				principalName = nil;
			}
			
			err = cc_ccache_new_credentials_iterator (theCache, &credsIterator);
			if (err != ccNoError) {
				cout << "***\t cc_ccache_new_credentials_iterator returned error " << err << endl;
				goto cleanup;
			}
				
			for (;;) {
				err = cc_credentials_iterator_next (credsIterator, &theCreds);
				if ((err != ccNoError) && (err != ccIteratorEnd)) {
					cout << "***\t cc_credentials_iterator_next returned error " << err << endl;
					goto cleanup;
				} else if (err == ccIteratorEnd) {
					err = ccNoError; // this is an error we can deal with, so reset so we continue to dump data
					break;
				}
				
				cc_credentials_v4_t*	creds4 = theCreds -> data -> credentials.credentials_v4;
				cc_credentials_v5_t*	creds5 = theCreds -> data -> credentials.credentials_v5;
				
				switch (theCreds -> data -> version) {
					case cc_credentials_v4: {
						cout << "\t\tv4: ";
						dumpdate (cout, creds4 -> issue_date);
						cout << " - ";
						dumpdate (cout, creds4 -> issue_date + creds4 -> lifetime * 5 * 60);
						cout << ": " << creds4 -> service;
						cout << "." << creds4 -> service_instance;
						cout << "@" << creds4 -> realm << endl;
						if ((choice != 'b') && (choice != 'B')) {
							cout << "\t\t\tKey version: " << creds4 -> kvno << endl;
							switch (creds4 -> string_to_key_type) {
								case cc_v4_stk_afs:
									cout << "\t\t\tString to key hash: AFS" << endl;
									break;

								case cc_v4_stk_des:
									cout << "\t\t\tString to key hash: DES" << endl;
									break;

								case cc_v4_stk_columbia_special:
									cout << "\t\t\tString to key hash: Columbia special" << endl;
									break;

								case cc_v4_stk_unknown:
									cout << "\t\t\tString to key hash: Unknown" << endl;
									break;
									
								default:
									cout << "***\t\t\tString to key hash: Invalid (" << creds4 -> string_to_key_type << ")" << endl;
							}
							cc_uint32	address = creds4 -> address;
							cout << "\t\t\tAddress: " << ((address & 0xFF000000) >> 24) << ".";
							cout << ((address & 0x00FF0000) >> 16) << ".";
							cout << ((address & 0x0000FF00) >> 8) << ".";
							cout << (address & 0x000000FF) << endl;
						}
						if ((choice == 'c') || (choice == 'C')) {
							cout << "\t\t\tSession key:" << endl;
							dumpbytes (cout, "\t\t\t\t" , sizeof (creds4 -> session_key), creds4 -> session_key);
							cout << "\t\t\tTicket: " << creds4 -> ticket_size << " bytes" << endl;
							dumpbytes (cout, "\t\t\t\t" , creds4 -> ticket_size, creds4 -> ticket);
						}	
						break;
					}
					
					case cc_credentials_v5: {
						cout << "\t\tv5: ";
						dumpdate (cout, creds5 -> starttime);
						cout << " - ";
						dumpdate (cout, creds5 -> endtime);
						cout << ": " << creds5 -> server << endl;
						if ((choice != 'b') && (choice != 'B')) {
							cout << "\t\t\tIssue time: ";
							dumpdate (cout, creds5 -> authtime) << endl;
							if (creds5 -> renew_till != 0) {
								cout << "\t\t\tRenewable until: ";
								dumpdate (cout, creds5 -> renew_till) << endl;
							}
							cout << "\t\t\t" << (creds5 -> is_skey ? "Is an S-Key" : "Is not an S-Key") << endl;
							cout << "\t\t\tTicket flags: " << endl;
							if ((creds5 -> ticket_flags) & TKT_FLG_FORWARDABLE) {
								cout << "\t\t\t\tForwardable" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_FORWARDED) {
								cout << "\t\t\t\tForwarded" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_PROXIABLE) {
								cout << "\t\t\t\tProxiable" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_PROXY) {
								cout << "\t\t\t\tProxy" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_MAY_POSTDATE) {
								cout << "\t\t\t\tMay postdate" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_POSTDATED) {
								cout << "\t\t\t\tPostdated" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_INVALID) {
								cout << "\t\t\t\tInvalid" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_RENEWABLE) {
								cout << "\t\t\t\tRenewable" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_INITIAL) {
								cout << "\t\t\t\tInitial" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_PRE_AUTH) {
								cout << "\t\t\t\tPre-authenticated" << endl;
							}
							if ((creds5 -> ticket_flags) & TKT_FLG_HW_AUTH) {
								cout << "\t\t\t\tHardware authenticated" << endl;
							}
							cout << "\t\t\tAddresses: ";
							if ((creds5 -> addresses == NULL) || (*(creds5 -> addresses) == NULL)) {
								cout << "none" << endl;
							} else {
								cout << endl;
								for (cc_data** data = creds5 -> addresses; *data != NULL; data++) {
									switch ((*data) -> type) {
										case ADDRTYPE_INET: {
											cc_uint32	address = *(cc_uint32*)((*data) -> data);
											cout << "\t\t\t\tIP v4: "<< ((address & 0xFF000000) >> 24) << ".";
											cout << ((address & 0x00FF0000) >> 16) << ".";
											cout << ((address & 0x0000FF00) >> 8) << ".";
											cout << (address & 0x000000FF) << endl;
											break;
										}
										
										default:
											if ((choice != 'c') && (choice != 'C')) {
												cout << "\t\t\t\tOther address type (" << (*data) -> type << ")" << endl;
											} else {
												cout << "\t\t\t\tType " << (*data) -> type << " (" << (*data) -> length << " bytes)" << endl;
												dumpbytes (cout, "\t\t\t\t\t", (*data) -> length, (*data) -> data);
											}
									}
								}
							}
						}
						if ((choice == 'c') || (choice == 'C')) {
							cout << "\t\t\tKey: (" << creds5 -> keyblock.length << " bytes)" << endl;
							dumpbytes (cout, "\t\t\t\t", creds5 -> keyblock.length,creds5 -> keyblock.data);
							cout << "\t\t\tTicket: (" << creds5 -> ticket.length << " bytes)" << endl;
							dumpbytes (cout, "\t\t\t\t", creds5 -> ticket.length,creds5 -> ticket.data);
							cout << "\t\t\tSecond ticket: (" << creds5 -> second_ticket.length << " bytes)" << endl;
							dumpbytes (cout, "\t\t\t\t", creds5 -> second_ticket.length,creds5 -> second_ticket.data);
							cout << "\t\t\tAuthentication data: ";
							if ((creds5 -> authdata == NULL) || (*(creds5 -> authdata) == NULL)) {
								cout << "none" << endl;
							} else {
								cout << endl;
								for (cc_data** data = creds5 -> authdata; *data != NULL; data++) {
									cout << "\t\t\t\tBlock " << ((int) (data - creds5 -> authdata)) << "(";
									cout << (*data) -> length << "bytes )" << endl;
									dumpbytes (cout, "\t\t\t\t\t", (*data) -> length, (*data) -> data);
								}
							}
						}
						break;
					}
				}
				
				cc_credentials_release (theCreds);
				theCreds = nil;
			}

			cc_credentials_iterator_release (credsIterator);
			credsIterator = nil;
			
			cc_ccache_release (theCache);
			theCache = nil;
			
			cout << endl;
		}
		
cleanup:
		if (theCreds != nil) {
			cc_credentials_release (theCreds);
			theCreds = nil;
		}
		
		if (credsIterator != nil) {
			cc_credentials_iterator_release (credsIterator);
			credsIterator = nil;
		}
		
		if (theCache != nil) {
			cc_ccache_release (theCache);
			theCache = nil;
		}
		
		if (ccacheIterator != nil) {
			cc_ccache_iterator_release (ccacheIterator);
			ccacheIterator = nil;
		}
		
		if (defaultCCacheName != nil) {
			cc_string_release (defaultCCacheName);
			defaultCCacheName = nil;
		}

		if (context != nil) {
			cc_context_release (context);
			context = nil;
		}
	}
}

/* Lame date formatting, like ctime but with no \n */

static const char *day_name[]
	= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

static const char *month_name[]
	= {"January", "February", "March","April","May","June",
	   "July", "August",  "September", "October", "November","December"};

ostream& dumpdate (ostream& inStream, cc_time_t inDate)
{
	char	datestring [64];
	/* we come in in 1970 time */
	time_t timer = (time_t) inDate;
	struct tm tm;
	
	/* then we go to 1900 time */
	unix_time_to_mac_time (&timer);
	
	tm = *localtime (&timer);

	sprintf(datestring, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
	day_name[tm.tm_wday],
	month_name[tm.tm_mon],
	tm.tm_mday,
	tm.tm_hour,
	tm.tm_min,
	tm.tm_sec,
	tm.tm_year + 1900);
	
	return inStream << datestring;
}

void dumpbytes (
	std::ostream&		outStream,
	const char*			inPrefix,
	cc_uint32			inSize,
	void*				inData) {
	
	// Print in groups of 16 bytes

	int numRows = inSize / 16;
	if (numRows * 16 < inSize)
		numRows++;	
	
	for (cc_uint32 i = 0; i < numRows; i++) {
		outStream << inPrefix;
		// Print a single row as two groups of 8 bytes
		for (int j = 0; j < 2; j++) {
			// print 8 bytes in 4 groups of 2
			for (int k = 0; k < 4; k++) {
				for (int l = 0; l < 2; l++) {
					int offset = 16 * i + 8 * j + 2 * k + l;
					if (offset < inSize) {
						UInt32 data = *((unsigned char*)(inData) + offset);
						outStream << std::hex << std::uppercase;
						outStream << std::setw (2) << std::setfill ('0') << data;
						outStream << setbase (10);
					} else {
						outStream << "  ";
					}
				}
				cout << " ";
			}
			cout << " ";
		}
		// Then print the 16 characters
		for (int j = 0; j < 16; j++) {
			int offset = 16 * i + j;
			if (offset < inSize) {
				char data = *((char*)(inData) + offset);
				if (data < ' ') {
					cout << ".";
				} else {
					cout << data;
				}
			} else {
				outStream << "  ";
			}
		}
		cout << endl;
	}
}
