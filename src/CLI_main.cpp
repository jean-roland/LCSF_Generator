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
#include "codegenerator.h"
#include "codeextractor.h"
#include "deschandler.h"

// Cmake/qmake coexistence issue
#ifndef APP_VERSION
    #include <version.h>
#endif

// Private variables
static QString protocolName;
static QString protocolId;
static QList<Command *> cmdArray;
static QString outAPath = "./OutputA";
static QString outBPath = "./OutputB";
static QString docPath = "./Export";

static CodeGenerator codegen;
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
            QCoreApplication::translate("main", "Import specific protocol code, A point of view"),
            QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(importAOption);

    QCommandLineOption importBOption(QStringList() << "b" << "import-b",
            QCoreApplication::translate("main", "Import specific protocol code, B point of view"),
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
    out << "*** LCSF_Generator_CLI v" << APP_VERSION << " ***" << endl;

    if (!parser.isSet("l")) {
        out << "Error, missing protocol description file." << endl;
        exit(EXIT_FAILURE);
    }
    // Get description file
    QFile descFile(descFilePath);

    if (!descFile.open(QIODevice::ReadOnly)) {
        out << "Couldn't open file: " << descFilePath << ", reason: " << descFile.errorString() << endl;
        exit(EXIT_FAILURE);
    }
    // Process description file
    DescHandler::load_desc(descFile, cmdArray, protocolName, protocolId);
    descFile.close();

    // Check import
    if (parser.isSet("a")) {
        // Get import A data
        QFile importAFile(importAFilePath);

        if (!importAFile.open(QIODevice::ReadOnly)) {
            out << "Error, couldn't open file: " << importAFilePath << ", reason: " << importAFile.errorString() << endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importAStream(&importAFile);
        if (!codeextractA.extractFromSourceFile(protocolName, &importAStream, cmdArray)) {
            out << "Error while extracting import A info." << endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importAInfo(importAFile);
        outAPath = importAInfo.absoluteDir().absolutePath();
        out << "Import A extraction successful." << endl;
    }
    if (parser.isSet("b")) {
        // Get import A data
        QFile importBFile(importBFilePath);

        if (!importBFile.open(QIODevice::ReadOnly)) {
            out << "Couldn't open file: " << importBFilePath << ", reason: " << importBFile.errorString() << endl;
            exit(EXIT_FAILURE);
        }
        QTextStream importBStream(&importBFile);
        if (!codeextractB.extractFromSourceFile(protocolName, &importBStream, cmdArray)) {
            out << "Error while extracting import B info." << endl;
            exit(EXIT_FAILURE);
        }
        QFileInfo importBInfo(importBFile);
        outBPath = importBInfo.absoluteDir().absolutePath();
        out << "Import B extraction successful." << endl;
    }
    // Check data
    if (cmdArray.size() <= 0) {
        out << "Error, protocol has no command!" << endl;
        exit(EXIT_FAILURE);
    }
    QString dupName = checkAttNameDuplicate(cmdArray);
    if (dupName.size() > 0) {
        out << "Error, duplicate complex attribute name: '"+ dupName +"'." << endl;
        exit(EXIT_FAILURE);
    }
    // Generate "A" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractA, outAPath);
    codegen.generateMain(protocolName, cmdArray, codeextractA, true, outAPath);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, outAPath);
    codegen.generateBridge(protocolName, cmdArray, true, outAPath);
    codegen.generateDescription(protocolName, cmdArray, outAPath);
    // Generate "B" C files
    codegen.generateMainHeader(protocolName, cmdArray, codeextractB, outBPath);
    codegen.generateMain(protocolName, cmdArray, codeextractB, false, outBPath);
    codegen.generateBridgeHeader(protocolName, protocolId, cmdArray, outBPath);
    codegen.generateBridge(protocolName, cmdArray, false, outBPath);
    codegen.generateDescription(protocolName, cmdArray, outBPath);
    // Generate doc (if needed)
    if (parser.isSet("d")) {
        codegen.generateWikiTable(protocolName, cmdArray, docPath);
        codegen.generateDokuWikiTable(protocolName, cmdArray, docPath);
        codegen.generateMkdownTable(protocolName, cmdArray, docPath);
    }
    // End output
    out << "Generation complete." << endl;
    out << "Code A generated in: " << outAPath << ", code B generated in: " << outBPath << endl;
    if (parser.isSet("d")) {
        out << "Documentation generated in: " << docPath << endl;
    }
    exit(EXIT_FAILURE);
    return a.exec();
}
