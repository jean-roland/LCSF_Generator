/**
 * \file attribute.cpp
 * \brief Attribute data class for the LCSF Generator.
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

#include "attribute.h"

Attribute::Attribute(void) {
    this->m_name = "default";
    this->m_id = 0;
    this->m_isOptional = true;
    this->m_dataType = NS_AttDataType::UNKNOWN;
    this->m_subAttArray = QList<Attribute *>();
    this->m_desc = QString();
}

Attribute::Attribute(
    QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc) {
    this->m_name = attName;
    this->m_id = attId;
    this->m_isOptional = attIsOptional;
    this->m_dataType = attDataType;
    this->m_subAttArray = QList<Attribute *>();
    this->m_desc = attDesc;
}

Attribute::Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType,
    QList<Attribute *> attSubAttArray, QString attDesc) {
    this->m_name = attName;
    this->m_id = attId;
    this->m_isOptional = attIsOptional;
    this->m_dataType = attDataType;
    this->m_subAttArray = attSubAttArray;
    this->m_desc = attDesc;
}

void Attribute::setName(QString attName) {
    this->m_name = attName;
}

QString Attribute::getName(void) {
    return this->m_name;
}

void Attribute::setId(short attId) {
    this->m_id = attId;
}

short Attribute::getId(void) {
    return this->m_id;
}

void Attribute::setIsOptional(bool attIsOptional) {
    this->m_isOptional = attIsOptional;
}

bool Attribute::getIsOptional(void) {
    return this->m_isOptional;
}

void Attribute::setDataType(NS_AttDataType::T_AttDataType attDataType) {
    this->m_dataType = attDataType;
}

NS_AttDataType::T_AttDataType Attribute::getDataType(void) {
    return this->m_dataType;
}

void Attribute::setDesc(QString attDesc) {
    this->m_desc = attDesc;
}

QString Attribute::getDesc(void) {
    return this->m_desc;
}

void Attribute::setObjPtr(void *pObj) {
    this->pObj = pObj;
}

void *Attribute::getObjPtr(void) {
    return this->pObj;
}

void Attribute::setSubAttArray(QList<Attribute *> attSubAttArray) {
    this->m_subAttArray = attSubAttArray;
}

void Attribute::addSubAtt(Attribute *newAtt) {
    this->m_subAttArray.append(newAtt);
}

QList<Attribute *> Attribute::getSubAttArray(void) {
    return this->m_subAttArray;
}

int Attribute::getTotalAttNb_rec(void) {
    // Note sub attribute array size
    int attNb = this->getSubAttArray().size();
    // Parse sub attributes recursively
    for (Attribute *attribute : this->getSubAttArray()) {
        attNb += attribute->getTotalAttNb_rec();
    }
    return attNb;
}

QStringList Attribute::getSubAttNamesList(void) {
    QStringList subAttNames = QStringList();

    for (Attribute *attribute : this->getSubAttArray()) {
        subAttNames.append(attribute->getName());
    }
    return subAttNames;
}

void Attribute::sortArrayById(void) {
    QList<Attribute *> sortedAttArray = this->m_subAttArray;
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
    this->m_subAttArray = sortedAttArray;
}

void Attribute::editParams(
    QString attName, short attCode, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc) {
    this->m_name = attName;
    this->m_id = attCode;
    this->m_isOptional = attIsOptional;
    this->m_dataType = attDataType;
    this->m_desc = attDesc;
}

void Attribute::removeAtt(QString attName) {
    Attribute *attStorage = this->getSubAttPointer(attName);
    if (attStorage != nullptr) {
        attStorage->clearArray_rec();
        this->m_subAttArray.removeAll(attStorage);
        delete attStorage;
    }
}

void Attribute::clearArray_rec(void) {
    for (Attribute *attribute : this->m_subAttArray) {
        if (attribute->getSubAttArray().size() > 0) {
            attribute->clearArray_rec();
        }
    }
    while (!this->m_subAttArray.isEmpty()) {
        delete this->m_subAttArray.takeFirst();
    }
}

Attribute *Attribute::getSubAttPointer(QString subAttName) {
    Attribute *subAttPointer = nullptr;

    for (Attribute *attribute : this->m_subAttArray) {
        if ((attribute != nullptr) && (attribute->getName().compare(subAttName) == 0)) {
            subAttPointer = attribute;
            break;
        }
    }
    return subAttPointer;
}

bool Attribute::compareRefAttList(QList<Attribute *> a_list, QList<Attribute *> b_list, int &err_idx) {
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
