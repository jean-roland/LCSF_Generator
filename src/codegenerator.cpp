/**
 * \file codegenerator.cpp
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

#include "codegenerator.h"

CodeGenerator::CodeGenerator()
{
}

QString CodeGenerator::getAttDataRxPath(QString protocolName, QStringList parentNames, QString attName)
{
   QString namePrefix = protocolName.toUpper() + "_";
   QString attDataPath;

   if (parentNames.size() > 1)
   {
      attDataPath = "pAttArray[";
      for (int idx = 0; idx < parentNames.size() - 1; idx++)
      {
         attDataPath.append(namePrefix + parentNames.at(idx).toUpper() + "_ATT_" + parentNames.at(idx + 1).toUpper() + "].Payload.pSubAttArray[");
      }
      attDataPath.append(namePrefix + parentNames.last().toUpper() + "_ATT_" + attName.toUpper() + "].Payload");
   }
   return attDataPath;
}

QString CodeGenerator::getAttDataTxPath(QString protocolName, QStringList parentNames, QString attName)
{
   QString namePrefix = protocolName.toUpper() + "_";
   QString attDataPath;

   if (parentNames.size() > 1)
   {
      attDataPath = "pAttArray[";
      for (int idx = 0; idx < parentNames.size() - 1; idx++)
      {
         attDataPath.append(namePrefix + parentNames.at(idx).toUpper() + "_ATT_" + parentNames.at(idx + 1).toUpper() + "].Payload.pSubAttArray[");
      }
      attDataPath.append(namePrefix + parentNames.last().toUpper() + "_ATT_" + attName.toUpper() + "].");
   }
   return attDataPath;
}

QString CodeGenerator::getPayloadPath(QStringList parentNames)
{
   QString payloadPath = "";
   for (int idx = 0; idx < parentNames.size(); idx++)
   {
      payloadPath.append(parentNames.at(idx).toLower() + "_payload.");
   }
   return payloadPath;
}

QString CodeGenerator::getIndent(int indentNb)
{
   QString indentString = "";
   if (indentNb > 0)
   {
      for (int idx = 0; idx < indentNb; idx++)
      {
         indentString.append("    ");
      }
   }
   return indentString;
}

QString CodeGenerator::getFlagTypeStringFromAttNb(int attNb, QString varName)
{
   QString typeString = QString();

   if (attNb > 0)
   {
      if (attNb <= 8)
      {
         typeString = "uint8_t " + varName;
      }
      else if (attNb <= 16)
      {
         typeString = "uint16_t " + varName;
      }
      else if (attNb <= 32)
      {
         typeString = "uint32_t " + varName;
      }
      else
      {
         int ByteNb = attNb / 8;
         if (attNb % 8 != 0)
            ByteNb++;
         typeString = "uint8_t ";
         typeString.append(varName + "[");
         typeString.append(QString::number(ByteNb));
         typeString.append("] ");
      }
   }
   return typeString;
}

QString CodeGenerator::getTypeStringFromDataType(NS_AttDataType::T_AttDataType dataType)
{
   QString typeString;
   switch (dataType)
   {
   case NS_AttDataType::UINT8:
      typeString = "uint8_t ";
      break;

   case NS_AttDataType::UINT16:
      typeString = "uint16_t ";
      break;

   case NS_AttDataType::UINT32:
      typeString = "uint32_t ";
      break;

   case NS_AttDataType::UINT64:
      typeString = "uint64_t ";
      break;

   case NS_AttDataType::FLOAT32:
      typeString = "float ";
      break;

   case NS_AttDataType::FLOAT64:
      typeString = "double ";
      break;

   case NS_AttDataType::BYTE_ARRAY:
      typeString = "uint8_t *";
      break;

   case NS_AttDataType::STRING:
      typeString = "char *";
      break;

   default:
      typeString = "void *";
      break;
   }
   return typeString;
}

QString CodeGenerator::getInitStringFromDataType(NS_AttDataType::T_AttDataType dataType)
{
   QString initString;
   switch (dataType)
   {
   case NS_AttDataType::UINT8:
   case NS_AttDataType::UINT16:
   case NS_AttDataType::UINT32:
   case NS_AttDataType::UINT64:
   case NS_AttDataType::FLOAT32:
   case NS_AttDataType::FLOAT64:
   default:
      initString = " = 0";
      break;

   case NS_AttDataType::BYTE_ARRAY:
   case NS_AttDataType::STRING:
      initString = " = NULL";
      break;
   }
   return initString;
}

QString CodeGenerator::getAttDescString(QString protocolName, QString parentName, Attribute *attribute)
{
   QString descString = "";
   if (attribute != nullptr)
   {
      // Optionality
      if (attribute->getIsOptional())
      {
         descString.append("true, ");
      }
      else
      {
         descString.append("false, ");
      }
      // Type
      descString.append(NS_AttDataType::SL_AttDataType.at(attribute->getDataType()) + ", ");
      // Id
      descString.append("LCSF_" + protocolName.toUpper() + "_" + parentName.toUpper() + "_ATT_ID_" + attribute->getName().toUpper() + ", ");
      // Sub-attributes descriptor
      if (attribute->getSubAttArray().size() > 0)
      {
         descString.append("LCSF_" + protocolName.toUpper() + "_ATT_" + attribute->getName().toUpper() + "_SUBATT_NB, ");
         descString.append("LCSF_" + protocolName + "_" + attribute->getName() + "_AttDescArray");
      }
      else
      {
         descString.append("0, NULL");
      }
   }
   return descString;
}

QList<Attribute::T_attInfos> CodeGenerator::getAttInfos_Rec(QString parentName, QList<Attribute *> attList)
{
   QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
   Attribute::T_attInfos localAttInfos;
   for (Attribute *attribute : attList)
   {
      localAttInfos.parentName = parentName;
      localAttInfos.attName = attribute->getName();
      localAttInfos.attId = attribute->getId();
      localAttInfos.attPointer = attribute;
      localAttInfos.dataType = attribute->getDataType();
      localAttInfos.isOptional = attribute->getIsOptional();
      localAttInfos.subAttNb = attribute->getSubAttArray().size();
      localAttInfos.attDesc = attribute->getDesc();
      resultList.append(localAttInfos);
      if ((attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) && (localAttInfos.subAttNb > 0))
      {
         if (!this->protocolHasSubAtt)
         {
            this->protocolHasSubAtt = true;
         }
         resultList.append(this->getAttInfos_Rec(localAttInfos.attName, attribute->getSubAttArray()));
      }
   }
   return resultList;
}

QList<Attribute::T_attInfos> CodeGenerator::getAttInfos(QList<Command *> cmdList)
{
   QList<Attribute::T_attInfos> resultList = QList<Attribute::T_attInfos>();
   this->protocolHasSubAtt = false;
   for (Command *command : cmdList)
   {
      QString cmdName = command->getName();
      if ((command->getHasAtt()) && (command->getAttArray().size() > 0))
      {
         resultList.append(this->getAttInfos_Rec(cmdName, command->getAttArray()));
      }
   }
   return resultList;
}

QList<Attribute::T_attInfos> CodeGenerator::insertSortAttInfosListByParentName(QList<Attribute::T_attInfos> attInfosList)
{
   QList<Attribute::T_attInfos> sortedList = attInfosList;
   Attribute::T_attInfos bufferAttInfos;
   int j = 0;
   int listSize = sortedList.size();
   for (int i = 0; i < listSize; i++)
   {
      j = i;
      while ((j > 0) && (sortedList.at(j - 1).parentName.compare(sortedList.at(j).parentName) > 0))
      {
         bufferAttInfos = sortedList.at(j);
         sortedList.replace(j, sortedList.at(j - 1));
         sortedList.replace(j - 1, bufferAttInfos);
         j = j - 1;
      }
   }
   return sortedList;
}

bool CodeGenerator::findAttInfoId(short attId, QList<Attribute::T_attInfos> attInfosList)
{
   bool isThere = false;

   for (Attribute::T_attInfos attInfo : attInfosList)
   {
      if (attInfo.attId == attId)
      {
         isThere = true;
         break;
      }
   }
   return isThere;
}

QList<Attribute::T_attInfos> CodeGenerator::removeAttInfosDuplicate(QList<Attribute::T_attInfos> attInfosList)
{
   QList<Attribute::T_attInfos> noDuplicateAttInfosList = QList<Attribute::T_attInfos>();

   for (Attribute::T_attInfos attInfo : attInfosList)
   {
      if (!this->findAttInfoId(attInfo.attId, noDuplicateAttInfosList))
      {
         noDuplicateAttInfosList.append(attInfo);
      }
   }
   return noDuplicateAttInfosList;
}

void CodeGenerator::fillSubAttData_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb)
{
   if ((pOut == nullptr) || (attList.size() == 0))
   {
      return;
   }
   QStringList nextParentNames = QStringList();
   QString indent = this->getIndent(indentNb);

   for (Attribute *attribute : attList)
   {
      if (attribute->getIsOptional())
      {
         *pOut << indent << "// Initialize optional attribute flags bitfield" << Qt::endl;
         *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield = 0;" << Qt::endl;
         break;
      }
   }
   for (Attribute *attribute : attList)
   {
      // Clear sub-attribute list between attributes
      nextParentNames = parentNames;
      QString attDataPath = this->getAttDataRxPath(protocolName, parentNames, attribute->getName()) + ".pData";
      QString attSizePath = this->getAttDataRxPath(protocolName, parentNames, attribute->getName()) + "Size";
      *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName() << Qt::endl;
      if (attribute->getIsOptional())
      {
         *pOut << indent << "if (" << attDataPath << " != NULL) {" << Qt::endl;
         indent = this->getIndent(indentNb + 1);
         *pOut << indent << "// Note presence of optional attribute " << attribute->getName() << Qt::endl;
         *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield |= "
               << protocolName.toUpper() << "_" << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG;" << Qt::endl;
         if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
         {
            nextParentNames.append(attribute->getName());
            this->fillSubAttData_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
         }
         else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
         {
            *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName() << Qt::endl;
            *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower()
                  << " = " << attDataPath << ";" << Qt::endl;
         }
         else
         {
            *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName() << Qt::endl;
            *pOut << indent << "memcpy(&(pCmdPayload->" << this->getPayloadPath(parentNames) << attribute->getName().toLower()
                  << "), " << attDataPath << ", " << attSizePath << ");" << Qt::endl;
         }
         indent = this->getIndent(indentNb);
         *pOut << indent << "}" << Qt::endl;
      }
      else
      {
         if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
         {
            nextParentNames.append(attribute->getName());
            this->fillSubAttData_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
         }
         else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
         {
            *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower()
                  << " = " << attDataPath << ";" << Qt::endl;
         }
         else
         {
            *pOut << indent << "memcpy(&(pCmdPayload->" << this->getPayloadPath(parentNames) << attribute->getName().toLower()
                  << "), " << attDataPath << ", " << attSizePath << ");" << Qt::endl;
         }
      }
   }
}

void CodeGenerator::fillSubAttPayload_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb)
{
   if ((pOut == nullptr) || (attList.size() == 0))
   {
      return;
   }
   QString attDataPath;
   QStringList nextParentNames = QStringList();
   QString indent = this->getIndent(indentNb);

   for (Attribute *attribute : attList)
   {
      // Clear sub-attribute list between attributes
      nextParentNames = parentNames;
      attDataPath = this->getAttDataTxPath(protocolName, parentNames, attribute->getName());
      *pOut << indent << "// Fill data of sub-attribute " << attribute->getName() << Qt::endl;

      if (attribute->getIsOptional())
      {
         *pOut << indent << "if ((pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield & " << protocolName.toUpper() << "_"
               << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << Qt::endl;
         indent = this->getIndent(indentNb + 1);

         switch (attribute->getDataType())
         {
         case NS_AttDataType::SUB_ATTRIBUTES:
         {
            *pOut << indent << "// Intermediary variable" << Qt::endl;
            *pOut << indent << "pSubAttArray = &(" << attDataPath << "Payload.pSubAttArray);" << Qt::endl;
            *pOut << indent << "// Allocate sub-attribute array" << Qt::endl;
            *pOut << indent << "if (!FiloGet(&LcsfBridge" << protocolName << "Info.Filo, LCSF_" << protocolName.toUpper() << "_ATT_"
                  << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << Qt::endl;
            *pOut << indent << "    return false;" << Qt::endl;
            *pOut << indent << "}" << Qt::endl;
            nextParentNames.append(attribute->getName());
            this->fillSubAttPayload_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
         }
         break;
         case NS_AttDataType::BYTE_ARRAY:
            *pOut << indent << attDataPath << "PayloadSize = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << "Size;" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
            break;
         case NS_AttDataType::STRING:
            *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
            break;
         case NS_AttDataType::FLOAT32:
            *pOut << indent << attDataPath << "PayloadSize = sizeof(float);" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         case NS_AttDataType::FLOAT64:
            *pOut << indent << attDataPath << "PayloadSize = sizeof(double);" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         default:
            *pOut << indent << attDataPath << "PayloadSize = GetVLESize(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         }
         indent = this->getIndent(indentNb);
         *pOut << indent << "} else {" << Qt::endl;
         *pOut << indent << "    " << attDataPath << "Payload.pData = NULL;" << Qt::endl;
         *pOut << indent << "}" << Qt::endl;
      }
      else
      {
         switch (attribute->getDataType())
         {
         case NS_AttDataType::SUB_ATTRIBUTES:
         {
            *pOut << indent << "// Intermediary variable" << Qt::endl;
            *pOut << indent << "pSubAttArray = &(" << attDataPath << "Payload.pSubAttArray);" << Qt::endl;
            *pOut << indent << "// Allocate sub-attribute array" << Qt::endl;
            *pOut << indent << "if (!FiloGet(&LcsfBridge" << protocolName << "Info.Filo, LCSF_" << protocolName.toUpper() << "_ATT_"
                  << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << Qt::endl;
            *pOut << indent << "    return false;" << Qt::endl;
            *pOut << indent << "}" << Qt::endl;
            nextParentNames.append(attribute->getName());
            this->fillSubAttPayload_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
         }
         break;
         case NS_AttDataType::BYTE_ARRAY:
            *pOut << indent << attDataPath << "PayloadSize = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << "Size;" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
            break;
         case NS_AttDataType::STRING:
            *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
            break;
         case NS_AttDataType::FLOAT32:
            *pOut << indent << attDataPath << "PayloadSize = sizeof(float);" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         case NS_AttDataType::FLOAT64:
            *pOut << indent << attDataPath << "PayloadSize = sizeof(double);" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         default:
            *pOut << indent << attDataPath << "PayloadSize = GetVLESize(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << Qt::endl;
            break;
         }
      }
   }
}
void CodeGenerator::declareAtt_REC(QString parentName, QList<Attribute *> attList, QTextStream *pOut)
{
   QString indent = "    ";

   for (Attribute *attribute : attList)
   {
      if (attribute->getIsOptional())
      {
         *pOut << indent << "bool m_" << parentName.toLower() << "_" << attribute->getName().toLower() << "_isHere = false;" << Qt::endl;
      }
      if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
      {
         this->declareAtt_REC(attribute->getName(), attribute->getSubAttArray(), pOut);
      }
      else
      {
         QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
         QString initString = this->getInitStringFromDataType(attribute->getDataType());
         *pOut << indent << typeString << "m_" << parentName.toLower() << "_" << attribute->getName().toLower() << initString << ";" << Qt::endl;
      }
   }
}

void CodeGenerator::grabAttValues_REC(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb)
{
   QString attPayloadPath;
   QStringList nextParentNames = QStringList();
   QString indent = this->getIndent(indentNb);

   for (Attribute *attribute : attList)
   {
      // Clear sub-attribute list between attributes
      nextParentNames = parentNames;
      attPayloadPath = this->getPayloadPath(parentNames);

      if (attribute->getIsOptional())
      {
         *pOut << indent << "if ((pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield & " << protocolName.toUpper() << "_"
               << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << Qt::endl;
         indent = this->getIndent(indentNb + 1);

         if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
         {
            nextParentNames.append(attribute->getName());
            *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << "_isHere = true;" << Qt::endl;
            this->grabAttValues_REC(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
         }
         else
         {
            if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
            {
               *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                     << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
            }
            else
            {
               *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                     << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << Qt::endl;
            }
            *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << "_isHere = true;" << Qt::endl;
         }
         indent = this->getIndent(indentNb);
         *pOut << indent << "}" << Qt::endl;
      }
      else
      {
         if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
         {
            nextParentNames.append(attribute->getName());
            this->grabAttValues_REC(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb);
         }
         else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
         {
            *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
         }
         else
         {
            *pOut << indent << "m_" << parentNames.last().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                  << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << Qt::endl;
         }
      }
   }
}

QString CodeGenerator::toCamelCase(const QString &s)
{
   QStringList parts = s.split('_', Qt::SkipEmptyParts);
   for (int i = 1; i < parts.size(); ++i)
   {
      parts[i].replace(0, 1, parts[i][0].toUpper());
   }
   return parts.join("");
}

QString CodeGenerator::toFirstLetterUpperCase(const QString &s)
{
   QStringList parts = s.split('_', Qt::SkipEmptyParts);
   for (int i = 0; i < parts.size(); ++i)
   {
      parts[i].replace(0, 1, parts[i][0].toUpper());
   }
   return parts.join("");
}

QList<Attribute::T_attInfos> CodeGenerator::removeCommandAttributes(QList<Attribute::T_attInfos> attInfosList, QList<Command *> cmdList)
{
   QList<Attribute::T_attInfos> trimmedAttInfosList = QList<Attribute::T_attInfos>();

   for (Attribute::T_attInfos attInfo : attInfosList)
   {
      // If attribute doesn't have command as parent
      if (Command::findCmdAddr(attInfo.parentName, cmdList) == nullptr)
      {
         trimmedAttInfosList.append(attInfo);
      }
   }
   return trimmedAttInfosList;
}

int findCmdFctIdxFromName(QString cmdName, QStringList cmdFcts)
{
   int idx = 0;
   for (QString function : cmdFcts)
   {
      if (function.contains(cmdName))
      {
         return idx;
      }
      idx++;
   }
   return -1;
}

void CodeGenerator::generateMainHeader(QString protocolName, QList<Command *> cmdList, CodeExtractor codeExtract, QString dirPath)
{
   QDir dir(dirPath);
   if (!dir.exists())
   {
      dir.mkpath(".");
   }
   QString fileName = dirPath + "/" + protocolName + "_Main.h";
   QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
   bool hasOptAtt = false;
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
      QFileInfo fileInfo(file);
      out << "/**" << Qt::endl;
      out << " * \\file " << protocolName << "_Main.h" << Qt::endl;
      out << " * \\brief " << protocolName << " protocol module" << Qt::endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << Qt::endl;
      out << " *" << Qt::endl;
      out << " */" << Qt::endl;
      out << Qt::endl;
      out << "#ifndef " << protocolName.toLower() << "_main_h" << Qt::endl;
      out << "#define " << protocolName.toLower() << "_main_h" << Qt::endl;
      out << Qt::endl;
      out << "// *** Libraries include ***" << Qt::endl;
      out << "// Standard lib" << Qt::endl;
      out << "// Custom lib" << Qt::endl;
      out << "#include <LCSF_Config.h>" << Qt::endl;
      out << Qt::endl;
      out << "// *** Definitions ***" << Qt::endl;
      out << "// --- Public Types ---" << Qt::endl;
      out << Qt::endl;

      // Commands enum
      out << "// Command name enum" << Qt::endl;
      out << "enum _" << protocolName.toLower() << "_cmd_names {" << Qt::endl;
      for (Command *command : cmdList)
      {
         out << "    " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "," << Qt::endl;
      }
      out << "    " << protocolName.toUpper() << "_CMD_COUNT," << Qt::endl;
      out << "};" << Qt::endl;
      out << Qt::endl;

      // Attributes enums
      QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
      if (sortedAttInfosList.size() > 0)
      {
         out << "// Attributes enums" << Qt::endl;
         for (int idx = 0; idx < sortedAttInfosList.size(); idx++)
         {
            Attribute::T_attInfos currentAttInfo = sortedAttInfosList.at(idx);
            out << "enum _" << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_names {" << Qt::endl;
            QString previousParentName = currentAttInfo.parentName;
            out << "    " << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "," << Qt::endl;
            if (idx < sortedAttInfosList.size() - 1)
            {
               currentAttInfo = sortedAttInfosList.at(idx + 1);
               while (currentAttInfo.parentName.compare(previousParentName) == 0)
               {
                  out << "    " << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "," << Qt::endl;
                  previousParentName = currentAttInfo.parentName;
                  idx++;
                  if (idx < sortedAttInfosList.size() - 1)
                  {
                     currentAttInfo = sortedAttInfosList.at(idx + 1);
                  }
                  else
                  {
                     break;
                  }
               }
            }
            out << "};" << Qt::endl;
            out << Qt::endl;
         }
      }

      // Attributes payload structs
      if (this->protocolHasSubAtt)
      {
         out << "// Attribute with sub-attributes structures" << Qt::endl;
         Attribute::T_attInfos currentAttInfo;
         // Parse the list on reverse to declare sub-attributes before their parent and solve dependencies
         for (int idx = attInfosList.size() - 1; idx >= 0; idx--)
         {
            hasOptAtt = false;
            currentAttInfo = attInfosList.at(idx);
            if (currentAttInfo.dataType == NS_AttDataType::SUB_ATTRIBUTES)
            {
               int attCounter = 0;
               for (Attribute *attribute : currentAttInfo.attPointer->getSubAttArray())
               {
                  if (attribute->getIsOptional())
                  {
                     if (!hasOptAtt)
                     {
                        hasOptAtt = true;
                     }
                     out << "#define " << protocolName.toUpper() << "_" << currentAttInfo.attName.toUpper()
                         << "_ATT_" << attribute->getName().toUpper() << "_FLAG (1 << " << attCounter << ")" << Qt::endl;
                     attCounter++;
                  }
               }
               out << "typedef struct _" << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_" << currentAttInfo.attName.toLower() << "_att_payload {" << Qt::endl;
               if (hasOptAtt)
               {
                  QString typeString = getFlagTypeStringFromAttNb(attCounter, "optAttFlagsBitfield");
                  out << "    " << typeString << ";" << Qt::endl;
               }
               int subIdx = idx + 1;
               int subAttCpt = 0;
               while ((subIdx < attInfosList.size()) || (subAttCpt < currentAttInfo.subAttNb))
               {
                  Attribute::T_attInfos currentSubAttInfo = attInfosList.at(subIdx);
                  if (currentSubAttInfo.parentName == currentAttInfo.attName)
                  {
                     // Skip the sub-attributes
                     subAttCpt++;
                     if (currentSubAttInfo.dataType == NS_AttDataType::SUB_ATTRIBUTES)
                     {
                        out << "    " << protocolName.toLower() << "_" << currentSubAttInfo.parentName.toLower() << "_att_" << currentSubAttInfo.attName.toLower() << "_att_payload_t "
                            << currentSubAttInfo.attName.toLower() << "_payload;" << Qt::endl;
                     }
                     else if (currentSubAttInfo.dataType == NS_AttDataType::BYTE_ARRAY)
                     {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << "uint32_t " << currentSubAttInfo.attName.toLower() << "Size;" << Qt::endl;
                        out << "    " << typeString << "p_" << currentSubAttInfo.attName.toLower() << ";" << Qt::endl;
                     }
                     else if (currentSubAttInfo.dataType == NS_AttDataType::STRING)
                     {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << typeString << "p_" << currentSubAttInfo.attName.toLower() << ";" << Qt::endl;
                     }
                     else
                     {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << typeString << currentSubAttInfo.attName.toLower() << ";" << Qt::endl;
                     }
                  }
                  // Next attribute
                  subIdx++;
               }
               out << "} " << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_" << currentAttInfo.attName.toLower() << "_att_payload_t;" << Qt::endl;
               out << Qt::endl;
            }
         }
      }

      // Commands payload struct
      out << "// Commands data structures" << Qt::endl;
      for (Command *command : cmdList)
      {
         hasOptAtt = false;
         if (command->getAttArray().size() > 0)
         {
            int attCounter = 0;
            for (Attribute *attribute : command->getAttArray())
            {
               if (attribute->getIsOptional())
               {
                  if (!hasOptAtt)
                  {
                     hasOptAtt = true;
                  }
                  out << "#define " << protocolName.toUpper() << "_" << command->getName().toUpper()
                      << "_ATT_" << attribute->getName().toUpper() << "_FLAG (1 << " << attCounter << ")" << Qt::endl;
                  attCounter++;
               }
            }
            out << "typedef struct _" << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload {" << Qt::endl;
            if (hasOptAtt)
            {
               QString typeString = getFlagTypeStringFromAttNb(attCounter, "optAttFlagsBitfield");
               out << "    " << typeString << ";" << Qt::endl;
            }
            for (Attribute *attribute : command->getAttArray())
            {
               if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
               {
                  out << "    " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_" << attribute->getName().toLower() << "_att_payload_t "
                      << attribute->getName().toLower() << "_payload;" << Qt::endl;
               }
               else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY)
               {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << "uint32_t " << attribute->getName().toLower() << "Size;" << Qt::endl;
                  out << "    " << typeString << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
               }
               else if (attribute->getDataType() == NS_AttDataType::STRING)
               {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << typeString << "p_" << attribute->getName().toLower() << ";" << Qt::endl;
               }
               else
               {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << typeString << attribute->getName().toLower() << ";" << Qt::endl;
               }
            }
            out << "} " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload_t;" << Qt::endl;
            out << Qt::endl;
         }
      }

      // Command payload union
      out << "// Command payload union" << Qt::endl;
      out << "typedef union _" << protocolName.toLower() << "_cmd_payload {" << Qt::endl;
      for (Command *command : cmdList)
      {
         if (command->getAttArray().size() > 0)
         {
            out << "    " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload_t "
                << command->getName().toLower() << "_payload;" << Qt::endl;
         }
      }
      out << "} " << protocolName.toLower() << "_cmd_payload_t;" << Qt::endl;
      out << Qt::endl;

      // Function headers
      out << "// --- Public Function Prototypes ---" << Qt::endl;
      out << Qt::endl;
      if (codeExtract.getExtractionComplete())
      {
         out << codeExtract.getUnknownPublicFunctionsHeaders();
      }
      else
      {
         out << "/**" << Qt::endl;
         out << " * \\fn bool " << protocolName << "_MainInit(uint8_t *pBuffer, size_t buffSize)" << Qt::endl;
         out << " * \\brief Initialize the module" << Qt::endl;
         out << " *" << Qt::endl;
         out << " * \\param pBuffer pointer to send buffer" << Qt::endl;
         out << " * \\param buffSize buffer size" << Qt::endl;
         out << " * \\return bool: true if operation was a success" << Qt::endl;
         out << " */" << Qt::endl;
         out << "bool " << protocolName << "_MainInit(uint8_t *pBuffer, size_t buffSize);" << Qt::endl;
         out << Qt::endl;
      }
      out << "/**" << Qt::endl;
      out << " * \\fn bool " << protocolName << "_MainExecute(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Execute a command" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdName name of the command" << Qt::endl;
      out << " * \\param pCmdPayload pointer to command payload" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "bool " << protocolName << "_MainExecute(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
      out << Qt::endl;
      out << "// *** End Definitions ***" << Qt::endl;
      out << "#endif // " << protocolName.toLower() << "_Main_h" << Qt::endl;
      file.close();
   }
}

