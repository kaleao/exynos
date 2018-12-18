/*
 * Header file for SHA firmware
 *
 * Copyright (c) 2010  Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __ACE_FW_SHA_H__
#define __ACE_FW_SHA_H__


#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_ARCH_EXYNOS)

/* Hash Code Length */
#define SHA1_DIGEST_LEN                 20
#define SHA1_BLOCK_LEN                  64
#define SHA256_DIGEST_LEN               32
#define SHA256_BLOCK_LEN                64

enum {
	ALG_SHA1,
	ALG_SHA256,
};

struct ace_hash_ctx {
	int alg;
	size_t buflen;
	unsigned char buffer[SHA256_BLOCK_LEN];
	unsigned int state[SHA256_DIGEST_LEN/4];
	unsigned int prelen_high;
	unsigned int prelen_low;
};

/*****************************************************************
	Functions
*****************************************************************/
int ace_hash_sha_digest(
	unsigned char	*pOut,
	unsigned char	*pBuf,
	unsigned int	bufLen,
	int		alg
);

int ace_hash_sha_init(
	struct ace_hash_ctx *ctx,
	int alg
);

int ace_hash_sha_update(
	struct ace_hash_ctx *ctx,
	unsigned char *pData,
	unsigned int dataLen
);

int ace_hash_sha_final(
	struct ace_hash_ctx *ctx,
	unsigned char *pOut
);

#endif

#ifdef __cplusplus
}
#endif

#endif

