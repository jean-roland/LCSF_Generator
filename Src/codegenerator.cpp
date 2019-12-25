#include "codegenerator.h"

#include <QApplication>
#include <QDate>
#include <QStringBuilder>

CodeGenerator::CodeGenerator() {

}

QString CodeGenerator::getAttDataRxPath(QString protocolName, QStringList parentNames, QString attName) {
   QString namePrefix = protocolName.toUpper() + "_";
   QString attDataPath;

   if (parentNames.size() > 1) {
      attDataPath = "pAttArray[";
      for (int idx = 0; idx < parentNames.size() - 1; idx++) {
         attDataPath.append(namePrefix + parentNames.at(idx).toUpper() + "_ATT_" + parentNames.at(idx + 1).toUpper() + "].Payload.pSubAttArray[" );
      }
      attDataPath.append(namePrefix + parentNames.last().toUpper() + "_ATT_" + attName.toUpper() + "].Payload.pData");
   }
   return attDataPath;
}

QString CodeGenerator::getAttDataTxPath(QString protocolName, QStringList parentNames, QString attName) {
   QString namePrefix = protocolName.toUpper() + "_";
   QString attDataPath;

   if (parentNames.size() > 1) {
      attDataPath = "pAttArray[";
      for (int idx = 0; idx < parentNames.size() - 1; idx++) {
         attDataPath.append(namePrefix + parentNames.at(idx).toUpper() + "_ATT_" + parentNames.at(idx + 1).toUpper() + "].Payload.pSubAttArray[" );
      }
      attDataPath.append(namePrefix + parentNames.last().toUpper() + "_ATT_" + attName.toUpper() + "].");
   }
   return attDataPath;
}

QString CodeGenerator::getPayloadPath(QStringList parentNames) {
   QString payloadPath = "";
   for (int idx = 0; idx < parentNames.size(); idx++) {
      payloadPath.append(parentNames.at(idx).toLower() + "_payload.");
   }
   return payloadPath;
}

QString CodeGenerator::getIndent(int indentNb) {
   QString indentString = "";
   if (indentNb > 0) {
      for (int idx = 0; idx < indentNb; idx++) {
         indentString.append("    ");
      }
   }
   return indentString;
}

QString CodeGenerator::getFlagTypeStringFromAttNb(int attNb, QString varName) {
   QString typeString = QString();

   if (attNb > 0) {
      if (attNb <= 8) {
         typeString = "uint8_t " + varName;
      } else if (attNb <= 16) {
         typeString = "uint16_t " + varName;
      } else if (attNb <= 32) {
         typeString = "uint32_t " + varName;
      } else {
         int ByteNb = attNb / 8;
         if (attNb % 8 != 0) ByteNb++;
         typeString = "uint8_t ";
         typeString.append(varName + "[");
         typeString.append(QString::number(ByteNb));
         typeString.append("] ");
      }
   }
   return typeString;
}

QString CodeGenerator::getTypeStringFromDataType(NS_AttDataType::T_AttDataType dataType) {
   QString typeString;
  switch (dataType) {
      case NS_AttDataType::UINT8 :
         typeString = "uint8_t ";
         break;

      case NS_AttDataType::UINT16 :
         typeString = "uint16_t ";
         break;

      case NS_AttDataType::UINT32 :
         typeString = "uint32_t ";
         break;

      case NS_AttDataType::BYTE_ARRAY :
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

QString CodeGenerator::getAttDescString(QString protocolName, Attribute *attribute) {
   QString descString = "";
   if (attribute != nullptr) {
      if (attribute->getIsOptional()) {
         descString.append("true, ");
      } else {
         descString.append("false, ");
      }
      descString.append(NS_AttDataType::SL_AttDataType.at(attribute->getDataType()) + ", ");
      descString.append("LCSF_" + protocolName.toUpper() + "_ATT_" + attribute->getName().toUpper() + ", ");
      if (attribute->getSubAttArray().size() > 0) {
         descString.append("LCSF_" + protocolName.toUpper() + "_ATT_" + attribute->getName().toUpper() + "_SUBATT_NB, ");
         descString.append("LCSF_" + protocolName + "_" + attribute->getName() + "_AttDescArray");
      } else {
         descString.append("0, NULL");
      }
   }
   return descString;
}

QList<CodeGenerator::T_attInfos> CodeGenerator::getAttInfos_Rec(QString parentName, QList<Attribute *> attList) {
   QList<CodeGenerator::T_attInfos> resultList = QList<CodeGenerator::T_attInfos>();
   CodeGenerator::T_attInfos localAttInfos;
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
         if (!this->protocolHasSubAtt) {
             this->protocolHasSubAtt = true;
         }
         resultList.append(this->getAttInfos_Rec(localAttInfos.attName, attribute->getSubAttArray()));
      }
   }
   return resultList;
}

QList<CodeGenerator::T_attInfos> CodeGenerator::getAttInfos(QList<Command *> cmdList) {
   QList<CodeGenerator::T_attInfos> resultList = QList<CodeGenerator::T_attInfos>();
   this->protocolHasSubAtt = false;
   for (Command *command : cmdList) {
      QString cmdName = command->getName();
      if ((command->getHasAtt()) && (command->getAttArray().size() > 0)) {
         resultList.append(this->getAttInfos_Rec(cmdName, command->getAttArray()));
      }
   }
   return resultList;
}

QList<CodeGenerator::T_attInfos> CodeGenerator::insertSortAttInfosListByParentName(QList<CodeGenerator::T_attInfos> attInfosList) {
   QList<CodeGenerator::T_attInfos> sortedList = attInfosList;
   CodeGenerator::T_attInfos bufferAttInfos;
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

bool CodeGenerator::findAttInfoId(short attId, QList<CodeGenerator::T_attInfos> attInfosList) {
   bool isThere = false;

   for (CodeGenerator::T_attInfos attInfo : attInfosList) {
      if (attInfo.attId == attId) {
         isThere = true;
         break;
      }
   }
   return isThere;
}

QList<CodeGenerator::T_attInfos> CodeGenerator::removeAttInfosDuplicate(QList<CodeGenerator::T_attInfos> attInfosList) {
   QList<CodeGenerator::T_attInfos> noDuplicateAttInfosList = QList<CodeGenerator::T_attInfos>();

   for (CodeGenerator::T_attInfos attInfo : attInfosList) {
      if (!this->findAttInfoId(attInfo.attId, noDuplicateAttInfosList)) {
         noDuplicateAttInfosList.append(attInfo);
      }
   }
   return noDuplicateAttInfosList;
}


void CodeGenerator::fillSubAttData_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    if ((pOut == nullptr) || (attList.size() == 0)) {
        return;
    }
    QStringList nextParentNames = QStringList();
    QString indent = this->getIndent(indentNb);
    QString attDataPath;

    for (Attribute *attribute : attList) {
       if (attribute->getIsOptional()) {
          *pOut << indent << "// Initialize optional attribute flags bitfield" << endl;
          *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield = 0;" << endl;
          break;
       }
    }
    for (Attribute *attribute : attList) {
       // Clear sub-attribute list between attributes
       nextParentNames = parentNames;
       attDataPath = this->getAttDataRxPath(protocolName, parentNames, attribute->getName());
       *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName() << endl;
       if (attribute->getIsOptional()) {
          *pOut << indent << "if (" << attDataPath << " != NULL) {" << endl;
          indent = this->getIndent(indentNb + 1);
          *pOut << indent << "// Note presence of optional attribute " << attribute->getName() << endl;
          *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield |= "
                   << protocolName.toUpper() << "_" << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG;" << endl;
          if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
             nextParentNames.append(attribute->getName());
             this->fillSubAttData_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
          } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
             *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName()  << endl;
             *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower()
                      << " = " << attDataPath << ";" << endl;
          } else {
             *pOut << indent << "// Retrieve data of sub-attribute " << attribute->getName() << endl;
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << "memcpy(&(pCmdPayload->" << this->getPayloadPath(parentNames) << attribute->getName().toLower()
                      << "), " << attDataPath << ", sizeof(" << typeString << "));" << endl;
          }
          indent = this->getIndent(indentNb);
          *pOut << indent << "}" << endl;
       } else {
          if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
             nextParentNames.append(attribute->getName());
             this->fillSubAttData_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
          } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
             *pOut << indent << "pCmdPayload->" << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower()
                      << " = " << attDataPath << ";" << endl;
          } else {
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << "memcpy(&(pCmdPayload->" << this->getPayloadPath(parentNames) << attribute->getName().toLower()
                      << "), " << attDataPath << ", sizeof(" << typeString << "));" << endl;
          }
       }
    }
}

