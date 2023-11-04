/**
 * \file DocGenerator.cpp
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

#include <QStringBuilder>

#include "DocGenerator.h"

DocGenerator::DocGenerator() {

}

QList<Attribute::T_attInfos> DocGenerator::getAttInfos_Rec(QString parentName, QList<Attribute *> attList) {
   QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
   Attribute::T_attInfos localAttInfos;
   for (Attribute *attribute : attList) {
      localAttInfos.parentName = parentName;
      localAttInfos.attName = attribute->getName();
      localAttInfos.attId = attribute->getId();
      localAttInfos.attPointer = attribute;
      localAttInfos.dataType = attribute->getDataType();
      localAttInfos.isOptional = attribute->getIsOptional();
      localAttInfos.subAttNb = attribute->getSubAttArray().size();
      localAttInfos.attDesc = attribute->getDesc();
      resultList.append(localAttInfos);
      if ((attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) && (localAttInfos.subAttNb > 0)) {
         resultList.append(this->getAttInfos_Rec(localAttInfos.attName, attribute->getSubAttArray()));
      }
   }
   return resultList;
}

QList<Attribute::T_attInfos> DocGenerator::getAttInfos(QList<Command *> cmdList) {
   QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
   for (Command *command : cmdList) {
      QString cmdName = command->getName();
      if ((command->getHasAtt()) && (command->getAttArray().size() > 0)) {
         resultList.append(this->getAttInfos_Rec(cmdName, command->getAttArray()));
      }
   }
   return resultList;
}

QList<Attribute::T_attInfos> DocGenerator::insertSortAttInfosListByParentName(QList<Attribute::T_attInfos> attInfosList) {
   QList<Attribute::T_attInfos> sortedList = attInfosList;
   Attribute::T_attInfos bufferAttInfos;
   int j = 0;
   int listSize = sortedList.size();
   for (int i = 0; i < listSize; i++) {
      j = i;
      while ((j > 0) && (sortedList.at(j-1).parentName.compare(sortedList.at(j).parentName) > 0)) {
         bufferAttInfos = sortedList.at(j);
         sortedList.replace(j, sortedList.at(j-1));
         sortedList.replace(j-1, bufferAttInfos);
         j = j - 1;
      }
   }
   return sortedList;
}

QList<Attribute::T_attInfos> DocGenerator::removeCommandAttributes(QList<Attribute::T_attInfos> attInfosList, QList<Command *> cmdList) {
   QList<Attribute::T_attInfos> trimmedAttInfosList = QList<Attribute::T_attInfos>();

   for (Attribute::T_attInfos attInfo : attInfosList) {
       // If attribute doesn't have command as parent
      if (Command::findCmdAddr(attInfo.parentName, cmdList) == nullptr) {
         trimmedAttInfosList.append(attInfo);
      }
   }
   return trimmedAttInfosList;
}

void DocGenerator::generateWikiTable(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_" + protocolName + "_WikiTables.txt";
   QFile saveFile(fileName);
   QFileInfo fileInfo(saveFile);

   if (saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&saveFile);

      // Main table
      out << "=== " << protocolName << " protocol tables ===" << Qt::endl;
      out << Qt::endl;
      out << protocolName << " protocol id: " << protocolId << Qt::endl;
      out << Qt::endl;
      out << "==== Commands table ====" << Qt::endl;
      out << Qt::endl;
      out << "{| class=\"wikitable sortable\"" << Qt::endl;
      out << "|-" << Qt::endl;
      out << "! Name !! Id !! Direction !! Description !! Attribute(s) Name !! Attribute(s) Id !! Optional? !! Data type !! Attribute Desc" << Qt::endl;
      out << "|-" << Qt::endl;

      for (Command *command : cmdList) {
         if ((command->getHasAtt()) && (command->getAttArray().size() > 0)) {
            bool isFirtAttribute = true;
            int attNb = command->getAttArray().size();
            out << "| rowspan=\"" << attNb << "\" | " << command->getName() << " || rowspan=\"" << attNb << "\" | '''"
                      << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || rowspan=\"" << attNb << "\" | "
                      <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << " || rowspan=\"" << attNb << "\" | " << command->getDesc() << " || ";
            for (Attribute *attribute : command->getAttArray()) {
               if (isFirtAttribute) {
                  isFirtAttribute = false;
                  out << attribute->getName() << " || '''" <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || "
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << "''' || " << attribute->getDesc() << Qt::endl;
                  out << "|-" << Qt::endl;
               } else {
                  out << "| " << attribute->getName() << " || '''" <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || "
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << "''' || " << attribute->getDesc() << Qt::endl;
                  out << "|-" << Qt::endl;
               }
            }
         } else {
            out << "| " << command->getName() << " || '''" << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || "
                      <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << " || " << command->getDesc() << " || || || || || " << Qt::endl;
            out << "|-" << Qt::endl;
         }
      }
      out << "|}" << Qt::endl;
      out << Qt::endl;

      // Secondary tables
      QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
      QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
      QList<Attribute::T_attInfos> trimmedAttInfosList = this->removeCommandAttributes(sortedAttInfosList, cmdList);

      for (int idx = 0; idx < trimmedAttInfosList.size(); idx++) {
         Attribute::T_attInfos currentAttInfo = trimmedAttInfosList.at(idx);
         QString previousParentName = currentAttInfo.parentName;

         out << "==== " << previousParentName << " sub-attributes table ====" << Qt::endl;
         out << Qt::endl;
         out << "{| class=\"wikitable sortable\"" << Qt::endl;
         out << "|-" << Qt::endl;
         out << "! Name !! Id !! Optional? !! Data type !! Description" << Qt::endl;
         out << "|-" << Qt::endl;
         out << "| " << currentAttInfo.attName << " || '''" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "''' || "
             << ((currentAttInfo.isOptional) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "''' || " << currentAttInfo.attDesc << Qt::endl;
         out << "|-" << Qt::endl;

         if (idx < trimmedAttInfosList.size() - 1) {
            currentAttInfo = trimmedAttInfosList.at(idx+1);

            while (currentAttInfo.parentName.compare(previousParentName) == 0) {
               out << "| " << currentAttInfo.attName << " || '''" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "''' || "
                   << ((currentAttInfo.isOptional) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "''' || " << currentAttInfo.attDesc << Qt::endl;
               out << "|-" << Qt::endl;
               previousParentName = currentAttInfo.parentName;
               idx++;

               if (idx < trimmedAttInfosList.size() - 1) {
                  currentAttInfo = trimmedAttInfosList.at(idx+1);
               } else {
                  break;
               }
            }
         }
         out << "|}" << Qt::endl;
         out << Qt::endl;
      }
      saveFile.close();
   }
}

void DocGenerator::generateDokuWikiTable(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_" + protocolName + "_DokuWikiTables.txt";
   QFile saveFile(fileName);
   QFileInfo fileInfo(saveFile);

   if (saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&saveFile);

      // Main table
      out << "=== " << protocolName << " protocol tables ===" << Qt::endl;
      out << Qt::endl;
      out << protocolName << " protocol id: " << protocolId << Qt::endl;
      out << Qt::endl;
      out << "== Commands table ==" << Qt::endl;
      out << Qt::endl;
      out << "^ Name ^ Id ^ Direction ^ Description ^ Attribute(s) Name ^ Attribute(s) Id ^ Optional? ^ Data type ^ Attribute Desc ^" << Qt::endl;

      for (Command *command : cmdList) {
         if ((command->getHasAtt()) && (command->getAttArray().size() > 0)) {
            bool isFirtAttribute = true;
            int attNb = command->getAttArray().size();
            out << "| " << command->getName() << " | "
                      << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x") << " | "
                      <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << " | " << command->getDesc() << " | ";
            for (Attribute *attribute : command->getAttArray()) {
               if (isFirtAttribute) {
                  isFirtAttribute = false;
                  out << attribute->getName() << " | " <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x") << " | "
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " | " << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << " | " << attribute->getDesc() << " |" << Qt::endl;
               } else {
                  out << "| ::: | ::: | ::: | ::: " << "| " << attribute->getName() << " | " <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x") << " | "
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " | " << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << " | " << attribute->getDesc() << " |" << Qt::endl;
               }
            }
         } else {
            out << "| " << command->getName() << " | " << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x") << " | "
                      <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << " | " << command->getDesc() << " | | | | | " << Qt::endl;
         }
      }
      out << Qt::endl;

      // Secondary tables
      QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
      QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
      QList<Attribute::T_attInfos> trimmedAttInfosList = this->removeCommandAttributes(sortedAttInfosList, cmdList);

      for (int idx = 0; idx < trimmedAttInfosList.size(); idx++) {
         Attribute::T_attInfos currentAttInfo = trimmedAttInfosList.at(idx);
         QString previousParentName = currentAttInfo.parentName;

         out << "== " << previousParentName << " sub-attributes table ==" << Qt::endl;
         out << Qt::endl;
         out << "^ Name ^ Id ^ Optional? ^ Data type ^ Description ^" << Qt::endl;
         out << "| " << currentAttInfo.attName << " | " <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << " | "
             << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | " << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << " | " << currentAttInfo.attDesc << " |" << Qt::endl;

         if (idx < trimmedAttInfosList.size() - 1) {
            currentAttInfo = trimmedAttInfosList.at(idx+1);

            while (currentAttInfo.parentName.compare(previousParentName) == 0) {
               out << "| " << currentAttInfo.attName << " | " <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << " | "
                   << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | " << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << " | " << currentAttInfo.attDesc << " |" << Qt::endl;
               previousParentName = currentAttInfo.parentName;
               idx++;

               if (idx < trimmedAttInfosList.size() - 1) {
                  currentAttInfo = trimmedAttInfosList.at(idx+1);
               } else {
                  break;
               }
            }
         }
         out << Qt::endl;
      }
      saveFile.close();
   }
}

void DocGenerator::generateMkdownTable(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString fileName = dirPath + "/LCSF_" + protocolName + "_MkdownTables.md";
    QFile saveFile(fileName);
    QFileInfo fileInfo(saveFile);

    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&saveFile);

        // Main table
        out << "# " << protocolName << " protocol tables" << Qt::endl;
        out << Qt::endl;
        out << protocolName << " protocol id: " << protocolId << Qt::endl;
        out << Qt::endl;
        out << "## Commands table" << Qt::endl;
        out << Qt::endl;
        out << "| Name | Id | Direction | Description | Attributes? |" << Qt::endl;
        out << "|:----:|:--:|:---------:|:-----------:|:------------:|" << Qt::endl;
        for (Command *command : cmdList) {
            out << "| " << command->getName() << " | `" << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x")
                << "` | `" <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << "` | " << command->getDesc() << " | "
                << ((command->getHasAtt() && (command->getAttArray().size() > 0)) ? "Yes" : "No") << " |" << Qt::endl;
        }
        out << Qt::endl;
        // Command tables
        for (Command *command : cmdList) {
            if (!command->getHasAtt() || (command->getAttArray().size() == 0)) {
                continue;
            }
            out << "## " << command->getName() << " attributes table" << Qt::endl;
            out << Qt::endl;
            out << "| Name | Id | Optional? | Data type | Attribute Description |" << Qt::endl;
            out << "|:----:|:--:|:----------:|:---------:|:---------------------:|" << Qt::endl;
            for (Attribute *attribute : command->getAttArray()) {
                out << "| " << attribute->getName() << " | `" <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x")
                    << "` | " << ((attribute->getIsOptional()) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()]
                    << "` | " << attribute->getDesc() << " |" << Qt::endl;
            }
            out << Qt::endl;
        }
        // Attribute tables
        QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
        QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
        QList<Attribute::T_attInfos> trimmedAttInfosList = this->removeCommandAttributes(sortedAttInfosList, cmdList);

        for (int idx = 0; idx < trimmedAttInfosList.size(); idx++) {
            Attribute::T_attInfos currentAttInfo = trimmedAttInfosList.at(idx);
            QString previousParentName = currentAttInfo.parentName;

            out << "## " << previousParentName << " sub-attributes table" << Qt::endl;
            out << Qt::endl;
            out << "| Name | Id | Optional? | Data type | Description |" << Qt::endl;
            out << "|:----:|:--:|:----------:|:---------:|:-----------:|" << Qt::endl;
            out << "| " << currentAttInfo.attName << " | `" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "` | "
            << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "` | " << currentAttInfo.attDesc << Qt::endl;

            if (idx < trimmedAttInfosList.size() - 1) {
                currentAttInfo = trimmedAttInfosList.at(idx+1);

                while (currentAttInfo.parentName.compare(previousParentName) == 0) {
                    out << "| " << currentAttInfo.attName << " | `" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "` | "
                    << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "` | " << currentAttInfo.attDesc << Qt::endl;
                    previousParentName = currentAttInfo.parentName;
                    idx++;

                    if (idx < trimmedAttInfosList.size() - 1) {
                        currentAttInfo = trimmedAttInfosList.at(idx+1);
                    } else {
                        break;
                    }
                }
            }
            out << Qt::endl;
        }
        saveFile.close();
    }
}
