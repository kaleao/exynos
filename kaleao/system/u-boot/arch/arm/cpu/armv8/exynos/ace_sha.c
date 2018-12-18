/*
 * Advanced Crypto Engine - SHA Firmware
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

#include <common.h>
#include <asm/arch/ace_sfr.h>
#include <asm/arch/ace_sha.h>


#if defined(CONFIG_ARCH_EXYNOS)

/*****************************************************************
	Definitions
*****************************************************************/
#define ACE_read_sfr(_sfr_)	\
	(*(volatile unsigned int*)(ACE_SFR_BASE + _sfr_))
#define ACE_write_sfr(_sfr_, _val_)	\
	do {*(volatile unsigned int*)(ACE_SFR_BASE + _sfr_)	\
		= (unsigned int)(_val_); } while (0)


/* SHA1 value for the message of zero length */
const unsigned char sha1_digest_emptymsg[20] = {
	0xDA, 0x39, 0xA3, 0xEE, 0x5E, 0x6B, 0x4B, 0x0D,
	0x32, 0x55, 0xBF, 0xFF, 0x95, 0x60, 0x18, 0x90,
	0xAF, 0xD8, 0x07, 0x09};

/* SHA256 value for the message of zero length */
const unsigned char sha256_digest_emptymsg[32] = {
	0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14,
	0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
	0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C,
	0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55};

unsigned int change_endian(
	unsigned int input
)
{
	return (unsigned int)(((input & (0x000000ffUL)) << 24) ^
			  ((input & (0x0000ff00UL)) <<  8) ^
			  ((input & (0x00ff0000UL)) >>  8) ^
			  ((input & (0xff000000UL)) >> 24));
}