void CodeGenerator::generateMain(QString protocolName, QList<Command *> cmdList, CodeExtractor codeExtract, bool isA, QString dirPath)
{
   QDir dir(dirPath);
   if (!dir.exists())
   {
      dir.mkpath(".");
   }
   QString fileName = dirPath + "/" + protocolName + "_Main_" + ((isA) ? "a" : "b") + ".c";
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
      QFileInfo fileInfo(file);

      if (codeExtract.getExtractionComplete())
      {
         QString test = codeExtract.getUnknownIncludes();
         out << test;
      }
      else
      {
         out << "/**" << Qt::endl;
         out << " * \\file " << protocolName << "_Main_" << ((isA) ? "a" : "b") << ".c" << Qt::endl;
         out << " * \\brief " << protocolName << " protocol module (" << ((isA) ? "A" : "B") << ")" << Qt::endl;
         out << " * \\author LCSF Generator v" << APP_VERSION << Qt::endl;
         out << " *" << Qt::endl;
         out << " */" << Qt::endl;
         out << Qt::endl;
         out << "// *** Libraries include ***" << Qt::endl;
         out << "// Standard lib" << Qt::endl;
         out << "// Custom lib" << Qt::endl;
         out << "#include \"LCSF_Bridge_" << protocolName << ".h\"" << Qt::endl;
         out << "#include \"" << protocolName << "_Main.h\"" << Qt::endl;
         out << "#include <LCSF_Config.h>" << Qt::endl;
         out << Qt::endl;
         out << "// *** Definitions ***" << Qt::endl;
      }

      if (codeExtract.getExtractionComplete())
      {
         out << codeExtract.getUnknownDefinitions();
      }
      else
      {
         out << "// --- Private Macros ---" << Qt::endl;
         out << "// --- Private Types ---" << Qt::endl;
         out << "typedef struct _" << protocolName.toLower() << "_info {" << Qt::endl;
         out << "    uint8_t *pSendBuffer;" << Qt::endl;
         out << "    uint16_t buffSize;" << Qt::endl;
         out << "    " << protocolName.toLower() << "_cmd_payload_t SendCmdPayload;" << Qt::endl;
         out << "} " << protocolName.toLower() << "_info_t;" << Qt::endl;
         out << Qt::endl;
         out << "// --- Private Function Prototypes ---" << Qt::endl;
         out << "// Generated functions" << Qt::endl;
         out << "static bool " << protocolName << "SendCommand(uint_fast16_t cmdName, bool hasPayload);" << Qt::endl;

         for (int idx = 0; idx < cmdList.size(); idx++)
         {
            Command *command = cmdList.at(idx);

            if (command->isReceivable(isA))
            {
               out << "static bool " << protocolName << "Execute" << command->getName() << "(";
               if (command->getAttArray().size() > 0)
               {
                  out << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
               }
               else
               {
                  out << "void);" << Qt::endl;
               }
            }
         }
         out << Qt::endl;
         out << "// --- Private Variables ---" << Qt::endl;
         out << "static " << protocolName.toLower() << "_info_t " << protocolName << "Info;" << Qt::endl;
         out << Qt::endl;
         out << "// *** End Definitions ***" << Qt::endl;
         out << Qt::endl;
         out << "// *** Private Functions ***" << Qt::endl;
      }
      out << Qt::endl;

      if (codeExtract.getExtractionComplete())
      {
         out << codeExtract.getUnknownPrivateFunctions();
      }
      else
      {
         out << "/**" << Qt::endl;
         out << " * \\fn static bool " << protocolName << "SendCommand(uint_fast16_t cmdName, bool hasPayload)" << Qt::endl;
         out << " * \\brief Send a command" << Qt::endl;
         out << " *=" << Qt::endl;
         out << " * \\param cmdName name of the command to send" << Qt::endl;
         out << " * \\param hasPayload indicates if command has a payload or not" << Qt::endl;
         out << " * \\return bool: true if operation was a success" << Qt::endl;
         out << " */" << Qt::endl;
         out << "static bool " << protocolName << "SendCommand(uint_fast16_t cmdName, bool hasPayload) {" << Qt::endl;
         out << "    if (cmdName >= " << protocolName.toUpper() << "_CMD_COUNT) {" << Qt::endl;
         out << "        return false;" << Qt::endl;
         out << "    }" << Qt::endl;
         out << "    int msgSize = 0;" << Qt::endl;
         out << "    if (hasPayload) {" << Qt::endl;
         out << "        " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload = &" << protocolName << "Info.SendCmdPayload;" << Qt::endl;
         out << "        msgSize = LCSF_Bridge_" << protocolName << "Encode(cmdName, pCmdPayload, " << protocolName << "Info.pSendBuffer, " << protocolName << "Info.buffSize);" << Qt::endl;
         out << "    } else {" << Qt::endl;
         out << "        msgSize = LCSF_Bridge_" << protocolName << "Encode(cmdName, NULL, " << protocolName << "Info.pSendBuffer, " << protocolName << "Info.buffSize);" << Qt::endl;
         out << "    }" << Qt::endl;
         out << "    if (msgSize <= 0) {" << Qt::endl;
         out << "        return false;" << Qt::endl;
         out << "    }" << Qt::endl;
         out << "    // TODO Pass buffer to send function" << Qt::endl;
         out << "    return true;" << Qt::endl;
         out << "}" << Qt::endl;
         out << Qt::endl;
         out << "/**" << Qt::endl;
         out << " * \\fn static bool " << protocolName << "ExecuteX(void)" << Qt::endl;
         out << " * \\brief Execute command X (no payload)" << Qt::endl;
         out << " *" << Qt::endl;
         out << " * \\return bool: true if operation was a success" << Qt::endl;
         out << " */" << Qt::endl;
         out << Qt::endl;
         out << "/**" << Qt::endl;
         out << " * \\fn static bool " << protocolName << "ExecuteX(" << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
         out << " * \\brief Execute command X (with payload)" << Qt::endl;
         out << " *" << Qt::endl;
         out << " * \\param pCmdPayload pointer to the command payload" << Qt::endl;
         out << " * \\return bool: true if operation was a success" << Qt::endl;
         out << " */" << Qt::endl;
         out << Qt::endl;
      }

      for (int idx = 0; idx < cmdList.size(); idx++)
      {
         QStringList commandFunctionsList;
         QString commandFunction;
         Command *command = cmdList.at(idx);

         if (codeExtract.getExtractionComplete())
         {
            commandFunctionsList = codeExtract.getCommandFunctions();
            int cmdIdx = findCmdFctIdxFromName(command->getName(), commandFunctionsList);
            if (cmdIdx >= 0)
            {
               commandFunction = commandFunctionsList.at(cmdIdx);
            }
         }
         if (command->isReceivable(isA))
         {
            if ((codeExtract.getExtractionComplete()) && (commandFunction.compare("") != 0))
            {
               out << commandFunction;
            }
            else
            {
               out << "static bool " << protocolName << "Execute" << command->getName() << "(";
               if (command->getAttArray().size() > 0)
               {
                  out << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
                  out << "    if (pCmdPayload == NULL) {" << Qt::endl;
                  out << "        return false;" << Qt::endl;
                  out << "    }" << Qt::endl;
                  out << "    // Declare attributes" << Qt::endl;
                  declareAtt_REC(command->getName(), command->getAttArray(), &out);
                  out << "    // Retrieve attributes data" << Qt::endl;
                  for (Attribute *attribute : command->getAttArray())
                  {
                     if (attribute->getIsOptional())
                     {
                        out << "    if ((pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield & " << protocolName.toUpper() << "_"
                            << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << Qt::endl;

                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
                        {
                           QStringList parentNames = {command->getName(), attribute->getName()};
                           out << "        m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << "_isHere = true;" << Qt::endl;
                           this->grabAttValues_REC(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                        }
                        else
                        {
                           if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
                           {
                              out << "        " << "m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                           }
                           else
                           {
                              QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                              out << "        " << "m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ";" << Qt::endl;
                           }
                           out << "        m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << "_isHere = true;" << Qt::endl;
                        }
                        out << "    }" << Qt::endl;
                     }
                     else
                     {
                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
                        {
                           QStringList parentNames = {command->getName(), attribute->getName()};
                           this->grabAttValues_REC(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                        }
                        else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
                        {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "    " << "m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                               << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                        }
                        else
                        {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "    " << "m_" << command->getName().toLower() << "_" << attribute->getName().toLower() << " = pCmdPayload->"
                               << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ";" << Qt::endl;
                        }
                     }
                  }
                  out << "    // Process data" << Qt::endl;
                  out << "    #pragma GCC warning \"#warning TODO: function to implement\"" << Qt::endl;
               }
               else
               {
                  out << "void) {" << Qt::endl;
                  out << "    #pragma GCC warning \"#warning TODO: function to implement\"" << Qt::endl;
               }
               out << "    return true;" << Qt::endl;
               out << "}" << Qt::endl;
               out << Qt::endl;
            }
         }
      }

      out << "// *** Public Functions ***" << Qt::endl;
      out << Qt::endl;

      if (codeExtract.getExtractionComplete())
      {
         out << codeExtract.getUnknownPublicFunctions();
      }
      else
      {
         out << "/**" << Qt::endl;
         out << " * \\fn bool " << protocolName << "_MainInit(uint8_t *pBuffer, size_t buffSize)" << Qt::endl;
         out << " * \\brief Initialize the module" << Qt::endl;
         out << " *" << Qt::endl;
         out << " * \\param pBuffer pointer to send buffer" << Qt::endl;
         out << " * \\param buffSize buffer size" << Qt::endl;
         out << " * \\return bool: true if operation was a success" << Qt::endl;
         out << " */" << Qt::endl;
         out << "bool " << protocolName << "_MainInit(uint8_t *pBuffer, size_t buffSize) {" << Qt::endl;
         out << "    if (pBuffer == NULL) {" << Qt::endl;
         out << "        return false;" << Qt::endl;
         out << "    }" << Qt::endl;
         out << "    // Note infos" << Qt::endl;
         out << "    " << protocolName << "Info.pSendBuffer = pBuffer;" << Qt::endl;
         out << "    " << protocolName << "Info.buffSize = buffSize;" << Qt::endl;
         out << "    return true;" << Qt::endl;
         out << "}" << Qt::endl;
         out << Qt::endl;
         out << "// Place custom public functions here" << Qt::endl;
         out << Qt::endl;
      }
      out << "/**" << Qt::endl;
      out << " * \\fn bool " << protocolName << "_MainExecute(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Execute a command" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdName name of the command" << Qt::endl;
      out << " * \\param pCmdPayload pointer to command payload" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "bool " << protocolName << "_MainExecute(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
      out << "    switch (cmdName) {" << Qt::endl;
      for (Command *command : cmdList)
      {
         if (command->isReceivable(isA))
         {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << Qt::endl;
            out << "            return " << protocolName << "Execute" << command->getName();
            if (command->getAttArray().size() > 0)
            {
               out << "(pCmdPayload);" << Qt::endl;
            }
            else
            {
               out << "();" << Qt::endl;
            }
            out << Qt::endl;
         }
      }
      out << "        default:" << Qt::endl;

      QString defaultCommandHandler = codeExtract.getDefaultCommandHandler();

      if ((codeExtract.getExtractionComplete()) && (defaultCommandHandler.compare("") != 0))
      {
         out << defaultCommandHandler;
      }
      else
      {
         out << "            // This case can be customized (e.g to send an error command)" << Qt::endl;
         out << "            return false;" << Qt::endl;
      }
      out << "    }" << Qt::endl;
      out << "}" << Qt::endl;

      file.close();
   }
}

void CodeGenerator::generateBridgeHeader(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath)
{
   QDir dir(dirPath);
   if (!dir.exists())
   {
      dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Bridge_" + protocolName + ".h";
   QList<Attribute::T_attInfos> attIdxList = this->getAttInfos(cmdList);
   QList<Attribute::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attIdxList);
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << Qt::endl;
      out << " * \\file LCSF_Bridge_" << protocolName << ".h" << Qt::endl;
      out << " * \\brief " << protocolName << " LCSF bridge module" << Qt::endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << Qt::endl;
      out << " *" << Qt::endl;
      out << " */" << Qt::endl;
      out << Qt::endl;
      out << "#ifndef Lcsf_bridge_" << protocolName.toLower() << "_h" << Qt::endl;
      out << "#define Lcsf_bridge_" << protocolName.toLower() << "_h" << Qt::endl;
      out << Qt::endl;
      out << "// *** Libraries include ***" << Qt::endl;
      out << "// Standard lib" << Qt::endl;
      out << "// Custom lib" << Qt::endl;
      out << "#include \"" << protocolName << "_Main.h\"" << Qt::endl;
      out << "#include <LCSF_Config.h>" << Qt::endl;
      out << "#include <lib/LCSF_Validator.h>" << Qt::endl;
      out << Qt::endl;
      out << "// *** Definitions ***" << Qt::endl;
      out << "// --- Public Types ---" << Qt::endl;
      out << Qt::endl;

      // Commands Ids enum
      out << "// Command identifier enum" << Qt::endl;
      out << "enum _lcsf_" << protocolName.toLower() << "_cmd_id {" << Qt::endl;
      for (Command *command : cmdList)
      {
         out << "    LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << " = 0x" << QString::number(command->getId(), 16) << "," << Qt::endl;
      }
      out << "};" << Qt::endl;
      out << Qt::endl;

      // Attributes Ids enums
      if (!attIdxList.isEmpty())
      {
         out << "// Attribute identifier enums" << Qt::endl;
         for (int idx = 0; idx < sortedAttInfosList.size(); idx++)
         {
            Attribute::T_attInfos currentAttInfo = sortedAttInfosList.at(idx);
            out << "enum _lcsf_" << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_id {" << Qt::endl;
            QString previousParentName = currentAttInfo.parentName;
            out << "    LCSF_" << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_ID_" << currentAttInfo.attName.toUpper() << " = 0x"
                << QString::number(currentAttInfo.attId, 16) << "," << Qt::endl;
            if (idx < sortedAttInfosList.size() - 1)
            {
               currentAttInfo = sortedAttInfosList.at(idx + 1);
               while (currentAttInfo.parentName.compare(previousParentName) == 0)
               {
                  out << "    LCSF_" << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_ID_" << currentAttInfo.attName.toUpper() << " = 0x"
                      << QString::number(currentAttInfo.attId, 16) << "," << Qt::endl;
                  previousParentName = currentAttInfo.parentName;
                  idx++;
                  if (idx < sortedAttInfosList.size() - 1)
                  {
                     currentAttInfo = sortedAttInfosList.at(idx + 1);
                  }
                  else
                  {
                     break;
                  }
               }
            }
            out << "};" << Qt::endl;
            out << Qt::endl;
         }
      }
      out << "// --- Public Constants ---" << Qt::endl;
      out << Qt::endl;
      out << "// Bridge decoder filo size" << Qt::endl;
      out << "#define LCSF_BRIDGE_" << protocolName.toUpper() << "_FILO_SIZE " << Command::getMaxAttNb(cmdList) << Qt::endl;
      out << "// Lcsf protocol identifier" << Qt::endl;
      out << "#define LCSF_" << protocolName.toUpper() << "_PROTOCOL_ID 0x" << protocolId << Qt::endl;
      out << "// Command number" << Qt::endl;
      out << "#define LCSF_" << protocolName.toUpper() << "_CMD_NB " << protocolName.toUpper() << "_CMD_COUNT" << Qt::endl;
      if (!attIdxList.isEmpty())
      {
         out << "// Command attribute number" << Qt::endl;
         for (Command *command : cmdList)
         {
            if (command->getAttArray().size() > 0)
            {
               out << "#define LCSF_" << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB " << command->getAttArray().size() << Qt::endl;
            }
         }
         if (this->protocolHasSubAtt)
         {
            out << "// Attribute sub-attributes number" << Qt::endl;
            for (Attribute::T_attInfos currentAttInfo : attIdxList)
            {
               if (currentAttInfo.subAttNb > 0)
               {
                  out << "#define LCSF_" << protocolName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "_SUBATT_NB "
                      << currentAttInfo.subAttNb << Qt::endl;
               }
            }
         }
      }
      out << Qt::endl;
      out << "// Protocol descriptor" << Qt::endl;
      out << "extern const lcsf_protocol_desc_t LCSF_" << protocolName << "_ProtDesc;" << Qt::endl;
      out << Qt::endl;
      out << "// --- Public Function Prototypes ---" << Qt::endl;
      out << Qt::endl;
      out << "/**" << Qt::endl;
      out << " * \\fn bool LCSF_Bridge_" << protocolName << "Init(void)" << Qt::endl;
      out << " * \\brief Initialize the module" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "bool LCSF_Bridge_" << protocolName << "Init(void);" << Qt::endl;
      out << Qt::endl;
      out << "/**" << Qt::endl;
      out << " * \\fn bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd)" << Qt::endl;
      out << " * \\brief Receive valid command from LCSF_Validator and transmit to " << protocolName << "_Main" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param pValidCmd pointer to the valid command" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd);" << Qt::endl;
      out << Qt::endl;
      out << "/**" << Qt::endl;
      out << " * \\fn int LCSF_Bridge_" << protocolName << "Encode(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize)" << Qt::endl;
      out << " * \\brief Receive command from " << protocolName << "_Main and transmit to LCSF_Validator for encoding" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdName name of the command" << Qt::endl;
      out << " * \\param pValidCmd pointer to the valid command" << Qt::endl;
      out << " * \\param pBuffer pointer to the send buffer" << Qt::endl;
      out << " * \\param buffSize buffer size" << Qt::endl;
      out << " * \\return int: -1 if operation failed, encoded message size if success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "int LCSF_Bridge_" << protocolName << "Encode(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize);" << Qt::endl;
      out << Qt::endl;
      out << "// *** End Definitions ***" << Qt::endl;
      out << "#endif // Lcsf_bridge_" << protocolName.toLower() << "_h" << Qt::endl;

      file.close();
   }
}

void CodeGenerator::generateBridge(QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath)
{
   QDir dir(dirPath);
   if (!dir.exists())
   {
      dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Bridge_" + protocolName + "_" + ((isA) ? "a" : "b") + ".c";
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << Qt::endl;
      out << " * \\file LCSF_Bridge_" << protocolName << "_" << ((isA) ? "a" : "b") << ".c" << Qt::endl;
      out << " * \\brief " << protocolName << " LCSF bridge module (" << ((isA) ? "A" : "B") << ")" << Qt::endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << Qt::endl;
      out << " *" << Qt::endl;
      out << " */" << Qt::endl;
      out << Qt::endl;
      out << "// *** Libraries include ***" << Qt::endl;
      out << "// Standard lib" << Qt::endl;
      out << "#include <string.h>" << Qt::endl;
      out << "// Custom lib" << Qt::endl;
      out << "#include \"LCSF_Bridge_" << protocolName << ".h\"" << Qt::endl;
      out << "#include <LCSF_Config.h>" << Qt::endl;
      out << "#include <lib/Filo.h>" << Qt::endl;
      out << "#include <lib/LCSF_Transcoder.h>" << Qt::endl;
      out << Qt::endl;
      out << "// *** Definitions ***" << Qt::endl;
      out << "// --- Private Types ---" << Qt::endl;
      out << Qt::endl;
      out << "// Module information structure" << Qt::endl;
      out << "typedef struct _lcsf_bridge_" << protocolName.toLower() << "_info {" << Qt::endl;
      out << "    uint8_t FiloData[LCSF_BRIDGE_" << protocolName.toUpper() << "_FILO_SIZE * sizeof(lcsf_valid_att_t)];" << Qt::endl;
      out << "    filo_desc_t Filo;" << Qt::endl;
      out << "    " << protocolName.toLower() << "_cmd_payload_t CmdPayload;" << Qt::endl;
      out << "} lcsf_bridge_" << protocolName.toLower() << "_info_t;" << Qt::endl;
      out << Qt::endl;
      out << "// --- Private Constants ---" << Qt::endl;
      out << "// Array to convert command name value to their lcsf command id" << Qt::endl;
      out << "static const uint16_t LCSF_Bridge_" << protocolName << "_CMDNAME2CMDID[LCSF_" << protocolName.toUpper() << "_CMD_NB] = {" << Qt::endl;
      for (Command *command : cmdList)
      {
         out << "    LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << "," << Qt::endl;
      }
      out << "};" << Qt::endl;
      out << Qt::endl;
      out << "// --- Private Function Prototypes ---" << Qt::endl;
      out << "static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint_fast16_t cmdId);" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA)))
         {
            out << "static void LCSF_Bridge_" << protocolName << command->getName() << "GetData(lcsf_valid_att_t *pAttArray, "
                << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
         }
      }
      out << "static void LCSF_Bridge_" << protocolName << "GetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(!isA)))
         {
            out << "static bool LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(lcsf_valid_att_t **pAttArrayAddr, "
                << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
         }
      }
      out << "static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << Qt::endl;
      out << Qt::endl;
      out << "// --- Private Variables ---" << Qt::endl;
      out << "static lcsf_bridge_" << protocolName.toLower() << "_info_t LcsfBridge" << protocolName << "Info;" << Qt::endl;
      out << Qt::endl;
      out << "// *** End Definitions ***" << Qt::endl;
      out << Qt::endl;
      out << "// *** Private Functions ***" << Qt::endl;
      out << Qt::endl;
      out << "/**" << Qt::endl;
      out << " * \\fn static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint_fast16_t cmdId)" << Qt::endl;
      out << " * \\brief Translate an lcsf command id to its name value" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdId lcsf command identifier to translate" << Qt::endl;
      out << " * \\return uint16_t: name value of the command" << Qt::endl;
      out << " */" << Qt::endl;
      out << "static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint_fast16_t cmdId) {" << Qt::endl;
      out << "    switch (cmdId) {" << Qt::endl;
      out << "        default:" << Qt::endl;
      for (Command *command : cmdList)
      {
         out << "        case LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << ":" << Qt::endl;
         out << "            return " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ";" << Qt::endl;
      }
      out << "    }" << Qt::endl;
      out << "}" << Qt::endl;
      out << Qt::endl;

      // Command get data functions
      out << "/**" << Qt::endl;
      out << " * \\fn static void LCSF_Bridge_" << protocolName << "XGetData(lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Retrieve data of command X from its valid attribute array and store it in a payload" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param pAttArray pointer to the command attribute array" << Qt::endl;
      out << " * \\param pCmdPayload pointer to the payload to contain the command data" << Qt::endl;
      out << " * \\return void" << Qt::endl;
      out << " */" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA)))
         {
            out << "static void LCSF_Bridge_" << protocolName << command->getName() << "GetData(lcsf_valid_att_t *pAttArray, "
                << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
            out << "    if (pCmdPayload == NULL) {" << Qt::endl;
            out << "        return;" << Qt::endl;
            out << "    }" << Qt::endl;
            for (Attribute *attribute : command->getAttArray())
            {
               if (attribute->getIsOptional())
               {
                  out << "    // Initialize optional attribute flags bitfield" << Qt::endl;
                  out << "    pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield = 0;" << Qt::endl;
                  break;
               }
            }
            for (Attribute *attribute : command->getAttArray())
            {
               out << "    // Retrieve data of attribute " << attribute->getName() << Qt::endl;
               if (attribute->getIsOptional())
               {
                  out << "    if (pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                      << attribute->getName().toUpper() << "].Payload.pData != NULL) {" << Qt::endl;
                  out << "        // Note presence of optional attribute" << Qt::endl;
                  out << "        pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield |= "
                      << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG;" << Qt::endl;
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
                  {
                     QStringList parentNames = {command->getName(), attribute->getName()};
                     this->fillSubAttData_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                  }
                  else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
                  {
                     out << "        pCmdPayload->" << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << " = pAttArray["
                         << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData;" << Qt::endl;
                  }
                  else
                  {
                     out << "        memcpy(&(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "), pAttArray["
                         << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData, pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize);" << Qt::endl;
                  }
                  out << "    }" << Qt::endl;
               }
               else
               {
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES)
                  {
                     QStringList parentNames = {command->getName(), attribute->getName()};
                     this->fillSubAttData_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                  }
                  else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING))
                  {
                     out << "    pCmdPayload->" << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << " = pAttArray["
                         << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData;" << Qt::endl;
                  }
                  else
                  {
                     out << "    memcpy(&(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "), pAttArray["
                         << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData, pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize);" << Qt::endl;
                  }
               }
            }
            out << "}" << Qt::endl;
            out << Qt::endl;
         }
      }

      // Master get data function
      out << "/**" << Qt::endl;
      out << " * \\fn static void LCSF_Bridge_" << protocolName << "GetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Retrieve command data from its attribute array and store it in a payload" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdName name of the command" << Qt::endl;
      out << " * \\param pAttArray pointer to the command attribute array" << Qt::endl;
      out << " * \\param pPayload pointer to the payload to contain the command data" << Qt::endl;
      out << " * \\return void" << Qt::endl;
      out << " */" << Qt::endl;
      out << "static void LCSF_Bridge_" << protocolName << "GetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, "
          << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
      out << "    if (pAttArray == NULL) {" << Qt::endl;
      out << "	    return;" << Qt::endl;
      out << "    }" << Qt::endl;
      out << "    switch (cmdName) {" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA)))
         {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << Qt::endl;
            out << "            LCSF_Bridge_" << protocolName << command->getName() << "GetData(pAttArray, pCmdPayload);" << Qt::endl;
            out << "            break;" << Qt::endl;
            out << Qt::endl;
         }
      }
      out << "        default: // Commands that don't have payload" << Qt::endl;
      out << "            return;" << Qt::endl;
      out << "    }" << Qt::endl;
      out << "}" << Qt::endl;
      out << Qt::endl;

      // Command fill attribute functions
      out << "/**" << Qt::endl;
      out << " * \\fn static bool LCSF_Bridge_" << protocolName << "XFillAtt(lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Allocate and fill attribute array of command X from its payload" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param pAttArrayAddr pointer to contain the attribute array" << Qt::endl;
      out << " * \\param pCmdPayload pointer to the command payload" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA)))
         {
            out << "static bool LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(lcsf_valid_att_t **pAttArrayAddr, "
                << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
            out << "    if (pCmdPayload == NULL) {" << Qt::endl;
            out << "        return false;" << Qt::endl;
            out << "    }" << Qt::endl;
            out << "    // Allocate attribute array" << Qt::endl;
            out << "    if (!FiloGet(&LcsfBridge" << protocolName << "Info.Filo, LCSF_" << protocolName.toUpper() << "_CMD_"
                << command->getName().toUpper() << "_ATT_NB, (void *)pAttArrayAddr)) {" << Qt::endl;
            out << "        return false;" << Qt::endl;
            out << "    }" << Qt::endl;
            out << "    // Intermediary variable" << Qt::endl;
            out << "    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;" << Qt::endl;
            if (command->hasSubAtt())
            {
               out << "    lcsf_valid_att_t **pSubAttArray = NULL;" << Qt::endl;
            }
            for (Attribute *attribute : command->getAttArray())
            {
               out << "    // Fill data of attribute " << attribute->getName() << Qt::endl;
               if (attribute->getIsOptional())
               {
                  out << "    if ((pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield & " << protocolName.toUpper() << "_"
                      << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << Qt::endl;
                  switch (attribute->getDataType())
                  {
                  case NS_AttDataType::SUB_ATTRIBUTES:
                  {
                     out << "        // Intermediary variable" << Qt::endl;
                     out << "        pSubAttArray = &(pAttArray[" << protocolName.toUpper() << "_"
                         << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "].Payload.pSubAttArray);" << Qt::endl;
                     out << "        // Allocate sub-attribute array" << Qt::endl;
                     out << "        if (!FiloGet(&LcsfBridge" << protocolName << "Info.Filo, LCSF_" << protocolName.toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << Qt::endl;
                     out << "            return false;" << Qt::endl;
                     out << "        }" << Qt::endl;
                     QStringList parentNames = {command->getName(), attribute->getName()};
                     this->fillSubAttPayload_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                  }
                  break;
                  case NS_AttDataType::BYTE_ARRAY:
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].PayloadSize = pCmdPayload->"
                         << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "Size;" << Qt::endl;
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                         << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                     break;
                  case NS_AttDataType::STRING:
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                         << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                     break;
                  case NS_AttDataType::FLOAT32:
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = sizeof(float);" << Qt::endl;
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  case NS_AttDataType::FLOAT64:
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = sizeof(double);" << Qt::endl;
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  default:
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = GetVLESize(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  }
                  out << "    } else {" << Qt::endl;
                  out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                      << "].Payload.pData = NULL;" << Qt::endl;
                  out << "    }" << Qt::endl;
               }
               else
               {
                  switch (attribute->getDataType())
                  {
                  case NS_AttDataType::SUB_ATTRIBUTES:
                  {
                     out << "    // Intermediary variable" << Qt::endl;
                     out << "    pSubAttArray = &(pAttArray[" << protocolName.toUpper() << "_"
                         << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "].Payload.pSubAttArray);" << Qt::endl;
                     out << "    // Allocate sub-attribute array" << Qt::endl;
                     out << "    if (!FiloGet(&LcsfBridge" << protocolName << "Info.Filo, LCSF_" << protocolName.toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << Qt::endl;
                     out << "        return false;" << Qt::endl;
                     out << "    }" << Qt::endl;
                     QStringList parentNames = {command->getName(), attribute->getName()};
                     this->fillSubAttPayload_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                  }
                  break;
                  case NS_AttDataType::BYTE_ARRAY:
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].PayloadSize = pCmdPayload->"
                         << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "Size;" << Qt::endl;
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                         << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                     break;
                  case NS_AttDataType::STRING:
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                         << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << Qt::endl;
                     break;
                  case NS_AttDataType::FLOAT32:
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = sizeof(float);" << Qt::endl;
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  case NS_AttDataType::FLOAT64:
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = sizeof(double);" << Qt::endl;
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  default:
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].PayloadSize = GetVLESize(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                         << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << Qt::endl;
                     break;
                  }
               }
            }
            out << "    return true;" << Qt::endl;
            out << "}" << Qt::endl;
            out << Qt::endl;
         }
      }

      // Master fill attribute function
      out << "/**" << Qt::endl;
      out << " * \\fn static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << Qt::endl;
      out << " * \\brief Fill the attribute array of a command from its payload" << Qt::endl;
      out << " *" << Qt::endl;
      out << " * \\param cmdName name of the command" << Qt::endl;
      out << " * \\param pAttArrayAddr pointer to contain the attribute array" << Qt::endl;
      out << " * \\param pCmdPayload pointer to the command payload" << Qt::endl;
      out << " * \\return bool: true if operation was a success" << Qt::endl;
      out << " */" << Qt::endl;
      out << "static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << Qt::endl;
      out << "    switch (cmdName) {" << Qt::endl;
      for (Command *command : cmdList)
      {
         if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA)))
         {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << Qt::endl;
            out << "            return LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(pAttArrayAddr, pCmdPayload);" << Qt::endl;
            out << Qt::endl;
         }
      }
      out << "        default: // Commands that don't have attributes" << Qt::endl;
      out << "            *pAttArrayAddr = NULL;" << Qt::endl;
      out << "            return true;" << Qt::endl;
      out << "    }" << Qt::endl;
      out << "}" << Qt::endl;
      out << Qt::endl;

      // Public functions

      out << "// *** Public Functions ***" << Qt::endl;
      out << Qt::endl;

      out << "bool LCSF_Bridge_" << protocolName << "Init(void) {" << Qt::endl;
      out << "    return FiloInit(&LcsfBridge" << protocolName << "Info.Filo, LcsfBridge" << protocolName << "Info.FiloData, LCSF_BRIDGE_" << protocolName.toUpper() << "_FILO_SIZE, sizeof(lcsf_valid_att_t));" << Qt::endl;
      out << "}" << Qt::endl;
      out << Qt::endl;

      out << "bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd) {" << Qt::endl;
      out << "    uint16_t cmdName = LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(pValidCmd->CmdId);" << Qt::endl;
      out << "    " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload = &LcsfBridge" << protocolName << "Info.CmdPayload;" << Qt::endl;
      out << Qt::endl;
      out << "    LCSF_Bridge_" << protocolName << "GetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);" << Qt::endl;
      out << "    return " << protocolName << "_MainExecute(cmdName, pCmdPayload);" << Qt::endl;
      out << "}" << Qt::endl;
      out << Qt::endl;

      out << "int LCSF_Bridge_" << protocolName << "Encode(uint_fast16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {" << Qt::endl;
      out << "    lcsf_valid_cmd_t sendCmd;" << Qt::endl;
      out << "    sendCmd.CmdId = LCSF_Bridge_" << protocolName << "_CMDNAME2CMDID[cmdName];" << Qt::endl;
      out << "    FiloFreeAll(&LcsfBridge" << protocolName << "Info.Filo);" << Qt::endl;
      out << Qt::endl;
      out << "    if (!LCSF_Bridge_" << protocolName << "FillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {" << Qt::endl;
      out << "        return -1;" << Qt::endl;
      out << "    }" << Qt::endl;
      out << "    return LCSF_ValidatorEncode(LCSF_" << protocolName.toUpper() << "_PROTOCOL_ID, &sendCmd, pBuffer, buffSize);" << Qt::endl;
      out << "}" << Qt::endl;

      file.close();
   }
}

