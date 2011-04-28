/*
 * Copyright (c) 1990 Dennis Ferguson.  All rights reserved.
 *
 * Commercial use is permitted only if products which are derived from
 * or include this software are made available for purchase and/or use
 * in Canada.  Otherwise, redistribution and use in source and binary
 * forms are permitted.
 */

/*
 * des_ecb_encrypt.c - do an encryption in ECB mode
 */
#include "des.h"
#include "f_tables.h"


#if 0
/*
 * des_ecb_encrypt - {en,de}crypt a block in ECB mode
 */
int DES_CALLCONV_C
des_ecb_encrypt(in, out, schedule, encrypt)
	des_cblock *in;
	des_cblock *out;
	des_key_schedule schedule;
	int encrypt;
{
	unsigned KRB_INT32 left;
	unsigned KRB_INT32  right;
	unsigned KRB_INT32 temp;
	int i;

	{
		/*
		 * Need a temporary for copying the data in
		 */
		unsigned char *datap;

		/*
		 * Copy the input block into the registers
		 */
		datap = (unsigned char *)in;
		GET_HALF_BLOCK(left, datap);
		GET_HALF_BLOCK(right, datap);
	}

	/*
	 * Do the initial permutation.
	 */
	DES_INITIAL_PERM(left, right, temp);

	/*
	 * Now the rounds.  Use different code depending on whether it
	 * is an encryption or a decryption (gross, should keep both
	 * sets of keys in the key schedule instead).
	 */
	if (encrypt) {
		unsigned KRB_INT32 *kp;

		kp = (unsigned KRB_INT32 *)schedule;
		for (i = 0; i < 8; i++) {
			DES_SP_ENCRYPT_ROUND(left, right, temp, kp);
			DES_SP_ENCRYPT_ROUND(right, left, temp, kp);
		}
	} else {
		unsigned KRB_INT32 *kp;

		/*
		 * Point kp past end of schedule
		 */
		kp = ((unsigned KRB_INT32 *)schedule) + (2 * 16);;
		for (i = 0; i < 8; i++) {
			DES_SP_DECRYPT_ROUND(left, right, temp, kp);
			DES_SP_DECRYPT_ROUND(right, left, temp, kp);
		}
	}

	/*
	 * Do the final permutation
	 */
	DES_FINAL_PERM(left, right, temp);

	/*
	 * Finally, copy the result out a byte at a time
	 */
	{
		register unsigned char *datap;

		datap = (unsigned char *)out;
		PUT_HALF_BLOCK(left, datap);
		PUT_HALF_BLOCK(right, datap);
	}

	/*
	 * return nothing
	 */
	return (0);
}

#endif


