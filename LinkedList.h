//
//  LinkedList.h
//  KipaLang
//
//  Created by Kim Yeong-Hyeon on 2017. 1. 25..
//  Copyright © 2017년 Kim Yeong-Hyeon. All rights reserved.
//

#ifndef LinkedList_h
#define LinkedList_h

template<typename _T>
class LinkedList {
public:
    LinkedList<_T> *l, *n;
    _T v;
    LinkedList() {
        l = nullptr;
        n = nullptr;
    }
    void initialize(size_t s) {
        size_t i;
        for (i=0; i<s; ++i) {
            (this + i)->l = this + i - 1;
            (this + i)->n = this + i + 1;
        }
        this->l = nullptr;
        (this + i - 1)->n = nullptr;
    }
};

#endif /* LinkedList_h */
