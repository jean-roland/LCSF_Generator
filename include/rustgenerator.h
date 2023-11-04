/**
 * \file rustgenerator.h
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

#ifndef RUSTGENERATOR_H
#define RUSTGENERATOR_H

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

class RustGenerator {

private:
    bool protocolHasSubAtt;

    bool is_CString_needed(QList<Attribute::T_attInfos> attInfosList);
    QString capitalize(const QString &str);
    QString getPayloadPath(QStringList parentNames);
    QString getIndent(int indentNb);
    QList<Attribute::T_attInfos> insertSortAttInfosListByParentName(QList<Attribute::T_attInfos> attInfosList);
    QList<Attribute::T_attInfos> getAttInfos_Rec(QString parentName, QList<Attribute *> attList);
    QList<Attribute::T_attInfos> getAttInfos(QList<Command *> cmdList);
    void declareAtt_REC(QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, bool parentIsOpt);
    void grabAttValues_REC(QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb, bool parentIsOpt);
    void printInclude_Rec(QString protocolName, QString parentName, QList<Attribute *> attList, QTextStream *pOut);
    void printInitstruct_Rec(QString parentName, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
    void getSubAttData_Rec(QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
    void fillSubAttData_Rec(QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
    QString getAttDescString(bool isOptional, NS_AttDataType::T_AttDataType data_type);
    void printAttDesc_Rec(QString parentName, QList<Attribute *> attList, QTextStream *pOut, int indentNb);

public:
    RustGenerator();
    void generateMain(QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath);
    void generateBridge(QString protocolName, QString protocolId, QList<Command *> cmdList, bool isA, QString dirPath);
};

#endif // RUSTGENERATOR_H