void CodeGenerator::fillSubAttPayload_Rec(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    if ((pOut == nullptr) || (attList.size() == 0)) {
        return;
    }
    QString attDataPath;
    QStringList nextParentNames = QStringList();
    QString indent = this->getIndent(indentNb);

    for (Attribute *attribute : attList) {
        // Clear sub-attribute list between attributes
        nextParentNames = parentNames;
        attDataPath = this->getAttDataTxPath(protocolName, parentNames, attribute->getName());
        *pOut << indent << "// Fill data of sub-attribute " << attribute->getName() << endl;

        if (attribute->getIsOptional()) {
            *pOut << indent << "if ((pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield & " << protocolName.toUpper() << "_"
            << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << endl;
            indent = this->getIndent(indentNb + 1);

            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                *pOut << indent << "// Intermediary variable" << endl;
                *pOut << indent << "pSubAttArray = &(" << attDataPath << "Payload.pSubAttArray);" << endl;
                *pOut << indent << "// Allocate sub-attribute array" << endl;
                *pOut << indent << "if (!FiloGet(LcsfBridge" << protocolName << "Info.pFilo, LCSF_" << protocolName.toUpper() << "_ATT_"
                << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << endl;
                *pOut << indent << "    return false;" << endl;
                *pOut << indent << "}" << endl;
                nextParentNames.append(attribute->getName());
                this->fillSubAttPayload_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
            } else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                *pOut << indent << attDataPath << "PayloadSize = pCmdPayload->"
                << this->getPayloadPath(parentNames) << attribute->getName().toLower() << "Size;" << endl;
                *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
            } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
            } else {
                *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << endl;
            }
            indent = this->getIndent(indentNb);
            *pOut << indent << "} else {" << endl;
            *pOut << indent << "    " << attDataPath << "Payload.pData = NULL;" << endl;
            *pOut << indent << "}" << endl;
        } else {
            if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                *pOut << indent << "// Intermediary variable" << endl;
                *pOut << indent << "pSubAttArray = &(" << attDataPath << "Payload.pSubAttArray);" << endl;
                *pOut << indent << "// Allocate sub-attribute array" << endl;
                *pOut << indent << "if (!FiloGet(LcsfBridge" << protocolName << "Info.pFilo, LCSF_" << protocolName.toUpper() << "_ATT_"
                << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << endl;
                *pOut << indent << "    return false;" << endl;
                *pOut << indent << "}" << endl;
                nextParentNames.append(attribute->getName());
                this->fillSubAttPayload_Rec(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
            } else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                *pOut << indent << attDataPath << "PayloadSize = pCmdPayload->"
                << this->getPayloadPath(parentNames) << attribute->getName().toLower() << "Size;" << endl;
                *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
            } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                *pOut << indent << attDataPath << "Payload.pData = pCmdPayload->"
                << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
            } else {
                *pOut << indent << attDataPath << "Payload.pData = &(pCmdPayload->"
                << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ");" << endl;
            }
        }
    }
}

void CodeGenerator::grabAttValues_REC(QString protocolName, QStringList parentNames, QList<Attribute *> attList, QTextStream *pOut, int indentNb) {
    if ((pOut == nullptr) || (attList.size() == 0)) {
        return;
    }
    QString attPayloadPath;
    QStringList nextParentNames = QStringList();
    QString indent = this->getIndent(indentNb);

    for (Attribute *attribute : attList) {
       // Clear sub-attribute list between attributes
       nextParentNames = parentNames;
       attPayloadPath = this->getPayloadPath(parentNames);

       if (attribute->getIsOptional()) {
          *pOut << indent << "if ((pCmdPayload->" << this->getPayloadPath(parentNames) << "optAttFlagsBitfield & " << protocolName.toUpper() << "_"
                   << parentNames.last().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << endl;
          indent = this->getIndent(indentNb + 1);

          if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
             nextParentNames.append(attribute->getName());
             this->grabAttValues_REC(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb + 1);
             indent = this->getIndent(indentNb);
             *pOut << indent << "}" << endl;
          } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                      << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
             indent = this->getIndent(indentNb);
             *pOut << indent << "}" << endl;
             *pOut << endl;
          } else {
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                      << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << endl;
             indent = this->getIndent(indentNb);
             *pOut << indent << "}" << endl;
          }
       } else {
          if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
             nextParentNames.append(attribute->getName());
             this->grabAttValues_REC(protocolName, nextParentNames, attribute->getSubAttArray(), pOut, indentNb);
          } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                      << this->getPayloadPath(parentNames) << "p_" << attribute->getName().toLower() << ";" << endl;
             *pOut << endl;
          } else {
             QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
             *pOut << indent << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                      << this->getPayloadPath(parentNames) << attribute->getName().toLower() << ";" << endl;
          }
       }
    }
}

QString CodeGenerator::toCamelCase(const QString& s) {
   QStringList parts = s.split('_', QString::SkipEmptyParts);
   for (int i=1; i<parts.size(); ++i) {
      parts[i].replace(0, 1, parts[i][0].toUpper());
   }
   return parts.join("");
}

QString CodeGenerator::toFirstLetterUpperCase(const QString& s) {
   QStringList parts = s.split('_', QString::SkipEmptyParts);
   for (int i=0; i<parts.size(); ++i) {
      parts[i].replace(0, 1, parts[i][0].toUpper());
   }
   return parts.join("");
}

QList<CodeGenerator::T_attInfos> CodeGenerator::removeCommandAttributes(QList<CodeGenerator::T_attInfos> attInfosList, QList<Command *> cmdList) {
   QList<CodeGenerator::T_attInfos> trimmedAttInfosList = QList<CodeGenerator::T_attInfos>();

   for (CodeGenerator::T_attInfos attInfo : attInfosList) {
       // If attribute doesn't have command as parent
      if (Command::findCmdAddr(attInfo.parentName, cmdList) == nullptr) {
         trimmedAttInfosList.append(attInfo);
      }
   }
   return trimmedAttInfosList;
}

int findCmdFctIdxFromName(QString cmdName, QStringList cmdFcts) {
    int idx = 0;
    for ( QString function : cmdFcts ) {
        if (function.contains(cmdName)) {
            return idx;
        }
        idx++;
    }
    return -1;
}


