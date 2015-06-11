// Copyright 2004 "Gilles Degottex"

// This file is part of "fmit"

// "fmit" is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// "fmit" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "MicrotonalView.h"

static const unsigned char g_icon_ji[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x05,
    0xc3, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9c, 0xb5, 0x97, 0x4b, 0x6c, 0x24,
    0xc5, 0x19, 0xc7, 0x7f, 0x55, 0xfd, 0x98, 0x5d, 0x7b, 0x1e, 0x1e, 0xdb,
    0x0c, 0xb3, 0xbb, 0xf6, 0x78, 0xed, 0xf1, 0xda, 0xe0, 0x85, 0x95, 0x80,
    0x43, 0x88, 0x90, 0x15, 0xb8, 0x70, 0x88, 0xc0, 0x42, 0xe2, 0x86, 0xc4,
    0x85, 0xa0, 0x58, 0x4a, 0x4e, 0x44, 0x8a, 0x84, 0x72, 0x8a, 0xb4, 0x70,
    0x08, 0x91, 0x12, 0x5e, 0x97, 0x91, 0x78, 0x48, 0x51, 0x24, 0x94, 0x70,
    0xca, 0x04, 0x89, 0x03, 0x52, 0x72, 0x18, 0xe5, 0xc4, 0x29, 0x2c, 0x9b,
    0x08, 0xd6, 0x6b, 0x0f, 0xcc, 0xcb, 0x5e, 0xcf, 0xf8, 0xbd, 0x9e, 0x99,
    0x6e, 0x4f, 0x17, 0x87, 0xee, 0x9e, 0x99, 0x6e, 0x8f, 0xd7, 0xcb, 0xeb,
    0x93, 0x4a, 0xdd, 0x35, 0xea, 0xaf, 0xfe, 0xbf, 0xfa, 0x7f, 0x55, 0xd5,
    0xd3, 0x82, 0x1f, 0x28, 0x16, 0x97, 0x96, 0x47, 0x80, 0x14, 0x70, 0xaf,
    0x77, 0xed, 0x6f, 0x6d, 0xe0, 0xdd, 0x42, 0x3e, 0x77, 0x3d, 0x9c, 0x27,
    0xee, 0x72, 0xf0, 0x33, 0xc0, 0x14, 0x30, 0x3d, 0xa0, 0xa5, 0x81, 0x7b,
    0x00, 0xf3, 0x94, 0x61, 0x1c, 0x20, 0x5b, 0xc8, 0xe7, 0x8a, 0xfd, 0x3f,
    0xea, 0x21, 0x21, 0x09, 0xfc, 0x04, 0x78, 0x12, 0xb8, 0x14, 0x12, 0x39,
    0x06, 0x2b, 0x84, 0x20, 0x16, 0x1d, 0x22, 0x1e, 0x1b, 0x26, 0x11, 0x1b,
    0x26, 0x1e, 0x8b, 0x32, 0x12, 0x8f, 0x12, 0x8f, 0x0d, 0x13, 0x8f, 0x0f,
    0x93, 0x88, 0x45, 0x11, 0xc0, 0x87, 0xff, 0xfc, 0x17, 0xc5, 0x52, 0x4d,
    0x02, 0xbf, 0x04, 0x7e, 0x37, 0x10, 0x60, 0x71, 0x69, 0xf9, 0x37, 0xc0,
    0xcb, 0xde, 0x6c, 0x10, 0x42, 0x30, 0x79, 0x3e, 0xc5, 0xec, 0xcc, 0x24,
    0xd9, 0x8b, 0x17, 0x18, 0x1f, 0x1d, 0xe9, 0x8a, 0xc4, 0xa3, 0xc3, 0x98,
    0x67, 0x86, 0x10, 0x7a, 0x84, 0x96, 0xad, 0x68, 0x5a, 0x0e, 0x6d, 0x5b,
    0xd1, 0xb6, 0x1d, 0x9a, 0x96, 0x43, 0xcb, 0x76, 0x30, 0x34, 0xc9, 0x50,
    0x44, 0xf2, 0xe8, 0x5c, 0x94, 0xb6, 0x65, 0xf3, 0xe6, 0x3b, 0x7f, 0x07,
    0x78, 0x2c, 0x3c, 0x09, 0xdd, 0x13, 0xff, 0x2d, 0xf0, 0x1a, 0xc0, 0xc3,
    0x0f, 0xce, 0xf3, 0xc2, 0x73, 0x4f, 0x73, 0x69, 0x26, 0x83, 0xe5, 0x68,
    0x34, 0x2d, 0x87, 0x66, 0xbb, 0x43, 0xd3, 0x52, 0x1c, 0x5a, 0x0e, 0xdb,
    0x6d, 0x87, 0x6a, 0xbd, 0x83, 0xe3, 0x74, 0xe8, 0xa8, 0x43, 0x94, 0x52,
    0x38, 0x0a, 0x1c, 0xa5, 0x50, 0x0a, 0x1c, 0xc7, 0xef, 0x77, 0x70, 0x1c,
    0xc5, 0x23, 0x33, 0xc3, 0x3c, 0x70, 0x5f, 0xd6, 0xd7, 0x5b, 0x18, 0x08,
    0x00, 0x3c, 0x03, 0xf0, 0xb3, 0x9f, 0x3e, 0xc4, 0xd5, 0x97, 0x97, 0x29,
    0x37, 0x2c, 0xfe, 0x5f, 0xb5, 0xb1, 0x8f, 0xac, 0x9e, 0x00, 0xa0, 0x3c,
    0x11, 0x1c, 0x9b, 0x8d, 0x5a, 0x8d, 0x6a, 0xa5, 0x42, 0xb5, 0x5a, 0xa1,
    0x56, 0xad, 0xf0, 0xf3, 0xa5, 0x67, 0x49, 0x9f, 0x9f, 0x70, 0x41, 0x1c,
    0x17, 0xc8, 0x51, 0x82, 0xa6, 0xe5, 0x90, 0xb9, 0x70, 0xaf, 0xaf, 0x37,
    0xbe, 0xb8, 0xb4, 0x9c, 0x2a, 0xe4, 0x73, 0xb7, 0xc2, 0x00, 0xa3, 0x00,
    0x17, 0x33, 0xe7, 0x59, 0xdf, 0xb1, 0x69, 0xec, 0x1f, 0x21, 0x05, 0x98,
    0x86, 0xc0, 0x51, 0x82, 0x7a, 0xbd, 0xc1, 0xe7, 0xd7, 0x3e, 0xa7, 0x54,
    0x29, 0x53, 0x2e, 0x95, 0xd9, 0xdc, 0xdc, 0x44, 0x29, 0x05, 0x80, 0x69,
    0x98, 0x5c, 0x9c, 0x99, 0x21, 0x62, 0x4a, 0x22, 0xba, 0x40, 0x21, 0xe8,
    0x38, 0x0a, 0xa5, 0x04, 0x8e, 0x82, 0x96, 0xed, 0x90, 0x4a, 0x9c, 0x61,
    0x6c, 0x34, 0x41, 0x63, 0x6b, 0x17, 0xe0, 0x32, 0x30, 0x18, 0x20, 0x7d,
    0xcf, 0x28, 0x07, 0xcd, 0x0e, 0xa6, 0x2e, 0x50, 0x4a, 0xa0, 0x70, 0x67,
    0xb2, 0xb1, 0x5e, 0xe3, 0x1f, 0xf9, 0x3c, 0xa6, 0x61, 0x60, 0xd9, 0x76,
    0xc0, 0xc2, 0x5f, 0xbc, 0xf8, 0x22, 0x53, 0xd3, 0xd3, 0xae, 0xfd, 0x9e,
    0x43, 0x4a, 0x09, 0xb7, 0x14, 0x80, 0x7d, 0xe4, 0x82, 0x4e, 0x5d, 0x48,
    0xfb, 0x00, 0x0b, 0xc0, 0xbf, 0xfd, 0x7c, 0xe9, 0x5d, 0x93, 0x00, 0xe9,
    0xd4, 0x18, 0xba, 0x26, 0x30, 0x34, 0x81, 0xa1, 0xbb, 0x57, 0x53, 0x13,
    0x3c, 0x74, 0x65, 0x81, 0x3f, 0xfd, 0xf1, 0x55, 0xde, 0xf8, 0xf3, 0x6b,
    0x3c, 0x78, 0x39, 0x58, 0xc6, 0x56, 0xf3, 0x36, 0x11, 0x5d, 0x62, 0xea,
    0x82, 0x88, 0x2e, 0xbb, 0xf7, 0xa6, 0xe1, 0x3a, 0x62, 0x77, 0x5c, 0x80,
    0xc9, 0x5e, 0x19, 0x2e, 0xf7, 0xe7, 0xfb, 0x00, 0x6d, 0x80, 0xe4, 0x48,
    0x8c, 0x88, 0xd1, 0x13, 0x37, 0x34, 0x81, 0xae, 0x0b, 0xce, 0x9e, 0x31,
    0x48, 0xc4, 0x86, 0x31, 0x34, 0xc1, 0xa5, 0xd9, 0x99, 0x00, 0x40, 0xb5,
    0x5a, 0x71, 0x05, 0x3d, 0x61, 0xa3, 0xef, 0xde, 0xd4, 0x25, 0x1d, 0xc7,
    0x05, 0xc8, 0x9c, 0x00, 0xe0, 0x97, 0x60, 0x0b, 0x88, 0xb6, 0x5a, 0x16,
    0xf1, 0xa4, 0x44, 0x29, 0x07, 0x47, 0x81, 0x02, 0xcf, 0xd2, 0x5e, 0xcb,
    0x4e, 0x4f, 0x06, 0x00, 0xca, 0xa5, 0x12, 0x86, 0x2e, 0x7a, 0xd6, 0xfb,
    0xbb, 0x41, 0x09, 0x4c, 0x5d, 0x30, 0x16, 0x73, 0x25, 0x32, 0x13, 0x69,
    0x3f, 0x25, 0x60, 0xa1, 0xef, 0xc0, 0x16, 0xc0, 0xed, 0x66, 0x13, 0x43,
    0x17, 0x68, 0xd2, 0x73, 0x40, 0xd2, 0x75, 0xc2, 0x77, 0x23, 0x0c, 0xf0,
    0x75, 0xb9, 0xd2, 0x7b, 0x46, 0x17, 0x18, 0xba, 0x24, 0x76, 0x56, 0x63,
    0x3a, 0x15, 0xe1, 0xfe, 0x89, 0xb3, 0x8c, 0x0c, 0xeb, 0x61, 0x07, 0xc6,
    0x17, 0x97, 0x96, 0x53, 0x61, 0x07, 0xb6, 0x01, 0x6e, 0xdf, 0x6e, 0xa2,
    0x6b, 0x02, 0x5d, 0x73, 0x17, 0x20, 0xfe, 0xcc, 0x7d, 0x27, 0x50, 0x24,
    0x13, 0x51, 0xc6, 0x46, 0x93, 0x34, 0xb6, 0xb6, 0x01, 0xd8, 0xdf, 0x3f,
    0x60, 0x7f, 0x6f, 0x87, 0xe4, 0x48, 0x12, 0xd3, 0x10, 0x24, 0x86, 0x74,
    0x86, 0x22, 0xfe, 0xbc, 0x7a, 0x91, 0x4e, 0x8d, 0x61, 0x9a, 0x06, 0x96,
    0x65, 0xfb, 0x65, 0xb8, 0x75, 0xdc, 0x81, 0xc3, 0x16, 0x9a, 0x04, 0x5d,
    0x03, 0x5d, 0x82, 0xa6, 0x09, 0xb7, 0x49, 0x17, 0x4a, 0x97, 0x12, 0x5d,
    0x1e, 0x77, 0xa1, 0x56, 0xad, 0x92, 0x4e, 0x1a, 0x9c, 0x4b, 0x9a, 0x03,
    0xc5, 0xc1, 0x3f, 0x59, 0xbb, 0x2e, 0x74, 0xcb, 0x10, 0x2a, 0x41, 0x0b,
    0x4d, 0x8a, 0x40, 0xd3, 0xa5, 0x40, 0xd7, 0x40, 0x93, 0xa0, 0x69, 0x2e,
    0xd4, 0x6c, 0x08, 0x60, 0xbd, 0x56, 0x25, 0x62, 0x0c, 0x16, 0xee, 0x8f,
    0xe9, 0xcc, 0x39, 0xff, 0xb6, 0xbb, 0x10, 0x03, 0x25, 0x38, 0x3c, 0x6c,
    0x22, 0x05, 0x68, 0x52, 0x74, 0xad, 0x47, 0x29, 0x14, 0xfd, 0x7d, 0x98,
    0x9b, 0xc9, 0x04, 0x06, 0xfe, 0xf2, 0xe6, 0xd7, 0xa7, 0x8a, 0x03, 0x5c,
    0x9c, 0x3c, 0x19, 0xa0, 0x0e, 0xb0, 0xb3, 0x77, 0x80, 0x10, 0x02, 0x29,
    0x14, 0x4a, 0xb8, 0x62, 0x4a, 0xb8, 0x2f, 0x41, 0x1f, 0x40, 0x00, 0x57,
    0xee, 0x9f, 0x0e, 0x02, 0xac, 0x7e, 0x6b, 0x80, 0x63, 0x25, 0x28, 0x02,
    0xd4, 0x36, 0xea, 0x08, 0x40, 0x0a, 0x81, 0x26, 0x04, 0x52, 0x0a, 0xa4,
    0x04, 0x29, 0x40, 0x4a, 0x30, 0x35, 0xc1, 0x50, 0x44, 0x32, 0x3e, 0x1a,
    0x67, 0x2c, 0x99, 0xe8, 0x0e, 0xbc, 0xd9, 0xd8, 0x61, 0x7b, 0x67, 0xef,
    0xdb, 0x00, 0x74, 0x77, 0x82, 0x0f, 0xb0, 0x0a, 0x50, 0x5d, 0xaf, 0x23,
    0x3c, 0x31, 0x21, 0x3c, 0x61, 0x0f, 0xc4, 0xdf, 0x66, 0x7e, 0xcc, 0x65,
    0x83, 0x65, 0xf8, 0xe2, 0x2e, 0xca, 0x70, 0xe1, 0x5c, 0x0a, 0x5d, 0xd7,
    0xfc, 0xee, 0xe5, 0x63, 0x00, 0xb5, 0x8d, 0x3a, 0x4a, 0xa9, 0xae, 0x78,
    0x0f, 0xc2, 0x5d, 0x17, 0xfd, 0x11, 0x06, 0xf8, 0x72, 0xb5, 0x74, 0x2a,
    0x80, 0xa6, 0xc9, 0xfe, 0xf3, 0x60, 0xa1, 0x0b, 0x50, 0xc8, 0xe7, 0xb6,
    0x81, 0x1d, 0xcb, 0x3e, 0xa2, 0xb1, 0xb5, 0x8b, 0x40, 0x78, 0x6b, 0xc1,
    0xbd, 0x86, 0xc5, 0x01, 0xe6, 0x66, 0x82, 0x3b, 0xe1, 0xbb, 0x2e, 0xc4,
    0xfe, 0xbd, 0xe3, 0x96, 0x61, 0xc3, 0x2d, 0x03, 0x5e, 0x1b, 0xa0, 0x0d,
    0xc0, 0xfc, 0xec, 0xd4, 0x8f, 0x04, 0xb0, 0xbe, 0x09, 0x3d, 0xfd, 0x13,
    0x23, 0x35, 0x9e, 0x24, 0x11, 0x8f, 0x76, 0xfb, 0xeb, 0xb7, 0x1a, 0xd4,
    0xdd, 0xd7, 0xad, 0x7f, 0xda, 0x9d, 0x06, 0xb0, 0x00, 0xc1, 0x3f, 0xa5,
    0xde, 0x3a, 0x68, 0xdc, 0x79, 0x0a, 0x7d, 0x31, 0x3e, 0x9a, 0x60, 0x77,
    0xef, 0xa0, 0xdb, 0x7f, 0xfe, 0xd7, 0xbf, 0x47, 0x29, 0xc5, 0xfc, 0xec,
    0x14, 0x6f, 0xbc, 0xf2, 0xd2, 0xc0, 0x9c, 0x91, 0x44, 0xac, 0x9b, 0xbe,
    0xb8, 0xb4, 0x9c, 0x3a, 0x06, 0x50, 0xdd, 0xa8, 0x9f, 0x2a, 0xfc, 0xfe,
    0x07, 0x1f, 0xf1, 0x9f, 0x4f, 0x3f, 0xe3, 0x66, 0xb1, 0xd2, 0xfd, 0xcd,
    0x34, 0x0d, 0xa6, 0x26, 0xd2, 0xcc, 0x65, 0x33, 0x5c, 0x59, 0xc8, 0x9e,
    0x98, 0x6b, 0x1a, 0x46, 0xa0, 0xdf, 0x0f, 0xb0, 0x06, 0xbd, 0x12, 0xdc,
    0x29, 0xfe, 0xfb, 0xbf, 0x1b, 0x44, 0x4c, 0x83, 0x67, 0x9f, 0x7a, 0x82,
    0xf9, 0x6c, 0x86, 0xb9, 0x6c, 0x86, 0xa9, 0x89, 0x73, 0x68, 0xda, 0x9d,
    0x8f, 0xe3, 0x8d, 0xcd, 0x2d, 0xde, 0x7e, 0xef, 0x43, 0xbf, 0xdb, 0x2a,
    0xe4, 0x73, 0xb7, 0xbe, 0x93, 0x03, 0xaf, 0x5f, 0x1d, 0x6c, 0xaf, 0x1f,
    0x47, 0x47, 0x1d, 0x8a, 0xa5, 0x2a, 0x2b, 0x6b, 0x65, 0x6e, 0xac, 0x95,
    0xb9, 0xb9, 0x56, 0xe6, 0xc6, 0x5a, 0x89, 0xfd, 0x83, 0xc3, 0xfe, 0xc7,
    0xfe, 0x0a, 0x41, 0x07, 0xbe, 0x02, 0x3a, 0x8d, 0xad, 0x5d, 0xad, 0x6d,
    0xd9, 0x44, 0xcc, 0xa0, 0x55, 0x27, 0xc5, 0xee, 0xde, 0x01, 0x2b, 0x45,
    0x5f, 0xa4, 0xcc, 0xca, 0x5a, 0x99, 0x62, 0xa9, 0x4a, 0xa7, 0xe3, 0x0c,
    0x7a, 0xbc, 0x09, 0x5c, 0x07, 0xfe, 0x06, 0xbc, 0x09, 0xa1, 0x85, 0xbe,
    0xb8, 0xb4, 0xbc, 0x0a, 0x4c, 0xbf, 0x7e, 0xf5, 0x25, 0x1e, 0xbe, 0x32,
    0x1f, 0xc8, 0x74, 0x1c, 0x87, 0xca, 0xfa, 0x26, 0x2b, 0x9e, 0xc8, 0xca,
    0x5a, 0x89, 0x95, 0xb5, 0x32, 0x9b, 0x8d, 0x9d, 0x93, 0xd8, 0x8a, 0xc0,
    0x67, 0xa1, 0x76, 0xa3, 0x90, 0xcf, 0x05, 0xc8, 0xf4, 0x50, 0xd2, 0xc7,
    0xc0, 0xaf, 0xfe, 0xf0, 0xd6, 0x5f, 0x78, 0xf2, 0x89, 0x47, 0xe9, 0x74,
    0x3a, 0xec, 0xee, 0x1d, 0xb0, 0xfa, 0x55, 0x85, 0x9b, 0xc5, 0x0a, 0xad,
    0xb6, 0x35, 0x48, 0x68, 0x1f, 0xb8, 0x16, 0x12, 0xba, 0x56, 0xc8, 0xe7,
    0x4e, 0x7f, 0x39, 0x70, 0xdc, 0x81, 0xf3, 0xc0, 0x27, 0x0c, 0xf8, 0x82,
    0xc1, 0xfd, 0x36, 0x59, 0x19, 0x30, 0xab, 0x62, 0x21, 0x9f, 0x53, 0x77,
    0x23, 0x76, 0x2a, 0x80, 0x07, 0x61, 0x02, 0x8f, 0x03, 0x4f, 0x7b, 0xa2,
    0xbe, 0xd0, 0xf5, 0x42, 0x3e, 0x77, 0x18, 0x7e, 0xfe, 0xfb, 0xc6, 0x37,
    0xa1, 0xfe, 0x5a, 0x08, 0x89, 0xe7, 0x8c, 0xbc, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include <qlayout.h>
#include <qpainter.h>
#ifdef QT3_SUPPORT
#include <q3filedialog.h>
#else
#include <qfiledialog.h>
#endif
#include <qmessagebox.h>
#include <qregexp.h>
#include <qimage.h>
#include <qevent.h>
#include <qwidgetaction.h>
#include <Music/Music.h>
using namespace Music;

// ---------------------- MScale ------------------------------

void MScale::load_scala(const QString& file_name)
{
    ifstream file(file_name.toLatin1().constData());

	if(!file.is_open())
		throw MicrotonalView::tr("Unkown filename: '")+file_name+"'";

	int lines_red = 0;
	int num_line = 1;

	char buff[1024];
	file.getline(buff, 1024);
	while(file)
	{
		if(buff[0]!='!')
		{
			if(lines_red==0)
			{
				m_name = QString(buff);
				m_name = m_name.remove("\r");
				m_name = m_name.remove("\n");
				m_name = m_name.simplified();
				lines_red++;
			}
			else if(lines_red==1)
			{
				// ignore number of lines ...
				lines_red++;
			}
			else
			{
				bool is_ratio = true;
				for(size_t i=0; is_ratio && i<strlen(buff); i++)
					is_ratio = buff[i]!='.';

				if(is_ratio)
				{
					int num=0, den=0;
					char slash;
					stringstream in(buff);
					in >> num >> slash >> den;
					if(num<0) throw QString("Line ")+QString::number(num_line)+": Parsed numerator is negative '"+QString::number(num)+"'";
					if(slash!='/') throw QString("Line ")+QString::number(num_line)+": There is no slash between numerator and denominator"+slash+"'";
					if(den<=0) throw QString("Line ")+QString::number(num_line)+": Parsed denominator is not positive'"+QString::number(den)+"'";
					values.push_back(MValue(num, den));
				}
				else
				{
					float cents=0.0;
					stringstream in(buff);
					in >> cents;
					if(cents<0.0) throw QString("Line ")+QString::number(num_line)+": Parsed cents value is negative '"+QString::number(cents)+"'";
					values.push_back(MValue(cents));
				}
			}
		}
		file.getline(buff, 1024);
		num_line++;
	}
}

MScale::MScale(const QString& name)
: m_name(name)
{
}
MScale::MScale(const QString& file_name, FileType type)
{
	m_path = file_name;

	if(type==SCALA)	load_scala(file_name);
	else
	{
		cerr << "MicrotonalView::Scale::Scale unsupported file type '" << type << "'" << endl;
	}
}

bool MScale::operator != (const MScale& scale)
{
	return m_name!=scale.m_name;
}

// ---------------------- QRoot button -------------------------

QRoot::QRoot(MicrotonalView* view, int ht)
: QPushButton(view)
, m_ht(ht)
{
//	cerr << "QRoot::QRoot " << m_ht << endl;

// 	if(ht>8) ht+=12;

	connect(this, SIGNAL(clicked()), this, SLOT(clicked2()));
	connect(this, SIGNAL(rootClicked(int)), view, SLOT(selectRoot(int)));
	setFlat(true);
	setText(QString::fromStdString(h2n(ht, GetNotesName(), GetTonality(), false)));
	setCheckable(true);
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	setFixedWidth(35);		// TODO
	setFocusPolicy(Qt::NoFocus);
}
void QRoot::clicked2()
{
	emit(rootClicked(m_ht));
}

// --------------------------- MicrotonalView ----------------------------

MicrotonalView::MicrotonalView(QWidget* parent)
: QFrame(parent)
, View(tr("Microtonal"), this)
, ui_scale_menu(tr("Used scale"), this)
{
	setting_selectedRoot = -1000;
	m_AFreq = 440.0f;
	m_selected_jivalue = NULL;
	m_tuningFreq = 0.0;
    setting_selectedScale = NULL;

	// settings
	QPixmap img;
	img.loadFromData(g_icon_ji, sizeof(g_icon_ji), "PNG");
	setting_show->setIcon(QIcon(img));
	setting_show->setChecked(false);
	hide();

	QHBoxLayout* octaveActionLayout = new QHBoxLayout(&m_popup_menu);

	QLabel* octaveActionTitle = new QLabel(tr("Root's octave"), &m_popup_menu);
	octaveActionLayout->addWidget(octaveActionTitle);

	setting_octave = new QSpinBox(&m_popup_menu);
	setting_octave->setMinimum(-3);
	setting_octave->setMaximum(8);
	setting_octave->setSingleStep(1);
	setting_octave->setToolTip(tr("Root's octave"));
	setting_octave->setValue(3);
// 	m_old_octave = setting_octave->value();
// 	connect(setting_octave, SIGNAL(valueChanged(int)), this, SLOT(octaveChanged(int)));
	connect(setting_octave, SIGNAL(valueChanged(int)), this, SLOT(emitTuningFreqChanged()));
	octaveActionLayout->addWidget(setting_octave);

	QWidget* octaveActionWidget = new QWidget(&m_popup_menu);
	octaveActionWidget->setLayout(octaveActionLayout);

	QWidgetAction* octaveAction = new QWidgetAction(&m_popup_menu);
	octaveAction->setDefaultWidget(octaveActionWidget);
	m_popup_menu.addAction(octaveAction);

	setting_keepRootToLeft = new QAction(tr("Keep root to left side"), this);
	setting_keepRootToLeft->setCheckable(true);
	connect(setting_keepRootToLeft, SIGNAL(toggled(bool)), this, SLOT(keepRootToLeft(bool)));
	m_popup_menu.addAction(setting_keepRootToLeft);

	setting_loadScale = new QAction(tr("Load Scala file ..."), this);
	m_popup_menu.addAction(setting_loadScale);
    connect(setting_loadScale, SIGNAL(triggered()), this, SLOT(loadScale()));

	m_popup_menu.addMenu(&ui_scale_menu);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	QHBoxLayout* roots_layout = new QHBoxLayout();
	roots_layout->setSpacing(0);
	layout->addLayout(roots_layout);
	for(int i=0; i<13; i++)
	{
		m_roots.push_back(new QRoot(this, i+3));
		roots_layout->addWidget(m_roots.back());
		if(i+1<13)
			roots_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	}
	m_roots.back()->setDisabled(true);
	ui_scale = new QScaleLabel(this);
	layout->addWidget(ui_scale);
	ui_ratios = new QRatiosLabel(this);
	layout->addWidget(ui_ratios);

	setMaximumHeight(ui_scale->maximumHeight()+ui_ratios->maximumHeight()+20);

	load_default_scales();

	refreshScaleList();

	selectRoot(3);
	selectScale(0);
}
void MicrotonalView::save()
{
	s_settings->setValue("octave", setting_octave->value());
	s_settings->setValue("keepRootToLeft", setting_keepRootToLeft->isChecked());
	QStringList scale_list;
	for(size_t i=0; i<setting_scales.size(); i++)
		if(!setting_scales[i]->getPath().isEmpty())
			scale_list.push_back(setting_scales[i]->getPath());
	s_settings->setValue("scales", scale_list);
	s_settings->setValue("selectedScale", setting_selectedScale->getName());
	s_settings->setValue("lastScalesDirectory", setting_lastScalesDirectory);
	s_settings->setValue("selectedRoot", setting_selectedRoot);
}
void MicrotonalView::load()
{
	setting_octave->setValue(s_settings->value("octave", setting_octave->value()).toInt());
	setting_keepRootToLeft->setChecked(s_settings->value("keepRootToLeft", setting_keepRootToLeft->isChecked()).toBool());
	QStringList scale_list = s_settings->value("scales").toStringList();
	for(QStringList::iterator it=scale_list.begin(); it!=scale_list.end(); ++it)
	{
		try
		{
			MScale* scale = new MScale(*it, MScale::SCALA);
			setting_scales.push_back(scale);
		}
		catch(QString error){cerr << "MicrotonalView::load " << error.toStdString() << endl;}
	}
	refreshScaleList();

	selectScale(s_settings->value("selectedScale", "default").toString());
	setting_lastScalesDirectory = s_settings->value("lastScalesDirectory", ".").toString();
	selectRoot(s_settings->value("selectedRoot", m_roots[0]->m_ht).toInt());
}
void MicrotonalView::clearSettings()
{
	s_settings->remove("octave");
	s_settings->remove("keepRootToLeft");
	s_settings->remove("scales");
	s_settings->remove("selectedScale");
	s_settings->remove("lastScalesDirectory");
	s_settings->remove("selectedRoot");
}

/*void MicrotonalView::octaveChanged(int value)
{
	if(value==0)
	{
		if(m_old_octave==1)
		{
			m_old_octave = -1;
			setting_octave->setValue(-1);
		}
		else if(m_old_octave==-1)
		{
			m_old_octave = 1;
			setting_octave->setValue(1);
		}
	}
	else
		m_old_octave = value;
}*/

int MicrotonalView::getOctaveShift()
{
	return 12*(setting_octave->value()-5);
}

QString MicrotonalView::getTuningNoteName()
{
	if(hasTuningFreqSelected())
	{
		if(m_selected_jivalue->is_ratio)
			return QString::fromStdString(h2n(setting_selectedRoot+getOctaveShift()))+":"+QString::number(m_selected_jivalue->num)+"/"+QString::number(m_selected_jivalue->den);
		else
			return QString::fromStdString(h2n(setting_selectedRoot+getOctaveShift()))+":"+QString::number(m_selected_jivalue->cents);
	}

	return "no tuning note selected";
}

void MicrotonalView::scaleActionToggled(bool checked)
{
	if (checked)
		selectScale(((QAction*)sender())->data().toInt());
}

void MicrotonalView::selectScale(int index)
{
	assert(index>=0 && index<int(setting_scales.size()));

	setting_selectedScale = setting_scales[index];

	QList<QAction*> scaleActions = ui_scale_menu.actions();

	for (int i = 0; i < scaleActions.size(); i++)
	{
		if (i == index)
			scaleActions[i]->setChecked(true);
		else
			scaleActions[i]->setChecked(false);
	}

	ui_ratios->repaint();
}
void MicrotonalView::selectScale(const QString& name)
{
	int index = -1;
	for(size_t i=0; index==-1 && i<setting_scales.size(); i++)
	{
		if(setting_scales[i]->getName()==name)
		{
			setting_selectedScale = setting_scales[i];
			index = i;
		}
	}

	if(index==-1)
	{
		cerr << "MicrotonalView::selectScale unknown scale '" << name.toStdString() << "'" << endl;
		return;
	}

	QList<QAction*> scaleActions = ui_scale_menu.actions();

	for (int i = 0; i < scaleActions.size(); i++)
	{
		if (i == index)
			scaleActions[i]->setChecked(true);
		else
			scaleActions[i]->setChecked(false);
	}

	ui_ratios->repaint();
}
int MicrotonalView::getIndex(MScale* scale)
{
	for(size_t i=0; i<setting_scales.size(); i++)
		if(setting_scales[i]==scale)
			return i;

	return -1;
}

void MicrotonalView::notesNameChanged()
{
	for(int i=0; i<13; i++)
		m_roots[i]->setText(QString::fromStdString(h2n(m_roots[i]->m_ht, GetNotesName(), GetTonality(), false)));
}

void MicrotonalView::keepRootToLeft(bool keep)
{
	if(keep)
	{
		for(int i=0; i<13; i++)
			m_roots[i]->m_ht = (setting_selectedRoot+i)%12;
		notesNameChanged();
		selectRoot(setting_selectedRoot);
		ui_ratios->repaint();
	}
	else
	{
	}
}

void MicrotonalView::setAFreq(float AFreq)
{
	float AFreq_old = m_AFreq;

	m_AFreq = AFreq;

	if(m_AFreq!=AFreq_old)
		emitTuningFreqChanged();
}

void MicrotonalView::updateCursor(float freq)
{
	ui_scale->m_htf = f2hf(freq, m_AFreq);
	ui_scale->repaint();
}

void MicrotonalView::selectRoot(int ht)
{
//	cerr << "MicrotonalView::selectRoot " << ht << endl;

	if(ht==setting_selectedRoot)
	{
		for(int i=0; i<13; i++)
			m_roots[i]->setChecked(m_roots[i]->m_ht==ht);
	}
	else
	{
		if(setting_keepRootToLeft->isChecked())
		{
			for(int i=0; i<13; i++)
				m_roots[i]->m_ht = (ht+i)%12;
			notesNameChanged();
		}

		for(int i=0; i<13; i++)
			m_roots[i]->setChecked(m_roots[i]->m_ht==ht);

		setting_selectedRoot = ht;

		ui_ratios->repaint();
		emitTuningFreqChanged();
	}
}
void MicrotonalView::emitTuningFreqChanged()
{
	m_tuningFreq = 0.0;
	if(m_selected_jivalue!=NULL)
	{
		if(m_selected_jivalue->is_ratio)
			m_tuningFreq = h2f(setting_selectedRoot+getOctaveShift(), m_AFreq)*m_selected_jivalue->ratio;
		else
			m_tuningFreq = h2f(setting_selectedRoot+getOctaveShift()+m_selected_jivalue->cents/100.0f, m_AFreq);
	}

	emit(tuningFreqChanged(m_tuningFreq));
}

MicrotonalView::QScaleLabel::QScaleLabel(MicrotonalView* view)
: QLabel(view)
, ui_view(view)
{
	setMaximumHeight(15);
	m_htf = -1000.0f;
}
void MicrotonalView::QScaleLabel::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);

	QPainter* p = new QPainter(this);

	int left = ui_view->m_roots[0]->x() + ui_view->m_roots[0]->width()/2;
	int right = ui_view->m_roots[12]->x() + ui_view->m_roots[12]->width()/2;
	int w = right - left;
	p->setBrush(QColor(255,255,255));
	p->setPen(QColor(128,128,128));
	p->drawRect(0, 0, left+1, height());
	p->drawRect(right, 0, width()-(right+1), height());
	p->setPen(QColor(0,0,0));
	p->drawRect(left, 0, w+1, height());

	if(m_htf!=-1000.0)
	{
		float htw = w*(m_htf-ui_view->m_roots[0]->m_ht)/12.0f;
		while(htw>=w) htw-=w;
		while(htw<0) htw+=w;
		int x = int(left+htw);
		p->setPen(QColor(0,0,255));
		p->setBrush(QColor(191,191,255));
		QPolygon polygon(3);
		polygon.setPoint(0, x-5, 0);
		polygon.setPoint(1, x+5, 0);
		polygon.setPoint(2, x, 2*height()/3);
		p->drawConvexPolygon(polygon);
		p->drawLine(x, 2*height()/3, x, height());

		if(htw<(w/12.0f)/4.0f || -(htw-w)<(w/12.0f)/4.0f)
		{
			if(htw<(w/12.0f)/4.0f)	x += w;
			else					x -= w;
			polygon.setPoint(0, x-5, 0);
			polygon.setPoint(1, x+5, 0);
			polygon.setPoint(2, x, 2*height()/3);
			p->drawConvexPolygon(polygon);
			p->drawLine(x, 2*height()/3, x, height());
		}
	}

	delete p;
}

