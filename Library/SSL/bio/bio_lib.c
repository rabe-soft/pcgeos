/* crypto/bio/bio_lib.c */
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
#include <errno.h>
#include "crypto.h"
#include "cryptlib.h"
#include "bio.h"
#include "stack.h"

static STACK *bio_meth=NULL;
static int bio_meth_num=0;

BIO *BIO_new(method)
BIO_METHOD *method;
	{
	BIO *ret=NULL;

	ret=(BIO *)Malloc(sizeof(BIO));
	if (ret == NULL)
		{
		BIOerr(BIO_F_BIO_NEW,ERR_R_MALLOC_FAILURE);
		return(NULL);
		}
	if (!BIO_set(ret,method))
		{
		Free(ret);
		ret=NULL;
		}
	return(ret);
	}

int BIO_set(bio,method)
BIO *bio;
BIO_METHOD *method;
	{
	bio->method=method;
	bio->callback=NULL;
	bio->cb_arg=NULL;
	bio->init=0;
	bio->shutdown=1;
	bio->flags=0;
	bio->retry_reason=0;
	bio->num=0;
	bio->ptr=NULL;
	bio->prev_bio=NULL;
	bio->next_bio=NULL;
	bio->references=1;
	bio->num_read=0L;
	bio->num_write=0L;
	PUSHDS;
	CRYPTO_new_ex_data(bio_meth,(char *)bio,&bio->ex_data);
	POPDS;
	if (method->create != NULL)
#ifdef __GEOS__
		if (!(CALLCB1(method->create,bio)))
#else
		if (!method->create(bio))
#endif
			return(0);
	return(1);
	}

int BIO_free(a)
BIO *a;
	{
	int ret=0,i;

	if (a == NULL) return(0);

	i=CRYPTO_add(&a->references,-1,CRYPTO_LOCK_BIO);
#ifdef REF_PRINT
	REF_PRINT("BIO",a);
#endif
        if (i > 0) return(1);
#ifdef REF_CHECK
	if (i < 0)
		{
		fprintf(stderr,"BIO_free, bad reference count\n");
		abort();
		}
#endif
	if ((a->callback != NULL) &&
#ifdef __GEOS__
		((i=(int)CALLCB6(a->callback,a,BIO_CB_FREE,NullPtr,0,0L,1L)) <= 0))
#else
		((i=(int)a->callback(a,BIO_CB_FREE,NULL,0,0L,1L)) <= 0))
#endif
			return(i);

	PUSHDS;
	CRYPTO_free_ex_data(bio_meth,(char *)a,&a->ex_data);
	POPDS;

	if ((a->method == NULL) || (a->method->destroy == NULL)) return(1);
#ifdef __GEOS__
	ret=CALLCB1(a->method->destroy,a);
#else
	ret=a->method->destroy(a);
#endif
	Free(a);
	return(1);
	}

int BIO_read(b,out,outl)
BIO *b;
char *out;
int outl;
	{
	int i;
	long (*cb)();

	if ((b == NULL) || (b->method == NULL) || (b->method->bread == NULL))
		{
		BIOerr(BIO_F_BIO_READ,BIO_R_UNSUPPORTED_METHOD);
		return(-2);
		}

	cb=b->callback;
	if ((cb != NULL) &&
#ifdef __GEOS__
		((i=(int)CALLCB6(cb,b,BIO_CB_READ,out,outl,0L,1L)) <= 0))
#else
		((i=(int)cb(b,BIO_CB_READ,out,outl,0L,1L)) <= 0))
#endif
			return(i);

	if (!b->init)
		{
		BIOerr(BIO_F_BIO_READ,BIO_R_UNINITALISED);
		return(-2);
		}

#ifdef __GEOS__
	i=CALLCB3(b->method->bread,b,out,outl);
#else
	i=b->method->bread(b,out,outl);
#endif
	if (i > 0) b->num_read+=(unsigned long)i;

	if (cb != NULL)
#ifdef __GEOS__
		i=(int)CALLCB6(cb,b,BIO_CB_READ|BIO_CB_RETURN,out,outl,
			0L,(long)i);
#else
		i=(int)cb(b,BIO_CB_READ|BIO_CB_RETURN,out,outl,
			0L,(long)i);
#endif
	return(i);
	}

int BIO_write(b,in,inl)
BIO *b;
char *in;
int inl;
	{
	int i;
	long (*cb)();

	if (b == NULL)
		return(0);

	cb=b->callback;
	if ((b->method == NULL) || (b->method->bwrite == NULL))
		{
		BIOerr(BIO_F_BIO_WRITE,BIO_R_UNSUPPORTED_METHOD);
		return(-2);
		}

	if ((cb != NULL) &&
#ifdef __GEOS__
		((i=(int)CALLCB6(cb,b,BIO_CB_WRITE,in,inl,0L,1L)) <= 0))
#else
		((i=(int)cb(b,BIO_CB_WRITE,in,inl,0L,1L)) <= 0))
#endif
			return(i);

	if (!b->init)
		{
		BIOerr(BIO_F_BIO_WRITE,BIO_R_UNINITALISED);
		return(-2);
		}