void CodeGenerator::generateDescription(QString protocolName, QList<Command *> cmdList, QString dirPath)
{
   QDir dir(dirPath);
   if (!dir.exists())
   {
      dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Desc_" + protocolName + ".c";
   QList<Attribute::T_attInfos> attInfosList = this->getAttInfos(cmdList);
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
   {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << Qt::endl;
      out << " * \\file LCSF_Desc_" << protocolName << ".c" << Qt::endl;
      out << " * \\brief " << protocolName << " LCSF descriptor" << Qt::endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << Qt::endl;
      out << " *" << Qt::endl;
      out << " */" << Qt::endl;
      out << Qt::endl;
      out << "// *** Libraries include ***" << Qt::endl;
      out << "// Standard lib" << Qt::endl;
      out << "// Custom lib" << Qt::endl;
      out << "#include \"LCSF_Bridge_" << protocolName << ".h\"" << Qt::endl;
      out << "#include <LCSF_Config.h>" << Qt::endl;
      out << Qt::endl;
      out << "// *** Definitions ***" << Qt::endl;
      out << "// --- Private Constants ---" << Qt::endl;
      out << Qt::endl;

      // Sub-attribute descriptor arrays
      if (this->protocolHasSubAtt)
      {
         for (int idx = attInfosList.size() - 1; idx >= 0; idx--)
         {
            Attribute::T_attInfos attInfo = attInfosList.at(idx);
            if (attInfo.subAttNb > 0)
            {
               out << "// Sub-attribute array descriptor of attribute " << attInfo.attName << Qt::endl;
               out << "static const lcsf_attribute_desc_t LCSF_" << protocolName << "_" << attInfo.attName << "_AttDescArray[LCSF_"
                   << protocolName.toUpper() + "_ATT_" + attInfo.attName.toUpper() + "_SUBATT_NB] = {" << Qt::endl;
               for (Attribute *attribute : attInfo.attPointer->getSubAttArray())
               {
                  out << "    {" << this->getAttDescString(protocolName, attInfo.attName, attribute) << "}," << Qt::endl;
               }
               out << "};" << Qt::endl;
               out << Qt::endl;
            }
         }
      }

      // Attribute descriptor arrays
      for (Command *command : cmdList)
      {
         if (command->getAttArray().size() > 0)
         {
            out << "// Attribute array descriptor of command " << command->getName() << Qt::endl;
            out << "static const lcsf_attribute_desc_t LCSF_" << protocolName << "_" << command->getName() << "_AttDescArray[LCSF_"
                << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB] = {" << Qt::endl;
            for (Attribute *attribute : command->getAttArray())
            {
               out << "    {" << this->getAttDescString(protocolName, command->getName(), attribute) << "}," << Qt::endl;
            }
            out << "};" << Qt::endl;
            out << Qt::endl;
         }
      }

      // Command descriptor array
      out << "// Command array descriptor" << Qt::endl;
      out << "static const lcsf_command_desc_t LCSF_" << protocolName << "_CmdDescArray[LCSF_" << protocolName.toUpper() << "_CMD_NB] = {" << Qt::endl;
      for (Command *command : cmdList)
      {
         if (command->getAttArray().size() > 0)
         {
            out << "    {LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << ", LCSF_"
                << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB" << ", LCSF_"
                << protocolName << "_" << command->getName() << "_AttDescArray" << "}," << Qt::endl;
         }
         else
         {
            out << "    {LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper()
                << ", 0, NULL}," << Qt::endl;
         }
      }
      out << "};" << Qt::endl;
      out << Qt::endl;
      out << "// --- Public Constants ---" << Qt::endl;
      out << Qt::endl;
      // Protocol descriptor
      out << "// Protocol descriptor" << Qt::endl;
      out << "const lcsf_protocol_desc_t LCSF_" << protocolName << "_ProtDesc = {.CmdNb = LCSF_"
          << protocolName.toUpper() << "_CMD_NB, .pCmdDescArray = LCSF_" << protocolName << "_CmdDescArray};" << Qt::endl;

      file.close();
   }
}