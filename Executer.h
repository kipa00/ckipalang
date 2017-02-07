//
//  Executer.h
//  KipaLang
//
//  Created by Kim Yeong-Hyeon on 2017. 1. 27..
//  Copyright © 2017년 Kim Yeong-Hyeon. All rights reserved.
//

#ifndef Executer_h
#define Executer_h

#include <string>
#include "CodeConstant.h"
#include "Data.h"
#include "InputManager.h"
#include "KipaList.h"
using std::string;

inline byte readByte(const byte **cpt) {
    return *(*cpt)++;
}

inline unsigned short readShort(const byte **cpt) {
    byte lower = readByte(cpt);
    return ((unsigned short)readByte(cpt) << 8) | lower;
}

inline int readInt(const byte **cpt) {
    int res;
    memcpy(&res, *cpt, sizeof(int));
    (*cpt) += sizeof(int);
    return res;
}

inline Data evaluate(Data d, vector<Data>::iterator it) {
    if (d.t == PREPROCESS_VARIABLE) {
        it += *(int *)d.d;
        return cloneAll(*it);
    } else if (d.t == PROCESS_POINTER) {
        return cloneAll(**(Data **)d.d);
    }
    return d;
}

inline string toString(Data d) {
    string res = "";
    if (d.t == PROCESS_LIST) {
        KipaList *ld = (KipaList *)d.d;
        res = "(";
        for (vector<Data>::iterator it = ld->v->begin(); it != ld->v->end(); ++it) {
            res += toString(*it) + ", ";
        }
        res = res.substr(0, res.length() - 2) + ")";
    } else {
        char *buf = (char *)malloc(50); // sufficiently large?
        if (d.t == PREPROCESS_INT) {
            sprintf(buf, "%d", *(int *)d.d);
        } else if (d.t == PREPROCESS_LONG) {
            sprintf(buf, "%lld", *(long long int *)d.d);
        } else if (d.t == PREPROCESS_FLOAT) {
            sprintf(buf, "%.12lf", *(double *)d.d);
        }
        res += buf;
        free(buf);
    }
    return res;
}

inline string putch(Data d) {
    string res = "";
    if (d.t == PROCESS_LIST) {
        KipaList *ld = (KipaList *)d.d;
        for (vector<Data>::iterator it = ld->v->begin(); it != ld->v->end(); ++it) {
            res += putch(*it);
        }
    } else {
        int uni = 0;
        if (d.t == PREPROCESS_INT) {
            uni = *(int *)d.d;
        } else if (d.t == PREPROCESS_LONG) {
            uni = (int)*(long long int *)d.d;
        } else if (d.t == PREPROCESS_FLOAT) {
            uni = (int)*(double *)d.d;
        }
        char *t = toUTF8(uni);
        res += t;
        free(t);
    }
    return res;
}

inline void subst(Data d1, Data d2, vector<Data>::iterator iter) {
    if (d1.t == PREPROCESS_VARIABLE) {
        (iter + *(int *)d1.d)->drop();
        *(iter + *(int *)d1.d) = cloneAll(d2);
    } else if (d1.t == PROCESS_POINTER) {
        Data *d = *(Data **)d1.d;
        d->drop();
        *d = cloneAll(d2);
    } else {
        // TODO: SubstitutionError
    }
}

