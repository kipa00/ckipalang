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

#ifndef Interpreter_h
#define Interpreter_h

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "CodeConstant.h"
#include "Data.h"
#include "LinkedList.h"
#include "StringList.h"
using std::make_pair;
using std::pair;
using std::string;
using std::vector;

typedef pair<void *, int> entry;

bool equals(const char *start, const char *end, const char *value) {
    for (const char *it = start; it != end; ++it) {
        if (*it != value[it - start]) return false;
    }
    return !value[end - start];
}

inline bool is_literal_type(int t) {
    return t >= 0 || t == (0x80000000 | OPERATOR_SCANINT);
}

inline bool compare_operator(pair<int, LinkedList<entry> *> a, pair<int, LinkedList<entry> *> b) {
    return a.first > b.first;
}

pair<pair<int, int>, byte *> makeCode(const char *code) {
    // Step 1: Parse
    vector<pair<pair<int, int>, int> > parsed;
    int string_mode = 0, last_mode = -1;
    int i, s = 0;
    for (i=0; code[i]; ++i) {
        if (!string_mode) {
            if (code[i] == 34) {
                if (last_mode >= 0) {
                    parsed.push_back(make_pair(make_pair(s, i), last_mode));
                }
                s = i + 1;
                string_mode = 1;
                last_mode = PARSE_MODE_STRING;
            } else {
                int mode = PARSE_MODE_SPECIAL;
                if (code[i] == 32 || code[i] == 10 || code[i] == 13 || code[i] == '\t') {
                    mode = -1;
                } else if (code[i] == 46 || (48 <= code[i] && code[i] < 58)) {
                    mode = PARSE_MODE_INT;
                    if (last_mode == PARSE_MODE_FLOAT) {
                        mode = PARSE_MODE_FLOAT;
                    }
                } else if ((65 <= code[i] && code[i] <= 90) || code[i] == 95 || (97 <= code[i] && code[i] <= 122)) {
                    mode = PARSE_MODE_ALPHABET;
                }
                if ((mode != last_mode) || (mode == PARSE_MODE_SPECIAL && !(code[i-1] == code[i] && code[i] == '='))) {
                    if (last_mode >= 0) {
                        parsed.push_back(make_pair(make_pair(s, i), last_mode));
                    }
                    last_mode = mode;
                    s = i;
                }
                if (code[i] == 46) {
                    last_mode = PARSE_MODE_FLOAT;
                }
            }
        } else if (string_mode == 1) {
            if (code[i] == '\\') {
                string_mode = 2;
            } else if (code[i] == '"') {
                parsed.push_back(make_pair(make_pair(s, i), PARSE_MODE_STRING));
                s = i + 1;
                string_mode = 0;
                last_mode = -1;
            }
        } else if (string_mode == 2) {
            string_mode = 1;
        }
    }
    if (last_mode >= 0) {
        parsed.push_back(make_pair(make_pair(s, i), last_mode));
    }
    // Step 2: Preprocess
    LinkedList<entry> *entries = (LinkedList<entry> *)malloc(parsed.size() * sizeof(LinkedList<entry>));
    vector<string> names;
    names.push_back("e");
    names.push_back("pi");
    vector<pair<int, LinkedList<entry> *> > priority;
    int nowprior = 0;
    const int pconst = 1024;
    entries[0].initialize(parsed.size());
    int entries_len = 0;
    for (vector<pair<pair<int, int>, int> >::iterator it = parsed.begin(); it != parsed.end(); ++it) {
        if (it->second == PARSE_MODE_INT) {
            long long int s = 0;
            for (i=it->first.first; i<it->first.second; ++i) {
                s = (s * 10) + (code[i] & 15);
            }
            entry *temp = &entries[entries_len++].v;
            if (s & 0xFFFFFFFF00000000L) {
                temp->first = (void *)malloc(temp->second = 1 + sizeof(long long int));
                *(byte *)temp->first = PREPROCESS_LONG;
                memcpy((byte *)temp->first + 1, (void *)&s, sizeof(long long int));
            } else {
                temp->first = (void *)malloc(temp->second = 1 + sizeof(int));
                *(byte *)temp->first = PREPROCESS_INT;
                memcpy((byte *)temp->first + 1, (void *)&s, sizeof(int));
            }
        } else if (it->second == PARSE_MODE_FLOAT) {
            double d;
            sscanf(code + it->first.first, "%lf", &d);
            entry *temp = &entries[entries_len++].v;
            temp->first = (void *)malloc(temp->second = 1 + sizeof(double));
            *(byte *)temp->first = PREPROCESS_FLOAT;
            memcpy((byte *)temp->first + 1, (void *)&d, sizeof(double));
        } else if (it->second == PARSE_MODE_STRING) {
            entry *temp = &entries[entries_len++].v;
            pair<int, byte *> str = fromUTF8(code + it->first.first, code + it->first.second);
            temp->first = str.second;
            temp->second = str.first;
        } else {
            int op = -1;
            const char *s = code + it->first.first, *e = code + it->first.second;
            if (it->second == PARSE_MODE_ALPHABET) {
                if (equals(s, e, "print")) {
                    op = OPERATOR_PRINT;
                } else if (equals(s, e, "putch")) {
                    op = OPERATOR_PUTCH;
                } else if (equals(s, e, "if")) {
                    op = OPERATOR_IF;
                } else if (equals(s, e, "in")) {
                    op = OPERATOR_IN;
                } else if (equals(s, e, "range")) {
                    op = OPERATOR_RANGE; 
                } else if (equals(s, e, "len")) {
                    op = OPERATOR_LEN;
                } else if (equals(s, e, "scanint")) {
                    op = OPERATOR_SCANINT;
                } else if (equals(s, e, "while")) {
                    op = OPERATOR_WHILE;
                }
            } else if (it->second == PARSE_MODE_SPECIAL) {
                const char *one_op = "+-*/+++++<>%,;=";
                if (it->first.second - it->first.first == 1) {
                    const char *idx;
                    if ((idx = strchr(one_op, code[it->first.first]))) {
                        op = (int)(idx - one_op);
                        if (op == 0 || op == 1) {
                            if (!entries_len || !is_literal_type(entries[entries_len - 1].v.second)) {
                                op += OPERATOR_NOTHING;
                            }
                        }
                    } else if (code[it->first.first] == '(') {
                        nowprior += pconst;
                        continue;
                    } else if (code[it->first.first] == ')') {
                        nowprior -= pconst;
                        continue;
                    } else if (code[it->first.first] == '[') {
                        nowprior += pconst;
                        op = OPERATOR_INDEX;
                    } else if (code[it->first.first] == ']') {
                        nowprior -= pconst;
                        continue;
                    }
                } else if (equals(s, e, "==")) {
                    op = OPERATOR_EQUALS;
                }
            }
            if (op >= 0) {
                entry *temp = &entries[entries_len].v;
                temp->second = 0x80000000 | op;
                priority.push_back(make_pair(nowprior + OPERATOR_PRIORITY[op], &entries[entries_len++]));
            } else if (it->second == PARSE_MODE_ALPHABET) {
                entry *temp = &entries[entries_len++].v;
                temp->first = (void *)malloc(temp->second = 1 + sizeof(int));
                *(byte *)temp->first = PREPROCESS_VARIABLE;
                int idx = (int)names.size();
                for (vector<string>::iterator it = names.begin(); it != names.end(); ++it) {
                    if (equals(s, e, (*it).c_str())) {
                        idx = (int)(it - names.begin());
                        break;
                    }
                }
                memcpy((byte *)temp->first + 1, (void *)&idx, sizeof(int));
                if (idx >= names.size()) {
                    int len = it->first.second - it->first.first;
                    char *name = (char *)malloc(len + 1);
                    memcpy(name, s, len);
                    name[len] = 0;
                    names.push_back(name);
                    free(name);
                }
            } else {
                // TODO: OperatorNotFoundError
            }
        }
    }
    entries[entries_len - 1].n = nullptr;
    // This is only a debugging code
#ifdef DEBUG_MODE
    LinkedList<entry> *a = entries;
    for (; a; a = a->n) {
        int b = a->v.second;
        if (b > 0) {
            for (i=0; i<b; ++i) {
                printf("%02X ", (int)*((byte *)a->v.first + i));
            }
            puts("");
        } else {
            printf("operator %d, %p\n", b & 0x7FFFFFFF, a);
        }
    }
#endif
    // Step 3: Make code with priority
    stable_sort(priority.begin(), priority.end(), compare_operator);
    LinkedList<entry> *first = entries;
    for (vector<pair<int, LinkedList<entry> *> >::iterator it = priority.begin(); it != priority.end(); ++it) {
        LinkedList<entry> *nowop = it->second;
        LinkedList<entry> *start = nowop, *end = nowop, *it2;
        int len = 0, opcode = nowop->v.second & 0x7FFFFFFF;
        for (i=0; i<OPERATOR_ORDER[opcode][0]; ++i) {
            start = start->l;
            if (start->v.second >= 0) len += start->v.second;
            else {
                // OperatorError
            }
        }
        for (i=0; i<OPERATOR_ORDER[opcode][1]; ++i) {
            end = end->n;
            if (end->v.second >= 0) len += end->v.second;
            else {
                // OperatorError
            }
        }
        if (opcode == OPERATOR_IN) {
            LinkedList<entry> *state = nowop->l->l, *var = nowop->l, *ls = nowop->n;
            void *nptr = nullptr;
            nowop->v.first = malloc(nowop->v.second = len + 3 * sizeof(void *) + 2 * sizeof(int) + 7);
            byte *temp = (byte *)nowop->v.first;
            memcpy(temp, var->v.first, var->v.second);
            temp += var->v.second;
            memcpy(temp, ls->v.first, ls->v.second);
            temp += ls->v.second;
            for (int th=2;th--;) {
                *temp++ = PREPROCESS_POINTER;
                memcpy(temp, &nptr, sizeof(void *));
                temp += sizeof(void *);
            }
            *temp++ = PREPROCESS_INT;
            int len = (state->v.second + 2 + sizeof(int));
            memcpy(temp, &len, sizeof(int));
            temp += sizeof(int);
            *temp++ = PREPROCESS_POINTER;
            memcpy(temp, &nptr, sizeof(void *));
            temp += sizeof(void *);
            *temp++ = (byte)(PREPROCESS_OPERATION | opcode);
            memcpy(temp, state->v.first, state->v.second);
            temp += state->v.second;
            len = -(state->v.second + 3 + sizeof(int));
            *temp++ = PREPROCESS_INT;
            memcpy(temp, &len, sizeof(int));
            temp += sizeof(int);
            *temp++ = (byte)(PREPROCESS_OPERATION | OPERATOR_JUMP);
        } else if (opcode == OPERATOR_IF) {
            LinkedList<entry> *cond = start->n, *state = start->n->n;
            nowop->v.first = malloc(nowop->v.second = 2 + len + sizeof(int));
            byte *temp = (byte *)nowop->v.first;
            memcpy(temp, cond->v.first, cond->v.second);
            temp += cond->v.second;
            *temp++ = PREPROCESS_INT;
            memcpy(temp, &state->v.second, sizeof(int));
            temp += sizeof(int);
            *temp++ = (byte)(PREPROCESS_OPERATION | opcode);
            memcpy(temp, state->v.first, state->v.second);
        } else if (opcode == OPERATOR_WHILE) { // 0 <cond> <idx> if pop <state> <idx> jump pop
            LinkedList<entry> *cond = start->n, *state = start->n->n;
            nowop->v.first = malloc(nowop->v.second = 7 + sizeof(int) * 3 + len);
            byte *temp = (byte *)nowop->v.first;
            *temp++ = PREPROCESS_INT;
            int l = 0;
            memcpy(temp, &l, sizeof(int));
            temp += sizeof(int);
            memcpy(temp, cond->v.first, cond->v.second);
            temp += cond->v.second;
            *temp++ = PREPROCESS_INT;
            l = state->v.second + 3 + sizeof(int);
            memcpy(temp, &l, sizeof(int));
            temp += sizeof(int);
            *temp++ = (byte)(PREPROCESS_OPERATION | OPERATOR_IF);
            *temp++ = (byte)(PREPROCESS_OPERATION | OPERATOR_POP);
            memcpy(temp, state->v.first, state->v.second);
            temp += state->v.second;
            *temp++ = PREPROCESS_INT;
            l = -l - 2 - sizeof(int) - cond->v.second;
            memcpy(temp, &l, sizeof(int));
            temp += sizeof(int);
            *temp++ = (byte)(PREPROCESS_OPERATION | OPERATOR_JUMP);
            *temp++ = (byte)(PREPROCESS_OPERATION | OPERATOR_POP);
        } else {
            nowop->v.first = malloc(nowop->v.second = 1 + len);
            byte *temp = (byte *)nowop->v.first;
            for (it2 = start; it2 != end->n; it2 = it2->n) {
                if (it2 != nowop) {
                    memcpy(temp, it2->v.first, it2->v.second);
                    temp += it2->v.second;
                    free(it2->v.first);
                }
            }
            *temp = (byte)(PREPROCESS_OPERATION | opcode);
        }
        if (start->l) {
            start->l->n = nowop;
        } else {
            first = nowop;
        }
        if (end->n) {
            end->n->l = nowop;
        }
        nowop->l = start->l;
        nowop->n = end->n;
    }
    if (first->n) {
        // OperandTieError
    }
    int sz = first->v.second;
    byte *result = (byte *)malloc(sz);
    memcpy(result, first->v.first, sz);
    free(first->v.first);
    free(entries);
    return make_pair(make_pair(sz, names.size()), result);
}

#endif /* Interpreter_h */
