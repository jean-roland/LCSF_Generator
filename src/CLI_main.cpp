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
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QList>
#include <QSet>
#include <QTextStream>

// Custom include
#include "attribute.h"
#include "codeextractor.h"
#include "codegenerator.h"
#include "command.h"
#include "deschandler.h"
#include "docgenerator.h"
#include "enumtype.h"
#include "rustextractor.h"
#include "rustgenerator.h"

// Private variables
static QString protocolName;
static QString protocolId;
static QString protocolDesc;
static QList<Command *> cmdArray;
static const QString defCOutPath = "./COutput";
static QString cOutPathA = defCOutPath;
static QString cOutPathB = defCOutPath;
static const QString defRustOutPath = "./RustOutput";
static QString rustOutPathA = defRustOutPath;
static QString rustOutPathB = defRustOutPath;
static QString docPath = "./Export";

static DocGenerator docgen;
static CodeGenerator codegen;
static RustGenerator rustgen;
static CodeExtractor codeextractA;
static CodeExtractor codeextractB;
static RustExtractor rustextractA;
static RustExtractor rustextractB;

// Private functions
static QString checkAttNameDuplicate_Rec(QSet<QString> *pSet, QList<Attribute *> attArray) {
    // Parse the attribute table
    for (Attribute *pAtt : attArray) {
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
    for (Command *pCmd : cmdList) {
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

    QCommandLineOption docGenOption(
        QStringList() << "d" << "doc", QCoreApplication::translate("main", "Activate doc generation"));
    parser.addOption(docGenOption);

    QCommandLineOption importAOption(QStringList() << "a" << "import-a",
        QCoreApplication::translate("main", "Import specific protocol C code, A point of view"),
        QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importAOption);

    QCommandLineOption importBOption(QStringList() << "b" << "import-b",
        QCoreApplication::translate("main", "Import specific protocol C code, B point of view"),
        QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importBOption);

    QCommandLineOption importRustAOption(QStringList() << "ra" << "import-rust-a",
        QCoreApplication::translate("main", "Import specific protocol Rust code, A point of view"),
        QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importRustAOption);

    QCommandLineOption importRustBOption(QStringList() << "rb" << "import-rust-b",
        QCoreApplication::translate("main", "Import specific protocol Rust code, B point of view"),
        QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importRustBOption);

    // Parse arguments
    parser.process(a);

    // Get arguments
    QString descFilePath = parser.value(loadDescOption);
    QString importAFilePath = parser.value(importAOption);
    QString importBFilePath = parser.value(importBOption);
    QString importRustAFilePath = parser.value(importRustAOption);
    QString importRustBFilePath = parser.value(importRustBOption);

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
        cOutPathA = importAInfo.absoluteDir().absolutePath();
        if (cOutPathB == defCOutPath) {
            cOutPathB = cOutPathA;
        }
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
        cOutPathB = importBInfo.absoluteDir().absolutePath();
        if (cOutPathA == defCOutPath) {
            cOutPathA = cOutPathB;
        }
        out << "Import B extraction successful." << Qt::endl;
    }
    if (parser.isSet("ra")) {
        QFile importRustAFile(importRustAFilePath);

        if (!importRustAFile.open(QIODevice::ReadOnly)) {
            out << "Error, couldn't open file: " << importRustAFilePath << ", reason: " << importRustAFile.errorString()
                << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importRustAStream(&importRustAFile);
        if (!rustextractA.extractFromSourceFile(protocolName, &importRustAStream, cmdArray)) {
            out << "Error while extracting Rust import A info." << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importRustAInfo(importRustAFile);
        rustOutPathA = importRustAInfo.absoluteDir().absolutePath();
        if (rustOutPathB == defRustOutPath) {
            rustOutPathB = rustOutPathA;
        }
        out << "Rust import A extraction successful." << Qt::endl;
    }
    if (parser.isSet("rb")) {
        QFile importRustBFile(importRustBFilePath);

        if (!importRustBFile.open(QIODevice::ReadOnly)) {
            out << "Error, couldn't open file: " << importRustBFilePath << ", reason: " << importRustBFile.errorString()
                << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importRustBStream(&importRustBFile);
        if (!rustextractB.extractFromSourceFile(protocolName, &importRustBStream, cmdArray)) {
            out << "Error while extracting Rust import B info." << Qt::endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importRustBInfo(importRustBFile);
        rustOutPathB = importRustBInfo.absoluteDir().absolutePath();
        if (rustOutPathA == defRustOutPath) {
            rustOutPathA = rustOutPathB;
        }
        out << "Rust import B extraction successful." << Qt::endl;
    }
    // Check data
    if (cmdArray.size() <= 0) {
        out << "Error, protocol has no command!" << Qt::endl;
        exit(EXIT_FAILURE);
    }
    QString dupName = checkAttNameDuplicate(cmdArray);
    if (dupName.size() > 0) {
        out << "Error, duplicate complex attribute name: '" + dupName + "'." << Qt::endl;
        exit(EXIT_FAILURE);
    }
    // Generate "A" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractA, cOutPathA);
    codegen.generateMain(protocolName, cmdArray, codeextractA, true, cOutPathA);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, cOutPathA);
    codegen.generateBridge(protocolName, cmdArray, true, cOutPathA);
    codegen.generateDescription(protocolName, cmdArray, cOutPathA);

    rustgen.generateMain(protocolName, cmdArray, true, rustOutPathA, rustextractA);
    rustgen.generateBridge(protocolName, protocolId, cmdArray, true, rustOutPathA);

    // Generate "B" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractB, cOutPathB);
    codegen.generateMain(protocolName, cmdArray, codeextractB, false, cOutPathB);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, cOutPathB);
    codegen.generateBridge(protocolName, cmdArray, false, cOutPathB);
    codegen.generateDescription(protocolName, cmdArray, cOutPathB);

    rustgen.generateMain(protocolName, cmdArray, false, rustOutPathB, rustextractB);
    rustgen.generateBridge(protocolName, protocolId, cmdArray, false, rustOutPathB);

    // Generate doc (if needed)
    if (parser.isSet("d")) {
        docgen.generateWikiTable(protocolName, protocolId, protocolDesc, cmdArray, docPath);
        docgen.generateDokuWikiTable(protocolName, protocolId, protocolDesc, cmdArray, docPath);
        docgen.generateMkdownTable(protocolName, protocolId, protocolDesc, cmdArray, docPath);
    }
    // End output
    out << "Generation complete." << Qt::endl;
    if (cOutPathA == cOutPathB) {
        out << "C code generated in: " << cOutPathA << Qt::endl;
    } else {
        out << "C code A generated in: " << cOutPathA << Qt::endl;
        out << "C code B generated in: " << cOutPathB << Qt::endl;
    }
    if (rustOutPathA == rustOutPathB) {
        out << "Rust code generated in: " << rustOutPathA << Qt::endl;
    } else {
        out << "Rust code A generated in: " << rustOutPathA << Qt::endl;
        out << "Rust code B generated in: " << rustOutPathB << Qt::endl;
    }
    if (parser.isSet("d")) {
        out << "Documentation generated in: " << docPath << Qt::endl;
    }
}
