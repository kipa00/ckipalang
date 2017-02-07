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

#ifndef KipaList_h
#define KipaList_h

#include <vector>
#include "Data.h"
using std::vector;

class KipaList {
public:
    vector<Data> *v;
    KipaList() {
        v = new vector<Data>;
    }
    void push(Data d) {
        v->push_back(d);
    }
    void pop() {
        v->pop_back();
    }
    void drop() {
        delete v;
    }
};

KipaList *ebyeProcess(KipaList *a, KipaList *b, byte opcode) {
    KipaList *res = new KipaList;
    int as = (int)a->v->size(), bs = (int)b->v->size();
    int mx = std::max(as, bs);
    int i;
    for (i=0; i<mx; ++i) {
        res->push(operation((*a->v)[i % as], (*b->v)[i % bs], opcode, false));
    }
    
    for (i=0; i<as; ++i) {
        (*a->v)[i].drop();
    }
    for (i=0; i<bs; ++i) {
        (*b->v)[i].drop();
    }
    return res;
}

#endif /* KipaList_h */
