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

#include <cstdio>
#include <ctime>
#include "Executer.h"

int main(int argc, const char *argv[]) {
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
    return 0;
}