string executeCode(const byte *code, int len, int varlen) {
    const byte *now = code;
    vector<Data> literals;
    vector<Data> variables;
    variables.push_back(dataMakeFrom(2.718281828459045, PREPROCESS_FLOAT));
    variables.push_back(dataMakeFrom(3.141592653589393, PREPROCESS_FLOAT));
    while (variables.size() < varlen) {
        variables.push_back(dataMakeFrom(0, PREPROCESS_INT));
    }
    string printBuffer;
    int i;
    while (now < code + len) {
        byte opcode = readByte(&now);
        int temp_int;
        long long int temp_long;
        double temp_float;
        void *temp_pointer;
        KipaList *temp_kl;
        switch (opcode) {
            case PREPROCESS_INT:
            case PREPROCESS_VARIABLE:
                temp_int = readInt(&now);
                literals.push_back(dataMakeFrom(temp_int, opcode));
                break;
            case PREPROCESS_LONG:
                temp_long = (unsigned int)readInt(&now);
                temp_long |= (long long int)readInt(&now) << 32;
                literals.push_back(dataMakeFrom(temp_long, PREPROCESS_LONG));
                break;
            case PREPROCESS_FLOAT:
                memcpy(&temp_float, now, sizeof(double));
                now += sizeof(double);
                literals.push_back(dataMakeFrom(temp_float, PREPROCESS_FLOAT));
                break;
            case PREPROCESS_POINTER:
                memcpy(&temp_pointer, now, sizeof(void *));
                now += sizeof(void *);
                literals.push_back(dataMakeFrom(temp_pointer, PROCESS_POINTER));
                break;
            case PREPROCESS_STRING:
            {
                Data d;
                temp_kl = new KipaList;
                int sz = (int)(unsigned short)readShort(&now), i;
                for (i=0; i<sz; ++i) {
                    temp_kl->push(dataMakeFrom((int)(unsigned short)readShort(&now), PREPROCESS_INT));
                }
                d.d = temp_kl;
                d.l = sizeof(KipaList);
                d.t = PROCESS_LIST;
                literals.push_back(d);
                break;
            }
            default:
            {
                opcode &= ~PREPROCESS_OPERATION;
                Data d1, d2, d;
                switch (opcode) {
                    case OPERATOR_SUBSTITUTE:
                        d2 = evaluate(literals.back(), variables.begin());
                        literals.pop_back();
                        d1 = literals.back();
                        subst(d1, d2, variables.begin());
                        d2.drop();
                        break;
                    case OPERATOR_INDEX:
                        d2 = type_cast(evaluate(literals.back(), variables.begin()), PREPROCESS_INT);
                        literals.pop_back();
                        d1 = literals.back();
                        if (d1.t == PROCESS_POINTER) {
                            Data dt = **(Data **)d1.d;
                            d1.drop();
                            d1 = dt;
                        }
                        if (d1.t == PREPROCESS_VARIABLE) {
                            Data dt = variables[*(int *)d1.d];
                            d1.drop();
                            d1 = dt;
                        }
                        if (d1.t != PROCESS_LIST) {
                            d1 = type_cast(d1, PROCESS_LIST);
                        }
                        literals.pop_back();
                        literals.push_back(dataMakeFrom(&*(((KipaList *)d1.d)->v->begin() + *(int *)d2.d), PROCESS_POINTER));
                        break;
                    case OPERATOR_SEPARATOR:
                        d2 = evaluate(literals.back(), variables.begin());
                        literals.pop_back();
                        literals.back().drop();
                        literals.pop_back();
                        literals.push_back(d2);
                        break;
                    case OPERATOR_CONCATENATE:
                    {
                        Data d2 = evaluate(literals.back(), variables.begin());
                        literals.pop_back();
                        Data d1 = evaluate(literals.back(), variables.begin());
                        literals.pop_back();
                        if (d1.t != PROCESS_LIST) {
                            d1 = type_cast(d1, PROCESS_LIST);
                        }
                        if (d2.t != PROCESS_LIST) {
                            d2 = type_cast(d2, PROCESS_LIST);
                        }
                        KipaList *res = new KipaList;
                        KipaList *fnt = (KipaList *)d1.d;
                        KipaList *bck = (KipaList *)d2.d;
                        vector<Data>::iterator it2;
                        for (it2 = fnt->v->begin(); it2 != fnt->v->end(); ++it2) {
                            res->push(*it2);
                        }
                        for (it2 = bck->v->begin(); it2 != bck->v->end(); ++it2) {
                            res->push(*it2);
                        }
                        fnt->drop();
                        bck->drop();
                        delete fnt;
                        delete bck;
                        d1.d = res;
                        literals.push_back(d1);
                        break;
                    }
                    case OPERATOR_IN: // 5<var> 4<ls> 3<lsitr> 2<resls> 1<idx> 0<res> in
                    {
                        vector<Data> tvc;
                        tvc.reserve(6);
                        for (int th=6;th--;) {
                            tvc.push_back(literals.back());
                            literals.pop_back();
                        }
                        tvc[4] = evaluate(tvc[4], variables.begin());
                        if (tvc[4].t != PROCESS_LIST) {
                            tvc[4] = type_cast(tvc[4], PROCESS_LIST);
                        }
                        if (__builtin_expect((!((KipaList *)tvc[4].d)->v->size()), 0)) {
                            KipaList *kl = new KipaList;
                            Data d;
                            d.d = kl;
                            d.t = PROCESS_LIST;
                            d.l = sizeof(KipaList);
                            literals.push_back(d);
                            now += *(int *)tvc[1].d;
                            for (int th=6; th--;) {
                                tvc[th].drop();
                            }
                        } else if (tvc[0].t == PREPROCESS_POINTER && !*(void **)tvc[0].d) {
                            KipaList *kl = new KipaList;
                            kl->v->reserve(((KipaList *)tvc[4].d)->v->size());
                            tvc[3].drop();
                            tvc[3] = dataMakeFrom(((KipaList *)tvc[4].d)->v->begin(), PROCESS_POINTER);
                            tvc[2].drop();
                            tvc[2].d = kl;
                            tvc[2].l = sizeof(KipaList);
                            tvc[2].t = PROCESS_LIST;
                            subst(tvc[5], **(vector<Data>::iterator *)tvc[3].d, variables.begin());
                            for (int th=5;th--;) {
                                literals.push_back(tvc.back());
                                tvc.pop_back();
                            }
                            tvc[0].drop();
                        } else {
                            vector<Data>::iterator *lsitrptr = (vector<Data>::iterator *)tvc[3].d;
                            ((KipaList *)tvc[2].d)->push(evaluate(tvc[0], variables.begin()));
                            if (tvc[0].t == PREPROCESS_VARIABLE || tvc[0].t == PROCESS_LIST) {
                                tvc[0].drop();
                            }
                            ++*lsitrptr;
                            if (*lsitrptr == ((KipaList *)tvc[4].d)->v->end()) {
                                literals.push_back(tvc[2]);
                                now += *(int *)tvc[1].d;
                                tvc[1].drop();
                                tvc[3].drop();
                                tvc[4].drop();
                                tvc[5].drop();
                            } else {
                                subst(tvc[5], **lsitrptr, variables.begin());
                                for (int th=5;th--;) {
                                    literals.push_back(tvc.back());
                                    tvc.pop_back();
                                }
                            }
                        }
                        break;
                    }
                    case OPERATOR_IF:
                    {
                        int idx = *(int *)literals.back().d;
                        literals.back().drop();
                        literals.pop_back();
                        d = evaluate(literals.back(), variables.begin());
                        bool jump = true;
                        for (i=0; i<d.l; ++i) {
                            if (*((byte *)d.d + i)) {
                                jump = false;
                                break;
                            }
                        }
                        if (literals.back().d != d.d) {
                            literals.back().drop();
                        }
                        d.drop();
                        literals.pop_back();
                        if (jump) {
                            literals.push_back(dataMakeFrom(0, PREPROCESS_INT));
                            now += idx;
                        }
                        break;
                    }
                    case OPERATOR_POP:
                        literals.back().drop();
                        literals.pop_back();
                        break;
                    case OPERATOR_PRINT:
                        printBuffer += toString(evaluate(literals.back(), variables.begin()));
                        break;
                    case OPERATOR_PUTCH:
                        printBuffer += putch(evaluate(literals.back(), variables.begin()));
                        break;
                    case OPERATOR_JUMP:
                        now += *(int *)literals.back().d;
                        literals.back().drop();
                        literals.pop_back();
                        break;
                    case OPERATOR_RANGE:
                    {
                        Data d = evaluate(literals.back(), variables.begin());
                        KipaList *kl = new KipaList;
                        if (d.t == PREPROCESS_INT) {
                            int sz = *(int *)d.d;
                            for (i=0; i<sz; ++i) {
                                kl->push(dataMakeFrom(i, PREPROCESS_INT));
                            }
                        } else if (d.t == PREPROCESS_LONG) {
                            long long int sz = *(long long int *)d.d, i;
                            for (i=0; i<sz; ++i) {
                                kl->push(dataMakeFrom(i, PREPROCESS_LONG));
                            }
                        } else {
                            // Treat as Error
                        }
                        d.drop();
                        literals.back().drop();
                        literals.back().d = kl;
                        literals.back().t = PROCESS_LIST;
                        literals.back().l = sizeof(KipaList);
                        break;
                    }
                    case OPERATOR_LEN:
                    {
                        d = evaluate(literals.back(), variables.begin());
                        Data dr;
                        if (d.t != PROCESS_LIST) {
                            dr = dataMakeFrom(1, PREPROCESS_INT);
                        } else {
                            dr = dataMakeFrom((int)(((KipaList *)d.d)->v->size()), PREPROCESS_INT);
                        }
                        Data dt = literals.back();
                        if (dt.t == PROCESS_POINTER || dt.t == PREPROCESS_VARIABLE) {
                            dt.drop();
                        }
                        literals.pop_back();
                        literals.push_back(dr);
                        d.drop();
                        break;
                    }
                    case OPERATOR_SCANINT:
                        literals.push_back(dataMakeFrom(bufin::scanint(), PREPROCESS_INT));
                        break;
                    default:
                    {
                        if ((OPERATOR_PARAMETERS[opcode] & 15) == 1) {
                            d = operation(evaluate(literals.back(), variables.begin()), opcode);
                            literals.pop_back();
                            literals.push_back(d);
                        } else if ((OPERATOR_PARAMETERS[opcode] & 15) == 2) {
                            d2 = evaluate(literals.back(), variables.begin());
                            literals.pop_back();
                            d1 = evaluate(literals.back(), variables.begin());
                            literals.pop_back();
                            literals.push_back(operation(d1, d2, opcode));
                        }
                    } // end of inner switch statement
                }
            } // end of outer switch statement
        }
#ifdef DEBUG_MODE
        printf("code point %d\n", (int)(now - code));
        for (i=0; i<literals.size(); ++i) {
            printf("%d ", literals[i].t);
            for (int j=0; j<literals[i].l; ++j) {
                printf("%02X ", *((byte *)literals[i].d + j));
            }
            puts("");
        }
        puts("-----");
#endif
    }
#ifdef DEBUG_MODE
    for (i=0; i<literals.size(); ++i) {
        printf("%d ", literals[i].t);
        for (int j=0; j<literals[i].l; ++j) {
            printf("%02X ", *((byte *)literals[i].d + j));
        }
        puts("");
    }
#endif
    if (printBuffer.empty()) {
        return toString(evaluate(literals[0], variables.begin()));
    }
    return printBuffer;
}

#endif /* Executer_h */
