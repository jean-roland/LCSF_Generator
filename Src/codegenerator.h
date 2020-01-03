#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QTextStream>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
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
    QString getAttDescString(QString protocolName, QString parentName, Attribute *attribute);
    QList<T_attInfos> getAttInfos_Rec(QString parentName, QList<Attribute *> attList);
    QList<T_attInfos> getAttInfos(QList<Command *> cmdList);
    QList<CodeGenerator::T_attInfos> insertSortAttInfosListByParentName(QList<CodeGenerator::T_attInfos> attInfosList);
    bool findAttInfoId(short attId, QList<CodeGenerator::T_attInfos> attInfosList);
    QList<CodeGenerator::T_attInfos> removeAttInfosDuplicate(QList<CodeGenerator::T_attInfos> attInfosList);
    void fillSubAttData_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut , int indentNb);
    void fillSubAttPayload_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb);
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
    void generateMkdownTable(QString protocolName, QList<Command *> cmdList, QString dirPath);
};

#endif // CODEGENERATOR_H
