//
//  KipaList.h
//  KipaLang
//
//  Created by Kim Yeong-Hyeon on 2017. 1. 28..
//  Copyright © 2017년 Kim Yeong-Hyeon. All rights reserved.
//

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
