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

//#define DEBUG_MODE
//#define COMPILE_MODE

#include <cstdio>
#include <ctime>
#include "Interpreter.h"
#include "Executer.h"

int main(int argc, const char *argv[]) {
#ifdef DEBUG_MODE
    pair<pair<int, int>, byte *> code;
    if (argc == 1) {
        char arr[1000];
        fgets(arr, 1000, stdin);
        code = makeCode(arr);
        int i;
        for (i=0; i<code.first.first; ++i) {
            printf("%02X ", code.second[i]);
        }
        puts("");
    } else {
        FILE *fp = fopen(argv[1], "rb");
        int bysz, varlen;
        fread(&bysz, sizeof(int), 1, fp);
        fread(&varlen, sizeof(int), 1, fp);
        byte *cp = (byte *)malloc(bysz);
        fread(cp, 1, bysz, fp);
        fclose(fp);
        code = std::make_pair(std::make_pair(bysz, varlen), cp);
    }
    printf("%s\n", executeCode(code.second, code.first.first, code.first.second).c_str());
#else
#ifdef COMPILE_MODE
    int i;
    const int MAX_FILE_SIZE = 102401; // 100 KiB
    const int MAX_PATH_LEN = 200;
    for (i=1; i<argc; ++i) {
        FILE *fp = fopen(argv[i], "r");
        char *cp = (char *)malloc(MAX_FILE_SIZE * sizeof(char));
        cp[fread(cp, 1, MAX_FILE_SIZE, fp)] = 0;
        printf("%s", cp);
        fclose(fp);
        pair<pair<int, int>, byte *> res = makeCode(cp);
        char *fn = (char *)malloc(MAX_PATH_LEN * sizeof(char));
        sprintf(fn, "%s", argv[i]);
        char *temp = fn + strlen(fn);
        bool processed = false;
        while (--temp > fn) {
            if (*temp == '.') {
                memcpy(temp, ".klb", 5 * sizeof(char));
                processed = true;
                break;
            } else if (*temp == '/') {
                memcpy(fn + strlen(fn), ".klb", 5 * sizeof(char));
                processed = true;
                break;
            }
        }
        if (!processed) {
            memcpy(fn + strlen(fn), ".klb", 5 * sizeof(char));
        }
        FILE *fp2 = fopen(fn, "wb");
        fwrite(&res.first.first, sizeof(int), 1, fp2);
        fwrite(&res.first.second, sizeof(int), 1, fp2);
        fwrite(res.second, res.first.first, 1, fp2);
        fclose(fp2);
    }
#else
    int i;
    for (i=1; i<argc; ++i) {
        FILE *fp = fopen(argv[i], "rb");
        int bysz, varlen;
        fread(&bysz, sizeof(int), 1, fp);
        fread(&varlen, sizeof(int), 1, fp);
        byte *cp = (byte *)malloc(bysz);
        fread(cp, 1, bysz, fp);
        fclose(fp);
        string res = executeCode(cp, bysz, varlen);
        fwrite(res.c_str(), 1, res.length(), stdout);
    }
#endif
#endif
    return 0;
}