void CodeGenerator::generateMainHeader(QString protocolName, QList<Command *> cmdList, CodeExtractor codeExtract, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/" +protocolName + "_Main.h";
   QString previousParentName;
   QList<CodeGenerator::T_attInfos> attInfosList = this->getAttInfos(cmdList);
   bool hasOptAtt = false;
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&file);
      QFileInfo fileInfo(file);
      out << "/**" << endl;
      out << " * \\file " << protocolName << "_Main.h" << endl;
      out << " * \\brief " << protocolName << " protocol module" << endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << endl;
      out << " *" << endl;
      out << " */" << endl;
      out << endl;
      out << "#ifndef " << protocolName.toLower() << "_main_h" << endl;
      out << "#define " << protocolName.toLower() << "_main_h" << endl;
      out << endl;
      out << "// *** Libraries include ***" << endl;
      out << "// Standard lib" << endl;
      out << "#include <stdbool.h>" << endl;
      out << "#include <stdint.h>" << endl;
      out << "// Custom lib" << endl;
      out << endl;
      out << "// *** Definitions ***" << endl;
      out << "// --- Public Types ---" << endl;
      out << endl;

      // Commands enum
      out << "// Command name enum" << endl;
      out << "enum _" << protocolName.toLower() << "_cmd_names {" << endl;
      for (Command *command : cmdList) {
         out << "    " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "," << endl;
      }
      out << "    " << protocolName.toUpper() << "_CMD_COUNT," << endl;
      out << "};" << endl;
      out << endl;

      // Attributes enums
      QList<CodeGenerator::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
      if (sortedAttInfosList.size() > 0) {
         out << "// Attributes enums" << endl;
         for (int idx = 0; idx < sortedAttInfosList.size(); idx++) {
            CodeGenerator::T_attInfos currentAttInfo = sortedAttInfosList.at(idx);
            out << "enum _" << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_names {" << endl;
            previousParentName = currentAttInfo.parentName;
            out << "    " << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "," << endl;
            if (idx < sortedAttInfosList.size() - 1) {
               currentAttInfo = sortedAttInfosList.at(idx+1);
               while (currentAttInfo.parentName.compare(previousParentName) == 0) {
                  out << "    " << protocolName.toUpper() << "_" << currentAttInfo.parentName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "," << endl;
                  previousParentName = currentAttInfo.parentName;
                  idx++;
                  if (idx < sortedAttInfosList.size() - 1) {
                     currentAttInfo = sortedAttInfosList.at(idx+1);
                  } else {
                     break;
                  }
               }
            }
            out << "};" << endl;
            out << endl;
         }
      }

      // Attributes payload structs
      if (this->protocolHasSubAtt) {
         out << "// Attribute with sub-attributes structures" << endl;
         CodeGenerator::T_attInfos currentAttInfo;
         // Parse the list on reverse to declare sub-attributes before their parent and solve dependencies
         for (int idx = attInfosList.size() - 1; idx >= 0; idx--) {
            hasOptAtt = false;
            currentAttInfo = attInfosList.at(idx);
            if (currentAttInfo.dataType == NS_AttDataType::SUB_ATTRIBUTES) {
               int attCounter = 0;
               for (Attribute *attribute : currentAttInfo.attPointer->getSubAttArray()) {
                  if (attribute->getIsOptional()) {
                     if (!hasOptAtt) {
                         hasOptAtt = true;
                     }
                     out << "#define " << protocolName.toUpper() << "_" << currentAttInfo.attName.toUpper()
                            << "_ATT_" << attribute->getName().toUpper() << "_FLAG (1 << " << attCounter << ")" << endl;
                  }
                  attCounter++;
               }
               out << "typedef struct _" << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_" << currentAttInfo.attName.toLower() << "_att_payload {" << endl;
               if (hasOptAtt) {
                  QString typeString = getFlagTypeStringFromAttNb(currentAttInfo.subAttNb, "optAttFlagsBitfield");
                  out << "    " << typeString << ";" << endl;
               }
               int subIdx = idx + 1;
               int subAttCpt = 0;
               while ((subIdx < attInfosList.size()) || (subAttCpt < currentAttInfo.subAttNb)) {
                  CodeGenerator::T_attInfos currentSubAttInfo = attInfosList.at(subIdx);
                  if (currentSubAttInfo.parentName == currentAttInfo.attName) {
                     // Skip the sub-attributes
                     subAttCpt++;
                     if (currentSubAttInfo.dataType == NS_AttDataType::SUB_ATTRIBUTES) {
                        out << "    " << protocolName.toLower() << "_" << currentSubAttInfo.parentName.toLower() << "_att_" << currentSubAttInfo.attName.toLower() << "_att_payload_t "
                               << currentSubAttInfo.attName.toLower() << "_payload;" << endl;
                     } else if (currentSubAttInfo.dataType == NS_AttDataType::BYTE_ARRAY) {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << "uint32_t " << currentSubAttInfo.attName.toLower() << "Size;" << endl;
                        out << "    " << typeString << "p_" << currentSubAttInfo.attName.toLower() << ";" << endl;
                     } else if (currentSubAttInfo.dataType == NS_AttDataType::STRING) {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << typeString << "p_" << currentSubAttInfo.attName.toLower() << ";" << endl;
                     } else {
                        QString typeString = this->getTypeStringFromDataType(currentSubAttInfo.dataType);
                        out << "    " << typeString << currentSubAttInfo.attName.toLower() << ";" << endl;
                     }
                  }
                  // Next attribute
                  subIdx++;
               }
               out << "} " << protocolName.toLower() << "_" << currentAttInfo.parentName.toLower() << "_att_" << currentAttInfo.attName.toLower() << "_att_payload_t;" << endl;
               out << endl;
            }
         }
      }

      // Commands payload struct
      out << "// Commands data structures" << endl;
      for (Command *command : cmdList) {
         hasOptAtt = false;
         if (command->getAttArray().size() > 0) {
            int attCounter = 0;
            for (Attribute *attribute : command->getAttArray()) {
               if (attribute->getIsOptional()) {
                  if (!hasOptAtt) {
                      hasOptAtt = true;
                  }
                  out << "#define " << protocolName.toUpper() << "_" << command->getName().toUpper()
                         << "_ATT_" << attribute->getName().toUpper() << "_FLAG (1 << " << attCounter << ")" << endl;
               }
               attCounter++;
            }
            out << "typedef struct _" << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload {" << endl;
            if (hasOptAtt) {
               QString typeString = getFlagTypeStringFromAttNb(command->getAttArray().size(), "optAttFlagsBitfield");
               out << "    " << typeString << ";" << endl;
            }
            for (Attribute *attribute : command->getAttArray()) {
               if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                  out << "    " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_" << attribute->getName().toLower() << "_att_payload_t "
                         << attribute->getName().toLower() << "_payload;" << endl;
               } else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << "uint32_t " << attribute->getName().toLower() << "Size;" << endl;
                  out << "    " << typeString << "p_" << attribute->getName().toLower() << ";" << endl;
               } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << typeString << "p_" << attribute->getName().toLower() << ";" << endl;
               } else {
                  QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                  out << "    " << typeString << attribute->getName().toLower() << ";" << endl;
               }
            }
            out << "} " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload_t;" << endl;
            out << endl;
         }
      }
      out << endl;

      // Command payload union
      out << "// Command payload union" << endl;
      out << "typedef union _" << protocolName.toLower() << "_cmd_payload {" << endl;
      for (Command *command : cmdList) {
         if (command->getAttArray().size() > 0) {
            out << "    " << protocolName.toLower() << "_" << command->getName().toLower() << "_att_payload_t "
                   << command->getName().toLower() << "_payload;" << endl;
         }
      }
      out << "} " << protocolName.toLower() << "_cmd_payload_t;" << endl;
      out << endl;

      // Other declarations
      out << "// Module initialization descriptor" << endl;
      out << "typedef struct _" << protocolName.toLower() << "_init_desc {" << endl;
      out << "    uint8_t FooValue;" << endl;
      out << "} " << protocolName.toLower() << "_init_desc_t;" << endl;
      out << endl;

      out << "// --- Public Function Prototypes ---" << endl;
      out << endl;
      if (codeExtract.getExtractionComplete()) {
         out << codeExtract.getUnknownPublicFunctionsHeaders();
      } else {
          out << "/**" << endl;
          out << " * \\fn bool " << protocolName << "_MainInit(const " << protocolName.toLower() << "_init_desc_t *pInitDesc)" << endl;
          out << " * \\brief Initialize the module" << endl;
          out << " *" << endl;
          out << " * \\param pInitDesc pointer to module initialization descriptor" << endl;
          out << " * \\return bool: true if operation was a success" << endl;
          out << " */" << endl;
          out << "bool " << protocolName << "_MainInit(const " << protocolName.toLower() << "_init_desc_t * pDescInit);" << endl;
          out << endl;
      }
      out << "/**" << endl;
      out << " * \\fn bool " << protocolName << "_MainExecute(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Execute a command" << endl;
      out << " *" << endl;
      out << " * \\param cmdName name of the command" << endl;
      out << " * \\param pCmdPayload pointer to command payload " << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "bool " << protocolName << "_MainExecute(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
      out << endl;
      out << "// *** End Definitions ***" << endl;
      out << "#endif // " << protocolName.toLower() << "_Main_h" << endl;
      file.close();
   }
}