MicrotonalView::QRatiosLabel::QRatiosLabel(MicrotonalView* view)
: QLabel(view)
, ui_view(view)
{
	setMaximumHeight(12+3*(2*fontMetrics().height()+10));
}
void MicrotonalView::QRatiosLabel::drawTicks(QPainter* p, float r, int h)
{
	int left = ui_view->m_roots[0]->x() + ui_view->m_roots[0]->width()/2;
	int right = ui_view->m_roots[12]->x() + ui_view->m_roots[12]->width()/2;
	int w = right - left;

	for(float i=r; i<=12.0f; i+=2*r)
	{
		int x = left + int(w*i/12.0f);
		p->drawLine(x, 0, x, h);
	}
}
void MicrotonalView::QRatiosLabel::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);

	QPainter* p = new QPainter(this);

	//	p->eraseRect(rect());

	int left = ui_view->m_roots[0]->x() + ui_view->m_roots[0]->width()/2;
	int right = ui_view->m_roots[12]->x() + ui_view->m_roots[12]->width()/2;
	int w = right - left;

	int tick_height = 12;
	int max_dec = 3;

	// draw ratios
	p->setPen(QColor(0,0,255));
	int dec = 0;
	int dec_h = 2*p->fontMetrics().height()+2;
	int dec_h2 = (height()-tick_height-p->fontMetrics().height()+2)/3;
	dec_h = min(dec_h, dec_h2);
	int ht = ui_view->setting_selectedRoot - ui_view->m_roots[0]->m_ht;
	if(ui_view->setting_keepRootToLeft->isChecked())	ht = 0;
	for(list<MScale::MValue>::iterator it=ui_view->setting_selectedScale->values.begin(); it!=ui_view->setting_selectedScale->values.end(); ++it)
	{
		float htw = 0.0f;

		if((*it).is_ratio)
			htw = w*f2hf(h2f(ht)*(*it).ratio)/12.0f;
		else
			htw = w*(ht+(*it).cents/100.0f)/12.0f;
		while(htw>w) htw-=w;
		while(htw<0) htw+=w;

		int x = left + int(htw);
		int y = tick_height + 2 + dec_h*dec;
		p->drawLine(x, 0, x, y+dec_h);

		if((*it).is_ratio)
		{
			QRect num_rect = fontMetrics().boundingRect(QString::number((*it).num));
			num_rect.translate(x+1, y+p->fontMetrics().height());
			QRect den_rect = fontMetrics().boundingRect(QString::number((*it).den));
			den_rect.translate(x+1, y+2*p->fontMetrics().height());
			(*it).bounding_rect = num_rect;
			(*it).bounding_rect |= den_rect;
		}
		else
		{
			QRect cents_rect = fontMetrics().boundingRect(QString::number((*it).cents));
			cents_rect.translate(x+1, y+2*p->fontMetrics().height());

			(*it).bounding_rect = cents_rect;
		}
		(*it).bounding_rect.setLeft(x);
		(*it).bounding_rect.setRight((*it).bounding_rect.right() + 2);
		(*it).bounding_rect.setTop((*it).bounding_rect.top() - 2);
		(*it).bounding_rect.setBottom(y+dec_h);

		if(ui_view->m_selected_jivalue==&(*it))
		{
			p->setBrush(QColor(210,210,255));
			p->drawRect((*it).bounding_rect);
		}

		if((*it).is_ratio)
		{
			p->drawText(x+1, y+p->fontMetrics().height(), QString::number((*it).num));
			p->drawLine(x, y+p->fontMetrics().height()+2, x+p->fontMetrics().width(QString::number((*it).num)), y+p->fontMetrics().height()+2);
			p->drawText(x+1, y+2*p->fontMetrics().height(), QString::number((*it).den));
		}
		else
		{
			p->drawText(x+1, y+2*p->fontMetrics().height(), QString::number((*it).cents));
		}

		dec++;
		if(dec>max_dec-1)	dec = 0;
		if(dec<0)			dec = max_dec-1;
	}

	// draw ticks
	p->setPen(QColor(0,0,0));
	for(float i=0.0f; i<=12.0f; i+=1.0f)
	{
		int x = left + int(w*i/12.0f);
		p->drawLine(x, 0, x, tick_height);
	}
	drawTicks(p, 0.5f, tick_height/2);
	drawTicks(p, 0.25f, tick_height/3);
	drawTicks(p, 0.125f, tick_height/4);

	delete p;
}

