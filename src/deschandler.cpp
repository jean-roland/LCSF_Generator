#include <QRandomGenerator>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>

#include "deschandler.h"
#include "attribute.h"

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

static void saveAtt_Rec(QJsonArray& attributes, QList<Attribute *> attList) {
   // Parse attributes
   for (Attribute *attribute : attList) {
      QJsonObject Attr;

      // Attribute parameters
      Attr.insert(QLatin1String("name"), attribute->getName());
      Attr.insert(QLatin1String("id"), attribute->getId());
      Attr.insert(QLatin1String("isOptional"), attribute->getIsOptional());
      Attr.insert(QLatin1String("dataType"), attribute->getDataType());
      Attr.insert(QLatin1String("desc"), attribute->getDesc());
      Attr.insert(QLatin1String("size"), attribute->getSubAttArray().size());

      // Parse sub-attributes
      if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
         QJsonArray SubAttr;
         saveAtt_Rec(SubAttr, attribute->getSubAttArray());
         Attr.insert(QLatin1String("subAttr"), SubAttr);
      }
      // Add the attribute
      attributes.append(Attr);
   }
}

DescHandler::DescHandler() {

}

void DescHandler::load_desc(QFile& file, QList<Command *>& cmdArray, QString& protocolName, QString& protocolId) {
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
        // Add into command array
        cmdArray.append(Cmd);
    }
}

bool DescHandler::save_desc(QString filename, QList<Command *> cmdArray, QString protocolName, QString protocolId) {
    // Generate JSON
    QJsonObject DescFile;
    DescFile.insert(QLatin1String("name"), protocolName);
    DescFile.insert(QLatin1String("id"), protocolId);

    // Save commands
    QJsonArray DescCmd;
    for (Command *Cmd : cmdArray) {
       // Commands parameters
       QJsonObject jsonCmd;
       jsonCmd.insert(QLatin1String("parentName"), "NONE");
       jsonCmd.insert(QLatin1String("name"), Cmd->getName());
       jsonCmd.insert(QLatin1String("id"), Cmd->getId());
       jsonCmd.insert(QLatin1String("hasAtt"), Cmd->getHasAtt());
       jsonCmd.insert(QLatin1String("direction"), Cmd->getDirection());
       jsonCmd.insert(QLatin1String("description"), Cmd->getDesc());
       jsonCmd.insert(QLatin1String("size"), Cmd->getAttArray().size());
       // Command attributes
       QJsonArray Attr;
       saveAtt_Rec(Attr, Cmd->getAttArray());
       jsonCmd.insert(QLatin1String("attributes"), Attr);
       // Add command in the array
       DescCmd.append(jsonCmd);
    }
    // Write commands in file
    DescFile.insert(QLatin1String("commands"), DescCmd);
    QFile JsonDescFile(filename + ".json");

    if (JsonDescFile.open(QIODevice::WriteOnly)) {
       JsonDescFile.write(QJsonDocument(DescFile).toJson());
       JsonDescFile.close();
       return true;
    } else {
       return false;
    }
}