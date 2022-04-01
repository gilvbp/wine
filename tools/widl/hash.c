/*
 * Oleaut32 hash functions
 *
 * Copyright 1999 Corel Corporation
 * Copyright 2001-2003 Jon Griffiths
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#include <stdio.h>
#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winnls.h"

#include "widltypes.h"
#include "hash.h"

static const unsigned char Lookup_16[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x8C,
 0x7F, 0x7F, 0x7F, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x7F, 0x7F, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x55, 0xDE, 0x55,

 /* Mac */
 0x41, 0x41, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0x41, 0x4F, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0x41, 0x4F, 0xBD, 0x41, 0x4F, 0xC0,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0x41, 0x41, 0x4F,
 0xCE, 0xCE, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x55, 0x55, 0xDA,
 0xDB, 0xDC, 0xDD, 0x3F, 0x3F, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0x41, 0x45, 0x41,
 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x3F, 0x4F, 0x55, 0x55, 0x55,
 0x49, 0x7F, 0xF7, 0x7F, 0xF9, 0xFA, 0xFB, 0x3F, 0xFD, 0xFE, 0x7F
};

static const unsigned char Lookup_32[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x7F, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x53,
 0x54, 0x5A, 0x5A, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x7F, 0x99,
 0x53, 0x9B, 0x53, 0x54, 0x5A, 0x5A, 0xA0, 0x7F, 0xA2, 0x4C, 0xA4, 0x41, 0xA6,
 0xA7, 0xA8, 0xA9, 0x53, 0xAB, 0xAC, 0x96, 0xAE, 0x5A, 0xB0, 0xB1, 0xB2, 0x4C,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x41, 0x53, 0xBB, 0x4C, 0xBD, 0x4C, 0x5A, 0x52,
 0x41, 0x41, 0x41, 0x41, 0x4C, 0x43, 0x43, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49,
 0x49, 0x44, 0xD0, 0x4E, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x52, 0x55, 0x55,
 0x55, 0x55, 0x59, 0x54, 0xDF, 0x52, 0x41, 0x41, 0x41, 0x41, 0x4C, 0x43, 0x43,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x44, 0xD0, 0x4E, 0x4E, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x52, 0x55, 0x55, 0x55, 0x55, 0x59, 0x54, 0xFF,

 /* Mac */
 0x41, 0x41, 0x41, 0x45, 0x41, 0x4F, 0x55, 0x41, 0x41, 0x43, 0x41, 0x43, 0x43,
 0x43, 0x45, 0x5A, 0x5A, 0x44, 0x49, 0x44, 0x45, 0x45, 0x45, 0x4F, 0x45, 0x4F,
 0x4F, 0x4F, 0x55, 0x45, 0x45, 0x55, 0xA0, 0xA1, 0x45, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0x45, 0xAC, 0xAD, 0x47, 0x49, 0x49, 0x49, 0xB2, 0xB3,
 0x49, 0x4B, 0xB6, 0xB7, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4E, 0x4E,
 0x4E, 0xC2, 0xC3, 0x4E, 0x4E, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0x4E, 0x4F, 0x4F,
 0x4F, 0x4F, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x4F, 0x52, 0x52,
 0x52, 0xDC, 0xDD, 0x52, 0x52, 0x52, 0x53, 0xE2, 0xE3, 0x53, 0x53, 0x53, 0x41,
 0x54, 0x54, 0x49, 0x5A, 0x5A, 0x55, 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x55,
 0x55, 0x55, 0x55, 0x59, 0x59, 0x4B, 0x5A, 0x4C, 0x4C, 0x47, 0xFF
};