void CodeGenerator::generateMain(QString protocolName, QList<Command *> cmdList , CodeExtractor codeExtract, bool isA, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/" + protocolName + "_Main.c";
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&file);
      QFileInfo fileInfo(file);

      if (codeExtract.getExtractionComplete()) {
         QString test = codeExtract.getUnknownIncludes();
         out << test;
      } else {
          out << "/**" << endl;
          out << " * \\file " << protocolName << "_Main.c" << endl;
          out << " * \\brief " << protocolName << " protocol module (" << ((isA) ? "A":"B") << ")" << endl;
          out << " * \\author LCSF Generator v" << APP_VERSION << endl;
          out << " *" << endl;
          out << " */" << endl;
          out << endl;
          out << "// *** Libraries include ***" << endl;
          out << "// Standard lib" << endl;
          out << "#include <stdlib.h>" << endl;
          out << "// Custom lib" << endl;
          out << "#include <MemAlloc.h>" << endl;
          out << "#include <LCSF_Bridge_" << protocolName << ".h>" << endl;
          out << "#include <" << protocolName << "_Main.h>" << endl;
          out << endl;
          out << "// *** Definitions ***" << endl;
      }

      if (codeExtract.getExtractionComplete()) {
         out << codeExtract.getUnknownDefinitions();
      } else {
         out << "// --- Private Types ---" << endl;
         out << "typedef struct _" << protocolName.toLower() << "_info {" << endl;
         out << "    const " << protocolName.toLower() << "_init_desc_t *pInitDesc;" << endl;
         out << "    " << protocolName.toLower() << "_cmd_payload_t *pSendCmdPayload;" << endl;
         out << "} " << protocolName.toLower() << "_info_t;" << endl;
         out << endl;
         out << "// --- Private Constants ---" << endl;
         out << "// --- Private Function Prototypes ---" << endl;
         out << "// Generated functions" << endl;
         out << "static bool " << protocolName << "SendCommand(uint16_t cmdName, bool hasPayload);" << endl;

         for (int idx = 0; idx < cmdList.size(); idx++) {
            Command *command = cmdList.at(idx);

            if (command->isReceivable(isA)) {
               out << "static bool " << protocolName << "Execute" << command->getName() << "(";
               if (command->getAttArray().size() > 0) {
                  out << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
               } else {
                  out << "void);" << endl;
               }
            }
         }
         out << endl;
         out << "// --- Private Variables ---" << endl;
         out << "static " << protocolName.toLower() << "_info_t " << protocolName << "Info;" << endl;
         out << endl;
         out << "// *** End Definitions ***" << endl;
         out << endl;
         out << "// *** Private Functions ***" << endl;
      }
      out << endl;

      if (codeExtract.getExtractionComplete()) {
         out << codeExtract.getUnknownPrivateFunctions();
      } else {
         out << "/**" << endl;
         out << " * \\fn static bool " << protocolName << "_MainSendCommand(uint16_t cmdName, bool hasPayload)" << endl;
         out << " * \\brief Send a command" << endl;
         out << " *=" << endl;
         out << " * \\param cmdName name of the command to send=" << endl;
         out << " * \\param hasPayload indicates if command has a payload or not=" << endl;
         out << " * \\return bool: true if operation was a success=" << endl;
         out << " */" << endl;
         out << "static bool " << protocolName << "SendCommand(uint16_t cmdName, bool hasPayload) {" << endl;
         out << endl;
         out << "    if (cmdName >= " << protocolName.toUpper() << "_CMD_COUNT) {" << endl;
         out << "        return false;" << endl;
         out << "    }" << endl;
         out << "    if (hasPayload) {" << endl;
         out << "        " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload = " << protocolName << "Info.pSendCmdPayload;" << endl;
         out << "        return LCSF_Bridge_" << protocolName << "Send(cmdName, pCmdPayload);" << endl;
         out << "    } else {" << endl;
         out << "        return LCSF_Bridge_" << protocolName << "Send(cmdName, NULL);" << endl;
         out << "    }" << endl;
         out << "}" << endl;
         out << endl;
         out << "/**" << endl;
         out << " * \\fn static bool " << protocolName << "ExecuteX(void)" << endl;
         out << " * \\brief Execute command X (no payload)" << endl;
         out << " *" << endl;
         out << " * \\return bool: true if operation was a success" << endl;
         out << " */" << endl;
         out << endl;
         out << "/**" << endl;
         out << " * \\fn static bool " << protocolName << "ExecuteX(" << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
         out << " * \\brief Execute command X (with payload)" << endl;
         out << " *" << endl;
         out << " * \\param pCmdPayload pointer to the command payload" << endl;
         out << " * \\return bool: true if operation was a success" << endl;
         out << " */" << endl;
         out << endl;
      }

      for (int idx = 0; idx < cmdList.size(); idx++) {
         QStringList commandFunctionsList;
         QString commandFunction;
         Command *command = cmdList.at(idx);

         if (codeExtract.getExtractionComplete()) {
            commandFunctionsList = codeExtract.getCommandFunctions();
            int cmdIdx = findCmdFctIdxFromName(command->getName(), commandFunctionsList);
            if (cmdIdx >= 0) {
                commandFunction = commandFunctionsList.at(cmdIdx);
            }
         }
         if (command->isReceivable(isA)) {
            if ((codeExtract.getExtractionComplete()) && (commandFunction.compare("") != 0)) {
               out << commandFunction;
            } else {
               out << "static bool " << protocolName << "Execute" << command->getName() << "(";
               if (command->getAttArray().size() > 0) {
                  out << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
                  out << "    if (pCmdPayload == NULL) {" << endl;
                  out << "       return false;" << endl;
                  out << "    }" << endl;

                  for (Attribute *attribute : command->getAttArray()) {
                     if (attribute->getIsOptional()) {
                        out << "    if ((pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield & " << protocolName.toUpper() << "_"
                               << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << endl;

                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                           QStringList parentNames = { command->getName(), attribute->getName() };
                           this->grabAttValues_REC(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                        } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "        " << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                        } else {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "        " << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ";" << endl;
                        }
                        out << "    }" << endl;
                     } else {
                        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                           QStringList parentNames = { command->getName(), attribute->getName() };
                           this->grabAttValues_REC(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                        } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "    " << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                        } else {
                           QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                           out << "    " << typeString << "m_" << attribute->getName().toLower() << " = pCmdPayload->"
                                  << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ";" << endl;
                        }
                     }
                  }
                  out << "    #pragma GCC warning \"#warning TODO: function to implement\"" << endl;
               } else {
                  out << "void) {" << endl;
                  out << "    #pragma GCC warning \"#warning TODO: function to implement\"" << endl;
               }
               out << "    return true;" << endl;
               out << "}" << endl;
               out << endl;
            }
         }
      }

      out << "// *** Public Functions ***" << endl;
      out << endl;

      if (codeExtract.getExtractionComplete()) {
         out << codeExtract.getUnknownPublicFunctions();
      } else {
         out << "/**" << endl;
         out << " * \\fn bool " << protocolName << "_MainInit(const " << protocolName.toLower() << "_init_desc_t *pInitDesc)" << endl;
         out << " * \\brief Initialize the module" << endl;
         out << " *" << endl;
         out << " * \\param pInitDesc pointer to module initialization descriptor" << endl;
         out << " * \\return bool: true if operation was a success" << endl;
         out << " */" << endl;
         out << "bool " << protocolName << "_MainInit(const " << protocolName.toLower() << "_init_desc_t *pDescInit) {" << endl;
         out << "    " << protocolName << "Info.pInitDesc = pDescInit;" << endl;
         out << "    " << protocolName << "Info.pSendCmdPayload = ("<< protocolName.toLower() << "_cmd_payload_t *)MemAllocCalloc(sizeof("<< protocolName.toLower() << "_cmd_payload_t));" << endl;
         out << "    return true;" << endl;
         out << "}" << endl;
         out << endl;
      }

      out << "/**" << endl;
      out << " * \\fn bool " << protocolName << "_MainExecute(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Execute a command" << endl;
      out << " *" << endl;
      out << " * \\param cmdName name of the command" << endl;
      out << " * \\param pCmdPayload pointer to command payload" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "bool " << protocolName << "_MainExecute(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
      out << "    switch (cmdName) {" << endl;
      for (Command *command : cmdList) {
         if (command->isReceivable(isA)) {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << endl;
            out << "            return " << protocolName << "Execute" << command->getName();
            if (command->getAttArray().size() > 0) {
                out<< "(pCmdPayload);" << endl;
            } else {
                out<< "();" << endl;
            }
            out << "        break;" << endl;
            out << endl;
         }
      }
      out << "        default:" << endl;

      QString defaultCommandHandler = codeExtract.getDefaultCommandHandler();

      if ((codeExtract.getExtractionComplete()) && (defaultCommandHandler.compare("") != 0)) {
        out << defaultCommandHandler;
      } else {
        out << "            return false;" << endl;
      }
      out << "        break;" << endl;
      out << "    }" << endl;
      out << "}" << endl;

      file.close();
   }
}

