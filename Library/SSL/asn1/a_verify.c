/* crypto/asn1/a_verify.c */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#ifndef COMPILE_OPTION_HOST_SERVICE_ONLY

#ifdef __GEOS__
#include <Ansi/stdio.h>
#else
#include <stdio.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cryptlib.h"
#include "bn.h"
#include "x509.h"
#include "objects.h"
#include "buffer.h"
#include "evp.h"
#include "pem.h"

#ifdef __GEOS__
int ASN1_verify(int (*i2d)(),X509_ALGOR *a,ASN1_BIT_STRING *signature,char *data,EVP_PKEY *pkey)
#else
int ASN1_verify(i2d,a,signature,data,pkey)
int (*i2d)();
X509_ALGOR *a;
ASN1_BIT_STRING *signature;
char *data;
EVP_PKEY *pkey;
#endif
	{
	EVP_MD_CTX ctx;
	EVP_MD *type;
	unsigned char *p,*buf_in=NULL;
	int ret= -1,i,inl;

	i=OBJ_obj2nid(a->algorithm);
	type=EVP_get_digestbyname(OBJ_nid2sn(i));
	if (type == NULL)
		{
		ASN1err(ASN1_F_ASN1_VERIFY,ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM);
		goto err;
		}
	
	inl=i2d(data,NULL);
	buf_in=(unsigned char *)Malloc((unsigned int)inl);
	if (buf_in == NULL)
		{
		ASN1err(ASN1_F_ASN1_VERIFY,ERR_R_MALLOC_FAILURE);
		goto err;
		}
	p=buf_in;

	i2d(data,&p);
	EVP_VerifyInit(&ctx,type);
	EVP_VerifyUpdate(&ctx,(unsigned char *)buf_in,inl);

	memset(buf_in,0,(unsigned int)inl);
	Free((char *)buf_in);

	if (EVP_VerifyFinal(&ctx,(unsigned char *)signature->data,
			(unsigned int)signature->length,pkey) <= 0)
		{
		ASN1err(ASN1_F_ASN1_VERIFY,ERR_R_EVP_LIB);
		ret=0;
		goto err;
		}
	/* we don't need to zero the 'ctx' because we just checked
	 * public information */
	/* memset(&ctx,0,sizeof(ctx)); */
	ret=1;
err:
	return(ret);
	}

#endif
