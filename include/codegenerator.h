/**
 * \file codegenerator.h
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

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

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

class CodeGenerator {

private:
    bool protocolHasSubAtt;
    typedef struct _attInfos {
        QString parentName;
        QString attName;
        NS_AttDataType::T_AttDataType  dataType;
        Attribute *attPointer;
        bool isOptional;
        short attId;
        int subAttNb;
        QString attDesc;
    }T_attInfos;

    QString getAttDataRxPath(QString protocolName, QStringList parentNames, QString attName);
    QString getAttDataTxPath(QString protocolName, QStringList parentNames, QString attName);
    QString getPayloadPath(QStringList parentNames);
    QString getIndent(int indentNb);
    QString getFlagTypeStringFromAttNb(int attNb, QString varName);
    QString getTypeStringFromDataType(NS_AttDataType::T_AttDataType dataType);
    QString getInitStringFromDataType(NS_AttDataType::T_AttDataType dataType);
    QString getAttDescString(QString protocolName, QString parentName, Attribute *attribute);
    QList<T_attInfos> getAttInfos_Rec(QString parentName, QList<Attribute *> attList);
    QList<T_attInfos> getAttInfos(QList<Command *> cmdList);
    QList<CodeGenerator::T_attInfos> insertSortAttInfosListByParentName(QList<CodeGenerator::T_attInfos> attInfosList);
    bool findAttInfoId(short attId, QList<CodeGenerator::T_attInfos> attInfosList);
    QList<CodeGenerator::T_attInfos> removeAttInfosDuplicate(QList<CodeGenerator::T_attInfos> attInfosList);
    void fillSubAttData_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut , int indentNb);
    void fillSubAttPayload_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
    void declareAtt_REC(QString parentName, QList<Attribute *> attList, QTextStream *pOut);
    void grabAttValues_REC(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
    QString toCamelCase(const QString& s);
    QString toFirstLetterUpperCase(const QString& s);
    QList<CodeGenerator::T_attInfos> removeCommandAttributes(QList<CodeGenerator::T_attInfos> attInfosList, QList<Command *> cmdList);

public:
    CodeGenerator();
    void generateMainHeader(QString protocolName, QList<Command *> cmdList, CodeExtractor codeExtract, QString dirPath);
    void generateMain(QString protocolName, QList<Command *> cmdList, CodeExtractor codeExtract, bool isA, QString dirPath);
    void generateBridgeHeader(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath);
    void generateBridge(QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath);
    void generateDescription(QString protocolName, QList<Command *> cmdList, QString dirPath);
    void generateWikiTable(QString protocolName, QList<Command *> cmdList, QString dirPath);
    void generateDokuWikiTable(QString protocolName, QList<Command *> cmdList, QString dirPath);
    void generateMkdownTable(QString protocolName, QList<Command *> cmdList, QString dirPath);
};

#endif // CODEGENERATOR_H
