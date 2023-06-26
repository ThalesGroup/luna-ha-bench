/****************************************************************************\
*
* This file is provided under the MIT license (see the following Web site
* for further details: https://mit-license.org/ ).
*
* Copyright © 2023 Thales Group
*
\****************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p11-toolkit.h"

#define LUNA_MODEL_VIRTUAL "LunaVirtual"
#define MAXIMUM_SLOT_COUNT 100

CK_BBOOL ckTrue = TRUE;
CK_BBOOL ckFalse = FALSE;

unsigned char P11_OIDS[] = {
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x06, /* [0] OID_secp112r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x07, /* [7] OID_secp112r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1C, /* [14] OID_secp128r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1D, /* [21] OID_secp128r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x09, /* [28] OID_secp160k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x08, /* [35] OID_secp160r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1E, /* [42] OID_secp160r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1F, /* [49] OID_secp192k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x20, /* [56] OID_secp224k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x21, /* [63] OID_secp224r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x0A, /* [70] OID_secp256k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x22, /* [77] OID_secp384r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x23, /* [84] OID_secp521r1 */

    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x01, /* [91] OID_X9_62_prime192v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x02, /* [101] OID_X9_62_prime192v2 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x03, /* [111] OID_X9_62_prime192v3 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x04, /* [121] OID_X9_62_prime239v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x05, /* [131] OID_X9_62_prime239v2 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x06, /* [141] OID_X9_62_prime239v3 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, /* [151] OID_X9_62_prime256v1 */

    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x04, /* [161] OID_sect113r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x05, /* [168] OID_sect113r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x16, /* [175] OID_sect131r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x17, /* [182] OID_sect131r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x01, /* [189] OID_sect163k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x02, /* [196] OID_sect163r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x0F, /* [203] OID_sect163r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x18, /* [210] OID_sect193r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x19, /* [217] OID_sect193r2 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1A, /* [224] OID_sect233k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x1B, /* [231] OID_sect233r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x03, /* [238] OID_sect239k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x10, /* [245] OID_sect283k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x11, /* [252] OID_sect283r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x24, /* [259] OID_sect409k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x25, /* [266] OID_sect409r1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x26, /* [273] OID_sect571k1 */
    0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x27, /* [280] OID_sect571r1 */

    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x01, /* [287] OID_X9_62_c2pnb163v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x02, /* [297] OID_X9_62_c2pnb163v2 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x03, /* [307] OID_X9_62_c2pnb163v3 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x04, /* [317] OID_X9_62_c2pnb176v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x05, /* [327] OID_X9_62_c2tnb191v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x06, /* [337] OID_X9_62_c2tnb191v2 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x07, /* [347] OID_X9_62_c2tnb191v3 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x0A, /* [357] OID_X9_62_c2pnb208w1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x0B, /* [367] OID_X9_62_c2tnb239v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x0C, /* [377] OID_X9_62_c2tnb239v2 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x0D, /* [387] OID_X9_62_c2tnb239v3 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x10, /* [397] OID_X9_62_c2pnb272w1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x11, /* [407] OID_X9_62_c2pnb304w1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x12, /* [417] OID_X9_62_c2tnb359v1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x13, /* [427] OID_X9_62_c2pnb368w1 */
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x00, 0x14, /* [437] OID_X9_62_c2tnb431r1 */

    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x01, /* [447] brainpoolP160r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x02, /* [458] brainpoolP160t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x03, /* [469] brainpoolP192r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x04, /* [480] brainpoolP192t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x05, /* [491] brainpoolP224r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x06, /* [502] brainpoolP224t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x07, /* [513] brainpoolP256r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x08, /* [524] brainpoolP256t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x09, /* [535] brainpoolP320r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0a, /* [546] brainpoolP320t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0b, /* [557] brainpoolP384r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0c, /* [568] brainpoolP384t1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0d, /* [579] brainpoolP512r1 */
    0x06, 0x09, 0x2B, 0x24, 0x03, 0x03, 0x02, 0x08, 0x01, 0x01, 0x0e, /* [590] brainpoolP512t1 */

    0x06, 0x0a, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x97, 0x55, 0x01, 0x05, 0x01, /* [601] curve25519 */
    0x06, 0x09, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xDA, 0x47, 0x0F, 0x01,       /* [613] ed25519 */

    0x06, 0x08, 0x2A, 0x81, 0x1C, 0xCF, 0x55, 0x01, 0x82, 0x2D, /* [624] sm2p256v1 */

    // next named curve oid to be defined
    /* [634] curve name */
};

unsigned char microsoftPlayReadP160_curveParams[] = {
    0x30, 0x81, 0x98, 0x02, 0x01, 0x01, 0x30, 0x20,
    0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x01,
    0x01, 0x02, 0x15, 0x00, 0x89, 0xab, 0xcd, 0xef,
    0x01, 0x23, 0x45, 0x67, 0x27, 0x18, 0x28, 0x18,
    0x31, 0x41, 0x59, 0x26, 0x14, 0x14, 0x24, 0xf7,
    0x30, 0x2c, 0x04, 0x14, 0x37, 0xa5, 0xab, 0xcc,
    0xd2, 0x77, 0xbc, 0xe8, 0x76, 0x32, 0xff, 0x3d,
    0x47, 0x80, 0xc0, 0x09, 0xeb, 0xe4, 0x14, 0x97,
    0x04, 0x14, 0x0d, 0xd8, 0xda, 0xbf, 0x72, 0x5e,
    0x2f, 0x32, 0x28, 0xe8, 0x5f, 0x1a, 0xd7, 0x8f,
    0xde, 0xdf, 0x93, 0x28, 0x23, 0x9e, 0x04, 0x29,
    0x04, 0x87, 0x23, 0x94, 0x7f, 0xd6, 0xa3, 0xa1,
    0xe5, 0x35, 0x10, 0xc0, 0x7d, 0xba, 0x38, 0xda,
    0xf0, 0x10, 0x9f, 0xa1, 0x20, 0x44, 0x57, 0x44,
    0x91, 0x10, 0x75, 0x52, 0x2d, 0x8c, 0x3c, 0x58,
    0x56, 0xd4, 0xed, 0x7a, 0xcd, 0xa3, 0x79, 0x93,
    0x6f, 0x02, 0x15, 0x00, 0x89, 0xab, 0xcd, 0xef,
    0x01, 0x23, 0x45, 0x67, 0x27, 0x16, 0xb2, 0x6e,
    0xec, 0x14, 0x90, 0x44, 0x28, 0xc2, 0xa6, 0x75,
    0x02, 0x01, 0x01};