void MicrotonalView::mouseReleaseEvent(QMouseEvent* e)
{
	if(e->button()==Qt::LeftButton)
	{
		MScale::MValue* selected_jivalue_old = m_selected_jivalue;
		m_selected_jivalue = NULL;

		QPoint mouse_pos = e->pos() - ui_ratios->pos();
		for(list<MScale::MValue>::iterator it=setting_selectedScale->values.begin(); m_selected_jivalue==NULL && it!=setting_selectedScale->values.end(); ++it)
			if((*it).bounding_rect.contains(mouse_pos.x(), mouse_pos.y()))
				m_selected_jivalue = &(*it);

		if(m_selected_jivalue!=selected_jivalue_old)
		{
			ui_ratios->repaint();
			emitTuningFreqChanged();
		}
	}

	View::mouseReleaseEvent(e);
}

void MicrotonalView::refreshScaleList()
{
	ui_scale_menu.clear();

	QActionGroup* scaleGroup = new QActionGroup(this);

	for(size_t i=0; i<setting_scales.size(); i++)
	{
		QAction* scaleAction = ui_scale_menu.addAction(setting_scales[i]->getName());
		scaleAction->setCheckable(true);
		scaleAction->setData((int)i);
		scaleGroup->addAction(scaleAction);

		if(setting_selectedScale==setting_scales[i])
			scaleAction->setChecked(true);

		connect(scaleAction, SIGNAL(toggled(bool)), this, SLOT(scaleActionToggled(bool)));
	}
}
void MicrotonalView::loadScale()
{
#ifdef QT3_SUPPORT
	Q3FileDialog dlg_file(setting_lastScalesDirectory, "Scala files (*.scl *.SCL)", this, "open file dialog", true);
	dlg_file.setCaption(tr("Open scale file"));
	dlg_file.setMode(Q3FileDialog::ExistingFile);
	dlg_file.setInfoPreviewEnabled(true);
	ScalePreview p(&dlg_file);
	dlg_file.setInfoPreview(&p,&p);
	dlg_file.setPreviewMode(Q3FileDialog::Info);
#else
	QFileDialog dlg_file(this, tr("Open scale file"), setting_lastScalesDirectory, "Scala files (*.scl *.SCL)");
	dlg_file.setFileMode(QFileDialog::ExistingFile);
#endif

	dlg_file.exec();

	if(dlg_file.result()==QDialog::Accepted)
	{
#ifdef QT3_SUPPORT
		setting_lastScalesDirectory = dlg_file.dirPath();
#else
		setting_lastScalesDirectory = dlg_file.directory().path();
#endif
		s_settings->setValue("MicrotonalView_lastScalesDirectory", setting_lastScalesDirectory);
//		cerr << "setting_lastScalesDirectory=" << setting_lastScalesDirectory << endl;
		try
		{
#ifdef QT3_SUPPORT
			MScale* scale = new MScale(dlg_file.selectedFile(), MScale::SCALA);
#else
			MScale* scale = new MScale(dlg_file.selectedFiles()[0], MScale::SCALA);
#endif
			bool new_one = true;
			for(size_t i=0; new_one && i<setting_scales.size(); i++)
				new_one = *scale != *(setting_scales[i]);
			if(new_one)
			{
				setting_scales.push_back(scale);
				selectScale(setting_scales.size()-1);
				refreshScaleList();
			}
			else
				QMessageBox::information(this, tr("Open scale file"), tr("Scale name already exist"), QMessageBox::Ok);
		}
		catch(QString error)
		{
			QMessageBox::information(this, tr("Open scale file"), tr("Invalid file content !")+"\n("+error+")", QMessageBox::Ok);
		}
	}

	ui_ratios->repaint();
}