#ifdef __GEOS__
	i=CALLCB3(b->method->bwrite,b,in,inl);
#else
	i=b->method->bwrite(b,in,inl);
#endif
	if (i > 0) b->num_write+=(unsigned long)i;

	if (cb != NULL)
#ifdef __GEOS__
		i=(int)CALLCB6(cb,b,BIO_CB_WRITE|BIO_CB_RETURN,in,inl,
			0L,(long)i);
#else
		i=(int)cb(b,BIO_CB_WRITE|BIO_CB_RETURN,in,inl,
			0L,(long)i);
#endif
	return(i);
	}

int BIO_puts(b,in)
BIO *b;
char *in;
	{
	int i;
	long (*cb)();

	if ((b == NULL) || (b->method == NULL) || (b->method->bputs == NULL))
		{
		BIOerr(BIO_F_BIO_PUTS,BIO_R_UNSUPPORTED_METHOD);
		return(-2);
		}

	cb=b->callback;

	if ((cb != NULL) &&
#ifdef __GEOS__
		((i=(int)CALLCB6(cb,b,BIO_CB_PUTS,in,0,0L,1L)) <= 0))
#else
		((i=(int)cb(b,BIO_CB_PUTS,in,0,0L,1L)) <= 0))
#endif
			return(i);

	if (!b->init)
		{
		BIOerr(BIO_F_BIO_PUTS,BIO_R_UNINITALISED);
		return(-2);
		}

#ifdef __GEOS__
	i=CALLCB2(b->method->bputs,b,in);
#else
	i=b->method->bputs(b,in);
#endif

	if (cb != NULL)
#ifdef __GEOS__
		i=(int)CALLCB6(cb,b,BIO_CB_PUTS|BIO_CB_RETURN,in,0,
			0L,(long)i);
#else
		i=(int)cb(b,BIO_CB_PUTS|BIO_CB_RETURN,in,0,
			0L,(long)i);
#endif
	return(i);
	}

int BIO_gets(b,in,inl)
BIO *b;
char *in;
int inl;
	{
	int i;
	long (*cb)();

	if ((b == NULL) || (b->method == NULL) || (b->method->bgets == NULL))
		{
		BIOerr(BIO_F_BIO_GETS,BIO_R_UNSUPPORTED_METHOD);
		return(-2);
		}

	cb=b->callback;

	if ((cb != NULL) &&
#ifdef __GEOS__
		((i=(int)CALLCB6(cb,b,BIO_CB_GETS,in,inl,0L,1L)) <= 0))
#else
		((i=(int)cb(b,BIO_CB_GETS,in,inl,0L,1L)) <= 0))
#endif
			return(i);

	if (!b->init)
		{
		BIOerr(BIO_F_BIO_GETS,BIO_R_UNINITALISED);
		return(-2);
		}

#ifdef __GEOS__
	i=CALLCB3(b->method->bgets,b,in,inl);
#else
	i=b->method->bgets(b,in,inl);
#endif

	if (cb != NULL)
#ifdef __GEOS__
		i=(int)CALLCB6(cb,b,BIO_CB_GETS|BIO_CB_RETURN,in,inl,
			0L,(long)i);
#else
		i=(int)cb(b,BIO_CB_GETS|BIO_CB_RETURN,in,inl,
			0L,(long)i);
#endif
	return(i);
	}

long BIO_int_ctrl(b,cmd,larg,iarg)
BIO *b;
int cmd;
long larg;
int iarg;
	{
	int i;

	i=iarg;
	return(BIO_ctrl(b,cmd,larg,(char *)&i));
	}

char *BIO_ptr_ctrl(b,cmd,larg)
BIO *b;
int cmd;
long larg;
	{
	char *p=NULL;

	if (BIO_ctrl(b,cmd,larg,(char *)&p) <= 0)
		return(NULL);
	else
		return(p);
	}

long BIO_ctrl(b,cmd,larg,parg)
BIO *b;
int cmd;
long larg;
char *parg;
	{
	long ret;
	long (*cb)();

	if (b == NULL) return(0);

	if ((b->method == NULL) || (b->method->ctrl == NULL))
		{
		BIOerr(BIO_F_BIO_CTRL,BIO_R_UNSUPPORTED_METHOD);
		return(-2);
		}

	cb=b->callback;

	if ((cb != NULL) &&
#ifdef __GEOS__
		((ret=CALLCB6(cb,b,BIO_CB_CTRL,parg,cmd,larg,1L)) <= 0))
#else
		((ret=cb(b,BIO_CB_CTRL,parg,cmd,larg,1L)) <= 0))
#endif
		return(ret);

#ifdef __GEOS__
	ret=(long)CALLCB4(b->method->ctrl,b,cmd,larg,parg);
#else
	ret=b->method->ctrl(b,cmd,larg,parg);
#endif

	if (cb != NULL)
#ifdef __GEOS__
		ret=(long)CALLCB6(cb,b,BIO_CB_CTRL|BIO_CB_RETURN,parg,cmd,
			larg,ret);