P11_ELLIPTIC_CURVES_LIST_ELEMENT P11_ELLIPTIC_CURVES_LIST[] = {
    /* prime field curves */
    /* secg curves */
    {7, 14, (112 + 7) / 8, &(P11_OIDS[0]), (char *)"secp112r1"},         // 0
    {7, 14, (112 + 7) / 8, &(P11_OIDS[7]), (char *)"secp112r2"},         // 1
    {7, 16, (128 + 7) / 2, &(P11_OIDS[14]), (char *)"secp128r1"},        // 2
    {7, 16, (128 + 7) / 8, &(P11_OIDS[21]), (char *)"secp128r2"},        // 3
    {7, 21, (160 + 7) / 8, &(P11_OIDS[28]), (char *)"secp160k1"},        // 4
    {7, 21, (160 + 7) / 8, &(P11_OIDS[35]), (char *)"secp160r1"},        // 5
    {7, 21, (160 + 7) / 8, &(P11_OIDS[42]), (char *)"secp160r2"},        // 6
    {7, 24, (192 + 7) / 8, &(P11_OIDS[49]), (char *)"secp192k1"},        // 7
    {7, 29, (224 + 7) / 8, &(P11_OIDS[56]), (char *)"secp224k1"},        // 8
    {7, 28, (224 + 7) / 8, &(P11_OIDS[63]), (char *)"secp224r1(P-224)"}, // 9
    {7, 32, (256 + 7) / 8, &(P11_OIDS[70]), (char *)"secp256k1"},        // 10
    {7, 48, (384 + 7) / 8, &(P11_OIDS[77]), (char *)"secp384r1(P-384)"}, // 11
    {7, 66, (521 + 7) / 8, &(P11_OIDS[84]), (char *)"secp521r1(P-521)"}, // 12

    /* X9.62 curves */
    {10, 24, (192 + 7) / 8, &(P11_OIDS[91]), (char *)"X9_62_prime192v1(P-192)"},  // 13
    {10, 24, (192 + 7) / 8, &(P11_OIDS[101]), (char *)"X9_62_prime192v2"},        // 14
    {10, 24, (192 + 7) / 8, &(P11_OIDS[111]), (char *)"X9_62_prime192v3"},        // 15
    {10, 30, (239 + 7) / 8, &(P11_OIDS[121]), (char *)"X9_62_prime239v1"},        // 16
    {10, 30, (239 + 7) / 8, &(P11_OIDS[131]), (char *)"X9_62_prime239v2"},        // 17
    {10, 30, (239 + 7) / 8, &(P11_OIDS[141]), (char *)"X9_62_prime239v3"},        // 18
    {10, 32, (256 + 7) / 8, &(P11_OIDS[151]), (char *)"X9_62_prime256v1(P-256)"}, // 19

    /* characteristic two field curves */
    /* secg curves */
    {7, 15, (113 + 7) / 8, &(P11_OIDS[161]), (char *)"sect113r1"}, // 20
    {7, 15, (113 + 7) / 8, &(P11_OIDS[168]), (char *)"sect113r2"}, // 21
    {7, 17, (131 + 7) / 8, &(P11_OIDS[175]), (char *)"sect131r1"}, // 22
    {7, 17, (131 + 7) / 8, &(P11_OIDS[182]), (char *)"sect131r2"}, // 23
    {7, 21, (163 + 7) / 8, &(P11_OIDS[189]), (char *)"sect163k1"}, // 24
    {7, 21, (163 + 7) / 8, &(P11_OIDS[196]), (char *)"sect163r1"}, // 25
    {7, 21, (163 + 7) / 8, &(P11_OIDS[203]), (char *)"sect163r2"}, // 26
    {7, 25, (193 + 7) / 8, &(P11_OIDS[210]), (char *)"sect193r1"}, // 27
    {7, 25, (193 + 7) / 8, &(P11_OIDS[217]), (char *)"sect193r2"}, // 28
    {7, 29, (233 + 7) / 8, &(P11_OIDS[224]), (char *)"sect233k1"}, // 29
    {7, 30, (233 + 7) / 8, &(P11_OIDS[231]), (char *)"sect233r1"}, // 30
    {7, 30, (239 + 7) / 8, &(P11_OIDS[238]), (char *)"sect239k1"}, // 31
    {7, 36, (283 + 7) / 8, &(P11_OIDS[245]), (char *)"sect283k1"}, // 32
    {7, 36, (283 + 7) / 8, &(P11_OIDS[252]), (char *)"sect283r1"}, // 33
    {7, 51, (409 + 7) / 8, &(P11_OIDS[259]), (char *)"sect409k1"}, // 34
    {7, 52, (409 + 7) / 8, &(P11_OIDS[266]), (char *)"sect409r1"}, // 35
    {7, 72, (571 + 7) / 8, &(P11_OIDS[273]), (char *)"sect571k1"}, // 36
    {7, 72, (571 + 7) / 8, &(P11_OIDS[280]), (char *)"sect571r1"}, // 37

    /* X9.62 curves */
    {10, 21, (163 + 7) / 8, &(P11_OIDS[287]), (char *)"X9_62_c2pnb163v1"}, // 38
    {10, 21, (163 + 7) / 8, &(P11_OIDS[297]), (char *)"X9_62_c2pnb163v2"}, // 39
    {10, 21, (163 + 7) / 8, &(P11_OIDS[307]), (char *)"X9_62_c2pnb163v3"}, // 40
    {10, 21, (176 + 7) / 8, &(P11_OIDS[317]), (char *)"X9_62_c2pnb176v1"}, // 41
    {10, 24, (191 + 7) / 8, &(P11_OIDS[327]), (char *)"X9_62_c2tnb191v1"}, // 42
    {10, 24, (191 + 7) / 8, &(P11_OIDS[337]), (char *)"X9_62_c2tnb191v2"}, // 43
    {10, 24, (191 + 7) / 8, &(P11_OIDS[347]), (char *)"X9_62_c2tnb191v3"}, // 44
    {10, 25, (208 + 7) / 8, &(P11_OIDS[357]), (char *)"X9_62_c2pnb208w1"}, // 45
    {10, 30, (239 + 7) / 8, &(P11_OIDS[367]), (char *)"X9_62_c2tnb239v1"}, // 46
    {10, 30, (239 + 7) / 8, &(P11_OIDS[377]), (char *)"X9_62_c2tnb239v2"}, // 47
    {10, 30, (239 + 7) / 8, &(P11_OIDS[387]), (char *)"X9_62_c2tnb239v3"}, // 48
    {10, 33, (272 + 7) / 8, &(P11_OIDS[397]), (char *)"X9_62_c2pnb272w1"}, // 49
    {10, 37, (304 + 7) / 8, &(P11_OIDS[407]), (char *)"X9_62_c2pnb304w1"}, // 50
    {10, 45, (359 + 7) / 8, &(P11_OIDS[417]), (char *)"X9_62_c2tnb359v1"}, // 51
    {10, 45, (368 + 7) / 8, &(P11_OIDS[427]), (char *)"X9_62_c2pnb368w1"}, // 52
    {10, 53, (431 + 7) / 8, &(P11_OIDS[437]), (char *)"X9_62_c2tnb431r1"}, // 53

    /* Brainpool prime curves */
    {11, 20, (160 + 7) / 8, &(P11_OIDS[447]), (char *)"brainpoolP160r1"}, // 54
    {11, 20, (160 + 7) / 8, &(P11_OIDS[458]), (char *)"brainpoolP160t1"}, // 55
    {11, 24, (192 + 7) / 8, &(P11_OIDS[469]), (char *)"brainpoolP192r1"}, // 56
    {11, 24, (192 + 7) / 8, &(P11_OIDS[480]), (char *)"brainpoolP192t1"}, // 57
    {11, 28, (224 + 7) / 8, &(P11_OIDS[491]), (char *)"brainpoolP224r1"}, // 58
    {11, 28, (224 + 7) / 8, &(P11_OIDS[502]), (char *)"brainpoolP224t1"}, // 59
    {11, 32, (256 + 7) / 8, &(P11_OIDS[513]), (char *)"brainpoolP256r1"}, // 60
    {11, 32, (256 + 7) / 8, &(P11_OIDS[524]), (char *)"brainpoolP256t1"}, // 61
    {11, 40, (320 + 7) / 8, &(P11_OIDS[535]), (char *)"brainpoolP320r1"}, // 62
    {11, 40, (320 + 7) / 8, &(P11_OIDS[546]), (char *)"brainpoolP320t1"}, // 63
    {11, 48, (384 + 7) / 8, &(P11_OIDS[557]), (char *)"brainpoolP384r1"}, // 64
    {11, 48, (384 + 7) / 8, &(P11_OIDS[568]), (char *)"brainpoolP384t1"}, // 65
    {11, 64, (512 + 7) / 8, &(P11_OIDS[579]), (char *)"brainpoolP512r1"}, // 66
    {11, 64, (512 + 7) / 8, &(P11_OIDS[590]), (char *)"brainpoolP512t1"}, // 67

    // User Defined Curves
    // Microsoft PlayReady P 160
    {sizeof(microsoftPlayReadP160_curveParams), 20, (160 + 7) / 8, &(microsoftPlayReadP160_curveParams[0]), (char *)"microsoftPlayReadyP160"}, // 68

    {12, 32, (256 + 7) / 8, &(P11_OIDS[601]), (char *)"curve25519"}, // 69
    {11, 32, (512 + 7) / 8, &(P11_OIDS[613]), (char *)"ed25519"},    // 70

    // SM2
    {10, 32, (256 + 7) / 8, &(P11_OIDS[624]), (char *)"sm2p256v1"}, // 71
};