void CodeGenerator::generateBridgeHeader(QString protocolName, QString protocolId, QList<Command *> cmdList, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Bridge_" + protocolName + ".h";
   QList<CodeGenerator::T_attInfos> attIdxList = this->getAttInfos(cmdList);
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << endl;
      out << " * \\file LCSF_Bridge_" << protocolName << ".h" << endl;
      out << " * \\brief " << protocolName << " LCSF bridge module" << endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << endl;
      out << " *" << endl;
      out << " */" << endl;
      out << endl;
      out << "#ifndef Lcsf_bridge_" << protocolName.toLower() << "_h" << endl;
      out << "#define Lcsf_bridge_" << protocolName.toLower() << "_h" << endl;
      out << endl;
      out << "// *** Libraries include ***" << endl;
      out << "// Standard lib" << endl;
      out << "#include <stdbool.h>" << endl;
      out << "#include <stdint.h>" << endl;
      out << "// Custom lib" << endl;
      out << "#include <LCSF_Validator.h>" << endl;
      out << "#include <" << protocolName << "_Main.h>" << endl;
      out << endl;
      out << "// *** Definitions ***" << endl;
      out << "// --- Public Types ---" << endl;
      out << endl;
      out << "// Lcsf protocol identifier" << endl;
      out << "#define LCSF_" << protocolName.toUpper() << "_PROTOCOL_ID 0x" << protocolId.toInt(nullptr, 16) << endl;
      out << endl;

      // Commands Ids enum
      out << "// Command identifier enum" << endl;
      out << "enum _lcsf_" << protocolName.toLower() << "_cmd_id {" << endl;
      for (Command *command : cmdList) {
         out << "    LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << " = 0x" << QString::number(command->getId(), 16) << "," << endl;
      }
      out << "};" << endl;
      out << endl;

      // Attributes Ids enum
      if (attIdxList.size() > 0) {
         out << "// Attribute identifier enum" << endl;
         out << "enum _lcsf_" << protocolName.toLower() << "_att_id {" << endl;
         for (CodeGenerator::T_attInfos currentAttInfo : attIdxList) {
            out << "    LCSF_" << protocolName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << " = 0x"
                   << QString::number(currentAttInfo.attId, 16) << "," << endl;
         }
         out << "};" << endl;
         out << endl;
      }
      out << "// --- Public Constants ---" << endl;
      out << endl;
      out << "// Command number" << endl;
      out << "#define LCSF_" << protocolName.toUpper() << "_CMD_NB " << protocolName.toUpper() << "_CMD_COUNT" << endl;
      if (attIdxList.size() > 0) {
         out << "// Command attribute number" << endl;
         for (Command *command : cmdList) {
            if (command->getAttArray().size() > 0) {
               out << "#define LCSF_" << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB " << command->getAttArray().size() << endl;
            }
         }
         if (this->protocolHasSubAtt) {
            out << "// Attribute sub-attributes number" << endl;
            for (CodeGenerator::T_attInfos currentAttInfo : attIdxList) {
               if (currentAttInfo.subAttNb > 0) {
                  out << "#define LCSF_" << protocolName.toUpper() << "_ATT_" << currentAttInfo.attName.toUpper() << "_SUBATT_NB "
                         << currentAttInfo.subAttNb << endl;
               }
            }
         }
      }
      out << endl;
      out << "// --- Public Function Prototypes ---" << endl;
      out << endl;
      out << "/**" << endl;
      out << " * \\fn bool LCSF_Bridge_" << protocolName << "Init(uint16_t filoSize)" << endl;
      out << " * \\brief Initialize the module" << endl;
      out << " *" << endl;
      out << " * \\param filoSize size of the module filo (number of element)" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "bool LCSF_Bridge_" << protocolName << "Init(uint16_t filoSize);" << endl;
      out << endl;
      out << "/**" << endl;
      out << " * \\fn bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd)" << endl;
      out << " * \\brief Receive valid command from LCSF_Validator and transmit to " << protocolName << "_Main" << endl;
      out << " *" << endl;
      out << " * \\param pValidCmd pointer to the valid command" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd);" << endl;
      out << endl;
      out << "/**" << endl;
      out << " * \\fn bool LCSF_Bridge_" << protocolName << "Send(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Receive command from " << protocolName << "_Main and transmit to LCSF_Validator" << endl;
      out << " *" << endl;
      out << " * \\param cmdName name of the command" << endl;
      out << " * \\param pValidCmd pointer to the valid command" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "bool LCSF_Bridge_" << protocolName << "Send(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
      out << endl;
      out << "// *** End Definitions ***" << endl;
      out << "#endif // Lcsf_bridge_" << protocolName.toLower() << "_h" << endl;

      file.close();
   }
}

