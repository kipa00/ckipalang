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