CK_RV p11tk_closeSession(const CK_SESSION_HANDLE sessionHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    return C_CloseSession(sessionHandle);
}

CK_RV p11tk_destroyObject(const CK_SESSION_HANDLE sessionHandle,
                          const CK_OBJECT_HANDLE objectHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(objectHandle != CK_INVALID_HANDLE);

    CK_RV rv = CKR_OK;

    rv = C_DestroyObject(sessionHandle,
                         objectHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_destroyObject()/C_DestroyObject() failed with error '0x%08lx'.\n",
                rv);
    }

    return rv;
}

CK_RV p11tk_destroyObjectsForClass(const CK_SESSION_HANDLE sessionHandle,
                                   const CK_OBJECT_CLASS objectClass)
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    CK_OBJECT_CLASS _objectClass = objectClass;
    CK_ATTRIBUTE objectTemplate[] = {{CKA_CLASS, &_objectClass, sizeof(_objectClass)}};

    return p11tk_destroyObjectsForObjectTemplate(sessionHandle,
                                                 objectTemplate,
                                                 GET_ARRAY_SIZE(objectTemplate));
}

CK_RV p11tk_destroyObjectsForLabel(const CK_SESSION_HANDLE sessionHandle,
                                   const CK_OBJECT_CLASS objectClass,
                                   const CK_CHAR *const objectLabel)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(objectLabel != NULL);

    CK_OBJECT_CLASS _objectClass = objectClass;
    CK_ATTRIBUTE objectTemplate[] = {{CKA_CLASS, &_objectClass, sizeof(_objectClass)},
                                     {CKA_LABEL, (CK_CHAR_PTR)objectLabel, (CK_ULONG)strlen((char *)objectLabel)}};

    return p11tk_destroyObjectsForObjectTemplate(sessionHandle,
                                                 objectTemplate,
                                                 GET_ARRAY_SIZE(objectTemplate));
}

CK_RV p11tk_destroyObjectsForObjectTemplate(const CK_SESSION_HANDLE sessionHandle,
                                            const CK_ATTRIBUTE *const objectTemplate,
                                            const size_t objectTemplateSize)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(objectTemplate != NULL);

    CK_RV rv = CKR_OK;

    rv = C_FindObjectsInit(sessionHandle,
                           (CK_ATTRIBUTE_PTR)objectTemplate,
                           objectTemplateSize);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_destroyObjectsForObjectTemplate()/C_FindObjectsInit() failed with error '0x%08lx'.\n",
                rv);

        goto EXIT;
    }

    CK_OBJECT_HANDLE foundObjectHandles[10] = {0};
    CK_ULONG numberOfFoundObjects = (CK_ULONG)0;

    do
    {
        rv = C_FindObjects(sessionHandle,
                           foundObjectHandles,
                           GET_ARRAY_SIZE(foundObjectHandles),
                           &numberOfFoundObjects);

        if (rv != CKR_OK)
        {
            fprintf(stderr,
                    "p11tk_destroyObjectsForObjectTemplate()/C_FindObjects() failed with error '0x%08lx'.\n",
                    rv);

            p11tk_findObjectsFinal(sessionHandle); // Ignore result value

            goto EXIT;
        }

        if (numberOfFoundObjects > 0)
        {
            for (unsigned int objectHandleIndex = 0;
                 objectHandleIndex < numberOfFoundObjects;
                 objectHandleIndex++)
            {
                rv = p11tk_destroyObject(sessionHandle,
                                         foundObjectHandles[objectHandleIndex]);

                if (rv != CKR_OK)
                {
                    p11tk_findObjectsFinal(sessionHandle); // Ignore result value

                    goto EXIT;
                }
            }
        }
    } while (numberOfFoundObjects > 0);

    rv = p11tk_findObjectsFinal(sessionHandle);

EXIT:
    return rv;
}

CK_RV p11tk_encryptWithAesKwp(const CK_SESSION_HANDLE sessionHandle,
                              const CK_OBJECT_HANDLE encryptionKeyHandle,
                              const CK_BYTE *const data,
                              const CK_ULONG dataLength,
                              CK_BYTE *const pEncryptedData,
                              CK_ULONG *const pEncryptedDataLength)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(encryptionKeyHandle != CK_INVALID_HANDLE);
    assert(data != NULL);
    assert(dataLength > 0);
    assert(pEncryptedData != NULL);
    assert(pEncryptedDataLength != NULL);

    CK_RV rv = CKR_OK;

    CK_MECHANISM mechanism = {0};

    mechanism.mechanism = CKM_AES_KWP;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = (CK_ULONG)0;

    rv = C_EncryptInit(sessionHandle,
                       &mechanism,
                       encryptionKeyHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_encryptWithAesKwp()/C_EncryptInit() failed with error '0x%08lx'.\n",
                rv);

        goto EXIT;
    }

    rv = C_Encrypt(sessionHandle,
                   (CK_BYTE_PTR)data,
                   dataLength,
                   (CK_BYTE_PTR)pEncryptedData,
                   pEncryptedDataLength);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_encryptWithAesKwp()/C_Encrypt() failed with error '0x%08lx'.\n",
                rv);
    }

EXIT:
    return rv;
}

CK_RV p11tk_finalizeClientLibrary()
{
    return C_Finalize(NULL);
}

CK_RV p11tk_findObjectForLabel(const CK_SESSION_HANDLE sessionHandle,
                               const CK_OBJECT_CLASS objectClass,
                               const CK_CHAR *const objectLabel,
                               CK_OBJECT_HANDLE *const pObjectHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(objectLabel != NULL);
    assert(pObjectHandle != NULL);

    CK_RV rv = CKR_OK;

    CK_OBJECT_CLASS _objectClass = objectClass;
    CK_ATTRIBUTE objectTemplate[] = {{CKA_CLASS, &_objectClass, sizeof(_objectClass)},
                                     {CKA_LABEL, (CK_CHAR_PTR)objectLabel, (CK_ULONG)strlen((char *)objectLabel)}};

    rv = C_FindObjectsInit(sessionHandle,
                           objectTemplate,
                           GET_ARRAY_SIZE(objectTemplate));

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_findObjectForLabel()/C_FindObjectsInit() failed with error '0x%08lx'.\n",
                rv);

        goto EXIT;
    }

    CK_ULONG numberOfObjectsFound = 0;

    rv = C_FindObjects(sessionHandle,
                       pObjectHandle,
                       1,
                       &numberOfObjectsFound);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_findObjectForLabel()/C_FindObjects() failed with error '0x%08lx'.\n",
                rv);
    }
    else
    {
        if (numberOfObjectsFound > 1)
        {
            fprintf(stderr,
                    "p11tk_findObjectForLabel() has found more than one object labeled '%s'.\n",
                    (char *)objectLabel);
        }
    }

    p11tk_findObjectsFinal(sessionHandle); // Ignore result value