void CodeGenerator::generateBridge(QString protocolName, QList<Command *> cmdList, bool isA, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Bridge_" + protocolName + ".c";
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << endl;
      out << " * \\file LCSF_Bridge_" << protocolName << ".c" << endl;
      out << " * \\brief " << protocolName << " LCSF bridge module (" << ((isA) ? "A":"B") << ")" << endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << endl;
      out << " *" << endl;
      out << " */" << endl;
      out << endl;
      out << "// *** Libraries include ***" << endl;
      out << "// Standard lib" << endl;
      out << "#include <string.h>" << endl;
      out << "// Custom lib" << endl;
      out << "#include <MemAlloc.h>" << endl;
      out << "#include <Filo.h>" << endl;
      out << "#include <LCSF_Transcoder.h>" << endl;
      out << "#include <LCSF_Validator.h>" << endl;
      out << "#include <LCSF_Bridge_" << protocolName << ".h>" << endl;
      out << endl;
      out << "// *** Definitions ***" << endl;
      out << "// --- Private Types ---" << endl;
      out << endl;
      out << "// Module information structure" << endl;
      out << "typedef struct _lcsf_bridge_" << protocolName.toLower() << "_info {" << endl;
      out << "    filo_desc_t *pFilo;" << endl;
      out << "    " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload;" << endl;
      out << "} lcsf_bridge_" << protocolName.toLower() << "_info_t;" << endl;
      out << endl;
      out << "// --- Private Constants ---" << endl;
      out << "// Array to convert command name value to their lcsf command id" << endl;
      out << "const uint16_t LCSF_Bridge_" << protocolName << "_CMDNAME2CMDID[LCSF_" << protocolName.toUpper() << "_CMD_NB] = {" << endl;
      for (Command *command : cmdList) {
         out << "    LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << "," << endl;
      }
      out << "};" << endl;
      out << endl;
      out << "// --- Private Function Prototypes ---" << endl;
      out << "static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint16_t cmdId);" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
            out << "static void LCSF_Bridge_" << protocolName << command->getName() << "GetData(lcsf_valid_att_t *pAttArray, "
                   << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
         }
      }
      out << "static void LCSF_Bridge_" << protocolName << "GetCmdData(uint16_t cmdName, lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
            out << "static bool LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(lcsf_valid_att_t **pAttArrayAddr, "
                   << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
         }
      }
      out << "static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload);" << endl;
      out << endl;
      out << "// --- Private Variables ---" << endl;
      out << "static lcsf_bridge_" << protocolName.toLower() << "_info_t LcsfBridge" << protocolName << "Info;" << endl;
      out << endl;
      out << "// *** End Definitions ***" << endl;
      out << endl;
      out << "// *** Private Functions ***" << endl;
      out << endl;
      out << "/**" << endl;
      out << " * \\fn static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint16_t cmdId)" << endl;
      out << " * \\brief Translate an lcsf command id to its name value" << endl;
      out << " *" << endl;
      out << " * \\param cmdId lcsf command identifier to translate" << endl;
      out << " * \\return uint16_t: name value of the command" << endl;
      out << " */" << endl;
      out << "static uint16_t LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(uint16_t cmdId) {" << endl;
      out << "    switch (cmdId) {" << endl;
      out << "        default:" << endl;
      for (Command *command : cmdList) {
         out << "        case LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << ":" << endl;
         out << "            return " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ";" << endl;
      }
      out << "    }" << endl;
      out << "}" << endl;
      out << endl;

      // Command get data functions
      out << "/**" << endl;
      out << " * \\fn static void LCSF_Bridge_" << protocolName << "XGetData(lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Retrieve data of command X from its valid attribute array and store it in a payload" << endl;
      out << " *" << endl;
      out << " * \\param pAttArray pointer to the command attribute array" << endl;
      out << " * \\param pCmdPayload pointer to the payload to contain the command data" << endl;
      out << " * \\return void" << endl;
      out << " */" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
            out << "static void LCSF_Bridge_" << protocolName << command->getName() << "GetData(lcsf_valid_att_t *pAttArray, "
                   << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
            out << "    if (pCmdPayload == NULL) {" << endl;
            out << "        return;" << endl;
            out << "    }" << endl;
            for (Attribute *attribute : command->getAttArray()) {
               if (attribute->getIsOptional()) {
                  out << "    // Initialize optional attribute flags bitfield" << endl;
                  out << "    pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield = 0;" << endl;
                  break;
               }
            }
            for (Attribute *attribute : command->getAttArray()) {
               out << "    // Retrieve data of attribute " << attribute->getName() << endl;
               if (attribute->getIsOptional()) {
                  out << "    if (pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                         << attribute->getName().toUpper() << "].Payload.pData != NULL) {" << endl;
                  out << "        // Note presence of optional attribute" << endl;
                  out << "        pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield |= "
                         << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG;" << endl;
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                     QStringList parentNames = { command->getName(), attribute->getName() };
                     this->fillSubAttData_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                  } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
                     out << "        pCmdPayload->" << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << " = pAttArray["
                            << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData;" << endl;
                  } else {
                     QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                     out << "        memcpy(&(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "), pAttArray["
                            << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData, sizeof(" << typeString << "));" << endl;
                  }
                  out << "    }" << endl;
               } else {
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                     QStringList parentNames = { command->getName(), attribute->getName() };
                     this->fillSubAttData_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                  } else if ((attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) || (attribute->getDataType() == NS_AttDataType::STRING)) {
                     out << "    pCmdPayload->" << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << " = pAttArray["
                            << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData;" << endl;
                  } else {
                     QString typeString = this->getTypeStringFromDataType(attribute->getDataType());
                     out << "    memcpy(&(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "), pAttArray["
                            << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData, sizeof(" << typeString << "));" << endl;
                  }
               }
            }
            out << "}" << endl;
            out << endl;
         }
      }

      // Master get data function
      out << "/**" << endl;
      out << " * \\fn static void LCSF_Bridge_" << protocolName << "GetCmdData(uint16_t cmdName, lcsf_valid_att_t *pAttArray, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Retrieve command data from its attribute array and store it in a payload" << endl;
      out << " *" << endl;
      out << " * \\param cmdName name of the command" << endl;
      out << " * \\param pAttArray pointer to the command attribute array" << endl;
      out << " * \\param pPayload pointer to the payload to contain the command data" << endl;
      out << " * \\return void" << endl;
      out << " */" << endl;
      out << "static void LCSF_Bridge_" << protocolName << "GetCmdData(uint16_t cmdName, lcsf_valid_att_t *pAttArray, "
             << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
      out << "    if (pAttArray == NULL) {" << endl;
      out << "	    return;" << endl;
      out << "    }" << endl;
      out << "    switch (cmdName) {" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isReceivable(isA))) {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << endl;
            out << "            LCSF_Bridge_" << protocolName << command->getName() << "GetData(pAttArray, pCmdPayload);" << endl;
            out << "        break;" << endl;
            out << endl;
         }
      }
      out << "        default: // Commands that don't have payload" << endl;
      out << "            return;" << endl;
      out << "        break;" << endl;
      out << "    }" << endl;
      out << "}" << endl;
      out << endl;

      // Command fill attribute functions
      out << "/**" << endl;
      out << " * \\fn static bool LCSF_Bridge_" << protocolName << "XFillAtt(lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Allocate and fill attribute array of command X from its payload" << endl;
      out << " *" << endl;
      out << " * \\param pAttArrayAddr pointer to contain the attribute array" << endl;
      out << " * \\param pCmdPayload pointer to the command payload" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA))) {
            out << "static bool LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(lcsf_valid_att_t **pAttArrayAddr, "
                   << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
            out << "    if (pCmdPayload == NULL) {" << endl;
            out << "        return false;" << endl;
            out << "    }" << endl;
            out << "    // Allocate attribute array" << endl;
            out << "    if (!FiloGet(LcsfBridge" << protocolName << "Info.pFilo, LCSF_" << protocolName.toUpper() << "_CMD_"
                   << command->getName().toUpper() << "_ATT_NB, (void *)pAttArrayAddr)) {" << endl;
            out << "        return false;" << endl;
            out << "    }" << endl;
            out << "    // Intermediary variable" << endl;
            out << "    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;" << endl;
            for (Attribute *attribute : command->getAttArray()) {
               out << "    // Fill data of attribute " << attribute->getName() << endl;
               if (attribute->getIsOptional()) {
                  out << "    if ((pCmdPayload->" << command->getName().toLower() << "_payload.optAttFlagsBitfield & " << protocolName.toUpper() << "_"
                         << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "_FLAG) != 0) {" << endl;
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                     out << "        // Intermediary variable" << endl;
                     out << "        lcsf_valid_att_t **pSubAttArray = &(pAttArray[" << protocolName.toUpper() << "_"
                            << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "].Payload.pSubAttArray);" << endl;
                     out << "        // Allocate sub-attribute array" << endl;
                     out << "        if (!FiloGet(LcsfBridge" << protocolName << "Info.pFilo, LCSF_" << protocolName.toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << endl;
                     out << "            return false;" << endl;
                     out << "        }" << endl;
                     QStringList parentNames = { command->getName(), attribute->getName() };
                     this->fillSubAttPayload_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 2);
                  } else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].PayloadSize = pCmdPayload->"
                            << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "Size;" << endl;
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                            << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                  } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                            << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                  } else {
                     out << "        pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << endl;
                  }
                  out << "    }" << endl;
               } else {
                  if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
                      out << "    // Intermediary variable" << endl;
                      out << "    lcsf_valid_att_t **pSubAttArray = &(pAttArray[" << protocolName.toUpper() << "_"
                             << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper() << "].Payload.pSubAttArray);" << endl;
                      out << "    // Allocate sub-attribute array" << endl;
                      out << "    if (!FiloGet(LcsfBridge" << protocolName << "Info.pFilo, LCSF_" << protocolName.toUpper() << "_CMD_"
                             << attribute->getName().toUpper() << "_SUBATT_NB, (void *)pSubAttArray)) {" << endl;
                      out << "        return false;" << endl;
                      out << "    }" << endl;
                      QStringList parentNames = { command->getName(), attribute->getName() };
                      this->fillSubAttPayload_Rec(protocolName, parentNames, attribute->getSubAttArray(), &out, 1);
                  } else if (attribute->getDataType() == NS_AttDataType::BYTE_ARRAY) {
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].PayloadSize = pCmdPayload->"
                            << command->getName().toLower() << "_payload." << attribute->getName().toLower() << "Size;" << endl;
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                            << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                  } else if (attribute->getDataType() == NS_AttDataType::STRING) {
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_"
                            << attribute->getName().toUpper() << "].Payload.pData = pCmdPayload->"
                            << command->getName().toLower() << "_payload.p_" << attribute->getName().toLower() << ";" << endl;
                  } else {
                     out << "    pAttArray[" << protocolName.toUpper() << "_" << command->getName().toUpper() << "_ATT_" << attribute->getName().toUpper()
                            << "].Payload.pData = &(pCmdPayload->" << command->getName().toLower() << "_payload." << attribute->getName().toLower() << ");" << endl;
                  }
               }
            }
            out << "    return true;" << endl;
            out << "}" << endl;
            out << endl;
         }
      }

      // Master fill attribute function
      out << "/**" << endl;
      out << " * \\fn static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload)" << endl;
      out << " * \\brief Fill the attribute array of a command from its payload " << endl;
      out << " *" << endl;
      out << " * \\param cmdName name of the command" << endl;
      out << " * \\param pAttArrayAddr pointer to contain the attribute array" << endl;
      out << " * \\param pCmdPayload pointer to the command payload" << endl;
      out << " * \\return bool: true if operation was a success" << endl;
      out << " */" << endl;
      out << "static bool LCSF_Bridge_" << protocolName << "FillCmdAtt(uint16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
      out << "    switch (cmdName) {" << endl;
      for (Command *command : cmdList) {
         if ((command->getAttArray().size() > 0) && (command->isTransmittable(isA))) {
            out << "        case " << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << ":" << endl;
            out << "            return LCSF_Bridge_" << protocolName << command->getName() << "FillAtt(pAttArrayAddr, pCmdPayload);" << endl;
            out << "        break;" << endl;
            out << endl;
         }
      }
      out << "        default: // Commands that don't have attributes" << endl;
      out << "            *pAttArrayAddr = NULL;" << endl;
      out << "            return true;" << endl;
      out << "        break;" << endl;
      out << "    }" << endl;
      out << "}" << endl;
      out << endl;

      // Public functions

      out << "// *** Public Functions ***" << endl;
      out << endl;

      out << "bool LCSF_Bridge_" <<  protocolName << "Init(uint16_t filoSize) {" << endl;
      out << "    LcsfBridge" << protocolName << "Info.pFilo = FiloCreate(filoSize, sizeof(lcsf_valid_att_t));" << endl;
      out << "    LcsfBridge" << protocolName << "Info.pCmdPayload = (" << protocolName.toLower() << "_cmd_payload_t *)MemAllocCalloc(sizeof(" << protocolName.toLower() << "_cmd_payload_t));" << endl;
      out << "    return true;" << endl;
      out << "}" << endl;
      out << endl;

      out << "bool LCSF_Bridge_" << protocolName << "Receive(lcsf_valid_cmd_t *pValidCmd) {" << endl;
      out << "    uint16_t cmdName = LCSF_Bridge_" << protocolName << "_CMDID2CMDNAME(pValidCmd->CmdId);" << endl;
      out << "    " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload = LcsfBridge" << protocolName << "Info.pCmdPayload;" << endl;
      out << "    memset(pCmdPayload, 0, sizeof(" << protocolName.toLower() << "_cmd_payload_t));" << endl;
      out << endl;
      out << "    LCSF_Bridge_" << protocolName << "GetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);" << endl;
      out << "    return " << protocolName << "_MainExecute(cmdName, pCmdPayload);" << endl;
      out << "}" << endl;
      out << endl;

      out << "bool LCSF_Bridge_" << protocolName << "Send(uint16_t cmdName, " << protocolName.toLower() << "_cmd_payload_t *pCmdPayload) {" << endl;
      out << "    lcsf_valid_cmd_t sendCmd;" << endl;
      out << "    sendCmd.CmdId = LCSF_Bridge_" << protocolName << "_CMDNAME2CMDID[cmdName];" << endl;
      out << "    FiloFreeAll(LcsfBridge" << protocolName << "Info.pFilo);" << endl;
      out << endl;
      out << "    if (!LCSF_Bridge_" << protocolName << "FillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {" << endl;
      out << "        return false;" << endl;
      out << "    }" << endl;
      out << "    return LCSF_ValidatorSend(LCSF_" << protocolName.toUpper() << "_PROTOCOL_ID, &sendCmd);" << endl;
      out << "}" << endl;

      file.close();
   }
}

