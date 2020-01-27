#include "command.h"

Command::Command(void) {
    this->m_name = "default";
    this->m_id = 0;
    this->m_hasAttributes = false;
    this->m_attArray = QList<Attribute *>();
    this->m_desc = QString();
}

Command::Command(QString cmdName, short cmdCode, bool cmdHasAtt , NS_DirectionType::T_DirectionType cmdDirection, QString commandDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_attArray = QList<Attribute *>();
    this->m_desc = commandDesc;
}

Command::Command(QString cmdName, short cmdCode, bool cmdHasAtt, NS_DirectionType::T_DirectionType cmdDirection, QList<Attribute *> attArray, QString commandDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_attArray = attArray;
    this->m_desc = commandDesc;
}

void Command::setName(QString cmdName) {
    this->m_name = cmdName;
}

QString Command::getName(void) {
    return this->m_name;
}



void Command::setId(short cmdCode) {
    this->m_id = cmdCode;
}

short Command::getId(void) {
    return this->m_id;
}



void Command::setHasAtt(bool cmdHasAtt) {
    this->m_hasAttributes = cmdHasAtt;
}

bool Command::getHasAtt(void) {
   return this->m_hasAttributes;
}



void Command::setDesc(QString cmdDesc) {
    this->m_desc = cmdDesc;
}

QString Command::getDesc(void) {
    return this->m_desc;
}



void Command::setDirection(NS_DirectionType::T_DirectionType cmdDirection) {
    this->m_direction = cmdDirection;
}

NS_DirectionType::T_DirectionType Command::getDirection(void) {
   return this->m_direction;
}

bool Command::isTransmittable(bool isA) {
    if (this->m_direction == NS_DirectionType::BIDIRECTIONAL) {
        return true;
    } else if(isA && (this->m_direction == NS_DirectionType::A_TO_B)) {
        return true;
    } else if(!isA && (this->m_direction == NS_DirectionType::B_TO_A)) {
        return true;
    }
    return false;
}

bool Command::isReceivable(bool isA) {
    if (this->m_direction == NS_DirectionType::BIDIRECTIONAL) {
        return true;
    } else if(isA && (this->m_direction == NS_DirectionType::B_TO_A)) {
        return true;
    } else if(!isA && (this->m_direction == NS_DirectionType::A_TO_B)) {
        return true;
    }
    return false;
}


void Command::setAttArray(QList<Attribute *> attArray) {
   this->m_attArray = attArray;
}

void Command::addAttribute(Attribute  *newAtt) {
    this->m_attArray.append(newAtt);
}

QList<Attribute *> Command::getAttArray(void) {
    return this->m_attArray;
}

QStringList Command::getAttNamesList(void) {
    QStringList attNames = QStringList();

    for (Attribute  *attribute : this->getAttArray()) {
        attNames.append(attribute->getName());
    }
    return attNames;
}

void Command::sortAttArrayById(void) {
    QList<Attribute *> sortedAttArray = this->m_attArray;
    Attribute *bufferAttribute;
    int j;
    int size = sortedAttArray.size();

    for (int i = 0; i < size; i++) {
        j = i;
        while((j > 0) && (sortedAttArray.at(j-1)->getId() > sortedAttArray.at(j)->getId())) {
          bufferAttribute = sortedAttArray.at(j);
          sortedAttArray.replace(j, sortedAttArray.at(j-1));
          sortedAttArray.replace(j-1, bufferAttribute);
          j = j - 1;
        }
    }
    this->m_attArray = sortedAttArray;
}



void Command::editParameters(QString cmdName, short cmdCode, bool cmdHasAtt , NS_DirectionType::T_DirectionType cmdDirection, QString cmdDesc) {
    this->m_name = cmdName;
    this->m_id = cmdCode;
    this->m_hasAttributes = cmdHasAtt;
    this->m_direction = cmdDirection;
    this->m_desc = cmdDesc;
}

void Command::removeAttByName(QString attName) {
    Attribute  *attStorage = this->getAttByName(attName);
    if (attStorage != nullptr) {
        attStorage->clearArray_rec();
        this->m_attArray.removeAll(attStorage);
        delete attStorage;
    }
}

void Command::clearAttArray(void) {
    for (Attribute  *attribute : this->m_attArray) {
        if (attribute->getSubAttArray().size() > 0) {
            attribute->clearArray_rec();
        }
    }
    while (!this->m_attArray.isEmpty()) {
        delete this->m_attArray.takeFirst();
    }
}

Attribute  *Command::getAttByName(QString cmdAttName) {
    Attribute  *cmdAttPointer = nullptr;

    for (Attribute  *attribute : this->m_attArray) {
        if ((attribute != nullptr) && (attribute->getName().compare(cmdAttName) == 0)) {
            cmdAttPointer = attribute;
            break;
        }
    }
    return cmdAttPointer;
}

Command  *Command::findCmdAddr(QString cmdName, QList<Command *> cmdList) {
   Command  *cmdStorage = nullptr;

   for (Command  *command : cmdList) {
      if (command->getName().compare(cmdName) == 0) {
         cmdStorage = command;
         break;
      }
   }
   return cmdStorage;
}

QList<Command *> Command::sortListById(QList<Command *> cmdList) {
   QList<Command *> sortedArray = cmdList;
   Command *bufferCmd;
   int j;
   int size = sortedArray.size();

   for (int i = 0; i < size; i++) {
      j = i;
      while ((j > 0) && (sortedArray.at(j-1)->getId() > sortedArray.at(j)->getId())) {
         bufferCmd = sortedArray.at(j);
         sortedArray.replace(j, sortedArray.at(j-1));
         sortedArray.replace(j-1, bufferCmd);
         j = j - 1;
      }
   }
   return sortedArray;
}

int Command::findCmdId(QString cmdName, QList<Command *> cmdList) {
   int cmdIdx = -1;

   for (int idx = 0; idx < cmdList.size(); idx++) {
      if (cmdList.at(idx)->getName().compare(cmdName) == 0) {
         cmdIdx = idx;
         break;
      }
   }
   return cmdIdx;
}

QStringList Command::getListCmdNames(QList<Command *> cmdList) {
   QStringList cmdNames = QStringList();

   for (Command  *command : cmdList) {
      cmdNames.append(command->getName());
   }
   return cmdNames;
}
