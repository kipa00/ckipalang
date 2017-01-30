//
//  InputManager.h
//  KipaLang
//
//  Created by Kim Yeong-Hyeon on 2017. 1. 29..
//  Copyright © 2017년 Kim Yeong-Hyeon. All rights reserved.
//

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
