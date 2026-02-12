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
    QString tmp = str;
    tmp = tmp.toLower();
    tmp[0] = str[0].toUpper();
    return tmp;
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
                if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower() << " = &Vec::new();" << Qt::endl;
                } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower()
                          << " = &CString::new("
                             ").unwrap();"
                          << Qt::endl;
                } else {
                    *pOut << "    let mut " << parentNames.last().toLower().toLower() << "_"
                          << attribute->getName().toLower() << " = 0;" << Qt::endl;
                }
            } else {
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) ||
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
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) ||
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
                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) ||
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
void RustGenerator::printInclude_Rec(
    QString protocolName, QString parentName, QList<Attribute *> attList, QTextStream *pOut) {
    for (Attribute *attribute : attList) {
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            *pOut << "use protocol_" << protocolName.toLower() << "::" << this->capitalize(parentName) << "Att"
                  << this->capitalize(attribute->getName()) << "Payload;" << Qt::endl;
            printInclude_Rec(protocolName, attribute->getName(), attribute->getSubAttArray(), pOut);
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
            *pOut << indent << "if let LcsfValidAttPayload::SubattArr(subatt_arr) = &subatt_iter.next().unwrap().payload {"
                  << Qt::endl;
            if (attribute->getIsOptional()) {
                *pOut << indent << "    if *subatt_arr != Vec::new() {" << Qt::endl;
                *pOut << indent << "        " << attDataPath << "is_" << attribute->getName().toLower() << "_here = true;"
                      << Qt::endl;
                *pOut << indent << "        // Get iterator" << Qt::endl;
                *pOut << indent << "        let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                this->getSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 2);
                *pOut << indent << "    }" << Qt::endl;
            } else {
                *pOut << indent << "    // Get iterator" << Qt::endl;
                *pOut << indent << "    let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                this->getSubAttData_Rec(nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
            }
            *pOut << indent << "}" << Qt::endl;
        } else {
            *pOut << indent << "if let LcsfValidAttPayload::Data(data) = &subatt_iter.next().unwrap().payload {" << Qt::endl;
            if (attribute->getIsOptional()) {
                *pOut << indent << "    if *data != Vec::new() {" << Qt::endl;
                *pOut << indent << "        " << attDataPath << "is_" << attribute->getName().toLower() << "_here = true;"
                      << Qt::endl;
                switch (attribute->getDataType()) {
                    case NS_AttDataType::UINT8:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT16:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = lcsf_validator::vle_decode(data) as u16;" << Qt::endl;
                        break;
                    case NS_AttDataType::UINT32:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
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
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower() << " = data.clone();"
                              << Qt::endl;
                        break;
                    case NS_AttDataType::STRING:
                        *pOut << indent << "        " << attDataPath << attribute->getName().toLower()
                              << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                        break;
                }
                *pOut << indent << "    }" << Qt::endl;
            } else {
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
                }
            }
            *pOut << indent << "}" << Qt::endl;
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
    switch (data_type) {
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
void RustGenerator::generateMain(QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath) {
    QString low_prot_name = protocolName.toLower();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString fileName = dirPath + "/protocol_" + low_prot_name + ".rs";
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
        out << "use crate::lcsf_prot::lcsf_protocol_" << low_prot_name << ";" << Qt::endl;
        out << "use lcsf_core::LcsfCore;" << Qt::endl;
        out << "use lcsf_validator::LcsfValidCmd;" << Qt::endl;
        // Check if Cstring needed
        if (this->is_CString_needed(attInfosList)) {
            out << "use std::ffi::CString;" << Qt::endl;
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

        // Command execute functions
        for (int idx = 0; idx < cmdList.size(); idx++) {
            Command *command = cmdList.at(idx);

            if (command->isReceivable(isA)) {
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
                                if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                                    out << "    let mut " << attribute->getName().toLower() << " = &Vec::new();" << Qt::endl;
                                } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                                    out << "    let mut " << attribute->getName().toLower()
                                        << " = &CString::new(\"\").unwrap();" << Qt::endl;
                                } else {
                                    out << "    let mut " << attribute->getName().toLower() << " = 0;" << Qt::endl;
                                }
                            } else {
                                if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) ||
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
                                (attribute->getDataType() == NS_AttDataType::STRING)) {
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
        out << Qt::endl;

        // Utility functions
        out << "/// Init a LcsfCore with the protocol" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// core: LcsfCore reference" << Qt::endl;
        out << "pub fn init_core(core: &mut LcsfCore) {" << Qt::endl;
        out << "    // Add protocol to LcsfCore" << Qt::endl;
        out << "    core.add_protocol(" << Qt::endl;
        out << "        lcsf_protocol_" << low_prot_name << "::PROT_ID," << Qt::endl;
        out << "        &lcsf_protocol_" << low_prot_name << "::PROT_DESC," << Qt::endl;
        out << "        process_cmd," << Qt::endl;
        out << "    );" << Qt::endl;
        out << "}" << Qt::endl;
        out << Qt::endl;
        out << "/// Process command callback, customize as you need" << Qt::endl;
        out << "///" << Qt::endl;
        out << "/// valid_cmd: received valid command" << Qt::endl;
        out << "fn process_cmd(core: &LcsfCore, valid_cmd: &LcsfValidCmd) {" << Qt::endl;
        out << "    // Process received command" << Qt::endl;
        out << "    let (cmd_name, cmd_payload) = lcsf_protocol_" << low_prot_name << "::receive_cmd(valid_cmd);"
            << Qt::endl;
        out << "    execute_cmd(cmd_name, &cmd_payload);" << Qt::endl;
        out << "    // Here the function will send back received commands like an echo" << Qt::endl;
        out << "    // Customize as needed" << Qt::endl;
        out << "    let valid_cmd = lcsf_protocol_" << low_prot_name << "::send_cmd(cmd_name, &cmd_payload);" << Qt::endl;
        out << "    core.send_cmd(lcsf_protocol_" << low_prot_name << "::PROT_ID, &valid_cmd);" << Qt::endl;
        out << "    todo!();" << Qt::endl;
        out << "}" << Qt::endl;

        file.close();
    }
}

void RustGenerator::generateBridge(
    QString protocolName, QString protocolId, QList<Command *> cmdList, bool isA, QString dirPath) {
    QString low_prot_name = protocolName.toLower();
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString fileName = dirPath + "/lcsf_protocol_" + low_prot_name + ".rs";
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
        out << "use crate::lcsf_prot::protocol_" << low_prot_name << ";" << Qt::endl;
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
                    out << "use protocol_" << low_prot_name << "::" << this->capitalize(command->getName()) << "AttPayload;"
                        << Qt::endl;
                    printInclude_Rec(protocolName, command->getName(), command->getAttArray(), &out);
                }
            }
        }
        out << "use protocol_" << low_prot_name << "::CmdEnum;" << Qt::endl;
        out << "use protocol_" << low_prot_name << "::CmdPayload;" << Qt::endl;
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
                    }
                }
                out << "    };" << Qt::endl;
                out << "    // Get iterator" << Qt::endl;
                out << "    let att_iter = &mut att_arr.iter();" << Qt::endl;
                //  Retrieve attribute data
                for (Attribute *attribute : command->getAttArray()) {
                    out << "    // Retrieve data of attribute " << attribute->getName().toLower() << Qt::endl;
                    if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                        out << "    if let LcsfValidAttPayload::SubattArr(subatt_arr) = &att_iter.next().unwrap().payload {"
                            << Qt::endl;
                        QStringList parentNames = {command->getName(), attribute->getName()};
                        if (attribute->getIsOptional()) {
                            out << "        if *subatt_arr != Vec::new() {" << Qt::endl;
                            out << "            " << command->getName().toLower() << "_payload.is_"
                                << attribute->getName().toLower() << "_here = true;" << Qt::endl;
                            out << "            // Get iterator" << Qt::endl;
                            out << "            let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                            this->getSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 3);
                            out << "        }" << Qt::endl;
                        } else {
                            out << "        // Get iterator" << Qt::endl;
                            out << "        let subatt_iter = &mut subatt_arr.iter();" << Qt::endl;
                            this->getSubAttData_Rec(parentNames, attribute->getSubAttArray(), &out, 2);
                        }
                        out << "    }" << Qt::endl;
                    } else {
                        out << "    if let LcsfValidAttPayload::Data(data) = &att_iter.next().unwrap().payload {"
                            << Qt::endl;
                        if (attribute->getIsOptional()) {
                            out << "        if *data != Vec::new() {" << Qt::endl;
                            out << "            " << command->getName().toLower() << "_payload.is_"
                                << attribute->getName().toLower() << "_here = true;" << Qt::endl;
                            switch (attribute->getDataType()) {
                                case NS_AttDataType::UINT8:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = u8::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT16:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u16;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT32:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data) as u32;"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::UINT64:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = lcsf_validator::vle_decode(data);"
                                        << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT32:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f32::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::FLOAT64:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = f64::from_le_bytes(data.as_slice().try_into().unwrap());" << Qt::endl;
                                    break;
                                case NS_AttDataType::BYTE_ARRAY:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower() << " = data.clone();" << Qt::endl;
                                    break;
                                case NS_AttDataType::STRING:
                                    out << "            " << command->getName().toLower() << "_payload."
                                        << attribute->getName().toLower()
                                        << " = CString::from_vec_with_nul(data.clone()).unwrap();" << Qt::endl;
                                    break;
                            }
                            out << "        }" << Qt::endl;
                        } else {
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
                            }
                        }
                        out << "    }" << Qt::endl;
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

        // Commands Ids enum
        out << "/// Commands ids" << Qt::endl;
        for (Command *command : cmdList) {
            out << "const CMD_ID_" << command->getName().toUpper() << ": u16 = 0x" << QString::number(command->getId(), 16)
                << ";" << Qt::endl;
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
        out << "    pub static ref PROT_DESC:LcsfProtDesc = LcsfProtDesc {cmd_desc_arr: vec![" << Qt::endl;
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
