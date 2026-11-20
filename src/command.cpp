/**
 * \file command.cpp
 * \brief Command class.
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

#include "command.h"

Command::Command(void) {
    this->m_name = "default";
    this->m_id = 0;
    this->m_hasAttributes = false;
    this->m_attArray = QList<Attribute *>();
    this->m_desc = QString();
}

Command::Command(
    QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QString commandDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_attArray = QList<Attribute *>();
    this->m_desc = commandDesc;
}

Command::Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection,
    QList<Attribute *> attArray, QString commandDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_attArray = attArray;
    this->m_desc = commandDesc;
}

void Command::setName(QString cmdName) {
    this->m_name = cmdName;
}

QString Command::getName(void) {
    return this->m_name;
}

void Command::setId(short cmdCode) {
    this->m_id = cmdCode;
}

short Command::getId(void) {
    return this->m_id;
}

void Command::setHasAtt(bool cmdHasAtt) {
    this->m_hasAttributes = cmdHasAtt;
}

bool Command::getHasAtt(void) {
    return this->m_hasAttributes;
}

bool Command::hasSubAtt(void) {
    for (Attribute *attribute : this->m_attArray) {
        if (attribute->getSubAttArray().size() > 0) {
            return true;
        }
    }
    return false;
}

void Command::setDesc(QString cmdDesc) {
    this->m_desc = cmdDesc;
}

QString Command::getDesc(void) {
    return this->m_desc;
}

void Command::setDirection(NS_DirectionType::T_DirectionType cmdDirection) {
    this->m_direction = cmdDirection;
}

NS_DirectionType::T_DirectionType Command::getDirection(void) {
    return this->m_direction;
}

bool Command::isTransmittable(bool isA) {
    if (this->m_direction == NS_DirectionType::BIDIRECTIONAL) {
        return true;
    } else if (isA && (this->m_direction == NS_DirectionType::A_TO_B)) {
        return true;
    } else if (!isA && (this->m_direction == NS_DirectionType::B_TO_A)) {
        return true;
    }
    return false;
}

bool Command::isReceivable(bool isA) {
    if (this->m_direction == NS_DirectionType::BIDIRECTIONAL) {
        return true;
    } else if (isA && (this->m_direction == NS_DirectionType::B_TO_A)) {
        return true;
    } else if (!isA && (this->m_direction == NS_DirectionType::A_TO_B)) {
        return true;
    }
    return false;
}

void Command::setAttArray(QList<Attribute *> attArray) {
    this->m_attArray = attArray;
}

void Command::addAttribute(Attribute *newAtt) {
    this->m_attArray.append(newAtt);
}

QList<Attribute *> Command::getAttArray(void) {
    return this->m_attArray;
}

int Command::getTotalAttNb(void) {
    // Note attribute nb
    int attNb = this->getAttArray().size();
    // Parse attribute array for sub-attributes
    for (Attribute *attribute : this->getAttArray()) {
        attNb += attribute->getTotalAttNb_rec();
    }
    return attNb;
}

QStringList Command::getAttNamesList(void) {
    QStringList attNames = QStringList();

    for (Attribute *attribute : this->getAttArray()) {
        attNames.append(attribute->getName());
    }
    return attNames;
}

void Command::sortAttArrayById(void) {
    QList<Attribute *> sortedAttArray = this->m_attArray;
    Attribute *bufferAttribute;
    int j;
    int size = sortedAttArray.size();

    for (int i = 0; i < size; i++) {
        j = i;
        while ((j > 0) && (sortedAttArray.at(j - 1)->getId() > sortedAttArray.at(j)->getId())) {
            bufferAttribute = sortedAttArray.at(j);
            sortedAttArray.replace(j, sortedAttArray.at(j - 1));
            sortedAttArray.replace(j - 1, bufferAttribute);
            j = j - 1;
        }
    }
    this->m_attArray = sortedAttArray;
}

void Command::editParameters(
    QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QString cmdDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_desc = cmdDesc;
}

void Command::removeAttByName(QString attName) {
    Attribute *attStorage = this->getAttByName(attName);
    if (attStorage != nullptr) {
        attStorage->clearArray_rec();
        this->m_attArray.removeAll(attStorage);
        delete attStorage;
    }
}

void Command::clearAttArray(void) {
    for (Attribute *attribute : this->m_attArray) {
        if (attribute->getSubAttArray().size() > 0) {
            attribute->clearArray_rec();
        }
    }
    while (!this->m_attArray.isEmpty()) {
        delete this->m_attArray.takeFirst();
    }
}

Attribute *Command::getAttByName(QString cmdAttName) {
    Attribute *cmdAttPointer = nullptr;

    for (Attribute *attribute : this->m_attArray) {
        if ((attribute != nullptr) && (attribute->getName().compare(cmdAttName) == 0)) {
            cmdAttPointer = attribute;
            break;
        }
    }
    return cmdAttPointer;
}

Command *Command::findCmdAddr(QString cmdName, QList<Command *> cmdList) {
    Command *cmdStorage = nullptr;

    for (Command *command : cmdList) {
        if (command->getName().compare(cmdName) == 0) {
            cmdStorage = command;
            break;
        }
    }
    return cmdStorage;
}

QList<Command *> Command::sortListById(QList<Command *> cmdList) {
    QList<Command *> sortedArray = cmdList;
    Command *bufferCmd;
    int j;
    int size = sortedArray.size();

    for (int i = 0; i < size; i++) {
        j = i;
        while ((j > 0) && (sortedArray.at(j - 1)->getId() > sortedArray.at(j)->getId())) {
            bufferCmd = sortedArray.at(j);
            sortedArray.replace(j, sortedArray.at(j - 1));
            sortedArray.replace(j - 1, bufferCmd);
            j = j - 1;
        }
    }
    return sortedArray;
}

int Command::findCmdIdx(QString cmdName, QList<Command *> cmdList) {
    int cmdIdx = -1;

    for (int idx = 0; idx < cmdList.size(); idx++) {
        if (cmdList.at(idx)->getName().compare(cmdName) == 0) {
            cmdIdx = idx;
            break;
        }
    }
    return cmdIdx;
}

QStringList Command::getListCmdNames(QList<Command *> cmdList) {
    QStringList cmdNames = QStringList();

    for (Command *command : cmdList) {
        cmdNames.append(command->getName());
    }
    return cmdNames;
}

bool Command::compareRefCmdList(QList<Command *> a_list, QList<Command *> b_list, int &err_idx) {
    if (a_list.count() != b_list.count()) {
        err_idx = -1;
        return false;
    }
    for (int idx = 0; idx < a_list.count(); idx++) {
        if (*a_list.at(idx) != *b_list.at(idx)) {
            err_idx = idx;
            return false;
        }
    }
    return true;
}

int Command::getMaxAttNb(QList<Command *> cmdList) {
    int max = 0;

    // Parse command list to find the max total attribute
    for (Command *command : cmdList) {
        int attNb = command->getTotalAttNb();
        // Max comparison
        if (attNb > max) {
            max = attNb;
        }
    }
    return max;
}

static void getStructAtt_Rec(QList<Attribute *> attList, QList<Attribute *> &structAttList) {
    for (Attribute *attribute : attList) {
        if (attribute->getDataType() == NS_AttDataType::STRUCT) {
            structAttList.append(attribute);
        }
        getStructAtt_Rec(attribute->getSubAttArray(), structAttList);
    }
}

int Command::getStructBuffSize(void) {
    QList<Attribute *> structAttList;
    int size = 0;

    getStructAtt_Rec(this->m_attArray, structAttList);
    // StructBuff holds single structs, the application serializes arrays with the array helpers
    for (Attribute *attribute : structAttList) {
        if ((attribute->getStruct() != nullptr) && (!attribute->getIsArray())) {
            size += Attribute::getStructSize(attribute->getStruct());
        }
    }
    return size;
}

int Command::getMaxStructBuffSize(QList<Command *> cmdList) {
    int max = 0;

    for (Command *command : cmdList) {
        int size = command->getStructBuffSize();
        if (size > max) {
            max = size;
        }
    }
    return max;
}

QList<Attribute::T_structType *> Command::getStructList(QList<Command *> cmdList) {
    QList<Attribute::T_structType *> structList;

    for (Command *command : cmdList) {
        QList<Attribute *> structAttList;
        getStructAtt_Rec(command->getAttArray(), structAttList);
        for (Attribute *attribute : structAttList) {
            if ((attribute->getStruct() != nullptr) && (!structList.contains(attribute->getStruct()))) {
                structList.append(attribute->getStruct());
            }
        }
    }
    return structList;
}

bool Command::hasStructFieldType(QList<Command *> cmdList, bool (*pIsType)(NS_FieldType::T_FieldType)) {
    for (Attribute::T_structType *pStruct : Command::getStructList(cmdList)) {
        for (const Attribute::T_structField &field : pStruct->fields) {
            if (pIsType(field.dataType)) {
                return true;
            }
        }
    }
    return false;
}

static void getStructAttNames_Rec(
    QString parentName, QList<Attribute *> attList, Attribute::T_structType *pStruct, QStringList &names) {
    for (Attribute *attribute : attList) {
        if ((attribute->getDataType() == NS_AttDataType::STRUCT) && (attribute->getStruct() == pStruct)) {
            names.append(parentName + "." + attribute->getName());
        }
        getStructAttNames_Rec(attribute->getName(), attribute->getSubAttArray(), pStruct, names);
    }
}

QStringList Command::getStructAttNames(QList<Command *> cmdList, Attribute::T_structType *pStruct) {
    QStringList names;

    for (Command *command : cmdList) {
        getStructAttNames_Rec(command->getName(), command->getAttArray(), pStruct, names);
    }
    return names;
}

QString Command::checkStructs(QList<Command *> cmdList) {
    for (Command *command : cmdList) {
        QList<Attribute *> structAttList;
        getStructAtt_Rec(command->getAttArray(), structAttList);
        for (Attribute *attribute : structAttList) {
            if (attribute->getStruct() == nullptr) {
                return "Attribute '" + attribute->getName() + "' has no struct type";
            }
        }
    }
    QStringList structNames;
    for (Attribute::T_structType *pStruct : Command::getStructList(cmdList)) {
        QStringList fieldNames;
        // Generated names are lower case
        if (structNames.contains(pStruct->name, Qt::CaseInsensitive)) {
            return "Struct '" + pStruct->name + "' is defined twice";
        }
        structNames.append(pStruct->name);
        if (pStruct->fields.isEmpty()) {
            return "Struct '" + pStruct->name + "' has no field";
        }
        for (const Attribute::T_structField &field : pStruct->fields) {
            if (fieldNames.contains(field.name, Qt::CaseInsensitive)) {
                return "Struct '" + pStruct->name + "' field '" + field.name + "' is defined twice";
            }
            fieldNames.append(field.name);
            if (field.dataType == NS_FieldType::UNKNOWN) {
                return "Struct '" + pStruct->name + "' field '" + field.name + "' has an unknown type";
            }
        }
        // Variable-length elements each carry their size, which is the extensible array format
        if ((Attribute::hasStructVarField(pStruct)) && (!pStruct->isExtensible)) {
            return "Struct '" + pStruct->name + "' has variable-length fields, it must be extensible";
        }
        // The array element size is sent on a byte
        if ((pStruct->isExtensible) && (Attribute::getStructSize(pStruct) > UINT8_MAX)) {
            return "Extensible struct '" + pStruct->name + "' is over 255 bytes";
        }
    }
    return QString();
}