int ace_sha_engine(
	struct ace_hash_ctx *ctx,
	unsigned char *pOut,
	const unsigned char *pIn,
	size_t dataLen
)
{
	unsigned int reg;
	unsigned int *buffer;
	unsigned int block_size, digest_size;
	int transformmode = 0;
	int i = 0;

	block_size = (ctx->alg == ALG_SHA1) ? SHA1_BLOCK_LEN : SHA256_BLOCK_LEN;
	digest_size = (ctx->alg == ALG_SHA1) ? SHA1_DIGEST_LEN : SHA256_DIGEST_LEN;

	if (pOut == NULL) {
		if (dataLen == 0)
			return 0;
		else if (dataLen < digest_size)
			return -1;
		transformmode = 1;
	}

	if (dataLen == 0) {
		for (i = 0; i < block_size - 8; i++)
			ctx->buffer[i] = 0;
		ctx->buffer[0] = 0x80;

		reg = change_endian(ctx->prelen_high);
		for (i = 0; i < 4; i++)
			(ctx->buffer + block_size - 8)[i] = ((unsigned char *)&reg)[i];
		reg = change_endian(ctx->prelen_low);
		for (i = 0; i < 4; i++)
			(ctx->buffer + block_size - 4)[i] = ((unsigned char *)&reg)[i];

		pIn = ctx->buffer;

		dataLen = block_size;
		transformmode = 1;
	}

	/* Flush HRDMA */
	ACE_write_sfr(ACE_FC_HRDMAC, ACE_FC_HRDMACFLUSH_ON);
	ACE_write_sfr(ACE_FC_HRDMAC, ACE_FC_HRDMACFLUSH_OFF);

	/* Set byte swap of data in */
	ACE_write_sfr(ACE_HASH_BYTESWAP,
		ACE_HASH_SWAPDI_ON | ACE_HASH_SWAPDO_ON | ACE_HASH_SWAPIV_ON);

	/* Select Hash input mux as external source */
	reg = ACE_read_sfr(ACE_FC_FIFOCTRL);
	reg = (reg & ~ACE_FC_SELHASH_MASK) | ACE_FC_SELHASH_EXOUT;
	ACE_write_sfr(ACE_FC_FIFOCTRL, reg);

	/* Set Hash as SHA1 or SHA256 and start Hash engine */
	if (ctx->alg == ALG_SHA1)
		reg = ACE_HASH_ENGSEL_SHA1HASH | ACE_HASH_STARTBIT_ON;
	else
		reg = ACE_HASH_ENGSEL_SHA256HASH | ACE_HASH_STARTBIT_ON;

	/* Set IV */
	if ((ctx->prelen_low | ctx->prelen_high) != 0) {
		reg |= ACE_HASH_USERIV_EN;
		buffer = (unsigned int *)ctx->state;
		ACE_write_sfr(ACE_HASH_IV1, buffer[0]);
		ACE_write_sfr(ACE_HASH_IV2, buffer[1]);
		ACE_write_sfr(ACE_HASH_IV3, buffer[2]);
		ACE_write_sfr(ACE_HASH_IV4, buffer[3]);
		ACE_write_sfr(ACE_HASH_IV5, buffer[4]);

		if (ctx->alg == ALG_SHA256) {
			ACE_write_sfr(ACE_HASH_IV6, buffer[5]);
			ACE_write_sfr(ACE_HASH_IV7, buffer[6]);
			ACE_write_sfr(ACE_HASH_IV8, buffer[7]);
		}
	} else
		reg &= ~ACE_HASH_USERIV_EN;
	ACE_write_sfr(ACE_HASH_CONTROL, reg);

	/* Enable FIFO mode */
	ACE_write_sfr(ACE_HASH_FIFO_MODE, ACE_HASH_FIFO_ON);

	/* Clean the cache with input region if it's cacheable */

	if (transformmode) {
		/* Set message length */
		ACE_write_sfr(ACE_HASH_MSGSIZE_LOW, 0);
		ACE_write_sfr(ACE_HASH_MSGSIZE_HIGH, 0x80000000);

		/* Set pre-message length */
		ACE_write_sfr(ACE_HASH_PRELEN_LOW, 0);
		ACE_write_sfr(ACE_HASH_PRELEN_HIGH, 0);
	} else {
		/* Set message length */
		ACE_write_sfr(ACE_HASH_MSGSIZE_LOW, dataLen);
		ACE_write_sfr(ACE_HASH_MSGSIZE_HIGH, 0);

		/* Set pre-message length */
		ACE_write_sfr(ACE_HASH_PRELEN_LOW, ctx->prelen_low);
		ACE_write_sfr(ACE_HASH_PRELEN_HIGH, ctx->prelen_high);
	}

	/* Set HRDMA */
	ACE_write_sfr(ACE_FC_HRDMAS, (unsigned int)virt_to_phys(pIn));
	ACE_write_sfr(ACE_FC_HRDMAL, dataLen);

	while (!(ACE_read_sfr(ACE_FC_INTPEND) & ACE_FC_HRDMA))
		;
	ACE_write_sfr(ACE_FC_INTPEND, ACE_FC_HRDMA);

	if (transformmode) {
		/* Set Pause bit */
		ACE_write_sfr(ACE_HASH_CONTROL2, ACE_HASH_PAUSE_ON);

		while ((ACE_read_sfr(ACE_HASH_STATUS) &
		ACE_HASH_PARTIALDONE_MASK) == ACE_HASH_PARTIALDONE_OFF)
			;

		/* Clear PARTIAL_DONE bit */
		ACE_write_sfr(ACE_HASH_STATUS, ACE_HASH_PARTIALDONE_ON);

		if (pOut == NULL) {
			/* Update chaining variables */
			buffer = (unsigned int *)ctx->state;

			/* Update pre-message length */
			/* Note that the unit of pre-message length is a BIT!
			 * */
			ctx->prelen_low += (dataLen << 3);
			if (ctx->prelen_low < (dataLen << 3))
				ctx->prelen_high++;
			ctx->prelen_high += (dataLen >> 29);
		} else {
			/* Read hash result */
			buffer = (unsigned int *)pOut;
		}
	} else {
		while ((ACE_read_sfr(ACE_HASH_STATUS) & ACE_HASH_MSGDONE_MASK)
				== ACE_HASH_MSGDONE_OFF)
			;

		/* Clear MSG_DONE bit */
		ACE_write_sfr(ACE_HASH_STATUS, ACE_HASH_MSGDONE_ON);

		/* Read hash result */
		buffer = (unsigned int *)pOut;
	}

	buffer[0] = ACE_read_sfr(ACE_HASH_RESULT1);
	buffer[1] = ACE_read_sfr(ACE_HASH_RESULT2);
	buffer[2] = ACE_read_sfr(ACE_HASH_RESULT3);
	buffer[3] = ACE_read_sfr(ACE_HASH_RESULT4);
	buffer[4] = ACE_read_sfr(ACE_HASH_RESULT5);

	if (ctx->alg == ALG_SHA256) {
		buffer[5] = ACE_read_sfr(ACE_HASH_RESULT6);
		buffer[6] = ACE_read_sfr(ACE_HASH_RESULT7);
		buffer[7] = ACE_read_sfr(ACE_HASH_RESULT8);
	}

	return 0;
}