EXIT:
    return rv;
}

CK_RV p11tk_findObjectForOuid(const CK_SESSION_HANDLE sessionHandle,
                              const CK_BYTE *const ouid,
                              const size_t ouidLength,
                              CK_OBJECT_HANDLE *const pObjectHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(ouid != NULL);
    assert(ouidLength >= P11TK_OUID_LENGTH);
    assert(pObjectHandle != NULL);

    CK_RV rv = CKR_OK;

    CK_ATTRIBUTE attribute = {CKA_OUID, (CK_CHAR_PTR)ouid, (CK_ULONG)ouidLength};

    rv = C_FindObjectsInit(sessionHandle,
                           &attribute,
                           1);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_findObjectForOuid()/C_FindObjectsInit() failed with error '0x%08lx'.\n",
                rv);

        writeBinaryData("p11tk_findObjectForOuid()/C_FindObjectsInit(): OUID=", ouid, ouidLength);

        goto EXIT;
    }

    CK_ULONG numberOfObjectsFound = 0;

    rv = C_FindObjects(sessionHandle,
                       pObjectHandle,
                       1,
                       &numberOfObjectsFound);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_findObjectForOuid()/C_FindObjects() failed with error '0x%08lx'.\n",
                rv);
    }
    else
    {
        if (numberOfObjectsFound > 1)
        {
            fprintf(stderr,
                    "p11tk_findObjectForOuid() has found more than one object for the provided OUID.\n");
        }
    }

    p11tk_findObjectsFinal(sessionHandle); // Ignore result value

EXIT:
    return rv;
}

CK_RV p11tk_findObjectsFinal(const CK_SESSION_HANDLE sessionHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    CK_RV rv = CKR_OK;

    rv = C_FindObjectsFinal(sessionHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_findObjectsFinal()/C_FindObjectsFinal() failed with error '0x%08lx'.\n",
                rv);
    }

    return rv;
}

CK_RV p11tk_generateEllipticKeyPair(const CK_SESSION_HANDLE sessionHandle,
                                    const CK_BBOOL isTokenObject,
                                    const unsigned int ellipticCurveIdentifier,
                                    const CK_CHAR *const publicKeyLabel,
                                    const CK_CHAR *const privateKeyLabel,
                                    CK_OBJECT_HANDLE *const pPublicKeyHandle,
                                    CK_OBJECT_HANDLE *const pPrivateKeyHandle)
{
    CK_RV rv = CKR_OK;

    const P11_ELLIPTIC_CURVES_LIST_ELEMENT *pEllipticCurveData = NULL;

    rv = p11tk_getEllipticCurveDataForCurveIdentifier(ellipticCurveIdentifier,
                                                      &pEllipticCurveData);

    if (rv != CKR_OK)
    {
        p11tk_writeError("p11tk_generateEllipticKeyPair()/getEllipticCurveData() failed.",
                         rv);

        goto EXIT;
    }

    size_t ellipticCurveOidLength = pEllipticCurveData->oidLength;
    unsigned char *ellipticCurveOid = pEllipticCurveData->oid;

    CK_MECHANISM mechanism = {0};

    mechanism.mechanism = CKM_EC_KEY_PAIR_GEN;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = (CK_ULONG)0;

    switch (ellipticCurveIdentifier)
    {
    case 69:
        mechanism.mechanism = CKM_EC_MONTGOMERY_KEY_PAIR_GEN;

        break;

    case 70:
        mechanism.mechanism = CKM_EC_EDWARDS_KEY_PAIR_GEN;

        break;

    default:
        break;
    }

    // Make sure CKA_DERIVE is set, for shim support.
    CK_BBOOL _isTokenObject = isTokenObject;
    CK_ATTRIBUTE publicKeyObjectTemplate[] = {{CKA_TOKEN, &_isTokenObject, sizeof(CK_BBOOL)},
                                              {CKA_PRIVATE, &ckTrue, sizeof(ckTrue)},
                                              {CKA_MODIFIABLE, &ckFalse, sizeof(ckFalse)},
                                              {CKA_LABEL, (CK_CHAR_PTR)publicKeyLabel, (CK_ULONG)strlen((char *)publicKeyLabel)},

                                              {CKA_DERIVE, &ckTrue, sizeof(ckTrue)},

                                              {CKA_ENCRYPT, &ckTrue, sizeof(ckTrue)},
                                              {CKA_VERIFY, &ckTrue, sizeof(ckTrue)},
                                              {CKA_WRAP, &ckFalse, sizeof(ckFalse)},

                                              {CKA_EC_PARAMS, ellipticCurveOid, ellipticCurveOidLength}};
    CK_ATTRIBUTE privateKeyObjectTemplate[] = {{CKA_TOKEN, &_isTokenObject, sizeof(CK_BBOOL)},
                                               {CKA_PRIVATE, &ckTrue, sizeof(ckTrue)},
                                               {CKA_MODIFIABLE, &ckFalse, sizeof(ckFalse)},
                                               {CKA_LABEL, (CK_CHAR_PTR)privateKeyLabel, (CK_ULONG)strlen((char *)privateKeyLabel)},

                                               {CKA_DERIVE, &ckTrue, sizeof(ckTrue)},

                                               {CKA_DECRYPT, &ckTrue, sizeof(ckTrue)},
                                               {CKA_SENSITIVE, &ckTrue, sizeof(ckTrue)},
                                               {CKA_SIGN, &ckTrue, sizeof(ckTrue)},
                                               {CKA_UNWRAP, &ckFalse, sizeof(ckFalse)}};

    rv = C_GenerateKeyPair(sessionHandle,
                           &mechanism,
                           publicKeyObjectTemplate,
                           GET_ARRAY_SIZE(publicKeyObjectTemplate),
                           privateKeyObjectTemplate,
                           GET_ARRAY_SIZE(privateKeyObjectTemplate),
                           pPublicKeyHandle,
                           pPrivateKeyHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_generateEllipticKeyPair()/C_GenerateKeyPair() failed with error '0x%08lx'.\n",
                rv);
    }

EXIT:
    return rv;
}