static const unsigned char Lookup_48[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

 /* Windows */
 0x7F, 0x7F, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x7F, 0x89, 0x53, 0x8B, 0x8C,
 0x7F, 0x7F, 0x7F, 0x7F, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x96, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x7F, 0x7F, 0x59, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x59, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0x55, 0x59, 0xDE, 0x59,

 /* Mac */
 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAB, 0xAD, 0xAE, 0xAE, 0xB0, 0xB1, 0xB2, 0xB3,
 0xA7, 0xB5, 0xB6, 0xB7, 0xB8, 0xB8, 0xBA, 0xBA, 0xBC, 0xBC, 0xBE, 0xBE, 0xB7,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCB, 0xCD,
 0xCD, 0xC1, 0xD0, 0xD0, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD8, 0xDA,
 0xDA, 0xDC, 0xDD, 0xDD, 0x9F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F
};

static const unsigned char Lookup_64[128 * 3] = {
 /* Common */
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

 /* Mac */
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x96, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0x55, 0x55, 0xDE, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char Lookup_80[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,

 /* Mac */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,
};

static const unsigned char Lookup_112[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x00, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x56, 0x58, 0x55, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,

 /* Mac */
 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55,
};

static const unsigned char Lookup_128[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x00, 0x8B, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x00, 0x99,
 0x00, 0x9B, 0x00, 0x00, 0x00, 0x00, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x00, 0xAB, 0xAC, 0x2D, 0xAE, 0x2D, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xBA,
 0xA2, 0xC2, 0xC3, 0xC4, 0xB8, 0xC6, 0xB9, 0xC8, 0xBA, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xBC, 0xD0, 0xD1, 0x00, 0xD3, 0xD4, 0xBE, 0xD6, 0xD7, 0xD8, 0xBF, 0xBA,
 0xBE, 0xA2, 0xB8, 0xB9, 0xBA, 0xBE, 0xA2, 0xC2, 0xC3, 0xC4, 0xB8, 0xC6, 0xB9,
 0xC8, 0xBA, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xBC, 0xD0, 0xD1, 0xD3, 0xD3, 0xD4,
 0xBE, 0xD6, 0xD7, 0xD8, 0xBF, 0xBA, 0xBE, 0xBC, 0xBE, 0xBF, 0x00,

 /* Mac */
 0x41, 0x31, 0x32, 0x45, 0x33, 0x4F, 0x55, 0x87, 0x41, 0x41, 0x41, 0x00, 0x8C,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x92, 0x93, 0x49, 0x49, 0x96, 0x97, 0x98, 0x4F,
 0x4F, 0x9B, 0x3F, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xAB, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xB0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0x09, 0xBD, 0xCC, 0xB0,
 0xB6, 0xCF, 0x2D, 0x2D, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xB8, 0xAB, 0xC3, 0xBD,
 0xB6, 0xB8, 0xAB, 0xC3, 0xBF, 0xBD, 0xB0, 0xB5, 0xBE, 0xA2, 0xB6, 0xBC, 0xA1,
 0xB8, 0xAB, 0xA5, 0xBA, 0xA4, 0xBB, 0xC1, 0xC3, 0xA6, 0xBF, 0xC4, 0xAA, 0xC6,
 0xA3, 0xBF, 0xAA, 0xCC, 0xBD, 0xB7, 0xAB, 0xBD, 0xAB, 0xBD, 0x3F,
};

static const unsigned char Lookup_144[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0xDA,
 0x55, 0x55, 0xDD, 0xDE, 0xDF, 0x41, 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0xDA, 0x55, 0x55, 0xDD, 0xDE, 0x59,

 /* Mac */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43, 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0xDA,
 0x55, 0x55, 0xDD, 0xDE, 0xDF, 0x41, 0xC1, 0x41, 0x41, 0x41, 0x41, 0xC6, 0x43,
 0x45, 0xC9, 0x45, 0x45, 0x49, 0xCD, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0xD3, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0xDA, 0x55, 0x55, 0xDD, 0xDE, 0x59,
};

static const unsigned char Lookup_160[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

/* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0xDC, 0xDD, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0xDC, 0xDD, 0xDE, 0x59,

 /* Mac */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0x4F, 0x55, 0x55,
 0x55, 0xDC, 0xDD, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0xC6, 0xC7,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD0, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0x4F, 0x55, 0x55, 0x55, 0xDC, 0xDD, 0xDE, 0x59,
};

static const unsigned char Lookup_176[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x00,

 /* Windows */
 0x00, 0x00, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x00, 0x89, 0x53, 0x8B, 0x8C,
 0x00, 0x00, 0x00, 0x00, 0x91, 0x92, 0x93, 0x94, 0x95, 0x2D, 0x2D, 0x98, 0x99,
 0x53, 0x9B, 0x8C, 0x00, 0x00, 0x59, 0x09, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0x41, 0xAB, 0xAC, 0x2D, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0x4F, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0x41,
 0x41, 0x41, 0x41, 0xC4, 0xC5, 0xC4, 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49,
 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0xD6, 0xD7, 0xD6, 0x55, 0x55,
 0x55, 0x59, 0x59, 0xDE, 0xDF, 0x41, 0x41, 0x41, 0x41, 0xC4, 0xC5, 0xC4, 0x43,
 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x44, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0xD6, 0xF7, 0xD6, 0x55, 0x55, 0x55, 0x59, 0x59, 0xDE, 0x59,

 /* Mac */
 0x80, 0x81, 0x43, 0x45, 0x4E, 0x85, 0x59, 0x41, 0x41, 0x41, 0x80, 0x41, 0x81,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x85, 0x4F, 0x55, 0x55, 0x55, 0x59, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0x80, 0x85, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0x41, 0x4F, 0xBD, 0x80, 0x85, 0xC0,
 0xC1, 0xC2, 0xC3, 0x46, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0x09, 0x41, 0x41, 0x4F,
 0xCE, 0xCE, 0x2D, 0x2D, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0x59, 0x59, 0xDA,
 0xDB, 0xDC, 0xDD, 0x3F, 0x3F, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0x41, 0x45, 0x41,
 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4F, 0x4F, 0x3F, 0x4F, 0x55, 0x55, 0x55,
 0x49, 0x00, 0xF7, 0x00, 0xF9, 0xFA, 0xFB, 0x3F, 0xFD, 0xFE, 0x00
};

static const unsigned char Lookup_208[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x80, 0x81, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x5E, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x8C, 0x9D, 0x00, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC1, 0xC1, 0xC1, 0xC1, 0xC7, 0xC8, 0xC9, 0xC9, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0x00, 0xDD, 0xDE, 0xDF, 0x41, 0xE1, 0x41, 0xE3, 0xE4, 0xE5, 0xE6, 0x43,
 0x45, 0x45, 0x45, 0x45, 0xEC, 0xEC, 0x49, 0x49, 0xF0, 0xF1, 0xF2, 0xF3, 0x4F,
 0xF5, 0xF6, 0xF7, 0xF8, 0x55, 0xFA, 0x55, 0x55, 0x00, 0x00, 0xFF,

 /* Mac */
 0x41, 0x81, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x8B, 0x8C,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x93, 0x49, 0x49, 0x4E, 0x4F, 0x98, 0x4F,
 0x4F, 0x9B, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC1, 0xC1, 0xC1, 0xC1, 0xC7, 0xC8, 0xC9, 0xC9, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0x00, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xE9, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4,
 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

static const unsigned char Lookup_224[128 * 3] = {
 /* Common */
 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26,
 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33,
 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40,
 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D,
 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A,
 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54,
 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,

/* Windows */
 0x80, 0x81, 0x82, 0x46, 0x84, 0x85, 0x86, 0x87, 0x5E, 0x89, 0x8A, 0x8B, 0x8C,
 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99,
 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0x32, 0x33,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0x31, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xEA, 0xEA, 0xEC, 0xED, 0xED, 0xEF, 0xEF, 0xF1, 0xF2, 0xF3, 0xF3,
 0xF5, 0xF5, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0x00, 0x00, 0xFF,

 /* Mac */
 0x41, 0x41, 0x43, 0x45, 0x4E, 0x4F, 0x55, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
 0x43, 0x45, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0x4E, 0x4F, 0x4F, 0x4F,
 0x4F, 0x4F, 0x55, 0x55, 0x55, 0x55, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3,
 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0,
 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD,
 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
 0xE8, 0xE9, 0xEA, 0xEA, 0xEC, 0xED, 0xED, 0xEF, 0xEF, 0xF1, 0xF2, 0xF3, 0xF3,
 0xF5, 0xF5, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/***********************************************************************
 *		lhash_val_of_name_sys
 *
 * Copy of oleaut32.LHashValOfNameSysA
 * Produce a string hash value.
 *
 * PARAMS
 *  skind [I] Type of the system.
 *  lcid  [I] Locale id for the hash.
 *  lpStr [I] String to hash.
 *
 * RETURNS
 *  Success: The hash value of the string.
 *  Failure: 0, if lpStr is NULL.
 *
 * NOTES
 *  This function produces a two part hash: The high word is based on
 *  skind and lcid, while the low word is based on a repeated string
 *  hash of skind/str.
 */
unsigned int lhash_val_of_name_sys( syskind_t skind, int lcid, const char *lpStr)
{
  unsigned int nOffset, nMask = skind == SYS_MAC ? 1 : 0;
  unsigned int nHiWord, nLoWord = 0x0deadbee;
  const unsigned char *str = (const unsigned char *)lpStr, *pnLookup = NULL;

  if (!str)
    return 0;

  switch (PRIMARYLANGID(LANGIDFROMLCID(lcid)))
  {
  default:
    nOffset = 16;
    pnLookup = Lookup_16;
    break;
  case LANG_CZECH:  case LANG_HUNGARIAN:  case LANG_POLISH:
  case LANG_SLOVAK: case LANG_SPANISH:
    nOffset = 32;
    pnLookup = Lookup_32;
    break;
  case LANG_HEBREW:
    nOffset = 48;
    pnLookup = Lookup_48;
    break;
  case LANG_JAPANESE:
    nOffset = 64;
    pnLookup = Lookup_64;
    break;
  case LANG_KOREAN:
    nOffset = 80;
    pnLookup = Lookup_80;
    break;
  case LANG_CHINESE:
    nOffset = 112;
    pnLookup = Lookup_112;
    break;
  case LANG_GREEK:
    nOffset = 128;
    pnLookup = Lookup_128;
    break;
  case LANG_ICELANDIC:
    nOffset = 144;
    pnLookup = Lookup_144;
    break;
  case LANG_TURKISH:
    nOffset = 160;
    pnLookup = Lookup_160;
    break;
  case LANG_NORWEGIAN:
    if (SUBLANGID(LANGIDFROMLCID(lcid)) == SUBLANG_NORWEGIAN_NYNORSK)
    {
      nOffset = 176;
      pnLookup = Lookup_176;
    }
    else
    {
      nOffset = 16;
      pnLookup = Lookup_16;
    }
    break;
  case LANG_ARABIC:
  case LANG_FARSI:
    nOffset = 208;
    pnLookup = Lookup_208;
    break;
  case LANG_RUSSIAN:
    nOffset = 224;
    pnLookup = Lookup_224;
    break;
  }

  nHiWord = (nOffset | nMask) << 16;

  while (*str)
  {
    nLoWord = 37 * nLoWord + pnLookup[*str > 0x7f && nMask ? *str + 0x80 : *str];
    str++;
  }
  /* Constrain to a prime modulo and sizeof(WORD) */
  nLoWord = (nLoWord % 65599) & 0xffff;

  return nHiWord | nLoWord;
}

/* SHA1 algorithm
 *
 * Based on public domain SHA code by Steve Reid <steve@edmweb.com>
 * Copied and adapted from ntdll.A_SHAInit / ntdll.A_SHAUpdate / ntdll.A_SHAFinal
 */

#ifdef WORDS_BIGENDIAN
#define DWORD2BE(x) (x)
#else
#define DWORD2BE(x) ((((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))
#endif

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
#define blk0(i) (block[i] = DWORD2BE(block[i]))
#define blk1(i) (block[i&15] = rol(block[(i+13)&15]^block[(i+8)&15]^block[(i+2)&15]^block[i&15],1))
#define f1(x,y,z) (z^(x&(y^z)))
#define f2(x,y,z) (x^y^z)
#define f3(x,y,z) ((x&y)|(z&(x|y)))
#define f4(x,y,z) (x^y^z)
/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=f1(w,x,y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=f1(w,x,y)+blk1(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=f2(w,x,y)+blk1(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=f3(w,x,y)+blk1(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=f4(w,x,y)+blk1(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void sha1_transform(struct sha1_context *ctx)
{
   unsigned int a, b, c, d, e, *block = (unsigned int *)ctx->buffer;

   /* Copy ctx->state[] to working variables */
   a = ctx->state[0];
   b = ctx->state[1];
   c = ctx->state[2];
   d = ctx->state[3];
   e = ctx->state[4];

   /* 4 rounds of 20 operations each. Loop unrolled. */
   R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
   R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
   R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
   R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
   R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
   R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
   R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
   R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
   R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
   R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
   R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
   R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
   R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
   R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
   R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
   R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
   R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
   R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
   R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
   R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

   /* Add the working variables back into ctx->state[] */
   ctx->state[0] += a;
   ctx->state[1] += b;
   ctx->state[2] += c;
   ctx->state[3] += d;
   ctx->state[4] += e;

   /* Wipe variables */
   a = b = c = d = e = 0;
}

void sha1_init(struct sha1_context *ctx)
{
   /* SHA1 initialization constants */
   ctx->state[0] = 0x67452301;
   ctx->state[1] = 0xEFCDAB89;
   ctx->state[2] = 0x98BADCFE;
   ctx->state[3] = 0x10325476;
   ctx->state[4] = 0xC3D2E1F0;
   ctx->count[0] = 0;
   ctx->count[1] = 0;
}

void sha1_update(struct sha1_context *ctx, const char *data, size_t data_size)
{
   size_t buffer_size;

   buffer_size = ctx->count[1] & 63;
   ctx->count[1] += data_size;
   if (ctx->count[1] < data_size) ctx->count[0]++;
   ctx->count[0] += (data_size >> 29);

   if (buffer_size + data_size < 64)
      memcpy(&ctx->buffer[buffer_size], data, data_size);
   else
   {
      while (buffer_size + data_size >= 64)
      {
         memcpy(ctx->buffer + buffer_size, data, 64 - buffer_size);
         data += 64 - buffer_size;
         data_size -= 64 - buffer_size;
         sha1_transform(ctx);
         buffer_size = 0;
      }
      memcpy(ctx->buffer + buffer_size, data, data_size);
   }
}

void sha1_finalize(struct sha1_context *ctx, unsigned int result[5])
{
   unsigned int *count, length_hi, length_lo, i;
   size_t pad_size, buffer_size;
   char pad[72];

   buffer_size = ctx->count[1] & 63;
   if (buffer_size >= 56) pad_size = 56 + 64 - buffer_size;
   else pad_size = 56 - buffer_size;

   length_hi = (ctx->count[0] << 3) | (ctx->count[1] >> (32 - 3));
   length_lo = (ctx->count[1] << 3);

   memset(pad + 1, 0, pad_size - 1);
   pad[0] = 0x80;
   count = (unsigned int*)(pad + pad_size);
   count[0] = DWORD2BE(length_hi);
   count[1] = DWORD2BE(length_lo);
   sha1_update(ctx, pad, pad_size + 8);

   for (i = 0; i < 5; i++) result[i] = DWORD2BE(ctx->state[i]);

   sha1_init(ctx);
}
