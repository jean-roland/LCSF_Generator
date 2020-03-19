// Qt include
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QSet>
#include <QStringBuilder>
#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
// Custom include
#include "command.h"
#include "attribute.h"
#include "enumtype.h"
#include "codegenerator.h"
#include "codeextractor.h"
#include <version.h>

// Private variables
static QString protocolName;
static QString protocolId;
static QList<Command *> cmdArray;
static QString outAPath = "./OutputA";
static QString outBPath = "./OutputB";

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

static QString correctInputString(QString input) {
    QRegExp re(R"([^\w])");
    QString tmp = input.replace(re, "");
    if (tmp.size() > 64) {
        return tmp.chopped(tmp.size() - 64);
    } else if (tmp.size() == 0) {
        return "default_" + QString::number(QRandomGenerator::global()->generate());
    } else {
        return tmp;
    }
}

static void loadAtt_Rec(Command *pParentCmd, Attribute *pParentAtt, const QJsonObject& attribute) {
    Attribute *Attr(new Attribute(attribute.value(QLatin1String("name")).toString(),
                                static_cast<short>(attribute.value(QLatin1String("id")).toInt()),
                                attribute.value(QLatin1String("isOptional")).toBool(),
                                NS_AttDataType::SLAttDataType2Enum[attribute.value(QLatin1String("dataType")).toInt()],
                                attribute.value(QLatin1String("desc")).toString()));
    // If this is a sub-attribute
    if (pParentAtt != nullptr) {
        pParentAtt->addSubAtt(Attr);
    }
    // Parse sub-attributes
    if (NS_AttDataType::SLAttDataType2Enum[attribute.value(QLatin1String("dataType")).toInt()] == NS_AttDataType::SUB_ATTRIBUTES) {
        for (const QJsonValueRef SubAttr : attribute.value(QLatin1String("subAttr")).toArray()) {
            QJsonObject SubAttrObject(SubAttr.toObject());
            loadAtt_Rec(nullptr, Attr, SubAttrObject);
        }
    }
    // If this is a command attribute
    if (pParentCmd != nullptr) {
        pParentCmd->addAttribute(Attr);
    }
}

static void load_desc(QFile& file) {
    QJsonDocument DescFile(QJsonDocument::fromJson(file.readAll()));
    QJsonObject DescFileObject(DescFile.object());

    protocolName = correctInputString(DescFileObject.value(QLatin1String("name")).toString());
    protocolId = DescFileObject.value(QLatin1String("id")).toString();

    // Attributes
    for (const QJsonValueRef CmdRef : DescFileObject.value(QLatin1String("commands")).toArray()) {
        QJsonObject CmdObject(CmdRef.toObject());
        // Force command name correction
        QString cmdName = correctInputString(CmdObject.value(QLatin1String("name")).toString());
        // Command creation
        Command *Cmd(new Command(cmdName,
                    static_cast<short>(CmdObject.value(QLatin1String("id")).toInt()),
                    CmdObject.value(QLatin1String("hasAtt")).toBool(),
                    NS_DirectionType::SLDirectionType2Enum[CmdObject.value(QLatin1String("direction")).toInt()],
                    CmdObject.value(QLatin1String("description")).toString()));
        // Parse sub-attributes
        if (CmdObject.value(QLatin1String("hasAtt")).toBool()) {
            for (const QJsonValueRef AttrRef : CmdObject.value(QLatin1String("attributes")).toArray()) {
                const QJsonObject AttrObject(AttrRef.toObject());
                loadAtt_Rec(Cmd, nullptr, AttrObject);
            }
        }
        // Add into graphical component
        cmdArray.append(Cmd);
    }
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
            QCoreApplication::translate("main", "Load a protocol description file (REQUIRED)"),
            QCoreApplication::translate("main", "path/to/file"));
    parser.addOption(loadDescOption);

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
    out << "LCSF_Generator_CLI v" << APP_VERSION << endl;

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
    load_desc(descFile);
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
    out << "Generation complete. Code A generated in: " << outAPath << ", code B generated in: " << outBPath << endl;
    exit(EXIT_FAILURE);
    return a.exec();
}