CK_RV p11tk_generateStorageKey(const CK_SESSION_HANDLE sessionHandle,
                               const CK_BBOOL isTokenObject,
                               const CK_CHAR *const keyLabel,
                               CK_OBJECT_HANDLE *const pKeyHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(keyLabel != NULL);
    assert(pKeyHandle != NULL);

    CK_RV rv = CKR_OK;
    CK_MECHANISM mechanism = {0};

    mechanism.mechanism = CKM_AES_KEY_GEN;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = (CK_ULONG)0;

    CK_OBJECT_CLASS objectClass = CKO_SECRET_KEY;
    CK_KEY_TYPE keyType = CKK_AES;
    CK_ULONG keyLength = (CK_ULONG)32; // 256 bits
    CK_BBOOL _isTokenObject = isTokenObject;
    CK_ATTRIBUTE objectTemplate[] = {{CKA_CLASS, &objectClass, sizeof(objectClass)},
                                     {CKA_KEY_TYPE, &keyType, sizeof(keyType)},
                                     {CKA_VALUE_LEN, &keyLength, sizeof(keyLength)},

                                     {CKA_TOKEN, &_isTokenObject, sizeof(_isTokenObject)},
                                     {CKA_PRIVATE, &ckTrue, sizeof(ckTrue)},
                                     {CKA_MODIFIABLE, &ckFalse, sizeof(ckFalse)},
                                     {CKA_LABEL, (CK_CHAR_PTR)keyLabel, (CK_ULONG)strlen((char *)keyLabel)},

                                     {CKA_DERIVE, &ckFalse, sizeof(ckFalse)},

                                     {CKA_ENCRYPT, &ckTrue, sizeof(ckTrue)},
                                     {CKA_DECRYPT, &ckFalse, sizeof(ckFalse)},
                                     {CKA_SENSITIVE, &ckTrue, sizeof(ckTrue)},
                                     {CKA_SIGN, &ckTrue, sizeof(ckTrue)},
                                     {CKA_VERIFY, &ckFalse, sizeof(ckFalse)},
                                     {CKA_WRAP, &ckFalse, sizeof(ckFalse)},
                                     {CKA_UNWRAP, &ckTrue, sizeof(ckTrue)},
                                     {CKA_EXTRACTABLE, &ckFalse, sizeof(ckFalse)}};

    rv = C_GenerateKey(sessionHandle,
                       &mechanism,
                       objectTemplate,
                       GET_ARRAY_SIZE(objectTemplate),
                       pKeyHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_generateStorageKey()/C_GenerateKey() failed with error '0x%08lx'.\n",
                rv);
    }

    return rv;
}

CK_RV p11tk_generateStorageKeyIfNeeded(const CK_SESSION_HANDLE sessionHandle,
                                       const CK_BBOOL isTokenObject,
                                       const CK_CHAR *const keyLabel,
                                       CK_OBJECT_HANDLE *const pKeyHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(keyLabel != NULL);
    assert(pKeyHandle != NULL);

    CK_RV rv = CKR_OK;

    // Look for an existing secret key.
    *pKeyHandle = CK_INVALID_HANDLE;

    rv = p11tk_findObjectForLabel(sessionHandle,
                                  CKO_SECRET_KEY,
                                  keyLabel,
                                  pKeyHandle);

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    if (*pKeyHandle != CK_INVALID_HANDLE)
    {
        fprintf(stderr,
                "p11tk_generateStorageKeyIfNeeded: Storage key labeled '%s' already exists with handle '%ld'.\n",
                keyLabel,
                *pKeyHandle);

        goto EXIT;
    }

    // Generate a secret key.
    rv = p11tk_generateStorageKey(sessionHandle,
                                  isTokenObject,
                                  keyLabel,
                                  pKeyHandle);

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    fprintf(stdout,
            "p11tk_generateStorageKeyIfNeeded:Storage key labeled '%s' generated with handle '%ld'.\n",
            keyLabel,
            *pKeyHandle);

EXIT:
    return rv;
}

CK_RV p11tk_getEllipticCurveDataForCurveIdentifier(const unsigned int ellipticCurveIdentifier,
                                                   const P11_ELLIPTIC_CURVES_LIST_ELEMENT **const pEllipticCurveData)
{
    if (ellipticCurveIdentifier < GET_ARRAY_SIZE(P11_ELLIPTIC_CURVES_LIST))
    {
        *pEllipticCurveData = &P11_ELLIPTIC_CURVES_LIST[ellipticCurveIdentifier];
    }
    else
    {
        *pEllipticCurveData = NULL;
    }

    return (*pEllipticCurveData == NULL ? CKR_GENERAL_ERROR : CKR_OK);
}

CK_RV p11tk_getEllipticCurveDataWithOid(const unsigned char *const oid,
                                        const size_t oidLength,
                                        const P11_ELLIPTIC_CURVES_LIST_ELEMENT **const pEllipticCurveData)
{
    *pEllipticCurveData = NULL;

    for (size_t keyIndex = 0;
         keyIndex < GET_ARRAY_SIZE(P11_ELLIPTIC_CURVES_LIST);
         keyIndex++)
    {
        if ((oidLength == P11_ELLIPTIC_CURVES_LIST[keyIndex].oidLength) &&
            (memcmp((void *)oid, (void *)P11_ELLIPTIC_CURVES_LIST[keyIndex].oid, oidLength) == 0))
        {
            *pEllipticCurveData = &P11_ELLIPTIC_CURVES_LIST[keyIndex];

            break;
        }
    }

    return (*pEllipticCurveData == NULL ? CKR_GENERAL_ERROR : CKR_OK);
}

CK_RV p11tk_getHaState(const GET_HA_STATE_ARGUMENTS *const pHaStateArguments)
{
    assert(pHaStateArguments != NULL);

    return CA_GetHAState(pHaStateArguments->slotId,
                         (CK_HA_STATE_PTR) & (pHaStateArguments->haState));
}

CK_RV p11tk_getObjectUniqueIdentifier(const CK_SESSION_HANDLE sessionHandle,
                                      const CK_SESSION_HANDLE objectHandle,
                                      CK_BYTE *const ouid,
                                      size_t *const pOuidLength)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(objectHandle != CK_INVALID_HANDLE);
    assert(ouid != NULL);
    assert(*pOuidLength >= P11TK_OUID_LENGTH);

    CK_RV rv = CKR_OK;

    CK_ATTRIBUTE objectAttribute = {CKA_OUID, ouid, *pOuidLength};

    rv = C_GetAttributeValue(sessionHandle,
                             objectHandle,
                             &objectAttribute,
                             (CK_ULONG)1);

    *pOuidLength = objectAttribute.usValueLen;

    return rv;
}

CK_RV p11tk_getSlotInfo(const CK_SLOT_ID slotId,
                        const CK_SLOT_INFO *const pSlotInfo)
{
    assert(pSlotInfo != NULL);

    return C_GetSlotInfo(slotId,
                         (CK_SLOT_INFO_PTR)pSlotInfo);
}

