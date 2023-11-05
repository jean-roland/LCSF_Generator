/**
 * \file enumtype.h
 * \brief Eunm type header
 * \author Jean-Roland Gosse

    This file is part of LCSF Generator.

    LCSF Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LCSF Generator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this project. If not, see <https://www.gnu.org/licenses/>
 */

#ifndef ENUMTYPE_H
#define ENUMTYPE_H

#include <QStringList>

namespace NS_AttDataType {
    typedef enum {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        FLOAT32,
        FLOAT64,
        BYTE_ARRAY,
        SUB_ATTRIBUTES,
        STRING,
        UNKNOWN,
    }T_AttDataType;

    const T_AttDataType SLAttDataType2Enum[9] = { UINT8, UINT16, UINT32, UINT64, FLOAT32, FLOAT64, BYTE_ARRAY, SUB_ATTRIBUTES, STRING };

    const QStringList SL_AttDataType = (QStringList() << "LCSF_UINT8" << "LCSF_UINT16" << "LCSF_UINT32" << "LCSF_UINT64" 
        << "LCSF_FLOAT32" << "LCSF_FLOAT64" << "LCSF_BYTE_ARRAY" << "LCSF_SUB_ATTRIBUTES" << "LCSF_STRING");
    const QStringList SL_DocAttDataType = (QStringList() << "(u)int8" << "(u)int16" << "(u)int32" << "(u)int64" 
        << "float32" << "float64" << "byte array" << "sub-attributes" << "string");
}

namespace NS_DirectionType {
    typedef enum {
        A_TO_B,
        B_TO_A,
        BIDIRECTIONAL,
        UNKNOWN,
    }T_DirectionType;

    const T_DirectionType SLDirectionType2Enum[3] = { A_TO_B, B_TO_A, BIDIRECTIONAL };

    const QStringList SL_DirectionType = (QStringList() << "A to B" << "B to A" << "Bidirectional");
}

#endif // ENUMTYPE_H
