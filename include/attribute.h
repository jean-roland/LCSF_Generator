/**
 * \file attribute.h
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

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <QString>
#include <QList>
#include "enumtype.h"

class Attribute {
private:
    QString m_name;
    short m_id;
    bool m_isOptional;
    NS_AttDataType::T_AttDataType m_dataType;
    QList<Attribute *> m_subAttArray;
    QString m_desc;
    void *pObj;

public:
    typedef struct _attInfos {
        QString parentName;
        QString attName;
        NS_AttDataType::T_AttDataType  dataType;
        Attribute *attPointer;
        bool isOptional;
        short attId;
        int subAttNb;
        QString attDesc;
    } T_attInfos;

    Attribute(void);
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    Attribute(QString attName, short attId, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QList<Attribute *> attSubAttArray, QString attDesc);
    void setName(QString attName);
    QString getName(void);
    void setId(short attId);
    short getId(void);
    void setIsOptional(bool attIsOptional);
    bool getIsOptional(void);
    void setDataType(NS_AttDataType::T_AttDataType attDataType);
    NS_AttDataType::T_AttDataType getDataType(void);
    void setDesc(QString attDesc);
    QString getDesc(void);
    void setObjPtr(void *pObj);
    void *getObjPtr(void);
    void setSubAttArray(QList<Attribute *> attSubAttArray);
    void addSubAtt(Attribute *newAtt);
    QList<Attribute *> getSubAttArray(void);

    /**
     * @brief      Gets attribute total attribute and sub-attribute number recursively
     *
     * @return     The total attribute number
     */
    int getTotalAttNb_rec(void);

    QStringList getSubAttNamesList(void);
    void sortArrayById(void);
    void editParams(QString attName, short attCode, bool attIsOptional, NS_AttDataType::T_AttDataType attDataType, QString attDesc);
    void clearArray_rec(void);
    void removeAtt(QString attName);
    Attribute *getSubAttPointer(QString subAttName);

    static bool compareRefAttList(QList<Attribute *> a_list, QList<Attribute *> b_list, int& err_idx);

    friend bool operator==(const Attribute& lhs, const Attribute& rhs) {
        if (lhs.m_name != rhs.m_name) {
            return false;
        }
        if (lhs.m_id != rhs.m_id) {
            return false;
        }
        if (lhs.m_isOptional != rhs.m_isOptional) {
            return false;
        }
        if (lhs.m_dataType != rhs.m_dataType) {
            return false;
        }
        if (lhs.m_desc != rhs.m_desc) {
            return false;
        }
        if (lhs.m_subAttArray.count() != rhs.m_subAttArray.count()) {
            return false;
        }
        if (lhs.m_subAttArray.count() > 0) {
            int err_val;
            if(!Attribute::compareRefAttList(lhs.m_subAttArray, rhs.m_subAttArray, err_val)) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const Attribute& lhs, const Attribute& rhs) {
        return !(lhs == rhs);
    }

};

#endif // ATTRIBUTE_H
