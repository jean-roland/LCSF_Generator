/**
 * \file deschandler.cpp
 * \brief Description file handler module.
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

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QRandomGenerator>

#include "attribute.h"
#include "deschandler.h"
#include "enumtype.h"

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

static NS_AttDataType::T_AttDataType convertDataType(QString type) {
    if (type == "LCSF_UINT8") {
        return NS_AttDataType::UINT8;
    } else if (type == "LCSF_UINT16") {
        return NS_AttDataType::UINT16;
    } else if (type == "LCSF_UINT32") {
        return NS_AttDataType::UINT32;
    } else if (type == "LCSF_UINT64") {
        return NS_AttDataType::UINT64;
    } else if (type == "LCSF_FLOAT32") {
        return NS_AttDataType::FLOAT32;
    } else if (type == "LCSF_FLOAT64") {
        return NS_AttDataType::FLOAT64;
    } else if (type == "LCSF_BYTE_ARRAY") {
        return NS_AttDataType::BYTE_ARRAY;
    } else if (type == "LCSF_SUB_ATTRIBUTES") {
        return NS_AttDataType::SUB_ATTRIBUTES;
    } else if (type == "LCSF_STRING") {
        return NS_AttDataType::STRING;
    } else {
        return NS_AttDataType::UNKNOWN;
    }
}

static void loadAtt_Rec(Command *pParentCmd, Attribute *pParentAtt, const QJsonObject &attribute) {
    Attribute *Attr(new Attribute(attribute.value(QLatin1String("name")).toString(),
        static_cast<short>(attribute.value(QLatin1String("id")).toInt()),
        attribute.value(QLatin1String("isOptional")).toBool(),
        convertDataType(attribute.value(QLatin1String("dataType")).toString()),
        attribute.value(QLatin1String("desc")).toString()));
    // If this is a sub-attribute
    if (pParentAtt != nullptr) {
        pParentAtt->addSubAtt(Attr);
    }
    // Parse sub-attributes
    if (Attr->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
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

static void saveAtt_Rec(QJsonArray &attributes, QList<Attribute *> attList) {
    // Parse attributes
    for (Attribute *attribute : attList) {
        QJsonObject Attr;

        // Attribute parameters
        Attr.insert(QLatin1String("name"), attribute->getName());
        Attr.insert(QLatin1String("id"), attribute->getId());
        Attr.insert(QLatin1String("isOptional"), attribute->getIsOptional());
        Attr.insert(QLatin1String("dataType"), NS_AttDataType::SL_AttDataType.at(attribute->getDataType()));
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

void DescHandler::load_desc(
    QFile &file, QList<Command *> &cmdArray, QString &protocolName, QString &protocolId, QString &protocolDesc) {
    QJsonDocument DescFile(QJsonDocument::fromJson(file.readAll()));
    QJsonObject DescFileObject(DescFile.object());

    protocolName = correctInputString(DescFileObject.value(QLatin1String("name")).toString());
    protocolId = DescFileObject.value(QLatin1String("id")).toString();
    protocolDesc = DescFileObject.value(QLatin1String("desc")).toString();

    // Attributes
    for (const QJsonValueRef CmdRef : DescFileObject.value(QLatin1String("commands")).toArray()) {
        QJsonObject CmdObject(CmdRef.toObject());
        // Force command name correction
        QString cmdName = correctInputString(CmdObject.value(QLatin1String("name")).toString());
        int direction = CmdObject.value(QLatin1String("direction")).toInt();
        if (direction >= NS_DirectionType::SL_DirectionType.size()) {
            direction = 0;
        }
        // Command creation
        Command *Cmd(new Command(cmdName, static_cast<short>(CmdObject.value(QLatin1String("id")).toInt()),
            CmdObject.value(QLatin1String("hasAtt")).toBool(), NS_DirectionType::SLDirectionType2Enum[direction],
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

bool DescHandler::save_desc(
    QString filename, QList<Command *> cmdArray, QString protocolName, QString protocolId, QString protocolDesc) {
    // Generate JSON
    QJsonObject DescFile;
    DescFile.insert(QLatin1String("name"), protocolName);
    DescFile.insert(QLatin1String("id"), protocolId);
    DescFile.insert(QLatin1String("desc"), protocolDesc);

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