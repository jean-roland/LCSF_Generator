#ifndef ENUMTYPE_H
#define ENUMTYPE_H

#include <QStringList>

namespace NS_AttDataType {
    typedef enum {
        UINT8,
        UINT16,
        UINT32,
        BYTE_ARRAY,
        SUB_ATTRIBUTES,
        STRING,
        UNKNOWN,
    }T_AttDataType;

    const T_AttDataType SLAttDataType2Enum[6] = { UINT8, UINT16, UINT32, BYTE_ARRAY, SUB_ATTRIBUTES, STRING };

    const QStringList SL_AttDataType = (QStringList() << "LCSF_UINT8" << "LCSF_UINT16" << "LCSF_UINT32" << "LCSF_BYTE_ARRAY" << "LCSF_SUB_ATTRIBUTES" << "LCSF_STRING");
    const QStringList SL_DocAttDataType = (QStringList() << "(u)int8" << "(u)int16" << "(u)int32" << "byte array" << "sub-attributes" << "string");
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
