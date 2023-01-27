/**
 * \file attribute.h
 * \brief Command data class for the LCSF Generator.
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

#ifndef COMMAND_H
#define COMMAND_H
#include <QString>
#include "attribute.h"

class Command {
private:
    QString m_name;
    short m_id;
    bool m_hasAttributes;
    NS_DirectionType::T_DirectionType m_direction;
    QList<Attribute *> m_attArray;
    QString m_desc;

public:
    Command(void);
    Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QString commandDesc);
    Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QList<Attribute *> attArray, QString commandDesc);
    void setName(QString cmdName);
    QString getName(void);
    void setId(short cmdCode);
    short getId(void);
    void setHasAtt(bool cmdHasAtt);
    bool getHasAtt(void);
    bool hasSubAtt(void);
    void setDesc(QString cmdDesc);
    QString getDesc(void);
    void setDirection(NS_DirectionType::T_DirectionType cmdDirection);
    NS_DirectionType::T_DirectionType getDirection();
    bool isTransmittable(bool isA);
    bool isReceivable(bool isA);
    void setAttArray(QList<Attribute *> attArray);
    void addAttribute(Attribute *newAtt);
    QList<Attribute *> getAttArray(void);

    /**
     * @brief      Gets command total attribute and sub-attribute number
     *
     * @return     The total attribute number
     */
    int getTotalAttNb(void);

    QStringList getAttNamesList(void);
    void sortAttArrayById(void);
    void editParameters(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection , QString cmdDesc);
    void clearAttArray(void);
    void removeAttByName(QString attName);
    Attribute *getAttByName(QString cmdAttName);

    static Command *findCmdAddr(QString cmdName, QList<Command *> cmdList);
    static QList<Command *> sortListById(QList<Command *> cmdList);
    static int findCmdIdx(QString cmdName, QList<Command *> cmdList);
    static QStringList getListCmdNames(QList<Command *> cmdList);
    static bool compareRefCmdList(QList<Command *> a_list, QList<Command *> b_list, int& err_idx);
    static int getMaxAttNb(QList<Command *> cmdList);

    friend bool operator==(const Command& lhs, const Command& rhs) {
        if (lhs.m_name != rhs.m_name) {
            return false;
        }
        if (lhs.m_id != rhs.m_id) {
            return false;
        }
        if (lhs.m_hasAttributes != rhs.m_hasAttributes) {
            return false;
        }
        if (lhs.m_direction != rhs.m_direction) {
            return false;
        }
        if (lhs.m_desc != rhs.m_desc) {
            return false;
        }
        if (lhs.m_attArray.count() != rhs.m_attArray.count()) {
            return false;
        }
        if (lhs.m_attArray.count() > 0) {
            int err_val;
            if(!Attribute::compareRefAttList(lhs.m_attArray, rhs.m_attArray, err_val)) {
                return false;
            }
        }
        return true;
    }

    friend bool operator!=(const Command& lhs, const Command& rhs) {
        return !(lhs == rhs);
    }
};

#endif // COMMAND_H
