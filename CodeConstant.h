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

#ifndef CodeConstant_h
#define CodeConstant_h

typedef unsigned char byte;

const int PARSE_MODE_INT = 0;
const int PARSE_MODE_FLOAT = 1;
const int PARSE_MODE_ALPHABET = 2;
const int PARSE_MODE_SPECIAL = 3;
const int PARSE_MODE_STRING = 4;

const byte PREPROCESS_INT = 0;
const byte PREPROCESS_LONG = 1;
const byte PREPROCESS_FLOAT = 2;
const byte PREPROCESS_STRING = 3;
const byte PREPROCESS_VARIABLE = 4;
const byte PREPROCESS_POINTER = 5;
const byte PREPROCESS_OPERATION = 128;

const byte PROCESS_LIST = 3;
const byte PROCESS_POINTER = 5;

const byte OPERATOR_PLUS = 0;
const byte OPERATOR_MINUS = 1;
const byte OPERATOR_MULTIPLICATION = 2;
const byte OPERATOR_DIVISION = 3;
const byte OPERATOR_NOTHING = 4;
const byte OPERATOR_NEGATE = 5;
const byte OPERATOR_PRINT = 6;
const byte OPERATOR_PUTCH = 7;
const byte OPERATOR_EQUALS = 8;
const byte OPERATOR_LESSTHAN = 9;
const byte OPERATOR_GREATERTHAN = 10;
const byte OPERATOR_REMAINDER = 11;
const byte OPERATOR_CONCATENATE = 12;
const byte OPERATOR_SEPARATOR = 13;
const byte OPERATOR_SUBSTITUTE = 14;
const byte OPERATOR_IF = 15;
const byte OPERATOR_INDEX = 16;
const byte OPERATOR_IN = 17;
const byte OPERATOR_RANGE = 18;
const byte OPERATOR_LEN = 19;
const byte OPERATOR_SCANINT = 20;
const byte OPERATOR_SUBSTITUTE_IMM = 21;
const byte OPERATOR_WHILE = 22; // unused by the byte code
const byte OPERATOR_JUMP = 23; // unused by the string code
const byte OPERATOR_POP = 24; // unused by the string code

const int OPERATOR_PRIORITY[] = {
    50, 50, 60, 60, 70,
    70, 30, 30, 40, 40,
    40, 60, 20, 0, 10,
    5, 80, 3, 90, 90,
    90, 10, 5
};
const int OPERATOR_ORDER[][2] = {
    {1, 1}, {1, 1}, {1, 1}, {1, 1}, {0, 1},
    {0, 1}, {0, 1}, {0, 1}, {1, 1}, {1, 1},
    {1, 1}, {1, 1}, {1, 1}, {1, 1}, {1, 1},
    {0, 2}, {1, 1}, {2, 1}, {0, 1}, {0, 1},
    {0, 0}, {1, 1}, {0, 2}
};
const int OPERATOR_PARAMETERS[] = {
    2, 2, 2, 2, 1,
    1, 1, 1, 18, 18,
    18, 2, 34, 2, 2,
    2, 2, 3, 1, 1,
    0, 2, 2
};

#endif /* CodeConstant_h */