void CodeGenerator::generateDescription(QString protocolName, QList<Command *> cmdList, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_Desc_" + protocolName + ".h";
   QList<CodeGenerator::T_attInfos> attInfosList = this->getAttInfos(cmdList);
   QFile file(fileName);

   if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&file);
      out.setCodec("ISO 8859-1");

      QFileInfo fileInfo(file);
      out << "/**" << endl;
      out << " * \\file LCSF_Desc_" << protocolName << ".h" << endl;
      out << " * \\brief " << protocolName << " LCSF descriptor" << endl;
      out << " * \\author LCSF Generator v" << APP_VERSION << endl;
      out << " *" << endl;
      out << " */" << endl;
      out << endl;
      out << "#ifndef Lcsf_desc_" << protocolName.toLower() << "_h" << endl;
      out << "#define Lcsf_desc_" << protocolName.toLower() << "_h" << endl;
      out << endl;
      out << "// *** Libraries include ***" << endl;
      out << "// Standard lib" << endl;
      out << "#include <stdbool.h>" << endl;
      out << "#include <stdint.h>" << endl;
      out << "// Custom lib" << endl;
      out << "#include <LCSF_Bridge_" << protocolName << ".h>" << endl;
      out << endl;
      out << "// *** Definitions ***" << endl;
      out << "// --- Public Constants ---" << endl;
      out << endl;

      // Sub-attribute descriptor arrays
      if (this->protocolHasSubAtt) {
         for (int idx = attInfosList.size() - 1; idx >= 0; idx--) {
            CodeGenerator::T_attInfos attInfo = attInfosList.at(idx);
            if (attInfo.subAttNb > 0) {
               out << "// Sub-attribute array descriptor of attribute " << attInfo.attName << endl;
               out << "const lcsf_attribute_desc_t LCSF_" << protocolName << "_" << attInfo.attName << "_AttDescArray[LCSF_"
                      << protocolName.toUpper() + "_ATT_" + attInfo.attName.toUpper() + "_SUBATT_NB] = {" << endl;
               for (Attribute *attribute : attInfo.attPointer->getSubAttArray()) {
                  out << "    { " << this->getAttDescString(protocolName, attribute) << " }," << endl;
               }
               out << "};" << endl;
               out << endl;
            }
         }
      }

      // Attribute descriptor array s
      for (Command *command : cmdList) {
         if (command->getAttArray().size() > 0) {
            out << "// Attribute array descriptor of command " << command->getName() << endl;
            out << "const lcsf_attribute_desc_t LCSF_" << protocolName << "_" << command->getName() << "_AttDescArray[LCSF_"
                   << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB] = {" << endl;
            for (Attribute *attribute : command->getAttArray()) {
               out << "    { " << this->getAttDescString(protocolName, attribute) << " }," << endl;
            }
            out << "};" << endl;
            out << endl;
         }
      }

      // Command descriptor array
      out << "// Command array descriptor" << endl;
      out << "const lcsf_command_desc_t LCSF_" << protocolName << "_CmdDescArray[LCSF_" << protocolName.toUpper() << "_CMD_NB] = {" << endl;
      for (Command *command : cmdList) {
         if (command->getAttArray().size() > 0) {
            out << "    { LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper() << ", LCSF_"
                   << protocolName.toUpper() << "_CMD_" << command->getName().toUpper() << "_ATT_NB" << ", LCSF_"
                   << protocolName << "_" << command->getName() << "_AttDescArray" << " }," << endl;
         } else {
            out << "    { LCSF_" << protocolName.toUpper() << "_CMD_ID_" << command->getName().toUpper()
                   << ", 0, NULL }," << endl;
         }
      }
      out << "};" << endl;
      out << endl;

      // Protocol descriptor
      out << "// Protocol descriptor" << endl;
      out << "const lcsf_protocol_desc_t LCSF_" << protocolName << "_ProtDesc = { .CmdNb = LCSF_"
             << protocolName.toUpper() << "_CMD_NB, .pCmdDescArray = LCSF_" << protocolName << "_CmdDescArray };" << endl;
      out << endl;

      out << "// *** End Definitions ***" << endl;
      out << "#endif // Lcsf_desc_" << protocolName.toLower() << "_h" << endl;
      file.close();
   }
}

