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

#ifndef InputManager_h
#define InputManager_h

#include <cstdio>

namespace bufin {
    const int BUFSIZE = 10240;
    char arr[BUFSIZE + 1];
    char *ptr = arr + BUFSIZE;
    
    char getch() {
        if (ptr == arr + BUFSIZE) {
            fread(arr, 1, BUFSIZE, stdin);
            ptr = arr;
        }
        return *ptr++;
    }
    
    int scanint() {
        int s = 0;
        int g = getch();
        while (g > 57 || 48 > g) g = getch();
        while (48 <= g && g <= 57) {
            s = (s * 10) + (g & 15);
            g = getch();
        }
        --ptr;
        return s;
    }
}

#endif /* InputManager_h */
