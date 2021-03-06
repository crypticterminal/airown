/*
 * Airown - WEP key generators
 *
 * Copyright (C) 2001 Tim Newsham
 * Copyright (C) 2006 toast
 * Copyright (C) 2010 sh0 <sh0@yutani.ee> - cosmetic changes
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
 */

// Int inc
#include "ao_config.h"
#include "enc_wep_keys.h"

// Ext inc
#include <openssl/md5.h>

/**
 * \brief Generate four WEP subkeys from a seed
 * \param val Seed value
 * \param keys Pointer to key buffer
 * \ingroup wep
 */
static void wep_seedkeygen(gint val, guint8* keys)
{
    gint i;

    for (i = 0; i < WEP_KEY40_STORE; i++) {
        val *= 0x343fd;
        val += 0x269ec3;
        keys[i] = val >> 16;
    }
    return;
}

/**
 * \brief Generate one 128 bit (16 byte) WEP key from a string
 * \param str Plaintext password
 * \param keys Pointer to key buffer (at least 16 bytes long)
 * \ingroup wep
 */
void wep_keygen104(const gchar* str, guint8* keys)
{
    MD5_CTX ctx;
    guint8 buf[64];
    gint i, j;

    // Repeat str until buf is full
    j = 0;
    for (i = 0; i < 64; i++) {
        if (str[j] == 0)
            j = 0;
        buf[i] = str[j++];
    }

    // Calculate MD5 hash
    MD5_Init(&ctx);
    MD5_Update(&ctx, buf, sizeof(buf));
    MD5_Final(buf, &ctx);

    // Copy result to key
    for (i = 0; i < WEP_KEY104_SIZE; i++) {
        keys[i] = buf[i];
    }
}

/**
 * \brief Generate four 40 bit (5 byte) WEP subkeys from a string
 * \param str Plaintext password
 * \param keys Pointer to key buffer (at least 21 bytes long)
 * \note Key locations: keys[0-4], keys[5-9], keys[10-14], keys[15-20].
 * \ingroup wep
 */
void wep_keygen40(const gchar* str, guint8* keys)
{
    gint val, i, shift;

    // Seed is generated by xor'ing in the keystring bytes
    // into the four bytes of the seed, starting at the little end
    val = 0;
    for (i = 0; str[i]; i++) {
        shift = i & 0x3;
        val ^= (str[i] << (shift * 8));
    }

    wep_seedkeygen(val, keys);
}

/**
 * \brief Print four 40 bit keys to stdout
 * \param keys Pointer to keys
 * \ingroup wep
 */
void wep_40keyprint(guint8* keys)
{
    gint i;
    gchar sepchar;

    for (i = 0; i < WEP_KEY40_STORE; i++) {
        if (i % WEP_KEY40_SIZE == 0)
            g_print("%d: ", i/WEP_KEY40_SIZE);
        sepchar = (i % WEP_KEY40_SIZE == WEP_KEY40_SIZE - 1) ? ' ' : ':';
        g_print("%02x%c", keys[i], sepchar);
    }
}

/**
 * \brief Print variable size key to stdout
 * \param keys Pointer to key
 * \param nbytes Key size in bytes
 * \ingroup wep
 */
void wep_nkeyprint(guint8* key, guint nbytes)
{
    guint i;
    gchar sepchar;

    for (i = 0; i < nbytes; i++) {
        sepchar = (i == nbytes - 1) ? ' ' : ':';
        g_print("%02x%c", key[i], sepchar);
    }
}