void MicrotonalView::load_default_scales()
{
	MScale* scale = new MScale("default");
	scale->values.push_back(MScale::MValue(25,24));
	scale->values.push_back(MScale::MValue(16,15));
	scale->values.push_back(MScale::MValue(9,8));
	scale->values.push_back(MScale::MValue(7,6));
	scale->values.push_back(MScale::MValue(75,64));
	scale->values.push_back(MScale::MValue(6,5));
	scale->values.push_back(MScale::MValue(5,4));
	scale->values.push_back(MScale::MValue(32,25));
	scale->values.push_back(MScale::MValue(125,96));
	scale->values.push_back(MScale::MValue(4,3));
	scale->values.push_back(MScale::MValue(25,18));
	scale->values.push_back(MScale::MValue(45,32));
	scale->values.push_back(MScale::MValue(36,25));
	scale->values.push_back(MScale::MValue(3,2));
	scale->values.push_back(MScale::MValue(25,16));
	scale->values.push_back(MScale::MValue(8,5));
	scale->values.push_back(MScale::MValue(5,3));
	scale->values.push_back(MScale::MValue(125,72));
	scale->values.push_back(MScale::MValue(16,9));
	scale->values.push_back(MScale::MValue(9,5));
	scale->values.push_back(MScale::MValue(15,8));
	scale->values.push_back(MScale::MValue(48,25));
	scale->values.push_back(MScale::MValue(125,64));
	setting_scales.push_back(scale);
}

// ------------------ MicrotonalView::ScalePreview --------------------
#ifdef QT3_SUPPORT
MicrotonalView::ScalePreview::ScalePreview(QWidget* parent)
: QLabel(parent)
{
	setAlignment(Qt::AlignVCenter);
	setWordWrap(true);
	setMinimumWidth(100);
}
void MicrotonalView::ScalePreview::previewUrl(const Q3Url& url)
{
	if(url.path().contains(QRegExp("\\.scl$"))==0 && url.path().contains(QRegExp("\\.SCL$"))==0)
	{
//		setText("");
	}
	else
	{
		try
		{
			MScale* scale = new MScale(url.path(), MScale::SCALA);
			QString txt;
			txt += tr("<b>name:</b> ") + scale->getName() + "<p>";
			txt += tr("<b>number of ratio:</b> ") + QString::number(scale->values.size());
			setText(txt);
			delete scale;
		}
		catch(QString error)
		{
			setText(tr("Invalid Scala file: ")+error);
		}
	}
}
#endif