int ace_hash_sha_init(
	struct ace_hash_ctx *ctx,
	int alg
)
{
	ctx->alg = alg;
	ctx->buflen = 0;
	ctx->prelen_high = ctx->prelen_low = 0;

	return 0;
}

int ace_hash_sha_update(
	struct ace_hash_ctx *ctx,
	unsigned char *pData,
	unsigned int dataLen
)
{
	const unsigned char *src;
	size_t partlen, tmplen, block_size;
	int ret = 0;
	int i;

	partlen = ctx->buflen;
	src = pData;

	block_size = (ctx->alg == ALG_SHA1) ? SHA1_BLOCK_LEN : SHA256_BLOCK_LEN;

	if (partlen != 0) {
		if (partlen + dataLen < block_size) {
			for (i = 0; i < dataLen; i++)
				(ctx->buffer + partlen)[i] = src[i];

			ctx->buflen += dataLen;
			return ret;
		} else {
			tmplen = block_size - partlen;
			for (i = 0; i < tmplen; i++)
				(ctx->buffer + partlen)[i] = src[i];

			ret = ace_sha_engine(ctx, NULL, ctx->buffer, block_size);
			if (ret)
				return ret;

			dataLen -= tmplen;
			src += tmplen;
		}
	}

	partlen = dataLen & (block_size - 1);
	dataLen -= partlen;

	if (dataLen > 0) {
		ret = ace_sha_engine(ctx, NULL, src, dataLen);
		if (ret)
			return ret;
	}

	for (i = 0; i < partlen; i++)
		ctx->buffer[i] = (src + dataLen)[i];
	ctx->buflen = partlen;

	return ret;
}

int ace_hash_sha_final(
	struct ace_hash_ctx *ctx,
	unsigned char *pOut
)
{
	return ace_sha_engine(ctx, pOut, ctx->buffer, ctx->buflen);
}

/*****************************************************************
	Functions
*****************************************************************/
/**
 * @brief	This function computes hash value of input (pBuf[0]..pBuf[buflen-1]).
 *
 * @param	pOut	A pointer to the output buffer. When operation is completed
 *			20/32 bytes are copied to pOut[0]...pOut[31]. Thus, a user
 *			should allocate at least 20/32 bytes at pOut in advance.
 * @param	pBuf	A pointer to the input buffer
 * @param	bufLen	Byte length of input buffer
 *
 * @return	0	Success
 *
 * @remark	This function assumes that pBuf is a physical address of input buffer.
 *
 * @version V1.00
 * @b Revision History
 *	- V01.00	2009.11.13/djpark	Initial Version
 *	- V01.10	2010.10.19/djpark	Modification to support C210/V310
 */
int ace_hash_sha_digest(
	unsigned char	*pOut,
	unsigned char	*pBuf,
	unsigned int	bufLen,
	int		alg
)
{
	struct ace_hash_ctx ctx;
	int ret;

	if (pOut == NULL)
		return -1;

	if (bufLen == 0) {
		if (alg == ALG_SHA1)
			memcpy(pOut, sha1_digest_emptymsg, SHA1_DIGEST_LEN);
		else
			memcpy(pOut, sha256_digest_emptymsg,
			SHA256_DIGEST_LEN);
		return 0;
	}

	ret = ace_hash_sha_init(&ctx, alg);
	if (ret)
		return -1;

	ret = ace_sha_engine(&ctx, pOut, pBuf, bufLen);
	if (ret)
		return -1;

	return 0;
}

#endif