CK_RV p11tk_getSlotList(const CK_BBOOL isTokenPresent,
                        SLOT_LIST *const pSlotList,
                        CK_ULONG *const pSlotCount)
{
    assert(pSlotList != NULL);
    assert(*pSlotList == NULL);
    assert(pSlotCount != NULL);
    assert(*pSlotCount == (CK_ULONG)0);

    SLOT_ID_LIST slotIdList = NULL;

    // Get slot scenarioIdentifier list.
    CK_RV rv = CKR_OK;

    rv = C_GetSlotList(isTokenPresent,
                       NULL,
                       pSlotCount);

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    size_t slotIdListSize = sizeof(CK_SLOT_ID);

    slotIdList = (SLOT_ID_LIST)calloc(*pSlotCount,
                                      slotIdListSize);

    if (slotIdList == NULL)
    {
        rv = CKR_GENERAL_ERROR;

        goto EXIT;
    }

    CLEAR_BUFFER(slotIdList,
                 slotIdListSize);

    while (1)
    {
        rv = C_GetSlotList(isTokenPresent,
                           slotIdList,
                           pSlotCount);

        if (rv == CKR_OK)
        {
            break;
        }
        else if (rv == CKR_BUFFER_TOO_SMALL)
        {
            const SLOT_ID_LIST newSlotIdList = (SLOT_ID_LIST)realloc(slotIdList,
                                                                     *pSlotCount * sizeof(CK_SLOT_ID));

            if (newSlotIdList == NULL)
            {
                rv = CKR_GENERAL_ERROR;

                goto EXIT;
            }
            else
            {
                slotIdList = newSlotIdList;
            }
        }
        else
        {
            goto EXIT;
        }
    }

    // Build slot list.
    *pSlotList = (SLOT_LIST)calloc(*pSlotCount,
                                   slotIdListSize);

    if (*pSlotList == NULL)
    {
        rv = CKR_GENERAL_ERROR;
    }
    else
    {
        CLEAR_BUFFER(*pSlotList,
                     slotIdListSize);

        CK_SLOT_ID slotId = (CK_SLOT_ID)0;
        CK_ULONG i = 0L;

        for (;
             i < *pSlotCount;
             i++)
        {
            slotId = slotIdList[i];

            (*pSlotList)[i].slotId = slotId;

            rv = p11tk_getSlotInfo(slotId,
                                   &((*pSlotList)[i].slotInfo));

            if (rv != CKR_OK)
            {
                (*pSlotList)[i].slotStatus = rv;
            }
            else
            {
                rv = p11tk_getTokenInfo(slotId,
                                        &(*pSlotList)[i].tokenInfo);

                if (rv != CKR_OK)
                {
                    (*pSlotList)[i].tokenStatus = rv;
                }
            }
        }
    }

EXIT:
    if (slotIdList != NULL)
    {
        free(slotIdList);

        slotIdList = NULL;
    }

    return rv;
}

CK_RV p11tk_getTokenInfo(const CK_SLOT_ID slotId,
                         const CK_TOKEN_INFO *const pTokenInfo)
{
    assert(pTokenInfo != NULL);

    return C_GetTokenInfo(slotId,
                          (CK_TOKEN_INFO_PTR)pTokenInfo);
}

CK_RV p11tk_initializeClientLibrary(const CK_C_INITIALIZE_ARGS *const pInitializeArguments)
{
    return C_Initialize((CK_VOID_PTR)pInitializeArguments);
}

CK_RV p11tk_login(const CK_SESSION_HANDLE sessionHandle,
                  const CK_CHAR *const password,
                  const CK_ULONG passwordLength)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(password != NULL);
    assert(passwordLength > 0);

    return C_Login(sessionHandle,
                   CKU_USER,
                   (CK_BYTE_PTR)password,
                   passwordLength);
}

CK_RV p11tk_logout(const CK_SESSION_HANDLE sessionHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    return C_Logout(sessionHandle);
}

CK_RV p11tk_openSession(const CK_SLOT_ID slotId,
                        CK_SESSION_HANDLE *const pSessionHandle)
{
    assert(pSessionHandle != CK_INVALID_HANDLE);

    return C_OpenSession(slotId,
                         CKF_SERIAL_SESSION | CKF_RW_SESSION,
                         NULL,
                         NULL,
                         pSessionHandle);
}

CK_RV p11tk_prepare(const CK_C_INITIALIZE_ARGS *const pInitializeArguments,
                    const CK_SLOT_ID slotId,
                    const CK_CHAR *const password,
                    const CK_ULONG passwordLength,
                    CK_SESSION_HANDLE *const pSessionHandle)
{
    assert(password != NULL);
    assert(passwordLength > 0);
    assert(pSessionHandle != CK_INVALID_HANDLE);

    // Initialize the client library.
    CK_RV rv1 = CKR_OK;
    CK_RV rv2 = CKR_OK;

    rv1 = p11tk_initializeClientLibrary(pInitializeArguments);

    if (rv1 != CKR_OK)
    {
        goto EXIT;
    }

    // Open a session.
    rv1 = p11tk_openSession(slotId,
                            pSessionHandle);

    if (rv1 != CKR_OK)
    {
        goto EXIT;
    }

    // Login.
    rv1 = p11tk_login(*pSessionHandle,
                      password,
                      passwordLength);

    if (rv1 != CKR_OK)
    {
        rv2 = p11tk_closeSession(*pSessionHandle);

        if (rv2 != CKR_OK)
        {
            p11tk_writeError("Cannot close the PKCS#11 session.",
                             rv2);
        }

        rv2 = p11tk_finalizeClientLibrary();

        if (rv2 != CKR_OK)
        {
            p11tk_writeError("Cannot close the PKCS#11 library.",
                             rv2);
        }
    }

EXIT:
    return rv1;
}

void p11tk_printHaState(CK_HA_STATE_PTR const pHaState)
{
    assert(pHaState != NULL);

    printCurrentTime("");

    fprintf(stdout,
            "HA state for group %s:\n",
            pHaState->groupSerial);

    if (pHaState->listSize > 0)
    {
        fprintf(stdout,
                "  Pos. | Serial Number        | State\n");

        int i = 0;

        for (;
             i < (int)(pHaState->listSize);
             i++)
        {
            fprintf(stdout,
                    "  %4d | %*ld | 0x%lx\n",
                    (i + 1),
                    (int)GET_STRUCT_FIELD_SIZE(CK_HA_MEMBER,
                                               memberSerial),
                    (unsigned long)atol((const char *)pHaState->memberList[i].memberSerial),
                    pHaState->memberList[i].memberStatus);
        }
    }
    else
    {
        fprintf(stdout,
                "  No registered member.\n");
    }
}

/*
 * This function looks for a virtual slot in a slot list and asks for it HA state.
 */
CK_RV p11tk_printHaStateWithSlotList()
{
    CK_RV rv = CKR_OK;
    SLOT_LIST slotList = NULL;
    CK_ULONG slotCount = (CK_ULONG)0;

    // Get the list of the currently known slots.
    rv = p11tk_getSlotList(CK_TRUE,
                           &slotList,
                           &slotCount);

    if (rv == CKR_OK)
    {
        // Print the state of the currently known slots.
        fprintf(stdout,
                "\n");
        p11tk_printSlotListStateWithSlotList(slotList,
                                             slotCount);

        GET_HA_STATE_ARGUMENTS getHaStateArguments;
        CK_ULONG slotIndex = 0L;

        for (;
             slotIndex < slotCount;
             slotIndex++)
        {
            if (strncmp((const char *)slotList[slotIndex].tokenInfo.model,
                        LUNA_MODEL_VIRTUAL,
                        strlen(LUNA_MODEL_VIRTUAL)) == 0)
            {
                getHaStateArguments.slotId = slotList[slotIndex].slotId;

                rv = p11tk_getHaState(&getHaStateArguments);

                if (rv != CKR_OK)
                {
                    goto EXIT;
                }

                fprintf(stdout,
                        "\n");
                p11tk_printHaState(&(getHaStateArguments.haState));
            }
        }
    }

EXIT:

    if (slotList != NULL)
    {
        free(slotList);

        slotCount = 0;
    }

    return rv;
}

/*
 * This function looks for a virtual slot in a slot range and asks for it HA state.
 */
CK_RV p11tk_printHaStateWithoutSlotList()
{
    CK_RV rv = CKR_OK;
    GET_HA_STATE_ARGUMENTS getHaStateArguments;

    getHaStateArguments.slotId = (CK_SLOT_ID)0;

    for (;
         getHaStateArguments.slotId < MAXIMUM_SLOT_COUNT;
         getHaStateArguments.slotId++)
    {
        rv = p11tk_getHaState(&getHaStateArguments);

        switch (rv)
        {
        case CKR_OK:
            fprintf(stdout,
                    "\n");
            p11tk_printHaState(&(getHaStateArguments.haState));

            break;
        case CKR_SLOT_ID_INVALID:
            // This slot is not used: ignore it.

            break;
        case CKR_FUNCTION_NOT_SUPPORTED:
            // This slot is not a virtual one: ignore it.

            break;

        case CKR_TOKEN_NOT_PRESENT:
            // This slot has no token: ignore it.

            break;
        default:
            p11tk_writeError("Unexpected error when updating the HA state without slot list.",
                             rv);

            break;
        }
    }

    return CKR_OK;
}