#else
		ret=cb(b,BIO_CB_CTRL|BIO_CB_RETURN,parg,cmd,
			larg,ret);
#endif
	return(ret);
	}

/* put the 'bio' on the end of b's list of operators */
BIO *BIO_push(b,bio)
BIO *b,*bio;
	{
	BIO *lb;

	if (b == NULL) return(bio);
	lb=b;
	while (lb->next_bio != NULL)
		lb=lb->next_bio;
	lb->next_bio=bio;
	if (bio != NULL)
		bio->prev_bio=lb;
	/* called to do internal processing */
	BIO_ctrl(b,BIO_CTRL_PUSH,0,NULL);
	return(b);
	}

/* Remove the first and return the rest */
BIO *BIO_pop(b)
BIO *b;
	{
	BIO *ret;

	if (b == NULL) return(NULL);
	ret=b->next_bio;

	if (b->prev_bio != NULL)
		b->prev_bio->next_bio=b->next_bio;
	if (b->next_bio != NULL)
		b->next_bio->prev_bio=b->prev_bio;

	b->next_bio=NULL;
	b->prev_bio=NULL;
	BIO_ctrl(b,BIO_CTRL_POP,0,NULL);
	return(ret);
	}

BIO *BIO_get_retry_BIO(bio,reason)
BIO *bio;
int *reason;
	{
	BIO *b,*last;

	b=last=bio;
	for (;;)
		{
		if (!BIO_should_retry(b)) break;
		last=b;
		b=b->next_bio;
		if (b == NULL) break;
		}
	if (reason != NULL) *reason=last->retry_reason;
	return(last);
	}

int BIO_get_retry_reason(bio)
BIO *bio;
	{
	return(bio->retry_reason);
	}

BIO *BIO_find_type(bio,type)
BIO *bio;
int type;
	{
	int mt,mask;

	mask=type&0xff;
	do	{
		if (bio->method != NULL)
			{
			mt=bio->method->type;

			if (!mask)
				{
				if (mt & type) return(bio);
				}
			else if (mt == type)
				return(bio);
			}
		bio=bio->next_bio;
		} while (bio != NULL);
	return(NULL);
	}

void BIO_free_all(bio)
BIO *bio;
	{
	BIO *b;
	int ref;

	while (bio != NULL)
		{
		b=bio;
		ref=b->references;
		bio=bio->next_bio;
		BIO_free(b);
		/* Since ref count > 1, don't free anyone else. */
		if (ref > 1) break;
		}
	}

BIO *BIO_dup_chain(in)
BIO *in;
	{
	BIO *ret=NULL,*eoc=NULL,*bio,*new;

	PUSHDS;
	for (bio=in; bio != NULL; bio=bio->next_bio)
		{
		if ((new=BIO_new(bio->method)) == NULL) goto err;
		new->callback=bio->callback;
		new->cb_arg=bio->cb_arg;
		new->init=bio->init;
		new->shutdown=bio->shutdown;
		new->flags=bio->flags;

		/* This will let SSL_s_sock() work with stdin/stdout */
		new->num=bio->num;

		if (!BIO_dup_state(bio,(char *)new))
			{
			BIO_free(new);
			goto err;
			}

	        /* copy app data */
	        if (!CRYPTO_dup_ex_data(bio_meth,&new->ex_data,&bio->ex_data))
	                goto err;

		if (ret == NULL)
			{
			eoc=new;
			ret=eoc;
			}
		else
			{
			BIO_push(eoc,new);
			eoc=new;
			}
		}
	POPDS;
	return(ret);
err:
	if (ret != NULL)
		BIO_free(ret);
	POPDS;
	return(NULL);	
	}

void BIO_copy_next_retry(b)
BIO *b;
	{
	BIO_set_flags(b,BIO_get_retry_flags(b->next_bio));
	b->retry_reason=b->next_bio->retry_reason;
	}

#ifdef __GEOS__
int BIO_get_ex_new_index(long argl,char *argp,int (*new_func)(),int (*dup_func)(),void (*free_func)())
#else
int BIO_get_ex_new_index(argl,argp,new_func,dup_func,free_func)
long argl;
char *argp;
int (*new_func)();
int (*dup_func)();
void (*free_func)();
#endif
        {
#ifdef __GEOS__
	int ret;
	PUSHDS;
        bio_meth_num++;
        ret = CRYPTO_get_ex_new_index(bio_meth_num-1,&bio_meth,
                argl,argp,new_func,dup_func,free_func);
	POPDS;
	return(ret);
#else
        bio_meth_num++;
        return(CRYPTO_get_ex_new_index(bio_meth_num-1,&bio_meth,
                argl,argp,new_func,dup_func,free_func));
#endif
        }

int BIO_set_ex_data(bio,idx,data)
BIO *bio;
int idx;
char *data;
	{
	return(CRYPTO_set_ex_data(&(bio->ex_data),idx,data));
	}

char *BIO_get_ex_data(bio,idx)
BIO *bio;
int idx;
	{
	return(CRYPTO_get_ex_data(&(bio->ex_data),idx));
	}

#endif