void CodeGenerator::generateWikiTable(QString protocolName, QList<Command *> cmdList, QString dirPath) {
   QDir dir(dirPath);
   if (!dir.exists()) {
       dir.mkpath(".");
   }
   QString fileName = dirPath + "/LCSF_" + protocolName + "_WikiTables.txt";
   qDebug() << "File selected: " << fileName;
   QFile saveFile(fileName);
   QFileInfo fileInfo(saveFile);

   if (saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
      QTextStream out(&saveFile);

      // Main table
      out << "=== " << protocolName << " protocol tables ===" << endl;
      out << endl;
      out << "==== Commands table ====" << endl;
      out << endl;
      out << "{| class=\"wikitable sortable\"" << endl;
      out << "|-" << endl;
      out << "! Name !! Id !! Direction !! Description !! Attribute(s) Name !! Attribute(s) Id !! Optional ? !! Data type !! Attribute Desc" << endl;
      out << "|-" << endl;

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
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << "''' || " << attribute->getDesc() << endl;
                  out << "|-" << endl;
               } else {
                  out << "| " << attribute->getName() << " || '''" <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || "
                      << ((attribute->getIsOptional()) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()] << "''' || " << attribute->getDesc() << endl;
                  out << "|-" << endl;
               }
            }
         } else {
            out << "| " << command->getName() << " || '''" << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x") << "''' || "
                      <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << " || " << command->getDesc() << " || || || || || " << endl;
            out << "|-" << endl;
         }
      }
      out << "|}" << endl;
      out << endl;

      // Secondary tables
      QList<CodeGenerator::T_attInfos> attInfosList = this->getAttInfos(cmdList);
      QList<CodeGenerator::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
      QList<CodeGenerator::T_attInfos> trimmedAttInfosList = this->removeCommandAttributes(sortedAttInfosList, cmdList);

      for (int idx = 0; idx < trimmedAttInfosList.size(); idx++) {
         CodeGenerator::T_attInfos currentAttInfo = trimmedAttInfosList.at(idx);
         QString previousParentName = currentAttInfo.parentName;

         out << "==== " << previousParentName << " sub-attributes table ====" << endl;
         out << endl;
         out << "{| class=\"wikitable sortable\"" << endl;
         out << "|-" << endl;
         out << "! Name !! Id !! Optional ? !! Data type !! Description" << endl;
         out << "|-" << endl;
         out << "| " << currentAttInfo.attName << " || '''" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "''' || "
             << ((currentAttInfo.isOptional) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "''' || " << currentAttInfo.attDesc << endl;
         out << "|-" << endl;

         if (idx < trimmedAttInfosList.size() - 1) {
            currentAttInfo = trimmedAttInfosList.at(idx+1);

            while (currentAttInfo.parentName.compare(previousParentName) == 0) {
               out << "| " << currentAttInfo.attName << " || '''" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "''' || "
                   << ((currentAttInfo.isOptional) ? "Yes" : "No") << " || '''" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "''' || " << currentAttInfo.attDesc << endl;
               out << "|-" << endl;
               previousParentName = currentAttInfo.parentName;
               idx++;

               if (idx < trimmedAttInfosList.size() - 1) {
                  currentAttInfo = trimmedAttInfosList.at(idx+1);
               } else {
                  break;
               }
            }
         }
         out << "|}" << endl;
         out << endl;
      }
      saveFile.close();
   }
}

void CodeGenerator::generateMkdownTable(QString protocolName, QList<Command *> cmdList, QString dirPath) {
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString fileName = dirPath + "/LCSF_" + protocolName + "_MkdownTables.md";
    qDebug() << "File selected: " << fileName;
    QFile saveFile(fileName);
    QFileInfo fileInfo(saveFile);

    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(&saveFile);

        // Main table
        out << "# " << protocolName << " protocol tables" << endl;
        out << endl;
        out << "## Commands table" << endl;
        out << endl;
        out << "| Name | Id | Direction | Description | Attributes ? |" << endl;
        out << "|:----:|:--:|:---------:|:-----------:|:------------:|" << endl;
        for (Command *command : cmdList) {
            out << "| " << command->getName() << " | `" << QString::number(command->getId(), 16).rightJustified(2, '0').prepend("0x")
                << "` | `" <<  NS_DirectionType::SL_DirectionType[command->getDirection()] << "` | " << command->getDesc() << " | "
                << ((command->getHasAtt() && (command->getAttArray().size() > 0)) ? "Yes" : "No") << " |" << endl;
        }
        out << endl;
        // Command tables
        for (Command *command : cmdList) {
            if (!command->getHasAtt() || (command->getAttArray().size() == 0)) {
                continue;
            }
            out << "## " << command->getName() << " attributes table" << endl;
            out << "| Name | Id | Optional ? | Data type | Attribute Description |" << endl;
            out << "|:----:|:--:|:----------:|:---------:|:---------------------:|" << endl;
            for (Attribute *attribute : command->getAttArray()) {
                out << "| " << attribute->getName() << " | `" <<  QString::number(attribute->getId(), 16).rightJustified(2, '0').prepend("0x")
                    << "` | " << ((attribute->getIsOptional()) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[attribute->getDataType()]
                    << "` | " << attribute->getDesc() << " |" << endl;
            }
        }
        // Attribute tables
        QList<CodeGenerator::T_attInfos> attInfosList = this->getAttInfos(cmdList);
        QList<CodeGenerator::T_attInfos> sortedAttInfosList = this->insertSortAttInfosListByParentName(attInfosList);
        QList<CodeGenerator::T_attInfos> trimmedAttInfosList = this->removeCommandAttributes(sortedAttInfosList, cmdList);

        for (int idx = 0; idx < trimmedAttInfosList.size(); idx++) {
            CodeGenerator::T_attInfos currentAttInfo = trimmedAttInfosList.at(idx);
            QString previousParentName = currentAttInfo.parentName;

            out << "## " << previousParentName << " sub-attributes table" << endl;
            out << endl;
            out << "| Name | Id | Optional ? | Data type | Description |" << endl;
            out << "|:----:|:--:|:----------:|:---------:|:-----------:|" << endl;
            out << "| " << currentAttInfo.attName << " | `" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "` | "
            << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "` | " << currentAttInfo.attDesc << endl;

            if (idx < trimmedAttInfosList.size() - 1) {
                currentAttInfo = trimmedAttInfosList.at(idx+1);

                while (currentAttInfo.parentName.compare(previousParentName) == 0) {
                    out << "| " << currentAttInfo.attName << " | `" <<  QString::number(currentAttInfo.attId, 16).rightJustified(2, '0').prepend("0x") << "` | "
                    << ((currentAttInfo.isOptional) ? "Yes" : "No") << " | `" << NS_AttDataType::SL_DocAttDataType[currentAttInfo.dataType] << "` | " << currentAttInfo.attDesc << endl;
                    previousParentName = currentAttInfo.parentName;
                    idx++;

                    if (idx < trimmedAttInfosList.size() - 1) {
                        currentAttInfo = trimmedAttInfosList.at(idx+1);
                    } else {
                        break;
                    }
                }
            }
            out << endl;
        }
        saveFile.close();
    }
}
