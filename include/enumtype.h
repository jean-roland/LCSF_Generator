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
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64,
    BYTE_ARRAY,
    SUB_ATTRIBUTES,
    STRING,
    STRUCT,
    UNKNOWN,
} T_AttDataType;

const T_AttDataType SLAttDataType2Enum[UNKNOWN] = {
    UINT8, UINT16, UINT32, UINT64, INT8, INT16, INT32, INT64, FLOAT32, FLOAT64, BYTE_ARRAY, SUB_ATTRIBUTES, STRING, STRUCT};

const QStringList SL_AttDataType =
    (QStringList() << "LCSF_UINT8" << "LCSF_UINT16" << "LCSF_UINT32" << "LCSF_UINT64" << "LCSF_INT8" << "LCSF_INT16"
                   << "LCSF_INT32" << "LCSF_INT64" << "LCSF_FLOAT32" << "LCSF_FLOAT64" << "LCSF_BYTE_ARRAY"
                   << "LCSF_SUB_ATTRIBUTES" << "LCSF_STRING" << "LCSF_STRUCT");
const QStringList SL_DocAttDataType =
    (QStringList() << "uint8" << "uint16" << "uint32" << "uint64" << "int8" << "int16" << "int32" << "int64" << "float32"
                   << "float64" << "byte array" << "sub-attributes" << "string" << "struct");

// Data type the stacks know: signed integers travel as the unsigned type of their width, structs as byte arrays
inline T_AttDataType getStackDataType(T_AttDataType dataType) {
    switch (dataType) {
        case INT8:
            return UINT8;
        case INT16:
            return UINT16;
        case INT32:
            return UINT32;
        case INT64:
            return UINT64;
        case STRUCT:
            return BYTE_ARRAY;
        default:
            return dataType;
    }
}
} // namespace NS_AttDataType

namespace NS_FieldType {
typedef enum {
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    INT8,
    INT16,
    INT32,
    INT64,
    FLOAT32,
    FLOAT64,
    VARUINT16,
    VARUINT32,
    VARUINT64,
    VARINT16,
    VARINT32,
    VARINT64,
    UNKNOWN,
} T_FieldType;

const QStringList SL_FieldType =
    (QStringList() << "LCSF_UINT8" << "LCSF_UINT16" << "LCSF_UINT32" << "LCSF_UINT64" << "LCSF_INT8" << "LCSF_INT16"
                   << "LCSF_INT32" << "LCSF_INT64" << "LCSF_FLOAT32" << "LCSF_FLOAT64" << "LCSF_VARUINT16"
                   << "LCSF_VARUINT32" << "LCSF_VARUINT64" << "LCSF_VARINT16" << "LCSF_VARINT32" << "LCSF_VARINT64");
const QStringList SL_DocFieldType =
    (QStringList() << "uint8" << "uint16" << "uint32" << "uint64" << "int8" << "int16" << "int32" << "int64" << "float32"
                   << "float64" << "varuint16" << "varuint32" << "varuint64" << "varint16" << "varint32" << "varint64");
const QStringList SL_CFieldType =
    (QStringList() << "uint8_t" << "uint16_t" << "uint32_t" << "uint64_t" << "int8_t" << "int16_t" << "int32_t" << "int64_t"
                   << "float" << "double" << "uint16_t" << "uint32_t" << "uint64_t" << "int16_t" << "int32_t" << "int64_t");
const QStringList SL_RustFieldType = (QStringList() << "u8" << "u16" << "u32" << "u64" << "i8" << "i16" << "i32" << "i64"
                                                    << "f32" << "f64" << "u16" << "u32" << "u64" << "i16" << "i32" << "i64");
// Wire size in bytes, maximum for variable-length types (7 value bits per byte)
const int FieldTypeSize[UNKNOWN] = {1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 3, 5, 10, 3, 5, 10};
// Value width in bits
const int FieldTypeBits[UNKNOWN] = {8, 16, 32, 64, 8, 16, 32, 64, 32, 64, 16, 32, 64, 16, 32, 64};

// Variable-length types are unsigned LEB128, signed ones zigzag encoded first
inline bool isVarType(T_FieldType type) {
    return (type >= VARUINT16) && (type <= VARINT64);
}

inline bool isSignedVarType(T_FieldType type) {
    return (type >= VARINT16) && (type <= VARINT64);
}
} // namespace NS_FieldType

namespace NS_DirectionType {
typedef enum {
    A_TO_B,
    B_TO_A,
    BIDIRECTIONAL,
    UNKNOWN,
} T_DirectionType;

const T_DirectionType SLDirectionType2Enum[3] = {A_TO_B, B_TO_A, BIDIRECTIONAL};

const QStringList SL_DirectionType = (QStringList() << "A to B" << "B to A" << "Bidirectional");
} // namespace NS_DirectionType

#endif // ENUMTYPE_H