void p11tk_printSlotList(const SLOT_LIST slotList,
                         const CK_ULONG slotCount)
{
    assert(slotList != NULL);

    // Print slot list.
    printCurrentTime("");

    fprintf(stdout,
            "Slot list (%ld entries):\n",
            slotCount);

    if (slotCount > 0)
    {
        CK_RV rv = CKR_OK;
        CK_SLOT_ID slotId = (CK_SLOT_ID)0;
        CK_SLOT_INFO_PTR pSlotInfo;
        CK_TOKEN_INFO_PTR pTokenInfo;
        CK_ULONG i = 0L;

        for (;
             i < slotCount;
             i++)
        {
            slotId = slotList[i].slotId;

            fprintf(stdout,
                    "  Slot %ld\n",
                    slotId);

            rv = slotList[i].slotStatus;

            if (rv != CKR_OK)
            {
                fprintf(stdout,
                        "  Slot information unavailable: 0x%lx.\n",
                        rv);
            }
            else
            {
                pSlotInfo = &slotList[i].slotInfo;

                fprintf(stdout,
                        "    Description        : %.*s\n",
                        (int)GET_STRUCT_FIELD_SIZE(CK_SLOT_INFO, slotDescription),
                        pSlotInfo->slotDescription);
                fprintf(stdout,
                        "    Manufacturer       : %.*s\n",
                        (int)GET_STRUCT_FIELD_SIZE(CK_SLOT_INFO, manufacturerID),
                        pSlotInfo->manufacturerID);
                fprintf(stdout,
                        "    Is present         : %s\n",
                        getBooleanString(pSlotInfo->flags & CKF_TOKEN_PRESENT));
                fprintf(stdout,
                        "    Is removable device: %s\n",
                        getBooleanString(pSlotInfo->flags & CKF_REMOVABLE_DEVICE));
                fprintf(stdout,
                        "    Is hardware slot   : %s\n",
                        getBooleanString(pSlotInfo->flags & CKF_HW_SLOT));
                fprintf(stdout,
                        "    Hardware version   : %lu.%lu\n",
                        (unsigned long)pSlotInfo->hardwareVersion.major,
                        (unsigned long)pSlotInfo->hardwareVersion.minor);

                if ((unsigned long)pSlotInfo->firmwareVersion.major < 4)
                {
                    fprintf(stdout,
                            "    Firmware version   : %lu.%lu\n",
                            (unsigned long)pSlotInfo->firmwareVersion.major,
                            (unsigned long)pSlotInfo->firmwareVersion.minor);
                }
                else
                {
                    CK_ULONG firmwareMajorVersion;
                    CK_ULONG firmwareMinorVersion;
                    CK_ULONG firmwareSubMinorVersion;

                    rv = CA_GetFirmwareVersion(slotId,
                                               &firmwareMajorVersion,
                                               &firmwareMinorVersion,
                                               &firmwareSubMinorVersion);

                    if (rv == CKR_OK)
                    {
                        fprintf(stdout,
                                "    Firmware version   : %lu.%lu.%lu \n",
                                (unsigned long)firmwareMajorVersion,
                                (unsigned long)firmwareMinorVersion,
                                (unsigned long)firmwareSubMinorVersion);
                    }
                    else
                    {
                        fprintf(stdout,
                                "    Firmware version   : unavailable (0x%lx)\n",
                                rv);
                    }
                }

                fprintf(stdout,
                        "\n");
                fprintf(stdout,
                        "    Token:\n");

                rv = slotList[i].tokenStatus;

                if (rv != CKR_OK)
                {
                    fprintf(stdout,
                            "      Token information unavailable: 0x%lx.\n",
                            rv);
                }
                else
                {
                    pTokenInfo = &slotList[i].tokenInfo;

                    fprintf(stdout,
                            "      Label                                    : %.*s\n",
                            (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, label),
                            pTokenInfo->label);
                    fprintf(stdout,
                            "      Manufactured Id.                         : %.*s\n",
                            (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, manufacturerID),
                            pTokenInfo->manufacturerID);
                    fprintf(stdout,
                            "      Model                                    : %.*s\n",
                            (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, model),
                            pTokenInfo->model);
                    fprintf(stdout,
                            "      Serial number                            : %.*s\n",
                            (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, serialNumber),
                            pTokenInfo->serialNumber);
                    fprintf(stdout,
                            "      Hardware version                         : %lu.%lu\n",
                            (unsigned long)pTokenInfo->hardwareVersion.major,
                            (unsigned long)pTokenInfo->hardwareVersion.minor);
                    fprintf(stdout,
                            "      Firmware version                         : %lu.%lu\n",
                            (unsigned long)pTokenInfo->firmwareVersion.major,
                            (unsigned long)pTokenInfo->firmwareVersion.minor);
                    fprintf(stdout,
                            "      UTC time                                 : %.*s\n",
                            (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, utcTime),
                            pTokenInfo->utcTime);
                    fprintf(stdout,
                            "      Has a random generator                   : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_RNG));
                    fprintf(stdout,
                            "      Is write protected                       : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_WRITE_PROTECTED));
                    fprintf(stdout,
                            "      Is login required                        : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_LOGIN_REQUIRED));
                    fprintf(stdout,
                            "      Is user PIN initialized                  : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_USER_PIN_INITIALIZED));
                    fprintf(stdout,
                            "      Is key not needed for restore            : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_RESTORE_KEY_NOT_NEEDED));
                    fprintf(stdout,
                            "      Has an hardware clock                    : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_CLOCK_ON_TOKEN));
                    fprintf(stdout,
                            "      Has a protected authentication path      : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_PROTECTED_AUTHENTICATION_PATH));
                    fprintf(stdout,
                            "      Can perform dual cryptographic operations: %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_DUAL_CRYPTO_OPERATIONS));
                    fprintf(stdout,
                            "      Is initialized                           : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_TOKEN_INITIALIZED));
                    fprintf(stdout,
                            "      Support secondary authentication         : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_SECONDARY_AUTHENTICATION));
                    fprintf(stdout,
                            "      Has a user PIN count low                 : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_USER_PIN_COUNT_LOW));
                    fprintf(stdout,
                            "      Has a user PIN final try                 : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_USER_PIN_FINAL_TRY));
                    fprintf(stdout,
                            "      Is user PIN locked                       : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_USER_PIN_LOCKED));
                    fprintf(stdout,
                            "      User PIN must be changed                 : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_USER_PIN_TO_BE_CHANGED));
                    fprintf(stdout,
                            "      Has an incorrect SO PIN been entered     : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_SO_PIN_COUNT_LOW));
                    fprintf(stdout,
                            "      Is final try for SO PIN                  : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_SO_PIN_FINAL_TRY));
                    fprintf(stdout,
                            "      Is SO PIN locked                         : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_SO_PIN_LOCKED));
                    fprintf(stdout,
                            "      SO PIN must be changed                   : %s\n",
                            getBooleanString(pTokenInfo->flags & CKF_SO_PIN_TO_BE_CHANGED));
                    fprintf(stdout,
                            "      Maximum session count                    : %ld\n",
                            pTokenInfo->usMaxSessionCount);
                    fprintf(stdout,
                            "      Session count                            : %ld\n",
                            pTokenInfo->usSessionCount);
                    fprintf(stdout,
                            "      Read/Write maximum session count         : %ld\n",
                            pTokenInfo->usMaxRwSessionCount);
                    fprintf(stdout,
                            "      Maximum PIN length                       : %ld\n",
                            pTokenInfo->usMaxPinLen);
                    fprintf(stdout,
                            "      Minimum PIN length                       : %ld\n",
                            pTokenInfo->usMinPinLen);
                    fprintf(stdout,
                            "      Total public memory                      : %ld\n",
                            pTokenInfo->ulTotalPublicMemory);
                    fprintf(stdout,
                            "      Free public memory                       : %ld\n",
                            pTokenInfo->ulFreePublicMemory);
                    fprintf(stdout,
                            "      Total private memory                     : %ld\n",
                            pTokenInfo->ulTotalPrivateMemory);
                    fprintf(stdout,
                            "      Free private memory                      : %ld\n",
                            pTokenInfo->ulFreePrivateMemory);
                }
            }
        }
    }
    else
    {
        fprintf(stdout,
                "  No registered HSM.\n");
    }
}

CK_RV p11tk_printSlotListStateWithoutSlotList()
{
    CK_RV rv = CKR_OK;
    SLOT_LIST slotList = NULL;
    CK_ULONG slotCount = (CK_ULONG)0;

    // Get the list of the currently known slots.
    rv = p11tk_getSlotList(CK_TRUE,
                           &slotList,
                           &slotCount);

    if (rv != CKR_OK)
    {
        goto EXIT;
    }

    // Print the state of the currently known slots.
    fprintf(stdout,
            "\n");

    p11tk_printSlotListStateWithSlotList(slotList,
                                         slotCount);

EXIT:
    return rv;
}

void p11tk_printSlotListStateWithSlotList(const SLOT_LIST slotList,
                                          const CK_ULONG slotCount)
{
    assert(slotList != NULL);

    printCurrentTime("");

    fprintf(stdout,
            "Slots state:\n");

    if (slotCount > 0)
    {
        fprintf(stdout,
                "  Slot | Serial Number    | State\n");

        CK_ULONG i = (CK_ULONG)0;

        for (;
             i < slotCount;
             i++)
        {
            CK_RV state = slotList[i].slotStatus;

            if (state == CKR_OK)
            {
                state = slotList[i].tokenStatus;
            }

            fprintf(stdout,
                    "  %4ld | %*ld | 0x%lx\n",
                    slotList[i].slotId,
                    (int)GET_STRUCT_FIELD_SIZE(CK_TOKEN_INFO, serialNumber),
                    (unsigned long)atol((const char *)slotList[i].tokenInfo.serialNumber),
                    state);
        }
    }
    else
    {
        fprintf(stdout,
                "  No slot available.\n");
    }
}

CK_RV p11tk_terminate(const CK_SESSION_HANDLE sessionHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);

    // Logout.
    CK_RV rv = CKR_OK;

    rv = p11tk_logout(sessionHandle);

    if ((rv != CKR_OK) &&
        (rv != CKR_USER_NOT_AUTHORIZED)) // TODO: need for clarification.
    {
        return rv;
    }

    // Close session.
    rv = p11tk_closeSession(sessionHandle);

    if (rv != CKR_OK)
    {
        return rv;
    }

    // Close client library.
    return p11tk_finalizeClientLibrary();
}

CK_RV p11tk_unwrapSensitiveData(const CK_SESSION_HANDLE sessionHandle,
                                const CK_OBJECT_HANDLE encryptionKeyHandle,
                                const CK_BYTE *const encryptedKey,
                                const CK_ULONG encryptedKeyLength,
                                const CK_BBOOL unwrappedKeyIsTokenObject,
                                const CK_ULONG unwrappedKeyLength,
                                const CK_CHAR *const unwrappedKeyLabel,
                                CK_OBJECT_HANDLE *const pUnwrappedKeyHandle)
{
    assert(sessionHandle != CK_INVALID_HANDLE);
    assert(encryptionKeyHandle != CK_INVALID_HANDLE);
    assert(encryptedKey != NULL);
    assert(encryptedKeyLength > 0);
    assert(unwrappedKeyLength > 0);
    assert(unwrappedKeyLabel != NULL);
    assert(pUnwrappedKeyHandle != NULL);

    CK_RV rv = CKR_OK;

    CK_MECHANISM mechanism = {0};

    mechanism.mechanism = CKM_AES_KWP;
    mechanism.pParameter = NULL;
    mechanism.ulParameterLen = (CK_ULONG)0;

    CK_OBJECT_CLASS unwrappedKeyClass = CKO_SECRET_KEY;
    CK_KEY_TYPE unwrappedKeyType = CKK_GENERIC_SECRET;
    CK_ULONG _unwrappedKeyLength = unwrappedKeyLength;
    CK_BBOOL _unwrappedKeyIsTokenObject = unwrappedKeyIsTokenObject;
    CK_ATTRIBUTE objectTemplate[] = {{CKA_CLASS, &unwrappedKeyClass, sizeof(unwrappedKeyClass)},
                                     {CKA_KEY_TYPE, &unwrappedKeyType, sizeof(unwrappedKeyType)},
                                     {CKA_VALUE_LEN, &_unwrappedKeyLength, sizeof(_unwrappedKeyLength)},

                                     {CKA_TOKEN, &_unwrappedKeyIsTokenObject, sizeof(_unwrappedKeyIsTokenObject)},
                                     {CKA_PRIVATE, &ckTrue, sizeof(ckTrue)},
                                     {CKA_MODIFIABLE, &ckFalse, sizeof(ckFalse)},
                                     {CKA_LABEL, (CK_CHAR_PTR)unwrappedKeyLabel, (CK_ULONG)strlen((char *)unwrappedKeyLabel)},

                                     {CKA_SENSITIVE, &ckTrue, sizeof(ckTrue)},
                                     {CKA_ENCRYPT, &ckFalse, sizeof(ckFalse)},
                                     {CKA_DECRYPT, &ckFalse, sizeof(ckFalse)},
                                     {CKA_DERIVE, &ckFalse, sizeof(ckFalse)},
                                     {CKA_SIGN, &ckFalse, sizeof(ckFalse)},
                                     {CKA_VERIFY, &ckFalse, sizeof(ckFalse)},
                                     {CKA_WRAP, &ckFalse, sizeof(ckFalse)},
                                     {CKA_UNWRAP, &ckFalse, sizeof(ckFalse)},
                                     {CKA_EXTRACTABLE, &ckFalse, sizeof(ckFalse)}};

    rv = C_UnwrapKey(sessionHandle,
                     &mechanism,
                     encryptionKeyHandle,
                     (CK_BYTE_PTR)encryptedKey,
                     encryptedKeyLength,
                     objectTemplate,
                     GET_ARRAY_SIZE(objectTemplate),
                     (CK_OBJECT_HANDLE_PTR)pUnwrappedKeyHandle);

    if (rv != CKR_OK)
    {
        fprintf(stderr,
                "p11tk_unwrapSensitiveData()/C_UnwrapKey() failed with error '0x%08lx'.\n",
                rv);
    }

    return rv;
}

void p11tk_writeError(const char *const message,
                      const CK_RV rv)
{
    fprintf(stderr,
            "%s ['0x%08lx']\n",
            message,
            rv);
}