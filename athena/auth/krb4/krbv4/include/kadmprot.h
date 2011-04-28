/* function prototypes jms 8/23/93 */

/* from kadm.c */
extern long get_pw_in_tkt(char *username, char *password);
#ifndef OS2
#ifndef LWP
extern long kadm_change_your_password(LPSTR principal, LPSTR old_password, 
							   LPSTR new_password, HANDLE *);
#else
extern long kadm_change_your_password(char *principal, char *old_password,
							   char *new_password, char **);
#endif							   
#else
extern long kadm_change_your_password(LPSTR principal, LPSTR old_password,
							   LPSTR new_password, char **);
#endif

/* from kadmc2.c */
extern long kadm_init_link(char n[], char i[], char r[]);
extern long kadm_change_pw(des_cblock newkey);
extern long kadm_change_pw2(des_cblock newkey, char *pwstring, u_char **retstring);
extern long kadm_add(Kadm_vals *vals);
extern long kadm_mod(Kadm_vals *vals1, Kadm_vals *vals2);
extern long kadm_get(Kadm_vals *vals, u_char fl[4]);
extern long kadm_check_pw(des_cblock newkey, char *pwstring, u_char **retstring);
extern long kadm_cli_send(u_char *st_dat, int st_siz, u_char **ret_dat, int *ret_siz);
extern long kadm_cli_keyd(des_cblock s_k, des_key_schedule s_s);
extern long kadm_cli_conn();
extern void kadm_cli_disconn();
extern long kadm_cli_out(u_char *dat, int dat_len, u_char **ret_dat, int *ret_siz);









