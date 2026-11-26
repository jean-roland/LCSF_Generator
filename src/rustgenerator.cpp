/**
 * \file rustgenerator.cpp
 * \brief Rust code generator module
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

#include <QStringBuilder>

#include "rustextractor.h"
#include "rustgenerator.h"

RustGenerator::RustGenerator() {
}

// Set all chars of a string to lower then the first to upper
bool RustGenerator::is_CString_needed(QList<Attribute::T_attInfos> attInfosList) {
    for (Attribute::T_attInfos attInfo : attInfosList) {
        if (attInfo.dataType == NS_AttDataType::STRING) {
            return true;
        }
    }
    return false;
}

// Set all chars of a string to lower then the first to upper
QString RustGenerator::capitalize(const QString &str) {
    QString result;
    bool capitalizeNext = true;
    for (int i = 0; i < str.length(); ++i) {
        QChar ch = str[i];
        if (ch == '_') {
            capitalizeNext = true;
        } else {
            if (capitalizeNext) {
                result += ch.toUpper();
                capitalizeNext = false;
            } else {
                result += ch.toLower();
            }
        }
    }
    return result;
}

// Retrieve the path of an attribute in a payload struct
QString RustGenerator::getPayloadPath(QStringList parentNames) {
    QString payloadPath = "";
    for (int idx = 0; idx < parentNames.size(); idx++) {
        payloadPath.append(parentNames.at(idx).toLower() + "_payload.");
    }
    return payloadPath;
}

// Convert indentation level number into string of spaces
QString RustGenerator::getIndent(int indentNb) {
    QString indentString = "";
    if (indentNb > 0) {
        for (int idx = 0; idx < indentNb; idx++) {
            indentString.append("    ");
        }
    }
    return indentString;
}

// Recursively flatten an attribute list
QList<Attribute::T_attInfos> RustGenerator::getAttInfos_Rec(QString parentName, QList<Attribute *> attList) {
    QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
    Attribute::T_attInfos localAttInfos;
    for (Attribute *attribute : attList) {
        localAttInfos.parentName = parentName;
        localAttInfos.attName = attribute->getName();
        localAttInfos.attId = attribute->getId();
        localAttInfos.attPointer = attribute;
        localAttInfos.dataType = attribute->getDataType();
        localAttInfos.isOptional = attribute->getIsOptional();
        localAttInfos.subAttNb = attribute->getSubAttArray().size();
        localAttInfos.attDesc = attribute->getDesc();
        resultList.append(localAttInfos);
        if ((attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) && (localAttInfos.subAttNb > 0)) {
            if (!this->protocolHasSubAtt) {
                this->protocolHasSubAtt = true;
            }
            resultList.append(this->getAttInfos_Rec(localAttInfos.attName, attribute->getSubAttArray()));
        }
    }
    return resultList;
}

// Header function for getAttInfos_Rec
QList<Attribute::T_attInfos> RustGenerator::getAttInfos(QList<Command *> cmdList) {
    QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
    this->protocolHasSubAtt = false;
    for (Command *command : cmdList) {
        QString cmdName = command->getName();
        if ((command->getHasAtt()) && (command->getAttArray().size() > 0)) {
            resultList.append(this->getAttInfos_Rec(cmdName, command->getAttArray()));
        }
    }
    return resultList;
}

QList<Attribute::T_attInfos> RustGenerator::insertSortAttInfosListByParentName(QList<Attribute::T_attInfos> attInfosList) {
    QList<Attribute::T_attInfos> sortedList = attInfosList;
    Attribute::T_attInfos bufferAttInfos;
    int j = 0;
    int listSize = sortedList.size();
    for (int i = 0; i < listSize; i++) {
        j = i;
        while ((j > 0) && (sortedList.at(j - 1).parentName.compare(sortedList.at(j).parentName) > 0)) {
            bufferAttInfos = sortedList.at(j);
            sortedList.replace(j, sortedList.at(j - 1));
            sortedList.replace(j - 1, bufferAttInfos);
            j = j - 1;
        }
    }
    return sortedList;
}

// Recursively print attribute declaration code
void RustGenerator::declareAtt_REC(
    QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, bool parentIsOpt) {
    QStringList nextParentNames = QStringList();

    for (Attribute *attribute : attList) {
        // Clear sub-attribute list between attributes
        nextParentNames = parentNames;

        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            nextParentNames.append(attribute->getName());
            this->declareAtt_REC(
                nextParentNames, attribute->getSubAttArray(), pOut, (parentIsOpt || attribute->getIsOptional()));
        } else {
            if (parentIsOpt || attribute->getIsOptional()) {
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray())) {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower() << " = &Vec::new();" << Qt::endl;
                } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower()
                          << " = &CString::new("
                             ").unwrap();"
                          << Qt::endl;
                } else if (attribute->getDataType() == NS_AttDataType::STRUCT) {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower() << " = Default::default();" << Qt::endl;
                } else {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower() << " = 0;" << Qt::endl;
                }
            } else {
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray()) ||
                    (attribute->getDataType() == NS_AttDataType::STRING)) {
                    *pOut << "    let " << parentNames.last().toLower().toLower() << "_" << attribute->getName().toLower()
                          << " = &payload." << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";"
                          << Qt::endl;
                } else {
                    *pOut << "    let " << parentNames.last().toLower().toLower() << "_" << attribute->getName().toLower()
                          << " = payload." << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";"
                          << Qt::endl;
                }
            }
        }
    }
}

// Recursively print retrieve sub-attributes data code
void RustGenerator::grabAttValues_REC(
    QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb, bool parentIsOpt) {
    QStringList nextParentNames = QStringList();
    QString indent = this->getIndent(indentNb);

    for (Attribute *attribute : attList) {
        // Clear sub-attribute list between attributes
        nextParentNames = parentNames;

        if (attribute->getIsOptional()) {
            *pOut << indent << "if payload." << this->getPayloadPath(parentNames) << "is_" << attribute->getName().toLower()
                  << "_here {" << Qt::endl;
            indent = this->getIndent(indentNb + 1);

            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                nextParentNames.append(attribute->getName());
                this->grabAttValues_REC(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1,
                    attribute->getIsOptional() || parentIsOpt);
            } else {
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray()) ||
                    (attribute->getDataType() == NS_AttDataType::STRING)) {
                    *pOut << indent << parentNames.last().toLower() << "_" << attribute->getName().toLower()
                          << " = &payload." << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";"
                          << Qt::endl;
                } else {
                    *pOut << indent << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = payload."
                          << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << Qt::endl;
                }
            }
            indent = this->getIndent(indentNb);
            *pOut << indent << "}" << Qt::endl;
        } else {
            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                nextParentNames.append(attribute->getName());
                this->grabAttValues_REC(
                    nextParentNames, attribute->getSubAttArray(), pOut, indentNb, attribute->getIsOptional() || parentIsOpt);
            } else if (attribute->getIsOptional() || parentIsOpt) {
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray()) ||
                    (attribute->getDataType() == NS_AttDataType::STRING)) {
                    *pOut << indent << parentNames.last().toLower() << "_" << attribute->getName().toLower()
                          << " = &payload." << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";"
                          << Qt::endl;
                } else {
                    *pOut << indent << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = payload."
                          << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << Qt::endl;
                }
            }
        }
    }
}

// Recursively print include type of attributes
void RustGenerator::printInclude_Rec(QString protSuffix, QString parentName, QList<Attribute *> attList, QTextStream *pOut) {
    for (Attribute *attribute : attList) {
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            *pOut << "use protocol_" << protSuffix << "::" << this->capitalize(parentName) << "Att"
                  << this->capitalize(attribute->getName()) << "Payload;" << Qt::endl;
            printInclude_Rec(protSuffix, attribute->getName(), attribute->getSubAttArray(), pOut);
        }
    }
}

// Recursively print init struct in get data functions
void RustGenerator::printInitstruct_Rec(QString parentName, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    QString indent = this->getIndent(indentNb);
    for (Attribute *attribute : attList) {
        if (attribute->getIsOptional()) {
            *pOut << indent << "is_" << attribute->getName().toLower() << "_here: false," << Qt::endl;
        }
        switch (attribute->getDataType()) {
            case NS_AttDataType::SUB_ATTRIBUTES:
                *pOut << indent << attribute->getName().toLower() << "_payload: " << this->capitalize(parentName) << "Att"
                      << this->capitalize(attribute->getName()) << "Payload {" << Qt::endl;
                printInitstruct_Rec(attribute->getName(), attribute->getSubAttArray(), pOut, indentNb + 1);
                *pOut << indent << "}," << Qt::endl;
                break;
            default:
            case NS_AttDataType::UINT8:
            case NS_AttDataType::UINT16:
            case NS_AttDataType::UINT32:
            case NS_AttDataType::UINT64:
            case NS_AttDataType::INT8:
            case NS_AttDataType::INT16:
            case NS_AttDataType::INT32:
            case NS_AttDataType::INT64:
                *pOut << indent << attribute->getName().toLower() << ": 0," << Qt::endl;
                break;
            case NS_AttDataType::FLOAT32:
            case NS_AttDataType::FLOAT64:
                *pOut << indent << attribute->getName().toLower() << ": 0.0," << Qt::endl;
                break;
            case NS_AttDataType::BYTE_ARRAY:
                *pOut << indent << attribute->getName().toLower() << ": Vec::new()," << Qt::endl;
                break;
            case NS_AttDataType::STRING:
                *pOut << indent << attribute->getName().toLower() << ": CString::new(\"\").unwrap()," << Qt::endl;
                break;
            case NS_AttDataType::STRUCT:
                *pOut << indent << attribute->getName().toLower() << ": Default::default()," << Qt::endl;
                if (!attribute->getIsArray()) {
                    *pOut << indent << attribute->getName().toLower() << "_field_nb: 0," << Qt::endl;
                }
                break;
        }
    }
}

// Recursively print code to get sub attribute data
void RustGenerator::getSubAttData_Rec(QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    QString indent = this->getIndent(indentNb);
    QStringList nextParentNames = QStringList();
    QString attDataPath;

    for (Attribute *attribute : attList) {
        // Clear sub-attribute list between attributes
        nextParentNames = parentNames;
        attDataPath = this->getPayloadPath(parentNames);
        *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName().toLower() << Qt::endl;
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            nextParentNames.append(attribute->getName());
            if (attribute->getIsOptional()) {
                *pOut
                    << indent
                    << "if let LcsfValidAttPayload::SubattArr(subatt_arr) = &subatt_iter.next().unwrap().payload && !subatt_arr.is_empty() {"
                    << Qt::endl;
                *pOut << indent << "    " << attDataPath << "is_" << attribute->getName().toLower() << "_here = true;"
                      << Qt::endl;
                *pOut << indent << "    // Get iterator" << Qt::endl;
                *pOut << indent << "    let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                this->getSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
                *pOut << indent << "}" << Qt::endl;
            } else {
                *pOut << indent
                      << "if let LcsfValidAttPayload::SubattArr(subatt_arr) = &subatt_iter.next().unwrap().payload {"
                      << Qt::endl;
                *pOut << indent << "    // Get iterator" << Qt::endl;
                *pOut << indent << "    let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                this->getSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
                *pOut << indent << "}" << Qt::endl;
            }
        } else {
            if (attribute->getIsOptional()) {
                *pOut
                    << indent
                    << "if let LcsfValidAttPayload::Data(data) = &subatt_iter.next().unwrap().payload && !data.is_empty() {"
                    << Qt::endl;
                *pOut << indent << "    " << attDataPath << "is_" << attribute->getName().toLower() << "_here = true;"
                      << Qt::endl;
                switch (attribute->getDataType()) {
                    case NS_AttDataType::UINT8:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT16:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data) as u16;" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT32:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data) as u32;" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT64:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data);" << Qt::endl;
                        break;
                    case NS_AttDataType::FLOAT32:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = f32::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::FLOAT64:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = f64::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::BYTE_ARRAY:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower() << " = data.clone();"
                              << Qt::endl;
                        break;
                    case NS_AttDataType::STRING:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                        break;
                    case NS_AttDataType::INT8:
                    case NS_AttDataType::INT16:
                    case NS_AttDataType::INT32:
                    case NS_AttDataType::INT64:
                        *pOut << indent << "    " << this->getSignedGetString(attDataPath, attribute) << Qt::endl;
                        break;
                    case NS_AttDataType::STRUCT:
                        *pOut << indent << "    " << this->getStructGetString(attDataPath, attribute) << Qt::endl;
                        break;
                }
                *pOut << indent << "}" << Qt::endl;
            } else {
                *pOut << indent << "if let LcsfValidAttPayload::Data(data) = &subatt_iter.next().unwrap().payload {"
                      << Qt::endl;
                switch (attribute->getDataType()) {
                    case NS_AttDataType::UINT8:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT16:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data) as u16;" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT32:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data) as u32;" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT64:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data);" << Qt::endl;
                        break;
                    case NS_AttDataType::FLOAT32:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = f32::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::FLOAT64:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = f64::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::BYTE_ARRAY:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower() << " = data.clone();"
                              << Qt::endl;
                        break;
                    case NS_AttDataType::STRING:
                        *pOut << indent << "    " << attDataPath << attribute->getName().toLower()
                              << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                        break;
                    case NS_AttDataType::INT8:
                    case NS_AttDataType::INT16:
                    case NS_AttDataType::INT32:
                    case NS_AttDataType::INT64:
                        *pOut << indent << "    " << this->getSignedGetString(attDataPath, attribute) << Qt::endl;
                        break;
                    case NS_AttDataType::STRUCT:
                        *pOut << indent << "    " << this->getStructGetString(attDataPath, attribute) << Qt::endl;
                        break;
                }
                *pOut << indent << "}" << Qt::endl;
            }
        }
    }
}

// Recursively print code to fill sub attribute data
void RustGenerator::fillSubAttData_Rec(
    QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    QString indent = this->getIndent(indentNb);
    QStringList nextParentNames = QStringList();
    QString attDataPath;

    for (Attribute *attribute : attList) {
        // Clear sub-attribute list between attributes
        nextParentNames = parentNames;
        attDataPath = this->getPayloadPath(parentNames);
        *pOut << indent << "// Fill data of sub-attribute " << attribute->getName().toLower() << Qt::endl;
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            nextParentNames.append(attribute->getName());
            *pOut << indent << "LcsfValidAtt { payload: ";
            if (attribute->getIsOptional()) {
                *pOut << "if payload." << attDataPath << "is_" << attribute->getName().toLower() << "_here {" << Qt::endl;
                *pOut << indent << "    LcsfValidAttPayload::SubattArr(vec![" << Qt::endl;
                this->fillSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 2);
                *pOut << indent << "    ])," << Qt::endl;
                *pOut << indent << "} else {" << Qt::endl;
                *pOut << indent << "    LcsfValidAttPayload::SubattArr(Vec::new())" << Qt::endl;
                *pOut << indent << "}" << Qt::endl;
            } else {
                *pOut << "LcsfValidAttPayload::SubattArr(vec![" << Qt::endl;
                this->fillSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
                *pOut << indent << "])}," << Qt::endl;
            }
        } else {
            *pOut << indent << "LcsfValidAtt { payload: ";
            if (attribute->getIsOptional()) {
                *pOut << "if payload." << attDataPath << "is_" << attribute->getName().toLower() << "_here {" << Qt::endl;
                switch (attribute->getDataType()) {
                    case NS_AttDataType::UINT8:
                    case NS_AttDataType::UINT16:
                    case NS_AttDataType::UINT32:
                        *pOut << indent << "    LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload." << attDataPath
                              << attribute->getName().toLower() << " as u64))" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT64:
                        *pOut << indent << "    LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload." << attDataPath
                              << attribute->getName().toLower() << "))" << Qt::endl;
                        break;
                    case NS_AttDataType::FLOAT32:
                    case NS_AttDataType::FLOAT64:
                        *pOut << indent << "    LcsfValidAttPayload::Data(payload." << attDataPath
                              << attribute->getName().toLower() << ".to_le_bytes().to_vec())" << Qt::endl;
                        break;
                    case NS_AttDataType::BYTE_ARRAY:
                        *pOut << indent << "    LcsfValidAttPayload::Data(payload." << attDataPath
                              << attribute->getName().toLower() << ".clone())" << Qt::endl;
                        break;
                    case NS_AttDataType::STRING:
                        *pOut << indent << "    LcsfValidAttPayload::Data(payload." << attDataPath
                              << attribute->getName().toLower() << ".clone().into_bytes_with_nul())" << Qt::endl;
                        break;
                    case NS_AttDataType::INT8:
                    case NS_AttDataType::INT16:
                    case NS_AttDataType::INT32:
                    case NS_AttDataType::INT64:
                        *pOut << indent << "    LcsfValidAttPayload::Data("
                              << this->getSignedFillString(attDataPath, attribute) << ")" << Qt::endl;
                        break;
                    case NS_AttDataType::STRUCT:
                        *pOut << indent << "    LcsfValidAttPayload::Data("
                              << this->getStructFillString(attDataPath, attribute) << ")" << Qt::endl;
                        break;
                }
                *pOut << indent << "} else {" << Qt::endl;
                *pOut << indent << "    LcsfValidAttPayload::Data(Vec::new())" << Qt::endl;
                *pOut << indent << "}}," << Qt::endl;
            } else {
                switch (attribute->getDataType()) {
                    case NS_AttDataType::UINT8:
                    case NS_AttDataType::UINT16:
                    case NS_AttDataType::UINT32:
                        *pOut << "LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload." << attDataPath
                              << attribute->getName().toLower() << " as u64))";
                        break;
                    case NS_AttDataType::UINT64:
                        *pOut << "LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload." << attDataPath
                              << attribute->getName().toLower() << "))";
                        break;
                    case NS_AttDataType::FLOAT32:
                    case NS_AttDataType::FLOAT64:
                        *pOut << "LcsfValidAttPayload::Data(payload." << attDataPath << attribute->getName().toLower()
                              << ".to_le_bytes().to_vec())" << Qt::endl;
                        break;
                    case NS_AttDataType::BYTE_ARRAY:
                        *pOut << "LcsfValidAttPayload::Data(payload." << attDataPath << attribute->getName().toLower()
                              << ".clone())";
                        break;
                    case NS_AttDataType::STRING:
                        *pOut << "LcsfValidAttPayload::Data(payload." << attDataPath << attribute->getName().toLower()
                              << ".clone().into_bytes_with_nul())";
                        break;
                    case NS_AttDataType::INT8:
                    case NS_AttDataType::INT16:
                    case NS_AttDataType::INT32:
                    case NS_AttDataType::INT64:
                        *pOut << "LcsfValidAttPayload::Data(" << this->getSignedFillString(attDataPath, attribute) << ")";
                        break;
                    case NS_AttDataType::STRUCT:
                        *pOut << "LcsfValidAttPayload::Data(" << this->getStructFillString(attDataPath, attribute) << ")";
                        break;
                }
                *pOut << "}," << Qt::endl;
            }
        }
    }
}

// Generate attribute description string from attribute args
QString RustGenerator::getAttDescString(bool isOptional, NS_AttDataType::T_AttDataType data_type) {
    QString descString = ", LcsfAttDesc { is_optional: ";
    // Optionality
    if (isOptional) {
        descString.append("true");
    } else {
        descString.append("false");
    }
    // Type
    descString.append(", data_type: LcsfDataType::");
    switch (NS_AttDataType::getStackDataType(data_type)) {
        case NS_AttDataType::UINT8:
            descString.append("Uint8");
            break;
        case NS_AttDataType::UINT16:
            descString.append("Uint16");
            break;
        case NS_AttDataType::UINT32:
            descString.append("Uint32");
            break;
        case NS_AttDataType::UINT64:
            descString.append("Uint64");
            break;
        case NS_AttDataType::FLOAT32:
            descString.append("Float32");
            break;
        case NS_AttDataType::FLOAT64:
            descString.append("Float64");
            break;
        case NS_AttDataType::BYTE_ARRAY:
            descString.append("ByteArray");
            break;
        case NS_AttDataType::STRING:
            descString.append("String");
            break;
        case NS_AttDataType::SUB_ATTRIBUTES:
            descString.append("Subattributes");
            break;
    }
    descString.append(", subatt_desc_arr: ");
    return descString;
}

// Payload field declaration of a struct attribute
QString RustGenerator::getStructFieldString(Attribute *attribute) {
    QString typeString = this->capitalize(attribute->getStruct()->name);
    if (attribute->getIsArray()) {
        return "    pub " + attribute->getName().toLower() + ": Vec<" + typeString + ">,";
    }
    return "    pub " + attribute->getName().toLower() + ": " + typeString + ",\n    pub " + attribute->getName().toLower() +
        "_field_nb: usize,";
}

// Retrieve code of a struct attribute from its data
QString RustGenerator::getStructGetString(QString attDataPath, Attribute *attribute) {
    QString typeString = this->capitalize(attribute->getStruct()->name);
    QString attPath = attDataPath + attribute->getName().toLower();
    if (attribute->getIsArray()) {
        return attPath + " = " + typeString + "::deserialize_array(data);";
    }
    return "(" + attPath + ", " + attPath + "_field_nb) = " + typeString + "::deserialize(data);";
}

// Fill code of a struct attribute data
QString RustGenerator::getStructFillString(QString attDataPath, Attribute *attribute) {
    QString typeString = this->capitalize(attribute->getStruct()->name);
    QString attPath = "&payload." + attDataPath + attribute->getName().toLower();
    if (attribute->getIsArray()) {
        return typeString + "::serialize_array(" + attPath + ")";
    }
    return typeString + "::serialize(" + attPath + ")";
}

// Rust type of a signed integer attribute
QString RustGenerator::getSignedTypeString(NS_AttDataType::T_AttDataType dataType) {
    switch (dataType) {
        case NS_AttDataType::INT8:
            return "i8";
        case NS_AttDataType::INT16:
            return "i16";
        case NS_AttDataType::INT32:
            return "i32";
        case NS_AttDataType::INT64:
        default:
            return "i64";
    }
}

// Cast of a signed integer to the unsigned type of its width, none at 64 bits where u64 is already that type
QString RustGenerator::getSignedCastString(NS_AttDataType::T_AttDataType dataType) {
    switch (dataType) {
        case NS_AttDataType::INT8:
            return " as u8";
        case NS_AttDataType::INT16:
            return " as u16";
        case NS_AttDataType::INT32:
            return " as u32";
        case NS_AttDataType::INT64:
        default:
            return "";
    }
}

// Payload field declaration of a signed integer attribute
QString RustGenerator::getSignedFieldString(Attribute *attribute) {
    return "    pub " + attribute->getName().toLower() + ": " + this->getSignedTypeString(attribute->getDataType()) + ",";
}

// Retrieve code of a signed integer attribute from its data, two's complement of the attribute width
QString RustGenerator::getSignedGetString(QString attDataPath, Attribute *attribute) {
    return attDataPath + attribute->getName().toLower() + " = lcsf_validator::vle_decode(data)" +
        this->getSignedCastString(attribute->getDataType()) + " as " + this->getSignedTypeString(attribute->getDataType()) +
        ";";
}

// Fill code of a signed integer attribute data, cast to its width first: a sign extension to u64 would encode 8 bytes
QString RustGenerator::getSignedFillString(QString attDataPath, Attribute *attribute) {
    return "lcsf_validator::vle_encode(payload." + attDataPath + attribute->getName().toLower() +
        this->getSignedCastString(attribute->getDataType()) + " as u64)";
}

void RustGenerator::printFixedStructImpl(QString typeString, Attribute::T_structType *pStruct, QTextStream *pOut) {
    QTextStream &out = *pOut;
    int offset = 0;
    out << "#[allow(dead_code)]" << Qt::endl;
    out << "impl " << typeString << " {" << Qt::endl;
    out << "    /// Serialized size in bytes" << Qt::endl;
    out << "    pub const SIZE: usize = " << Attribute::getStructSize(pStruct) << ";" << Qt::endl;
    out << Qt::endl;
    out << "    /// Serialize into little endian bytes" << Qt::endl;
    out << "    pub fn serialize(&self) -> Vec<u8> {" << Qt::endl;
    out << "        let mut data = Vec::with_capacity(Self::SIZE);" << Qt::endl;
    for (const Attribute::T_structField &field : pStruct->fields) {
        out << "        data.extend_from_slice(&self." << field.name.toLower() << ".to_le_bytes());" << Qt::endl;
    }
    out << "        data" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;
    out << "    /// Deserialize from little endian bytes, missing trailing fields are zeroed" << Qt::endl;
    out << "    ///" << Qt::endl;
    out << "    /// Returns the struct and the number of fields read" << Qt::endl;
    out << "    pub fn deserialize(data: &[u8]) -> (Self, usize) {" << Qt::endl;
    out << "        let mut value = Self::default();" << Qt::endl;
    for (int idx = 0; idx < pStruct->fields.size(); idx++) {
        const Attribute::T_structField &field = pStruct->fields.at(idx);
        int end = offset + NS_FieldType::FieldTypeSize[field.dataType];
        out << "        if data.len() < " << end << " {" << Qt::endl;
        out << "            return (value, " << idx << ");" << Qt::endl;
        out << "        }" << Qt::endl;
        out << "        value." << field.name.toLower() << " = " << NS_FieldType::SL_RustFieldType.at(field.dataType)
            << "::from_le_bytes(data[" << offset << ".." << end << "].try_into().unwrap());" << Qt::endl;
        offset = end;
    }
    out << "        (value, " << pStruct->fields.size() << ")" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;
    out << "    /// Serialize an array" << Qt::endl;
    out << "    pub fn serialize_array(arr: &[Self]) -> Vec<u8> {" << Qt::endl;
    if (pStruct->isExtensible) {
        out << "        let mut data = vec![Self::SIZE as u8];" << Qt::endl;
    } else {
        out << "        let mut data = Vec::with_capacity(arr.len() * Self::SIZE);" << Qt::endl;
    }
    out << "        for elem in arr {" << Qt::endl;
    out << "            data.extend(elem.serialize());" << Qt::endl;
    out << "        }" << Qt::endl;
    out << "        data" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;
    out << "    /// Deserialize an array" << Qt::endl;
    out << "    pub fn deserialize_array(data: &[u8]) -> Vec<Self> {" << Qt::endl;
    if (pStruct->isExtensible) {
        out << "        // Elements may be from another struct version" << Qt::endl;
        out << "        let Some((&elem_size, data)) = data.split_first() else {" << Qt::endl;
        out << "            return Vec::new();" << Qt::endl;
        out << "        };" << Qt::endl;
        out << "        if elem_size == 0 {" << Qt::endl;
        out << "            return Vec::new();" << Qt::endl;
        out << "        }" << Qt::endl;
        out << "        data.chunks_exact(elem_size as usize)" << Qt::endl;
    } else {
        out << "        data.chunks_exact(Self::SIZE)" << Qt::endl;
    }
    out << "            .map(|elem| Self::deserialize(elem).0)" << Qt::endl;
    out << "            .collect()" << Qt::endl;
    out << "    }" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
}

void RustGenerator::printVarStructImpl(QString typeString, Attribute::T_structType *pStruct, QTextStream *pOut) {
    QTextStream &out = *pOut;
    int fieldNb = pStruct->fields.size();

    out << "#[allow(dead_code)]" << Qt::endl;
    out << "impl " << typeString << " {" << Qt::endl;
    out << "    /// Maximum serialized size in bytes, fields are variable-length" << Qt::endl;
    out << "    pub const SIZE: usize = " << Attribute::getStructSize(pStruct) << ";" << Qt::endl;
    out << Qt::endl;

    // Serialize, a 64-bit field needs no widening
    out << "    /// Serialize into little endian bytes, variable-length fields in LEB128" << Qt::endl;
    out << "    pub fn serialize(&self) -> Vec<u8> {" << Qt::endl;
    out << "        let mut data = Vec::with_capacity(Self::SIZE);" << Qt::endl;
    for (const Attribute::T_structField &field : pStruct->fields) {
        QString fieldString = "self." + field.name.toLower();
        bool is64Bit = (NS_FieldType::FieldTypeBits[field.dataType] == 64);
        if (NS_FieldType::isSignedVarType(field.dataType)) {
            QString valueString = (is64Bit) ? fieldString : "i64::from(" + fieldString + ")";
            out << "        var_uint_encode(zigzag_encode(" << valueString << "), &mut data);" << Qt::endl;
        } else if (NS_FieldType::isVarType(field.dataType)) {
            QString valueString = (is64Bit) ? fieldString : "u64::from(" + fieldString + ")";
            out << "        var_uint_encode(" << valueString << ", &mut data);" << Qt::endl;
        } else {
            out << "        data.extend_from_slice(&" << fieldString << ".to_le_bytes());" << Qt::endl;
        }
    }
    out << "        data" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;

    // Deserialize, a field cut short or malformed ends the decode
    out << "    /// Deserialize from little endian bytes, fields after a missing or malformed one are zeroed" << Qt::endl;
    out << "    ///" << Qt::endl;
    out << "    /// Returns the struct and the number of fields read" << Qt::endl;
    out << "    pub fn deserialize(data: &[u8]) -> (Self, usize) {" << Qt::endl;
    out << "        let mut value = Self::default();" << Qt::endl;
    out << "        let " << ((fieldNb > 1) ? "mut " : "") << "idx = 0;" << Qt::endl;
    for (int fieldIdx = 0; fieldIdx < fieldNb; fieldIdx++) {
        const Attribute::T_structField &field = pStruct->fields.at(fieldIdx);
        QString fieldString = "value." + field.name.toLower();
        QString rustTypeString = NS_FieldType::SL_RustFieldType.at(field.dataType);
        int bits = NS_FieldType::FieldTypeBits[field.dataType];
        bool isLastField = (fieldIdx == fieldNb - 1);
        if (NS_FieldType::isVarType(field.dataType)) {
            QString maxString = (bits == 64) ? "u64::MAX" : "u64::from(u" + QString::number(bits) + "::MAX)";
            // The last field size is not needed
            out << "        let Some((field, " << ((isLastField) ? "_" : "size") << ")) = var_uint_decode(&data[idx..], "
                << maxString << ") else {" << Qt::endl;
            out << "            return (value, " << fieldIdx << ");" << Qt::endl;
            out << "        };" << Qt::endl;
            if (NS_FieldType::isSignedVarType(field.dataType)) {
                out << "        " << fieldString << " = zigzag_decode(field)"
                    << ((bits == 64) ? "" : " as " + rustTypeString) << ";" << Qt::endl;
            } else {
                out << "        " << fieldString << " = field" << ((bits == 64) ? "" : " as " + rustTypeString) << ";"
                    << Qt::endl;
            }
            if (!isLastField) {
                out << "        idx += size;" << Qt::endl;
            }
        } else {
            int size = NS_FieldType::FieldTypeSize[field.dataType];
            out << "        let Some(field) = data.get(idx..idx + " << size << ") else {" << Qt::endl;
            out << "            return (value, " << fieldIdx << ");" << Qt::endl;
            out << "        };" << Qt::endl;
            out << "        " << fieldString << " = " << rustTypeString << "::from_le_bytes(field.try_into().unwrap());"
                << Qt::endl;
            if (!isLastField) {
                out << "        idx += " << size << ";" << Qt::endl;
            }
        }
    }
    out << "        (value, " << fieldNb << ")" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;

    // Arrays, element sizes vary so each element starts with its own
    out << "    /// Serialize an array, each element starts with its size on one byte" << Qt::endl;
    out << "    pub fn serialize_array(arr: &[Self]) -> Vec<u8> {" << Qt::endl;
    out << "        let mut data = Vec::with_capacity(arr.len() * (1 + Self::SIZE));" << Qt::endl;
    out << "        for elem in arr {" << Qt::endl;
    out << "            let elem_data = elem.serialize();" << Qt::endl;
    out << "            data.push(elem_data.len() as u8);" << Qt::endl;
    out << "            data.extend(elem_data);" << Qt::endl;
    out << "        }" << Qt::endl;
    out << "        data" << Qt::endl;
    out << "    }" << Qt::endl;
    out << Qt::endl;
    out << "    /// Deserialize an array, each element starts with its size on one byte" << Qt::endl;
    out << "    pub fn deserialize_array(mut data: &[u8]) -> Vec<Self> {" << Qt::endl;
    out << "        let mut arr = Vec::new();" << Qt::endl;
    out << "        // Elements may be from another struct version" << Qt::endl;
    out << "        while let Some((&elem_size, rest)) = data.split_first() {" << Qt::endl;
    out << "            let Some(elem) = rest.get(..elem_size as usize) else {" << Qt::endl;
    out << "                break;" << Qt::endl;
    out << "            };" << Qt::endl;
    out << "            arr.push(Self::deserialize(elem).0);" << Qt::endl;
    out << "            data = &rest[elem_size as usize..];" << Qt::endl;
    out << "        }" << Qt::endl;
    out << "        arr" << Qt::endl;
    out << "    }" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
}

void RustGenerator::printVarUintFunctions(QTextStream *pOut) {
    QTextStream &out = *pOut;

    out << "/// Encode a value in unsigned LEB128, 7 bits per byte from the least significant, MSb set when more bytes "
           "follow"
        << Qt::endl;
    out << "fn var_uint_encode(mut value: u64, data: &mut Vec<u8>) {" << Qt::endl;
    out << "    while value >= 0x80 {" << Qt::endl;
    out << "        data.push((value as u8) | 0x80);" << Qt::endl;
    out << "        value >>= 7;" << Qt::endl;
    out << "    }" << Qt::endl;
    out << "    data.push(value as u8);" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
    out << "/// Decode an unsigned LEB128 value" << Qt::endl;
    out << "///" << Qt::endl;
    out << "/// Returns the value and its size in bytes, None if it is cut short, over 10 bytes or over max_value"
        << Qt::endl;
    out << "fn var_uint_decode(data: &[u8], max_value: u64) -> Option<(u64, usize)> {" << Qt::endl;
    out << "    let mut value: u64 = 0;" << Qt::endl;
    out << "    // A u64 takes up to 10 bytes, the 10th holds bit 63 only" << Qt::endl;
    out << "    for (idx, &byte) in data.iter().take(10).enumerate() {" << Qt::endl;
    out << "        let group = u64::from(byte & 0x7F);" << Qt::endl;
    out << "        if idx == 9 && group > 1 {" << Qt::endl;
    out << "            return None;" << Qt::endl;
    out << "        }" << Qt::endl;
    out << "        value |= group << (7 * idx);" << Qt::endl;
    out << "        if byte & 0x80 == 0 {" << Qt::endl;
    out << "            return (value <= max_value).then_some((value, idx + 1));" << Qt::endl;
    out << "        }" << Qt::endl;
    out << "    }" << Qt::endl;
    out << "    None" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
}

void RustGenerator::printZigZagFunctions(QTextStream *pOut) {
    QTextStream &out = *pOut;

    out << "/// Map a signed value to unsigned so small magnitudes stay small: 0, -1, 1, -2 give 0, 1, 2, 3" << Qt::endl;
    out << "fn zigzag_encode(value: i64) -> u64 {" << Qt::endl;
    out << "    ((value << 1) ^ (value >> 63)) as u64" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
    out << "/// Map a zigzag value back to signed" << Qt::endl;
    out << "fn zigzag_decode(value: u64) -> i64 {" << Qt::endl;
    out << "    ((value >> 1) as i64) ^ -((value & 1) as i64)" << Qt::endl;
    out << "}" << Qt::endl;
    out << Qt::endl;
}

// Print recursively attribute description
void RustGenerator::printAttDesc_Rec(QString parentName, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    QString indent = this->getIndent(indentNb);
    for (Attribute *attribute : attList) {
        QString attDescStr = this->getAttDescString(attribute->getIsOptional(), attribute->getDataType());
        *pOut << indent << "(" << parentName.toUpper() << "_ATT_ID_" << attribute->getName().toUpper() << attDescStr;
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            *pOut << "vec![" << Qt::endl;
            this->printAttDesc_Rec(attribute->getName(), attribute->getSubAttArray(), pOut, indentNb + 1);
            *pOut << indent << "]})," << Qt::endl;
        } else {
            *pOut << "Vec::new()})," << Qt::endl;
        }
    }
}

// Generate the protocol_<name>.rs file
void RustGenerator::generateMain(
    QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath, RustExtractor rustExtract) {
    QString low_prot_name = protocolName.toLower();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString protSuffix = low_prot_name + "_" + ((isA) ? "a" : "b");
    QString fileName = dirPath + "/protocol_" + protSuffix + ".rs";
    QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        QFileInfo fileInfo(file);

        // Header/Import
        out << "//! Main file (" << ((isA) ? "A" : "B") << ") for protocol: " << this->capitalize(protocolName) << Qt::endl;
        out << "//!" << Qt::endl;
        out << "//! This file has been auto-generated by LCSF Generator v" << APP_VERSION << Qt::endl;
        out << "//! Feel free to customize as needed" << Qt::endl;
        out << Qt::endl;
        out << "use crate::lcsf_lib::lcsf_core;" << Qt::endl;
        out << "use crate::lcsf_lib::lcsf_validator;" << Qt::endl;
        out << "use crate::lcsf_prot::lcsf_protocol_" << protSuffix << ";" << Qt::endl;
        out << "use lcsf_core::LcsfCore;" << Qt::endl;
        out << "use lcsf_validator::LcsfValidCmd;" << Qt::endl;
        // Check if Cstring needed
        if (this->is_CString_needed(attInfosList)) {
            out << "use std::ffi::CString;" << Qt::endl;
        }
        out << "// --- Custom uses ---" << Qt::endl;
        if (rustExtract.getExtractionComplete()) {
            QString uses = rustExtract.getExtraUses().trimmed();
            if (!uses.isEmpty()) {
                out << uses << Qt::endl;
            }
        }
        out << Qt::endl;

        // Command id enum
        out << "/// Command enum" << Qt::endl;
        out << "#[derive(Debug, PartialEq, Copy, Clone)]" << Qt::endl;
        out << "pub enum CmdEnum {" << Qt::endl;
        for (Command *command : cmdList) {
            out << "    " << this->capitalize(command->getName()) << "," << Qt::endl;
        }
        out << "}" << Qt::endl;
        out << Qt::endl;

        // Command payload union
        out << "/// Command payload union" << Qt::endl;
        out << "#[derive(Debug, PartialEq)]" << Qt::endl;
        out << "#[allow(dead_code)]" << Qt::endl;
        out << "pub enum CmdPayload {" << Qt::endl;
        out << "    Empty," << Qt::endl;
        for (Command *command : cmdList) {
            if (command->getAttArray().size() > 0) {
                out << "    " << this->capitalize(command->getName()) << "Payload(" << this->capitalize(command->getName())
                    << "AttPayload)," << Qt::endl;
            }
        }
        out << "}" << Qt::endl;
        out << Qt::endl;

        // Command payload structs
        out << "// Command data structures" << Qt::endl;
        for (Command *command : cmdList) {
            if (command->getAttArray().size() > 0) {
                out << "#[derive(Debug, PartialEq)]" << Qt::endl;
                out << "pub struct " << this->capitalize(command->getName()) << "AttPayload {" << Qt::endl;
                for (Attribute *attribute : command->getAttArray()) {
                    // Note optional field
                    if (attribute->getIsOptional()) {
                        out << "    pub is_" << attribute->getName().toLower() << "_here: bool," << Qt::endl;
                    }
                    // Note attribute field
                    switch (attribute->getDataType()) {
                        case NS_AttDataType::SUB_ATTRIBUTES:
                            out << "    pub " << attribute->getName().toLower()
                                << "_payload: " << this->capitalize(command->getName()) << "Att"
                                << this->capitalize(attribute->getName()) << "Payload," << Qt::endl;
                            break;
                        case NS_AttDataType::BYTE_ARRAY:
                            out << "    pub " << attribute->getName().toLower() << ": Vec<u8>," << Qt::endl;
                            break;
                        case NS_AttDataType::STRING:
                            out << "    pub " << attribute->getName().toLower() << ": CString," << Qt::endl;
                            break;
                        case NS_AttDataType::UINT8:
                            out << "    pub " << attribute->getName().toLower() << ": u8," << Qt::endl;
                            break;
                        case NS_AttDataType::UINT16:
                            out << "    pub " << attribute->getName().toLower() << ": u16," << Qt::endl;
                            break;
                        case NS_AttDataType::UINT32:
                            out << "    pub " << attribute->getName().toLower() << ": u32," << Qt::endl;
                            break;
                        case NS_AttDataType::UINT64:
                            out << "    pub " << attribute->getName().toLower() << ": u64," << Qt::endl;
                            break;
                        case NS_AttDataType::FLOAT32:
                            out << "    pub " << attribute->getName().toLower() << ": f32," << Qt::endl;
                            break;
                        case NS_AttDataType::FLOAT64:
                            out << "    pub " << attribute->getName().toLower() << ": f64," << Qt::endl;
                            break;
                        case NS_AttDataType::INT8:
                        case NS_AttDataType::INT16:
                        case NS_AttDataType::INT32:
                        case NS_AttDataType::INT64:
                            out << this->getSignedFieldString(attribute) << Qt::endl;
                            break;
                        case NS_AttDataType::STRUCT:
                            out << this->getStructFieldString(attribute) << Qt::endl;
                            break;
                    }
                }
                out << "}" << Qt::endl;
                out << Qt::endl;
            }
        }

        // Attribute payload structs
        if (this->protocolHasSubAtt) {
            out << "// Attribute with sub-attributes structures" << Qt::endl;
            Attribute::T_attInfos currentAttInfo;
            // Parse the list
            for (int idx = 0; idx < attInfosList.size(); idx++) {
                currentAttInfo = attInfosList.at(idx);
                if (currentAttInfo.dataType == NS_AttDataType::SUB_ATTRIBUTES) {
                    out << "#[derive(Debug, PartialEq)]" << Qt::endl;
                    out << "pub struct " << this->capitalize(currentAttInfo.parentName) << "Att"
                        << this->capitalize(currentAttInfo.attName) << "Payload {" << Qt::endl;
                    int subIdx = idx + 1;
                    int subAttCpt = 0;
                    while ((subIdx < attInfosList.size()) || (subAttCpt < currentAttInfo.subAttNb)) {
                        Attribute::T_attInfos currentSubAttInfo = attInfosList.at(subIdx);
                        if (currentSubAttInfo.parentName == currentAttInfo.attName) {
                            subAttCpt++;
                            // Note the optional attribute field
                            if (currentSubAttInfo.isOptional) {
                                out << "    pub is_" << currentSubAttInfo.attName.toLower() << "_here: bool," << Qt::endl;
                            }
                            // Note the sub-attribute field
                            switch (currentSubAttInfo.dataType) {
                                case NS_AttDataType::SUB_ATTRIBUTES:
                                    out << "    pub " << currentSubAttInfo.attName.toLower()
                                        << "_payload: " << this->capitalize(currentSubAttInfo.parentName) << "Att"
                                        << this->capitalize(currentSubAttInfo.attName) << "Payload," << Qt::endl;
                                    break;
                                case NS_AttDataType::BYTE_ARRAY:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": Vec<u8>," << Qt::endl;
                                    break;
                                case NS_AttDataType::STRING:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": CString," << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT8:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": u8," << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT16:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": u16," << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT32:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": u32," << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT64:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": u64," << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT32:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": f32," << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT64:
                                    out << "    pub " << currentSubAttInfo.attName.toLower() << ": f64," << Qt::endl;
                                    break;
                                case NS_AttDataType::INT8:
                                case NS_AttDataType::INT16:
                                case NS_AttDataType::INT32:
                                case NS_AttDataType::INT64:
                                    out << this->getSignedFieldString(currentSubAttInfo.attPointer) << Qt::endl;
                                    break;
                                case NS_AttDataType::STRUCT:
                                    out << this->getStructFieldString(currentSubAttInfo.attPointer) << Qt::endl;
                                    break;
                            }
                        }
                        // Next attribute
                        subIdx++;
                    }
                    out << "}" << Qt::endl;
                    out << Qt::endl;
                }
            }
        }

        // Struct types
        QList<Attribute::T_structType *> structList = Command::getStructList(cmdList);
        if (!structList.isEmpty()) {
            out << "// Struct types" << Qt::endl;
        }
        if (Command::hasStructFieldType(cmdList, NS_FieldType::isVarType)) {
            this->printVarUintFunctions(&out);
        }
        if (Command::hasStructFieldType(cmdList, NS_FieldType::isSignedVarType)) {
            this->printZigZagFunctions(&out);
        }
        for (Attribute::T_structType *pStruct : structList) {
            QString typeString = this->capitalize(pStruct->name);
            out << "#[derive(Debug, Default, PartialEq, Clone, Copy)]" << Qt::endl;
            out << "pub struct " << typeString << " {" << Qt::endl;
            for (const Attribute::T_structField &field : pStruct->fields) {
                out << "    pub " << field.name.toLower() << ": " << NS_FieldType::SL_RustFieldType.at(field.dataType) << ","
                    << Qt::endl;
            }
            out << "}" << Qt::endl;
            out << Qt::endl;
            if (Attribute::hasStructVarField(pStruct)) {
                this->printVarStructImpl(typeString, pStruct, &out);
            } else {
                this->printFixedStructImpl(typeString, pStruct, &out);
            }
        }

        out << "// --- Custom definitions ---" << Qt::endl;
        if (rustExtract.getExtractionComplete()) {
            QString defs = rustExtract.getCustomDefinitions().trimmed();
            if (!defs.isEmpty()) {
                out << defs << Qt::endl;
            }
        }
        out << Qt::endl;

        // Command execute functions
        bool useExtract = rustExtract.getExtractionComplete();
        QStringList cmdFunctions = useExtract ? rustExtract.getCommandFunctions() : QStringList();

        for (int idx = 0; idx < cmdList.size(); idx++) {
            Command *command = cmdList.at(idx);

            if (command->isReceivable(isA)) {
                if (useExtract && idx < cmdFunctions.size() && !cmdFunctions.at(idx).isEmpty()) {
                    out << cmdFunctions.at(idx);
                    continue;
                }
                out << "fn " << "execute_" << command->getName().toLower() << "(";

                if (command->getAttArray().size() > 0) {
                    out << "payload: &" << this->capitalize(command->getName()) << "AttPayload) {" << Qt::endl;
                    out << "    // Retrieve attributes" << Qt::endl;
                    // Init variables
                    for (Attribute *attribute : command->getAttArray()) {
                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                            QStringList parentNames = {attribute->getName()};
                            declareAtt_REC(parentNames, attribute->getSubAttArray(), &out, attribute->getIsOptional());
                        } else {
                            if (attribute->getIsOptional()) {
                                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray())) {
                                    out << "    let mut " << attribute->getName().toLower() << " = &Vec::new();" << Qt::endl;
                                } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                                    out << "    let mut " << attribute->getName().toLower()
                                        << " = &CString::new(\"\").unwrap();" << Qt::endl;
                                } else if (attribute->getDataType() == NS_AttDataType::STRUCT) {
                                    out << "    let mut " << attribute->getName().toLower() << " = Default::default();"
                                        << Qt::endl;
                                } else {
                                    out << "    let mut " << attribute->getName().toLower() << " = 0;" << Qt::endl;
                                }
                            } else {
                                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getIsArray()) ||
                                    (attribute->getDataType() == NS_AttDataType::STRING)) {
                                    out << "    let " << attribute->getName().toLower() << " = &payload."
                                        << attribute->getName().toLower() << ";" << Qt::endl;
                                } else {
                                    out << "    let " << attribute->getName().toLower() << " = payload."
                                        << attribute->getName().toLower() << ";" << Qt::endl;
                                }
                            }
                        }
                    }
                    // Get optional values
                    for (Attribute *attribute : command->getAttArray()) {
                        if (attribute->getIsOptional()) {
                            out << "    if payload.is_" << attribute->getName().toLower() << "_here {" << Qt::endl;
                            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                                QStringList parentNames = {attribute->getName()};
                                this->grabAttValues_REC(
                                    parentNames, attribute->getSubAttArray(), &out, 2, attribute->getIsOptional());
                            } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) ||
                                (attribute->getIsArray()) || (attribute->getDataType() == NS_AttDataType::STRING)) {
                                out << "        " << attribute->getName().toLower() << " = &payload."
                                    << attribute->getName().toLower() << ";" << Qt::endl;
                            } else {
                                out << "        " << attribute->getName().toLower() << " = payload."
                                    << attribute->getName().toLower() << ";" << Qt::endl;
                            }
                            out << "    }" << Qt::endl;
                        } else {
                            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                                QStringList parentNames = {attribute->getName()};
                                this->grabAttValues_REC(
                                    parentNames, attribute->getSubAttArray(), &out, 1, attribute->getIsOptional());
                            }
                        }
                    }
                    out << "    // Function to implement" << Qt::endl;
                    out << "    todo!();" << Qt::endl;
                } else {
                    out << ") {" << Qt::endl;
                    out << "    // Function to implement" << Qt::endl;
                    out << "    todo!();" << Qt::endl;
                }
                out << "}" << Qt::endl;
                out << Qt::endl;
            }
        }

        // Execute command
        out << "/// Execute a command, customize as needed" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// cmd_name: name of the command" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// cmd_payload: pointer to command payload" << Qt::endl;
        if (useExtract && !rustExtract.getExecuteCmdFunction().isEmpty()) {
            out << rustExtract.getExecuteCmdFunction();
        } else {
            out << "fn execute_cmd(cmd_name: CmdEnum, cmd_payload: &CmdPayload) {" << Qt::endl;
            out << "    match cmd_name {" << Qt::endl;
            for (Command *command : cmdList) {
                if (command->isReceivable(isA)) {
                    out << "        CmdEnum::" << this->capitalize(command->getName()) << " => ";
                    if (command->getAttArray().size() == 0) {
                        out << "execute_" << command->getName().toLower() << "()," << Qt::endl;
                    } else {
                        out << "{" << Qt::endl;
                        out << "            if let CmdPayload::" << this->capitalize(command->getName())
                            << "Payload(payload) = cmd_payload {" << Qt::endl;
                        out << "                execute_" << command->getName().toLower() << "(payload);" << Qt::endl;
                        out << "            }" << Qt::endl;
                        out << "        }" << Qt::endl;
                    }
                }
            }
            out << "        _ => {" << Qt::endl;
            out << "            // This case can be customized (e.g to send an error command)" << Qt::endl;
            out << "            todo!();" << Qt::endl;
            out << "        }" << Qt::endl;
            out << "    }" << Qt::endl;
            out << "}" << Qt::endl;
        }
        out << Qt::endl;
        out << "// --- Custom public functions ---" << Qt::endl;

        if (useExtract && !rustExtract.getCustomPublicFunctions().isEmpty()) {
            out << rustExtract.getCustomPublicFunctions();
        } else {
            // Utility functions
            out << "/// Init a LcsfCore with the protocol" << Qt::endl;
            out << "///" << Qt::endl;
            out << "/// core: LcsfCore reference" << Qt::endl;
            out << "pub fn init_core(core: &mut LcsfCore) {" << Qt::endl;
            out << "    // Add protocol to LcsfCore" << Qt::endl;
            out << "    core.add_protocol(" << Qt::endl;
            out << "        lcsf_protocol_" << protSuffix << "::PROT_ID," << Qt::endl;
            out << "        &lcsf_protocol_" << protSuffix << "::PROT_DESC," << Qt::endl;
            out << "        process_cmd," << Qt::endl;
            out << "    );" << Qt::endl;
            out << "}" << Qt::endl;
            out << Qt::endl;
            out << "/// Process command callback, customize as you need" << Qt::endl;
            out << "///" << Qt::endl;
            out << "/// valid_cmd: received valid command" << Qt::endl;
            out << "fn process_cmd(core: &LcsfCore, valid_cmd: &LcsfValidCmd) {" << Qt::endl;
            out << "    // Process received command" << Qt::endl;
            out << "    let (cmd_name, cmd_payload) = lcsf_protocol_" << protSuffix << "::receive_cmd(valid_cmd);"
                << Qt::endl;
            out << "    execute_cmd(cmd_name, &cmd_payload);" << Qt::endl;
            out << "    // Here the function will send back received commands like an echo" << Qt::endl;
            out << "    // Customize as needed" << Qt::endl;
            out << "    let valid_cmd = lcsf_protocol_" << protSuffix << "::send_cmd(cmd_name, &cmd_payload);" << Qt::endl;
            out << "    core.send_cmd(lcsf_protocol_" << protSuffix << "::PROT_ID, &valid_cmd);" << Qt::endl;
            out << "    todo!();" << Qt::endl;
            out << "}" << Qt::endl;
        }

        if (useExtract && !rustExtract.getTrailingContent().isEmpty()) {
            out << rustExtract.getTrailingContent();
        }

        file.close();
    }
}

void RustGenerator::generateBridge(
    QString protocolName, QString protocolId, QString protocolVersion, QList<Command *> cmdList, bool isA, QString dirPath) {
    QString low_prot_name = protocolName.toLower();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString protSuffix = low_prot_name + "_" + ((isA) ? "a" : "b");
    QString fileName = dirPath + "/lcsf_protocol_" + protSuffix + ".rs";
    QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
    QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&file);
        QFileInfo fileInfo(file);

        // Header/Import
        out << "//! Lcsf abstraction layer (" << ((isA) ? "A" : "B") << ") for protocol: " << this->capitalize(protocolName)
            << Qt::endl;
        out << "//!" << Qt::endl;
        out << "//! This file has been auto-generated by LCSF Generator v" << APP_VERSION << Qt::endl;
        out << "//! It shouldn't be edited manually" << Qt::endl;
        out << Qt::endl;
        out << "use crate::lcsf_lib::lcsf_validator;" << Qt::endl;
        out << "use crate::lcsf_prot::protocol_" << protSuffix << ";" << Qt::endl;
        out << "use lazy_static::lazy_static;" << Qt::endl;
        out << "use lcsf_validator::LcsfAttDesc;" << Qt::endl;
        out << "use lcsf_validator::LcsfCmdDesc;" << Qt::endl;
        out << "use lcsf_validator::LcsfDataType;" << Qt::endl;
        out << "use lcsf_validator::LcsfProtDesc;" << Qt::endl;
        out << "use lcsf_validator::LcsfValidAtt;" << Qt::endl;
        out << "use lcsf_validator::LcsfValidAttPayload;" << Qt::endl;
        out << "use lcsf_validator::LcsfValidCmd;" << Qt::endl;
        // Check if Cstring needed
        if (this->is_CString_needed(attInfosList)) {
            out << "use std::ffi::CString;" << Qt::endl;
        }
        out << Qt::endl;

        // Import Attribute
        for (Command *command : cmdList) {
            if (command->isReceivable(isA)) {
                if (command->getAttArray().size() > 0) {
                    out << "use protocol_" << protSuffix << "::" << this->capitalize(command->getName()) << "AttPayload;"
                        << Qt::endl;
                    printInclude_Rec(protSuffix, command->getName(), command->getAttArray(), &out);
                }
            }
        }
        for (Attribute::T_structType *pStruct : Command::getStructList(cmdList)) {
            out << "use protocol_" << protSuffix << "::" << this->capitalize(pStruct->name) << ";" << Qt::endl;
        }
        out << "use protocol_" << protSuffix << "::CmdEnum;" << Qt::endl;
        out << "use protocol_" << protSuffix << "::CmdPayload;" << Qt::endl;
        out << Qt::endl;

        // Id to and from enum functions
        out << "/// Convert command name to lcsf command id" << Qt::endl;
        out << "fn cmd_name_to_id(cmd_name: CmdEnum) -> u16 {" << Qt::endl;
        out << "    match cmd_name {" << Qt::endl;
        for (Command *command : cmdList) {
            out << "        CmdEnum::" << this->capitalize(command->getName()) << " => CMD_ID_"
                << command->getName().toUpper() << "," << Qt::endl;
        }
        out << "    }" << Qt::endl;
        out << "}" << Qt::endl;
        out << Qt::endl;
        out << "/// Convert lcsf command id to command name" << Qt::endl;
        out << "fn cmd_id_to_name(cmd_id: u16) -> CmdEnum {" << Qt::endl;
        out << "    match cmd_id {" << Qt::endl;
        for (Command *command : cmdList) {
            out << "        CMD_ID_" << command->getName().toUpper()
                << " => CmdEnum::" << this->capitalize(command->getName()) << "," << Qt::endl;
        }
        out << "        _ => panic!(\"Unreachable values\")," << Qt::endl;
        out << "    }" << Qt::endl;
        out << "}" << Qt::endl;
        out << Qt::endl;

        // Commands attribute get data functions
        out << "/// Retrieve data of a command from its valid attribute array and store it in a payload" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// att_arr command valid attribute array reference" << Qt::endl;
        for (Command *command : cmdList) {
            if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
                out << "fn " << command->getName().toLower() << "_get_data(att_arr: &[LcsfValidAtt]) -> CmdPayload {"
                    << Qt::endl;
                // Init return packet
                out << "    let mut " << command->getName().toLower()
                    << "_payload = " << this->capitalize(command->getName()) << "AttPayload {" << Qt::endl;
                for (Attribute *attribute : command->getAttArray()) {
                    if (attribute->getIsOptional()) {
                        out << "        is_" << attribute->getName().toLower() << "_here: false," << Qt::endl;
                    }
                    switch (attribute->getDataType()) {
                        case NS_AttDataType::SUB_ATTRIBUTES:
                            out << "        " << attribute->getName().toLower()
                                << "_payload: " << this->capitalize(command->getName()) << "Att"
                                << this->capitalize(attribute->getName()) << "Payload {" << Qt::endl;
                            printInitstruct_Rec(attribute->getName(), attribute->getSubAttArray(), &out, 3);
                            out << "        }," << Qt::endl;
                            break;
                        default:
                        case NS_AttDataType::UINT8:
                        case NS_AttDataType::UINT16:
                        case NS_AttDataType::UINT32:
                        case NS_AttDataType::UINT64:
                        case NS_AttDataType::INT8:
                        case NS_AttDataType::INT16:
                        case NS_AttDataType::INT32:
                        case NS_AttDataType::INT64:
                            out << "        " << attribute->getName().toLower() << ": 0," << Qt::endl;
                            break;
                        case NS_AttDataType::FLOAT32:
                        case NS_AttDataType::FLOAT64:
                            out << "        " << attribute->getName().toLower() << ": 0.0," << Qt::endl;
                            break;
                        case NS_AttDataType::BYTE_ARRAY:
                            out << "        " << attribute->getName().toLower() << ": Vec::new()," << Qt::endl;
                            break;
                        case NS_AttDataType::STRING:
                            out << "        " << attribute->getName().toLower() << ": CString::new(\"\").unwrap(),"
                                << Qt::endl;
                            break;
                        case NS_AttDataType::STRUCT:
                            out << "        " << attribute->getName().toLower() << ": Default::default()," << Qt::endl;
                            if (!attribute->getIsArray()) {
                                out << "        " << attribute->getName().toLower() << "_field_nb: 0," << Qt::endl;
                            }
                            break;
                    }
                }
                out << "    };" << Qt::endl;
                out << "    // Get iterator" << Qt::endl;
                out << "    let att_iter = &mut att_arr.iter();" << Qt::endl;
                //  Retrieve attribute data
                for (Attribute *attribute : command->getAttArray()) {
                    out << "    // Retrieve data of attribute " << attribute->getName().toLower() << Qt::endl;
                    if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                        QStringList parentNames = {command->getName(), attribute->getName()};
                        if (attribute->getIsOptional()) {
                            out << "    if let LcsfValidAttPayload::SubattArr(subatt_arr) = &att_iter.next().unwrap().payload && !subatt_arr.is_empty() {"
                                << Qt::endl;
                            out << "        " << command->getName().toLower() << "_payload.is_"
                                << attribute->getName().toLower() << "_here = true;" << Qt::endl;
                            out << "        // Get iterator" << Qt::endl;
                            out << "        let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                            this->getSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 2);
                            out << "    }" << Qt::endl;
                        } else {
                            out << "    if let LcsfValidAttPayload::SubattArr(subatt_arr) = &att_iter.next().unwrap().payload {"
                                << Qt::endl;
                            out << "        // Get iterator" << Qt::endl;
                            out << "        let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                            this->getSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 2);
                            out << "    }" << Qt::endl;
                        }
                    } else {
                        if (attribute->getIsOptional()) {
                            out << "    if let LcsfValidAttPayload::Data(data) = &att_iter.next().unwrap().payload && !data.is_empty() {"
                                << Qt::endl;
                            out << "        " << command->getName().toLower() << "_payload.is_"
                                << attribute->getName().toLower() << "_here = true;" << Qt::endl;
                            switch (attribute->getDataType()) {
                                case NS_AttDataType::UINT8:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT16:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u16;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT32:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u32;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT64:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data);"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT32:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f32::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT64:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f64::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::BYTE_ARRAY:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = data.clone();" << Qt::endl;
                                    break;
                                case NS_AttDataType::STRING:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                                    break;
                                case NS_AttDataType::INT8:
                                case NS_AttDataType::INT16:
                                case NS_AttDataType::INT32:
                                case NS_AttDataType::INT64:
                                    out << "        "
                                        << this->getSignedGetString(command->getName().toLower() + "_payload.", attribute)
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::STRUCT:
                                    out << "        "
                                        << this->getStructGetString(command->getName().toLower() + "_payload.", attribute)
                                        << Qt::endl;
                                    break;
                            }
                            out << "    }" << Qt::endl;
                        } else {
                            out << "    if let LcsfValidAttPayload::Data(data) = &att_iter.next().unwrap().payload {"
                                << Qt::endl;
                            switch (attribute->getDataType()) {
                                case NS_AttDataType::UINT8:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT16:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u16;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT32:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u32;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT64:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data);"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT32:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f32::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT64:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f64::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::BYTE_ARRAY:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = data.clone();" << Qt::endl;
                                    break;
                                case NS_AttDataType::STRING:
                                    out << "        " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                                    break;
                                case NS_AttDataType::INT8:
                                case NS_AttDataType::INT16:
                                case NS_AttDataType::INT32:
                                case NS_AttDataType::INT64:
                                    out << "        "
                                        << this->getSignedGetString(command->getName().toLower() + "_payload.", attribute)
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::STRUCT:
                                    out << "        "
                                        << this->getStructGetString(command->getName().toLower() + "_payload.", attribute)
                                        << Qt::endl;
                                    break;
                            }
                            out << "    }" << Qt::endl;
                        }
                    }
                }
                out << "    CmdPayload::" << this->capitalize(command->getName()) << "Payload("
                    << command->getName().toLower() << "_payload)" << Qt::endl;
                out << "}" << Qt::endl;
                out << Qt::endl;
            }
        }

        // Master get data function
        out << "/// Tranform a lcsf valid command into a protocol payload" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// valid_cmd: valid command reference" << Qt::endl;
        out << "pub fn receive_cmd(valid_cmd: &LcsfValidCmd) -> (CmdEnum, CmdPayload) {" << Qt::endl;
        out << "    let cmd_name = cmd_id_to_name(valid_cmd.cmd_id);" << Qt::endl;
        out << "    let cmd_payload = match cmd_name {" << Qt::endl;
        for (Command *command : cmdList) {
            if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
                out << "        CmdEnum::" << this->capitalize(command->getName()) << " => " << command->getName().toLower()
                    << "_get_data(&valid_cmd.att_arr)," << Qt::endl;
            } else {
                out << "        CmdEnum::" << this->capitalize(command->getName()) << " => CmdPayload::Empty," << Qt::endl;
            }
        }
        out << "    };" << Qt::endl;
        out << "    (cmd_name, cmd_payload)" << Qt::endl;
        out << "}" << Qt::endl;
        out << Qt::endl;

        // Command fill attribute functions
        out << "/// Fill a command lcsf valid attribute array from its payload" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// cmd_payload pointer to the command payload" << Qt::endl;
        for (Command *command : cmdList) {
            if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA))) {
                out << "fn " << command->getName().toLower() << "_fill_att(cmd_payload: &CmdPayload) -> Vec<LcsfValidAtt> {"
                    << Qt::endl;
                out << "    let mut att_arr: Vec<LcsfValidAtt> = Vec::new();" << Qt::endl;
                out << "    if let CmdPayload::" << this->capitalize(command->getName())
                    << "Payload(payload) = &cmd_payload {" << Qt::endl;
                for (Attribute *attribute : command->getAttArray()) {
                    out << "        // Fill data of attribute " << attribute->getName().toLower() << Qt::endl;
                    if (attribute->getIsOptional()) {
                        out << "        if payload.is_" << attribute->getName().toLower() << "_here {" << Qt::endl;
                        switch (attribute->getDataType()) {
                            case NS_AttDataType::SUB_ATTRIBUTES:
                                {
                                    out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::SubattArr(vec!["
                                        << Qt::endl;
                                    QStringList parentNames = {attribute->getName()};
                                    this->fillSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 4);
                                    out << "            ]),});" << Qt::endl;
                                }
                                break;
                            case NS_AttDataType::UINT8:
                            case NS_AttDataType::UINT16:
                            case NS_AttDataType::UINT32:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload."
                                    << attribute->getName().toLower() << " as u64)),});" << Qt::endl;
                                break;
                            case NS_AttDataType::UINT64:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload."
                                    << attribute->getName().toLower() << ")),});" << Qt::endl;
                                break;
                            case NS_AttDataType::FLOAT32:
                            case NS_AttDataType::FLOAT64:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".to_le_bytes().to_vec()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::BYTE_ARRAY:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".clone()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::STRING:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".clone().into_bytes_with_nul()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::INT8:
                            case NS_AttDataType::INT16:
                            case NS_AttDataType::INT32:
                            case NS_AttDataType::INT64:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data("
                                    << this->getSignedFillString("", attribute) << "),});" << Qt::endl;
                                break;
                            case NS_AttDataType::STRUCT:
                                out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data("
                                    << this->getStructFillString("", attribute) << "),});" << Qt::endl;
                                break;
                        }
                        out << "        } else {" << Qt::endl;
                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                            out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::SubattArr(Vec::new()),});"
                                << Qt::endl;
                        } else {
                            out << "            att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(Vec::new()),});"
                                << Qt::endl;
                        }
                        out << "        }" << Qt::endl;
                    } else {
                        switch (attribute->getDataType()) {
                            case NS_AttDataType::SUB_ATTRIBUTES:
                                {
                                    out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::SubattArr(vec!["
                                        << Qt::endl;
                                    QStringList parentNames = {attribute->getName()};
                                    this->fillSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 3);
                                    out << "        ]),});" << Qt::endl;
                                }
                                break;
                            case NS_AttDataType::UINT8:
                            case NS_AttDataType::UINT16:
                            case NS_AttDataType::UINT32:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload."
                                    << attribute->getName().toLower() << " as u64)),});" << Qt::endl;
                                break;
                            case NS_AttDataType::UINT64:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(lcsf_validator::vle_encode(payload."
                                    << attribute->getName().toLower() << ")),});" << Qt::endl;
                                break;
                            case NS_AttDataType::FLOAT32:
                            case NS_AttDataType::FLOAT64:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".to_le_bytes().to_vec()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::BYTE_ARRAY:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".clone()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::STRING:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data(payload."
                                    << attribute->getName().toLower() << ".clone().into_bytes_with_nul()),});" << Qt::endl;
                                break;
                            case NS_AttDataType::INT8:
                            case NS_AttDataType::INT16:
                            case NS_AttDataType::INT32:
                            case NS_AttDataType::INT64:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data("
                                    << this->getSignedFillString("", attribute) << "),});" << Qt::endl;
                                break;
                            case NS_AttDataType::STRUCT:
                                out << "        att_arr.push(LcsfValidAtt { payload: LcsfValidAttPayload::Data("
                                    << this->getStructFillString("", attribute) << "),});" << Qt::endl;
                                break;
                        }
                    }
                }
                out << "    }" << Qt::endl;
                out << "    att_arr" << Qt::endl;
                out << "}" << Qt::endl;
                out << Qt::endl;
            }
        }

        // Master fill attribute function
        out << "/// Tranform a protocol payload into a lcsf valid command" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// cmd_name: command name to send" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// cmd_payload: protocol payload reference" << Qt::endl;
        out << "pub fn send_cmd(cmd_name: CmdEnum, cmd_payload: &CmdPayload) -> LcsfValidCmd {" << Qt::endl;
        out << "    let mut send_cmd = LcsfValidCmd {" << Qt::endl;
        out << "        cmd_id: cmd_name_to_id(cmd_name)," << Qt::endl;
        out << "        att_arr: Vec::new()," << Qt::endl;
        out << "    };" << Qt::endl;
        out << "    match cmd_name {" << Qt::endl;
        for (Command *command : cmdList) {
            if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA))) {
                out << "        CmdEnum::" << this->capitalize(command->getName())
                    << " => send_cmd.att_arr = " << command->getName().toLower() << "_fill_att(cmd_payload)," << Qt::endl;
            } else {
                out << "        CmdEnum::" << this->capitalize(command->getName()) << " => {}," << Qt::endl;
            }
        }
        out << "    }" << Qt::endl;
        out << "    send_cmd" << Qt::endl;
        out << "}" << Qt::endl;
        out << Qt::endl;

        // Protocol id
        out << "// *** Protocol lcsf descriptor ***" << Qt::endl;
        out << Qt::endl;
        out << "/// Protocol id" << Qt::endl;
        out << "pub const PROT_ID: u16 = 0x" << protocolId << ";" << Qt::endl;
        out << Qt::endl;
        out << "/// Protocol version" << Qt::endl;
        out << "pub const PROT_VER: u16 = " << protocolVersion << ";" << Qt::endl;
        out << Qt::endl;

        // Commands Ids enum
        out << "/// Commands ids" << Qt::endl;
        for (Command *command : cmdList) {
            out << "pub const CMD_ID_" << command->getName().toUpper() << ": u16 = 0x"
                << QString::number(command->getId(), 16) << ";" << Qt::endl;
        }
        out << Qt::endl;

        // Attributes Ids enums
        if (!attInfosList.isEmpty()) {
            for (int idx = 0; idx < sortedAttInfosList.size(); idx++) {
                Attribute::T_attInfos currentAttInfo = sortedAttInfosList.at(idx);
                out << "// " << this->capitalize(currentAttInfo.parentName) << " attribute ids" << Qt::endl;
                QString previousParentName = currentAttInfo.parentName;
                out << "const " << currentAttInfo.parentName.toUpper() << "_ATT_ID_" << currentAttInfo.attName.toUpper()
                    << ": u16 = 0x" << QString::number(currentAttInfo.attId, 16) << ";" << Qt::endl;
                if (idx < sortedAttInfosList.size() - 1) {
                    currentAttInfo = sortedAttInfosList.at(idx + 1);
                    while (currentAttInfo.parentName.compare(previousParentName) == 0) {
                        out << "const " << currentAttInfo.parentName.toUpper() << "_ATT_ID_"
                            << currentAttInfo.attName.toUpper() << ": u16 = 0x" << QString::number(currentAttInfo.attId, 16)
                            << ";" << Qt::endl;
                        previousParentName = currentAttInfo.parentName;
                        idx++;
                        if (idx < sortedAttInfosList.size() - 1) {
                            currentAttInfo = sortedAttInfosList.at(idx + 1);
                        } else {
                            break;
                        }
                    }
                }
                out << Qt::endl;
            }
        }

        // Description
        out << "lazy_static! {" << Qt::endl;
        out << "    /// Test protocol descriptor" << Qt::endl;
        out << "    pub static ref PROT_DESC:LcsfProtDesc = LcsfProtDesc {prot_ver: PROT_VER, cmd_desc_arr: vec!["
            << Qt::endl;
        for (Command *command : cmdList) {
            out << "        (CMD_ID_" << command->getName().toUpper() << ", LcsfCmdDesc {att_desc_arr: ";
            if (command->getAttArray().size() == 0) {
                out << "Vec::new()})," << Qt::endl;
            } else {
                out << "vec![" << Qt::endl;
                for (Attribute *attribute : command->getAttArray()) {
                    QString desc_str = this->getAttDescString(attribute->getIsOptional(), attribute->getDataType());
                    out << "            (" << command->getName().toUpper() << "_ATT_ID_" << attribute->getName().toUpper()
                        << desc_str;
                    if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                        out << "vec![" << Qt::endl;
                        this->printAttDesc_Rec(attribute->getName(), attribute->getSubAttArray(), &out, 4);
                        out << "            ]})," << Qt::endl;
                    } else {
                        out << "Vec::new()})," << Qt::endl;
                    }
                }
                out << "        ]})," << Qt::endl;
            }
        }
        out << "    ]};" << Qt::endl;
        out << "}" << Qt::endl;

        file.close();
    }
}
