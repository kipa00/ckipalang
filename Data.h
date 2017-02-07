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

#ifndef Data_h
#define Data_h

#include <cstdlib>
#include <memory.h>
#include "CodeConstant.h"

const int DATA_TYPE_INT = 0;
const int DATA_TYPE_LONG = 1;
const int DATA_TYPE_FLOAT = 2;
const int DATA_TYPE_LIST = 3;

class Data {
public:
    void *d;
    int l, t;
    Data() {
        d = nullptr;
        t = 0;
        l = 0;
    }
    Data(int s) {
        reserve(s);
        t = 0;
    }
    Data(int s, void *dt) {
        reserve(s, dt);
        t = 0;
    }
    void reserve(int size) {
        d = malloc(size);
        l = size;
    }
    void reserve(int size, void* data) {
        d = malloc(size);
        memcpy(d, data, size);
        l = size;
    }
    void drop() {
        if (!d) {
            free(d);
            d = nullptr;
            l = 0;
        }
    }
    Data operator=(Data a) {
        this->d = a.d;
        this->t = a.t;
        this->l = a.l;
        return (*this);
    }
    Data clone() {
        Data res(this->l, this->d);
        res.t = this->t;
        return res;
    }
};

Data operation(Data d1, Data d2, byte opcode, bool force_drop);
#include "KipaList.h"

template <typename _T>
inline Data dataMakeFrom(_T i, int t) {
    Data res(sizeof(_T), &i);
    res.t = t;
    return res;
}

template <typename _T>
inline _T processOperator(_T a, byte op) {
    if (op == OPERATOR_NOTHING) {
        return a;
    } else if (op == OPERATOR_NEGATE) {
        return -a;
    }
    return 0;
}

template <typename _T>
inline _T processOperator(_T a, _T b, byte op) {
    switch (op) {
        case OPERATOR_PLUS:
            return a + b;
        case OPERATOR_MINUS:
            return a - b;
        case OPERATOR_MULTIPLICATION:
            return a * b;
        case OPERATOR_DIVISION:
            return a / b;
    }
    return 0;
}

template <typename _T>
inline int processBoolOperator(_T a, _T b, byte op) {
    switch (op) {
        case OPERATOR_EQUALS:
            return a == b;
        case OPERATOR_LESSTHAN:
            return a < b;
        case OPERATOR_GREATERTHAN:
            return a > b;
    }
    return 0;
}

template<typename _T1, typename _T2>
Data makeWithData(_T1 a, _T2 *b, int t, Data* ld) {
    _T2 temp = (_T2)a;
    Data d(sizeof(_T2), &temp);
    d.t = t;
    ld->drop();
    return d;
}

Data type_cast(Data d, int to) {
    if (to == PROCESS_LIST) {
        KipaList *res = new KipaList;
        res->push(d);
        Data dr;
        dr.d = (void *)res;
        dr.l = sizeof(KipaList);
        dr.t = PROCESS_LIST;
        return dr;
    }
    if (d.t == to) {
        return d;
    }
    if (d.t == PREPROCESS_INT && to == PREPROCESS_LONG) {
        return makeWithData(*(int *)d.d, (long long int *)nullptr, to, &d);
    } else if (d.t == PREPROCESS_INT && to == PREPROCESS_FLOAT) {
        return makeWithData(*(int *)d.d, (double *)nullptr, to, &d);
    } else if (d.t == PREPROCESS_LONG && to == PREPROCESS_INT) {
        return makeWithData(*(long long int *)d.d, (int *)nullptr, to, &d);
    } else if (d.t == PREPROCESS_LONG && to == PREPROCESS_FLOAT) {
        return makeWithData(*(long long int *)d.d, (double *)nullptr, to, &d);
    } else if (d.t == PREPROCESS_FLOAT && to == PREPROCESS_INT) {
        return makeWithData(*(double *)d.d, (int *)nullptr, to, &d);
    } else if (d.t == PREPROCESS_FLOAT && to == PREPROCESS_LONG) {
        return makeWithData(*(double *)d.d, (long long int *)nullptr, to, &d);
    }
    Data r;
    r.l = -1;
    return r;
}

Data operation(Data d, byte opcode) {
    Data dr;
    if (d.t == PREPROCESS_INT) {
        dr = dataMakeFrom(processOperator(*(int *)d.d, opcode), PREPROCESS_INT);
    } else if (d.t == PREPROCESS_LONG) {
        dr = dataMakeFrom(processOperator(*(long long int *)d.d, opcode), PREPROCESS_LONG);
    } else if (d.t == PREPROCESS_FLOAT) {
        dr = dataMakeFrom(processOperator(*(double *)d.d, opcode), PREPROCESS_FLOAT);
    }
    d.drop();
    return dr;
}

Data operation(Data d1, Data d2, byte opcode, bool force_drop = true) {
    Data res;
    if (d1.t != d2.t) {
        int m = std::max(d1.t, d2.t);
        if (d1.t != PROCESS_LIST) {
            d1 = type_cast(d1, m);
        }
        if (d2.t != PROCESS_LIST) {
            d2 = type_cast(d2, m);
        }
    }
    if (OPERATOR_PARAMETERS[opcode] == 2) {
        if (d1.t == PREPROCESS_INT) {
            res = dataMakeFrom(processOperator(*(int *)d1.d, *(int *)d2.d, opcode), PREPROCESS_INT);
        } else if (d1.t == PREPROCESS_LONG) {
            res = dataMakeFrom(processOperator(*(long long int *)d1.d, *(long long int *)d2.d, opcode), PREPROCESS_LONG);
        } else if (d1.t == PREPROCESS_FLOAT) {
            res = dataMakeFrom(processOperator(*(double *)d1.d, *(double *)d2.d, opcode), PREPROCESS_FLOAT);
        } else if (d1.t == PROCESS_LIST) {
            res.d = ebyeProcess((KipaList *)d1.d, (KipaList *)d2.d, opcode);
            res.t = PROCESS_LIST;
            res.l = sizeof(KipaList);
            return res;
        }
    } else if (OPERATOR_PARAMETERS[opcode] == 18) {
        int res_int;
        if (d1.t == PREPROCESS_INT) {
            res_int = processBoolOperator(*(int *)d1.d, *(int *)d2.d, opcode);
        } else if (d1.t == PREPROCESS_LONG) {
            res_int = processBoolOperator(*(long long int *)d1.d, *(long long int *)d2.d, opcode);
        } else if (d1.t == PREPROCESS_FLOAT) {
            res_int = processBoolOperator(*(double *)d1.d, *(double *)d2.d, opcode);
        }
        res = dataMakeFrom(res_int, PREPROCESS_INT);
    }
    if (force_drop) {
        d1.drop();
        d2.drop();
    }
    return res;
}

Data cloneAll(Data d) {
    if (d.t != PROCESS_LIST) {
        return d.clone();
    }
    KipaList *o = (KipaList *)d.d;
    KipaList *k = new KipaList;
    int i, sz = (int)o->v->size();
    for (i=0; i<sz; ++i) {
        k->push(cloneAll((*o->v)[i]));
    }
    Data dr;
    dr.d = k;
    dr.t = PROCESS_LIST;
    dr.l = sizeof(KipaList);
    return dr;
}

#endif /* Data_h */
