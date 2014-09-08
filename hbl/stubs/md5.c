/* $Id: md5.c,v 1.3 2006-05-01 16:57:31 quentin Exp $ */

/*
 * Implementation of the md5 algorithm as described in RFC1321
 * Copyright (C) 2005 Quentin Carbonneaux <crazyjoke@free.fr>
 *
 * This file is part of md5sum.
 *
 * md5sum is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Softawre Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * md5sum is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should hav received a copy of the GNU General Public License
 * along with md5sum; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 * Copyright (c) 2010-2014 valentine-hbl
 *
 * This code has been ported to valentine-hbl
 */

#include <common/utils/string.h>
#include <common/sdk.h>
#include <hbl/stubs/md5.h>

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static u8 MD5_PADDING[64] = { /* 512 Bits */
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void md5_encode(SceKernelUtilsMd5Context *ctx)
{
	unsigned int a = ctx->h[0], b = ctx->h[1], c = ctx->h[2], d = ctx->h[3];
	unsigned int *buf = (unsigned int *)ctx->buf;

	/* Round 1 */
	FF (a, b, c, d, buf[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, buf[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, buf[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, buf[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, buf[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, buf[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, buf[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, buf[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, buf[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, buf[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, buf[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, buf[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, buf[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, buf[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, buf[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, buf[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, buf[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, buf[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, buf[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, buf[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, buf[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, buf[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, buf[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, buf[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, buf[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, buf[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, buf[ 3], S23, 0xf4d50d87); /* 27 */

	GG (b, c, d, a, buf[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, buf[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, buf[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, buf[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, buf[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, buf[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, buf[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, buf[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, buf[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, buf[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, buf[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, buf[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, buf[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, buf[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, buf[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, buf[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, buf[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, buf[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, buf[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, buf[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, buf[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, buf[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, buf[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, buf[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, buf[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, buf[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, buf[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, buf[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, buf[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, buf[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, buf[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, buf[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, buf[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, buf[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, buf[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, buf[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, buf[ 9], S44, 0xeb86d391); /* 64 */

	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
}

/*
 * An easy way to do the md5 sum of a short memory space
 */
int _hook_sceKernelUtilsMd5Digest(u8 *data, u32 size, u8 *digest)
{
	SceKernelUtilsMd5Context ctx;

	if (data == NULL || digest == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	_hook_sceKernelUtilsMd5BlockInit(&ctx);
	_hook_sceKernelUtilsMd5BlockUpdate(&ctx, data, size);
	_hook_sceKernelUtilsMd5BlockResult(&ctx, digest);

	return 0;
}

int _hook_sceKernelUtilsMd5BlockInit(SceKernelUtilsMd5Context *ctx)
{
	if (ctx == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	ctx->usRemains = 0;
	ctx->usComputed = 0;
	ctx->ullTotalLen = 0;

	/* Init registries */
	ctx->h[0] = 0x67452301;
	ctx->h[1] = 0xefcdab89;
	ctx->h[2] = 0x98badcfe;
	ctx->h[3] = 0x10325476;

	return 0;
}

/*
 * Update a context by concatenating a new block
 */
int _hook_sceKernelUtilsMd5BlockUpdate(SceKernelUtilsMd5Context *ctx, u8 *data, u32 size)
{
	if (ctx == NULL || data == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	ctx->ullTotalLen += size;

	if (size + ctx->usRemains < sizeof(ctx->buf)) {
		memcpy(ctx->buf + ctx->usRemains, data, size);
		ctx->usRemains += size;
		return 0;
	}

	if (ctx->usRemains) {
		memcpy (ctx->buf + ctx->usRemains, data, sizeof(ctx->buf) - ctx->usRemains);
		data += sizeof(ctx->buf) - ctx->usRemains;
		size -= sizeof(ctx->buf) - ctx->usRemains;
		md5_encode(ctx);
		ctx->usRemains = 0;
		ctx->usComputed += sizeof(ctx->buf);
	}
	while (size >= sizeof(ctx->buf)) {
		memcpy (ctx->buf, data, sizeof(ctx->buf));
		data += sizeof(ctx->buf);
		size -= sizeof(ctx->buf);
		md5_encode(ctx);
		ctx->usComputed += sizeof(ctx->buf);
	}
	memcpy(ctx->buf, data, size);
	ctx->usRemains = size;

	return 0;
}

int _hook_sceKernelUtilsMd5BlockResult(SceKernelUtilsMd5Context *ctx, u8 *digest)
{
	int i;

	if (ctx == NULL || digest == NULL)
		return SCE_KERNEL_ERROR_ILLEGAL_ADDR;

	if (ctx->usRemains + 1 > 56) { /* We have to create another block */
		memcpy(ctx->buf + ctx->usRemains, MD5_PADDING, sizeof(ctx->buf) - ctx->usRemains);
		md5_encode(ctx);
		memset(ctx->buf, 0, 56);
		/*memcpy(ctx->buf, MD5_PADDING + 1, 56);*/
	} else
		memcpy(ctx->buf + ctx->usRemains, MD5_PADDING, 56 - ctx->usRemains);

	ctx->usComputed += ctx->usRemains;
	ctx->usRemains = 0;

	/* Proceed final block */
	//md5_addsize(ctx->buf, 56, ctx->usComputed);
	i = 56;
	ctx->buf[i++] = (u8)((ctx->usComputed << 3) & 0xFF);
	ctx->buf[i++] = (u8)((ctx->usComputed >> 5) & 0xFF);
	ctx->buf[i++] = (u8)((ctx->usComputed >> 13) & 0xFF);
	ctx->buf[i++] = (u8)((ctx->usComputed >> 21) & 0xFF);
	ctx->buf[i++] = 0;
	ctx->buf[i++] = 0;
	ctx->buf[i++] = 0;
	ctx->buf[i++] = 0;

	md5_encode(ctx);

	/* update digest */
	for (i = 0; i < 4; i++)
		digest[i] = (u8)((ctx->h[0] >> (i * 8)) & 0xFF);
	while (i < 8) {
		digest[i] = (u8)((ctx->h[1] >> ((i - 4) * 8)) & 0xFF);
		i++;
	}
	while (i < 12) {
		digest[i] = (u8)((ctx->h[2] >> ((i - 8) * 8)) & 0xFF);
		i++;
	}
	while (i < 16) {
		digest[i] = (u8)((ctx->h[3] >> ((i - 12) * 8)) & 0xFF);
		i++;
	}

	return 0;
}
