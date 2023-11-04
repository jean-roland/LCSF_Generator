/**
 * \file DocGenerator.h
 * \brief Code generator module
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

#ifndef DocGenerator_H
#define DocGenerator_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QDate>
#include "enumtype.h"
#include "attribute.h"
#include "command.h"
#include "codeextractor.h"

class DocGenerator {

private:
    QList<Attribute::T_attInfos> getAttInfos_Rec(QString parentName, QList<Attribute *> attList);
    QList<Attribute::T_attInfos> getAttInfos(QList<Command *> cmdList);
    QList<Attribute::T_attInfos> insertSortAttInfosListByParentName(QList<Attribute::T_attInfos> attInfosList);
    QList<Attribute::T_attInfos> removeCommandAttributes(QList<Attribute::T_attInfos> attInfosList, QList<Command *> cmdList);

public:
    DocGenerator();
    void generateWikiTable(QString protocolName, QString protocolId, QString protocolDesc, QList<Command *> cmdList, QString dirPath);
    void generateDokuWikiTable(QString protocolName, QString protocolId, QString protocolDesc, QList<Command *> cmdList, QString dirPath);
    void generateMkdownTable(QString protocolName, QString protocolId, QString protocolDesc, QList<Command *> cmdList, QString dirPath);
};

#endif // DocGenerator_H
