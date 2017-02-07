//  KipaLang
//  Copyright (C) 2017  Kim Yeong-Hyeon
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef StringList_h
#define StringList_h

#include <algorithm>
using std::pair;
using std::make_pair;

pair<int, byte *> fromUTF8(const char *str, const char *end = nullptr) {
    int i, len = (int)(end - str), shlen = 0;
    if (!end) {
        for (len=0; str[len]; ++len);
    }
    for (i = 0; i < len; ++i) {
        if ((str[i] & 0xC0) != 0x80) {
            ++shlen;
        }
    }
    byte *res = (byte *)malloc(1 + (shlen + 1) * sizeof(short));
    res[0] = PREPROCESS_STRING;
    unsigned short *s = (unsigned short *)(res + 1);
    bool encodeError = false;
    s[0] = (short)shlen;
    shlen = 1;
    for (i=0; i<len; ++i) {
        if (!(str[i] & 0x80)) {
            s[shlen++] = str[i];
        } else if ((str[i] & 0xE0) == 0xC0) {
            if (i+1 < len && (str[i+1] & 0xC0) == 0x80) {
                s[shlen++] = ((str[i] & 31) << 6) | (str[i+1] & 63);
                ++i;
            } else {
                encodeError = true;
            }
        } else if ((str[i] & 0xF0) == 0xE0) {
            if (i+2 < len && (str[i+1] & 0xC0) == 0x80 && (str[i+2] & 0xC0) == 0x80) {
                s[shlen++] = ((str[i] & 31) << 12) | ((str[i+1] & 63) << 6) | (str[i+2] & 63);
                i += 2;
            } else {
                encodeError = true;
            }
        } else {
            // Treat as UTF8EncodingError
        }
    }
    if (encodeError) {
        free(res);
        // TODO: UTF8EncodingError
        return make_pair(0, nullptr);
    }
    return make_pair(1 + shlen * sizeof(short), res);
}

char *toUTF8(int uni) {
    char *s;
    uni &= 0xFFFF;
    if (!(uni & 0xFF80)) {
        s = (char *)malloc(2 * sizeof(char));
        s[0] = uni;
        s[1] = 0;
        return s;
    } else if (!(uni & 0xF800)) {
        s = (char *)malloc(3 * sizeof(char));
        s[0] = ((uni >> 6) & 31) | 0xC0;
        s[1] = (uni & 63) | 0x80;
        s[2] = 0;
        return s;
    }
    s = (char *)malloc(4 * sizeof(char));
    s[0] = ((uni >> 12) & 15) | 0xE0;
    s[1] = ((uni >> 6) & 63) | 0x80;
    s[2] = (uni & 63) | 0x80;
    s[3] = 0;
    return s;
}

#endif /* StringList_h */
