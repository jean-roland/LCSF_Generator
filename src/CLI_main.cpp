/**
 * \file CLI_main.cpp
 * \brief CLI main source file.
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

// Qt include
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QSet>

// Custom include
#include "command.h"
#include "attribute.h"
#include "enumtype.h"
#include "docgenerator.h"
#include "codegenerator.h"
#include "rustgenerator.h"
#include "codeextractor.h"
#include "deschandler.h"

// Private variables
static QString protocolName;
static QString protocolId;
static QString protocolDesc;
static QList<Command *> cmdArray;
static QString outAPath = "./OutputA";
static QString outBPath = "./OutputB";
static QString rustOutAPath = "./RustOutputA";
static QString rustOutBPath = "./RustOutputB";
static QString docPath = "./Export";

static DocGenerator docgen;
static CodeGenerator codegen;
static RustGenerator rustgen;
static CodeExtractor codeextractA;
static CodeExtractor codeextractB;

// Private functions
static QString checkAttNameDuplicate_Rec(QSet<QString> *pSet, QList<Attribute *>attArray) {
    // Parse the attribute table
    for (Attribute *pAtt: attArray) {
        // Only check duplicate name in complex attributes
        if (pAtt->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            QString attName = pAtt->getName();
            // Check if the name already exists
            if (!pSet->contains(attName)) {
                // Add name to set
                pSet->insert(attName);
            } else {
                // Name already exists, duplicate found
                return attName;
            }
            // Parse the sub-attributes
            QString dupName = checkAttNameDuplicate_Rec(pSet, pAtt->getSubAttArray());
            if (dupName.size() > 0) {
                return dupName;
            }
        }
    }
    return "";
}

static QString checkAttNameDuplicate(QList<Command *> cmdList) {
    QSet<QString> set;
    // Parse the command table for attributes
    for (Command *pCmd: cmdList) {
        if (pCmd->getHasAtt()) {
            QString dupName = checkAttNameDuplicate_Rec(&set, pCmd->getAttArray());
            if (dupName.size() > 0) {
                return dupName;
            }
        }
    }
    return "";
}

// Main function
int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("LCSF_Generator_CLI");
    QCoreApplication::setApplicationVersion(APP_VERSION);

    // Create command line arg parser
    QCommandLineParser parser;
    parser.setApplicationDescription("CLI variant of LCSF_Generator");
    parser.addHelpOption();
    parser.addVersionOption();

    // Add specific options

    QCommandLineOption loadDescOption(QStringList() << "l" << "load",
            QCoreApplication::translate("main", "(REQUIRED) Load a protocol description file"),
            QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(loadDescOption);

    QCommandLineOption docGenOption(QStringList() << "d" << "doc",
            QCoreApplication::translate("main", "Activate doc generation"));
    parser.addOption(docGenOption);

    QCommandLineOption importAOption(QStringList() << "a" << "import-a",
            QCoreApplication::translate("main", "Import specific protocol C code, A point of view"),
            QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importAOption);

    QCommandLineOption importBOption(QStringList() << "b" << "import-b",
            QCoreApplication::translate("main", "Import specific protocol C code, B point of view"),
            QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importBOption);

    // Parse arguments
    parser.process(a);

    // Get arguments
    QString descFilePath = parser.value(loadDescOption);
    QString importAFilePath = parser.value(importAOption);
    QString importBFilePath = parser.value(importBOption);

    // Process args
    QTextStream out(stdout);
    out << "*** LCSF_Generator_CLI v" << APP_VERSION << " ***" << Qt::endl;

    if (!parser.isSet("l")) {
        out << "Error, missing protocol description file. Use -h for help." << Qt::endl;
        exit(EXIT_FAILURE);
    }
    // Get description file
    QFile descFile(descFilePath);

    if (!descFile.open(QIODevice::ReadOnly)) {
        out << "Couldn't open file: " << descFilePath << ", reason: " << descFile.errorString() << Qt::endl;
        exit(EXIT_FAILURE);
    }
    // Process description file
    DescHandler::load_desc(descFile, cmdArray, protocolName, protocolId, protocolDesc);
    descFile.close();

    // Check import
    if (parser.isSet("a")) {
        // Get import A data
        QFile importAFile(importAFilePath);

        if (!importAFile.open(QIODevice::ReadOnly)) {
            out << "Error, couldn't open file: " << importAFilePath << ", reason: " << importAFile.errorString() << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importAStream(&importAFile);
        if (!codeextractA.extractFromSourceFile(protocolName, &importAStream, cmdArray)) {
            out << "Error while extracting import A info." << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importAInfo(importAFile);
        outAPath = importAInfo.absoluteDir().absolutePath();
        out << "Import A extraction successful." << Qt::endl;
    }
    if (parser.isSet("b")) {
        // Get import A data
        QFile importBFile(importBFilePath);

        if (!importBFile.open(QIODevice::ReadOnly)) {
            out << "Couldn't open file: " << importBFilePath << ", reason: " << importBFile.errorString() << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importBStream(&importBFile);
        if (!codeextractB.extractFromSourceFile(protocolName, &importBStream, cmdArray)) {
            out << "Error while extracting import B info." << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importBInfo(importBFile);
        outBPath = importBInfo.absoluteDir().absolutePath();
        out << "Import B extraction successful." << Qt::endl;
    }
    // Check data
    if (cmdArray.size() <= 0) {
        out << "Error, protocol has no command!" << Qt::endl;
        exit(EXIT_FAILURE);
    }
    QString dupName = checkAttNameDuplicate(cmdArray);
    if (dupName.size() > 0) {
        out << "Error, duplicate complex attribute name: '"+ dupName +"'." << Qt::endl;
        exit(EXIT_FAILURE);
    }
    // Generate "A" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractA, outAPath);
    codegen.generateMain(protocolName, cmdArray, codeextractA, true, outAPath);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, outAPath);
    codegen.generateBridge(protocolName, cmdArray, true, outAPath);
    codegen.generateDescription(protocolName, cmdArray, outAPath);

    rustgen.generateMain(protocolName, cmdArray, true, rustOutAPath);
    rustgen.generateBridge(protocolName, protocolId, cmdArray, true, rustOutAPath);

    // Generate "B" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractB, outBPath);
    codegen.generateMain(protocolName, cmdArray, codeextractB, false, outBPath);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, outBPath);
    codegen.generateBridge(protocolName, cmdArray, false, outBPath);
    codegen.generateDescription(protocolName, cmdArray, outBPath);

    rustgen.generateMain(protocolName, cmdArray, false, rustOutBPath);
    rustgen.generateBridge(protocolName, protocolId, cmdArray, false, rustOutBPath);

    // Generate doc (if needed)
    if (parser.isSet("d")) {
        docgen.generateWikiTable(protocolName, protocolId, cmdArray, docPath);
        docgen.generateDokuWikiTable(protocolName, protocolId, cmdArray, docPath);
        docgen.generateMkdownTable(protocolName, protocolId, cmdArray, docPath);
    }
    // End output
    out << "Generation complete." << Qt::endl;
    out << "Code A generated in: " << outAPath << ", code B generated in: " << outBPath << Qt::endl;
    out << "Rust code A generated in: " << rustOutAPath << ", Rust code B generated in: " << rustOutBPath << Qt::endl;
    if (parser.isSet("d")) {
        out << "Documentation generated in: " << docPath << Qt::endl;
    }
}