int 
des_ecb_encrypt(in, out, schedule, encrypt)
	des_cblock *in;
	des_cblock *out;
	des_key_schedule schedule;
	int encrypt;
{
	unsigned long left;
	unsigned long right;
	unsigned long temp;
	unsigned long temp_left;
	unsigned long temp_right;
	int i;

	{
		


		unsigned char *datap;

		


		datap = (unsigned char *)in;

		(left) = ((unsigned long)(*(datap)++)) << 24;
		(left) |= ((unsigned long)(*(datap)++)) << 16;
		(left) |= ((unsigned long)(*(datap)++)) << 8;
		(left) |= (unsigned long)(*(datap)++);

		(right) = ((unsigned long)(*(datap)++)) << 24;
		(right) |= ((unsigned long)(*(datap)++)) << 16;
		(right) |= ((unsigned long)(*(datap)++)) << 8;
		(right) |= (unsigned long)(*(datap)++);

	}

	

	temp_left =  (left & 0xaaaaaaaa);
	temp_right = (right & 0xaaaaaaaa);
	temp = temp_left | (temp_right >> 1);

	(temp) = ((((left)) & 0xaaaaaaaa) |
		 ( ( (unsigned long) (((right)) & 0xaaaaaaaa) ) >> 1));

	temp_left =   (left & 0x55555555) << 1;
	temp_right =  (right & 0x55555555);

	(right) = temp_left | temp_right;

	(right) = (((((left)) & 0x55555555) << 1) | (((right)) & 0x55555555));

	temp_left = ((right) >> 24);
	temp_right = temp_left & 0xff;
	temp_left =	des_IP_table[temp_right];
	temp_right =	(des_IP_table[((right) >> 16) & 0xff] << 1);
	left =		temp_left | temp_right;
	temp_left =	(des_IP_table[((right) >> 8) & 0xff] << 2);
	temp_right =	(des_IP_table[(right) & 0xff] << 3);
	left =		left | temp_left | temp_right;

	(left) = des_IP_table[((right) >> 24) & 0xff] |
	       (des_IP_table[((right) >> 16) & 0xff] << 1) |
	       (des_IP_table[((right) >> 8) & 0xff] << 2) |
	       (des_IP_table[(right) & 0xff] << 3);

	temp_left =	des_IP_table[((temp) >> 24) & 0xff];
	temp_right =	(des_IP_table[((temp) >> 16) & 0xff] << 1);
	right = 	temp_left | temp_right;
	temp_left =	(des_IP_table[((temp) >> 8) & 0xff] << 2);
	temp_right =	(des_IP_table[(temp) & 0xff] << 3);
	right = 	right | temp_left | temp_right;

       (right) = des_IP_table[((temp) >> 24) & 0xff] |
		(des_IP_table[((temp) >> 16) & 0xff] << 1) |
		(des_IP_table[((temp) >> 8) & 0xff] << 2) |
		(des_IP_table[(temp) & 0xff] << 3);

	




	if (encrypt) {
		register unsigned long *kp;

		kp = (unsigned long *)schedule;
		for (i = 0; i < 8; i++) {
			(temp) = (((right) >> 11) | ((right) << 21)) ^ *(kp)++;

			(left) ^= des_SP_table[0][((temp) >> 24) & 0x3f] |
				  des_SP_table[1][((temp) >> 16) & 0x3f] |
				  des_SP_table[2][((temp) >> 8) & 0x3f] |
				  des_SP_table[3][((temp) ) & 0x3f];

			(temp) = (((right) >> 23) | ((right) << 9)) ^ *(kp)++;


			(left) ^= des_SP_table[4][((temp) >> 24) & 0x3f] |
				  des_SP_table[5][((temp) >> 16) & 0x3f] |
				  des_SP_table[6][((temp) >> 8) & 0x3f] |
				  des_SP_table[7][((temp) ) & 0x3f];

			(temp) = (((left) >> 11) | ((left) << 21)) ^ *(kp)++;

			(right) ^= des_SP_table[0][((temp) >> 24) & 0x3f] |
				   des_SP_table[1][((temp) >> 16) & 0x3f] |
				   des_SP_table[2][((temp) >> 8) & 0x3f] |
				   des_SP_table[3][((temp) ) & 0x3f];

			(temp) = (((left) >> 23) | ((left) << 9)) ^ *(kp)++;

			(right) ^= des_SP_table[4][((temp) >> 24) & 0x3f] |
				   des_SP_table[5][((temp) >> 16) & 0x3f] |
				   des_SP_table[6][((temp) >> 8) & 0x3f] |
				   des_SP_table[7][((temp) ) & 0x3f];
		}
	} else {
		register unsigned long *kp;

		


		kp = ((unsigned long *)schedule) + (2 * 16);;
		for (i = 0; i < 8; i++) {
			(temp) = (((right) >> 23) |
				 ((right) << 9)) ^ *(--(kp));

			(left) ^= des_SP_table[7][((temp) ) & 0x3f] |
				  des_SP_table[6][((temp) >> 8) & 0x3f] |
				  des_SP_table[5][((temp) >> 16) & 0x3f] |
				  des_SP_table[4][((temp) >> 24) & 0x3f];

			(temp) = (((right) >> 11) | ((right) << 21)) ^ *(--(kp));

			(left) ^= des_SP_table[3][((temp) ) & 0x3f] |
				  des_SP_table[2][((temp) >> 8) & 0x3f] |
				  des_SP_table[1][((temp) >> 16) & 0x3f] |
				  des_SP_table[0][((temp) >> 24) & 0x3f];

			(temp) = (((left) >> 23) | ((left) << 9)) ^ *(--(kp));

			(right) ^= des_SP_table[7][((temp) ) & 0x3f] |
				   des_SP_table[6][((temp) >> 8) & 0x3f] |
				   des_SP_table[5][((temp) >> 16) & 0x3f] |
				   des_SP_table[4][((temp) >> 24) & 0x3f];

			(temp) = (((left) >> 11) | ((left) << 21)) ^ *(--(kp));

			(right) ^= des_SP_table[3][((temp) ) & 0x3f] |
				   des_SP_table[2][((temp) >> 8) & 0x3f] |
				   des_SP_table[1][((temp) >> 16) & 0x3f] |
				   des_SP_table[0][((temp) >> 24) & 0x3f];

		}
	}

	


	(temp) = ((((right)) & 0xf0f0f0f0) |
		 ( ( (unsigned long) (((left)) & 0xf0f0f0f0) ) >> 4));

	(right) = (((((right)) & 0x0f0f0f0f) << 4) |
		  (((left)) & 0x0f0f0f0f));

	(left) = (des_FP_table[((right) >> 24) & 0xff] << 6) |
		 (des_FP_table[((right) >> 16) & 0xff] << 4) |
		 (des_FP_table[((right) >> 8) & 0xff] << 2) |
		 des_FP_table[(right) & 0xff];

	(right) = (des_FP_table[((temp) >> 24) & 0xff] << 6) |
		  (des_FP_table[((temp) >> 16) & 0xff] << 4) |
		  (des_FP_table[((temp) >> 8) & 0xff] << 2) |
		   des_FP_table[temp & 0xff];

	


	{
		register unsigned char *datap;

		datap = (unsigned char *)out;

		*(datap)++ = ((left) >> 24) & 0xff;
		*(datap)++ = ((left) >> 16) & 0xff;
		*(datap)++ = ((left) >> 8) & 0xff;
		*(datap)++ = (left) & 0xff;
		*(datap)++ = ((right) >> 24) & 0xff;
		*(datap)++ = ((right) >> 16) & 0xff;
		*(datap)++ = ((right) >> 8) & 0xff;
		*(datap)++ = (right) & 0xff;

	}

	


	return (0);
}
