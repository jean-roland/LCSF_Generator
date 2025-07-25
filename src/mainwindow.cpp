/**
 * \file mainwindow.cpp
 * \brief GUI controller source file.
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
#include <QDesktopServices>
#include <QRandomGenerator>
#include <QStringBuilder>
// Custom include
#include "mainwindow.h"
#include "deschandler.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->leProtocolName->setText(defProtocolName);
    ui->twDescTreeView->setCurrentItem(ui->twDescTreeView->topLevelItem(0));

    // Columns resize
    ui->twDescTableView->resizeColumnsToContents();
    ui->twDescTableView->horizontalHeader()->setStretchLastSection(true);

    // Default dir paths
    outAPath = defoutAPath;
    outBPath = defoutBPath;

    // Set tree clear state
    treeIsCleared = false;
}

MainWindow::~MainWindow(void) {
   delete ui;
}

void MainWindow::on_actionNew_protocol_triggered(void) {
    QString msgString = "Do you want to reinitialize the interface (all unsaved progress will be lost)?";
    int questionAnswer = QMessageBox::question(this, "Question", msgString, QMessageBox::Yes | QMessageBox::No);

    if (questionAnswer == QMessageBox::Yes) {
       this->clearData();
    }
}

void MainWindow::on_actionSave_protocol_triggered(void) {
    QString protocolName(ui->leProtocolName->text());
    const QString protocolId(ui->leProtocolId->text().trimmed());
    const QString protocolDesc(ui->leProtocolDesc->text());

    if (protocolName.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol name is empty!");
        return;
    }
    protocolName = CheckAndCorrectInputString(protocolName);

    if (protocolId.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol id is empty!");
        return;
    }
    // Save current table
    if (ui->twDescTreeView->currentItem() != nullptr) {
       this->saveCurrentDescTable(ui->twDescTreeView->currentItem());
    }
    // If no commands, don't generate
    if (this->m_cmdArray.isEmpty()) {
       QMessageBox::warning(nullptr, "Warning", "Protocol has no commands!");
       return;
    }
    // Create repertory if needed
    QDir dir(descDirPath);
    if (!dir.exists()) {
       dir.mkpath(".");
    }
    // Request file location
    QString filename(QFileDialog::getSaveFileName(this, "Choose save location", descDirPath + "/" + protocolName, "Descriptor (*.json)"));
    // Validity check
    if (filename.trimmed().isEmpty()) {
       QMessageBox::warning(nullptr, "Warning", "Invalid save location!");
       return;
    }
    // Trim descriptor name
    QFileInfo completeFileName = QFileInfo(filename);
    filename = descDirPath + "/" + completeFileName.baseName();
    qDebug() << "File selected: " << filename;
    // Save data
    if (!DescHandler::save_desc(filename, this->m_cmdArray, protocolName, protocolId, protocolDesc)) {
       QMessageBox::warning(nullptr, "Warning", "Couldn't create json descriptor file!");
    }
}

void MainWindow::on_actionLoad_protocol_triggered(void) {
    // Look for descriptor file
    QFile file (QFileDialog::getOpenFileName(this, "Choose file to load", descDirPath, "Descriptor (*.json)"));

    // Abort if no file selected
    if (!file.exists()) {
       QMessageBox::warning(nullptr, "Error", "No selected file!");
       return;
    }
    // Clear data
    this->clearData();
    // Open file
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", file.errorString());
        return;
    }
    // Extract data
    QString protocolName, protocolId, protocolDesc;
    DescHandler::load_desc(file, this->m_cmdArray, protocolName, protocolId, protocolDesc);

    ui->leProtocolName->setText(protocolName);
    ui->leProtocolId->setText(protocolId);
    ui->leProtocolDesc->setText(protocolDesc);

    // Update UI
    this->updateDescTreeCmd();
    ui->twDescTreeView->setCurrentItem(ui->twDescTreeView->topLevelItem(0));
    if (this->m_cmdArray.size() > 0) {
       this->loadCommandArray();
    }
    // Column resize
    for (int ColumnIndex = 0; ColumnIndex < ui->twDescTableView->rowCount(); ++ColumnIndex) {
       ui->twDescTableView->resizeColumnToContents(ColumnIndex);
    }
    // Close file
    file.close();
}

void MainWindow::on_actionDocumentation_triggered(void)
{
    QDesktopServices::openUrl(QUrl("https://jean-roland.github.io/LCSF_Generator/", QUrl::TolerantMode));
}

void MainWindow::on_actionAbout_triggered(void)
{
    QString msgPopup = "LCSF Generator v";
    msgPopup.append(APP_VERSION);
    msgPopup.append("\n\nThis software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.");
    QMessageBox::information(this, "Info", msgPopup);
}

QTreeWidgetItem *MainWindow::findTreeWidgetFromName_Rec(QTreeWidgetItem *parentWidget, QString treeWidgetName) {
   QTreeWidgetItem *treeWidget = nullptr;
   QTreeWidgetItem *parentTreeItem = parentWidget;
   QList<QTreeWidgetItem *> currentChildrenList = this->getTreeItemChildren(parentTreeItem);

   for (QTreeWidgetItem *childWidget: currentChildrenList) {
      if (childWidget->text(0).compare(treeWidgetName) == 0) {
         treeWidget = childWidget;
         break;
      }
      if (childWidget->childCount() > 0) {
         QTreeWidgetItem *buffer = this->findTreeWidgetFromName_Rec(childWidget, treeWidgetName);
         if (buffer != nullptr) {
            treeWidget = buffer;
            break;
         }
      }
   }
   return treeWidget;
}

QString MainWindow::CheckAttNameDuplicate_Rec(QSet<QString> *pSet, QList<Attribute *>attArray) {
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
            QString dupName = CheckAttNameDuplicate_Rec(pSet, pAtt->getSubAttArray());
            if (dupName.size() > 0) {
                return dupName;
            }
        }
    }
    return "";
}

QString MainWindow::CheckAttNameDuplicate(void) {
    QSet<QString> set;
    // Parse the commande table for attributes
    for (Command *pCmd: this->m_cmdArray) {
        if (pCmd->getHasAtt()) {
            QString dupName = CheckAttNameDuplicate_Rec(&set, pCmd->getAttArray());
            if (dupName.size() > 0) {
                return dupName;
            }
        }
    }
    return "";
}

bool MainWindow::CheckInputString(QString input) {
    QRegExp exp(R"(\w{1,64})");
    return exp.exactMatch(input);
}

QString MainWindow::CorrectInputString(QString input) {
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

QString MainWindow::CheckAndCorrectInputString(QString input) {
    if (input.size() == 0) {
        QString correctedInput = "default_" + QString::number(QRandomGenerator::global()->generate());
        QMessageBox::warning(nullptr, "Warning", "Empty names are not allowed.\nCorrected to: '"+correctedInput+"'");
        return correctedInput;
    } else if (!CheckInputString(input)) {
        QString correctedInput = CorrectInputString(input);
        QMessageBox::warning(nullptr, "Warning", QLatin1String("Invalid name, only 64 alphanumerical/underscore chars allowed.\nCorrected to: \"") % correctedInput % QLatin1String("\""));
        return correctedInput;
    } else {
        return input;
    }
}

QTreeWidgetItem *MainWindow::findTreeWidgetFromName(QString treeWidgetName) {
   QTreeWidgetItem *treeWidget = nullptr;
   QTreeWidgetItem *parentTreeItem = ui->twDescTreeView->topLevelItem(0);
   QList<QTreeWidgetItem *> currentChildrenList = this->getTreeItemChildren(parentTreeItem);
   if (treeWidgetName.compare(parentTreeItem->text(0)) == 0) {
      treeWidget = parentTreeItem;
   } else {
      for (QTreeWidgetItem *childWidget: currentChildrenList) {
         if (childWidget->text(0).compare(treeWidgetName) == 0) {
            treeWidget = childWidget;
            break;
         }
         if (childWidget->childCount() > 0) {
            QTreeWidgetItem *buffer = this->findTreeWidgetFromName_Rec(childWidget, treeWidgetName);
            if (buffer != nullptr) {
               treeWidget = buffer;
               break;
            }
         }
      }
   }
   return treeWidget;
}

void MainWindow::on_twDescTreeView_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    if ((previous != nullptr) && !treeIsCleared) {
        // Save only if tree wasn't cleared
        this->saveCurrentDescTable(previous);
        // Fix in case current item was renamed
        current = ui->twDescTreeView->currentItem();
    }
    if (current != nullptr) {
        QString tableName = current->text(0);
        ui->labelDescTableName->setText(baseTableLabel + tableName);
        ui->twDescTableView->clear();
        if (tableName.compare(cmdArrayName) == 0) {
            qDebug() << "Command array detected";
            this->showCommandArray();
            if (this->m_cmdArray.size() > 0) {
                this->loadCommandArray();
            }
            ui->twDescTableView->resizeColumnsToContents();
            ui->twDescTableView->horizontalHeader()->setStretchLastSection(true);
        } else if (tableName.contains(attArraySuffix)) {
            qDebug() << "Attribute array detected";
            this->showAttributeArray();
            if (current->parent() == nullptr) {
                return;
            }
            if (current->parent()->text(0).compare(cmdArrayName) == 0) {
                this->loadCommandAttArray(tableName.remove(tableName.length() - attArraySuffix.length(), attArraySuffix.length()));
            } else {
                this->loadAttArray(current);
            }
            ui->twDescTableView->resizeColumnsToContents();
            ui->twDescTableView->horizontalHeader()->setStretchLastSection(true);
        } else {
            qDebug() << "Error, unknown array type";
        }
    }
    // Clear treeIsCleared flag
    if (treeIsCleared) {
       treeIsCleared = false;
    }
}

void MainWindow::fillCellItems(int rowIdx) {
    ui->twDescTableView->setItem(rowIdx, 0, new QTableWidgetItem(0));
    ui->twDescTableView->setItem(rowIdx, 1, new QTableWidgetItem(0));
    ui->twDescTableView->setItem(rowIdx, 4, new QTableWidgetItem(0));
}

void MainWindow::fillCellWidgets(int rowIdx) {
    QComboBox *comboYesNo = new QComboBox();
    comboYesNo->addItems(QStringList() << "Yes" << "No");
    ui->twDescTableView->setCellWidget(rowIdx, 2, comboYesNo);

    if (ui->twDescTableView->horizontalHeaderItem(0)->text().compare("Attribute Name") == 0) {
        QComboBox *comboDataType = new QComboBox();
        comboDataType->addItems(NS_AttDataType::SL_DocAttDataType);
        ui->twDescTableView->setCellWidget(rowIdx, 3, comboDataType);
    } else {
        // Commands
        QComboBox *comboDirectionType = new QComboBox();
        comboDirectionType->addItems(NS_DirectionType::SL_DirectionType);
        ui->twDescTableView->setCellWidget(rowIdx, 3, comboDirectionType);
    }
}

void MainWindow::clearArray(void) {
    for (int rowIdx = 0; rowIdx < ui->twDescTableView->rowCount(); rowIdx++) {
        for (int colIdx = 0; colIdx < ui->twDescTableView->columnCount(); colIdx++) {
            delete ui->twDescTableView->takeItem(rowIdx, colIdx);
        }
    }
}

void MainWindow::showCommandArray(void) {
    QStringList Headers;
    this->clearArray();
    Headers << "Command Name" << "Command Id (0x)" << "Attributes?" << "Direction" << "Description";
    ui->twDescTableView->setRowCount(1);
    ui->twDescTableView->setColumnCount(5);
    ui->twDescTableView->setHorizontalHeaderLabels(Headers);
    this->fillCellItems(0);
    this->fillCellWidgets(0);
    this->preFillCmdIdxCell(0);
}

void MainWindow::loadCommandArray(void) {
   Command *command;
   QComboBox *hasAttComboBox, *directionComboBox;
   this->clearArray();
   ui->twDescTableView->setRowCount(this->m_cmdArray.size());
   for (int idx = 0; idx < this->m_cmdArray.size(); idx++) {
      this->fillCellItems(idx);
      this->fillCellWidgets(idx);
      command = this->m_cmdArray.at(idx);
      ui->twDescTableView->item(idx, 0)->setText(command->getName());
      ui->twDescTableView->item(idx, 1)->setText(QString::number(command->getId(), 16).rightJustified(2, '0'));
      hasAttComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
      if (command->getHasAtt()) {
         hasAttComboBox->setCurrentIndex(0);
      } else {
         hasAttComboBox->setCurrentIndex(1);
      }
      directionComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
      directionComboBox->setCurrentIndex(command->getDirection());
      ui->twDescTableView->item(idx, 4)->setText(command->getDesc());
   }
}

void MainWindow::loadCommandAttArray(QString cmdName) {
    Command *command = Command::findCmdAddr(cmdName, this->m_cmdArray);
    if (command == nullptr) {
        QMessageBox::warning(this,"Warning", "Unknown command!");
        return;
    }
    QList<Attribute *> cmdAttArray = command->getAttArray();
    this->clearArray();
    if (cmdAttArray.size() > 0) {
       QComboBox *comboBox;
       ui->twDescTableView->setRowCount(cmdAttArray.size());
       for (int idx = 0; idx < cmdAttArray.size(); idx++) {
          this->fillCellItems(idx);
          this->fillCellWidgets(idx);
          Attribute *attribute = cmdAttArray.at(idx);
          ui->twDescTableView->item(idx, 0)->setText(attribute->getName());
          ui->twDescTableView->item(idx, 1)->setText(QString::number(attribute->getId(), 16).rightJustified(2, '0'));
          comboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
          if (attribute->getIsOptional()) {
             comboBox->setCurrentIndex(0);
          } else {
             comboBox->setCurrentIndex(1);
          }
          comboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
          comboBox->setCurrentIndex(attribute->getDataType());
          ui->twDescTableView->item(idx, 4)->setText(attribute->getDesc());
       }
    }
}

void MainWindow::loadAttArray(QTreeWidgetItem *item) {
    Attribute *attribute = this->findAttStorage(item);
    if (attribute == nullptr) {
        qDebug() << "Null pointer, couldn't load attribute\n";
        return;
    }
    QList<Attribute *> attSubAttArray = attribute->getSubAttArray();
    this->clearArray();
    if (attSubAttArray.size() > 0) {
        QComboBox *comboBox;
        ui->twDescTableView->setRowCount(attSubAttArray.size());
        for (int idx = 0; idx < attSubAttArray.size(); idx++) {
            this->fillCellItems(idx);
            this->fillCellWidgets(idx);
            Attribute *attribute = attSubAttArray.at(idx);
            ui->twDescTableView->item(idx, 0)->setText(attribute->getName());
            ui->twDescTableView->item(idx, 1)->setText(QString::number(attribute->getId(), 16).rightJustified(2, '0'));
            comboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
            if (attribute->getIsOptional()) {
                comboBox->setCurrentIndex(0);
            } else {
                comboBox->setCurrentIndex(1);
            }
            comboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
            comboBox->setCurrentIndex(attribute->getDataType());
            ui->twDescTableView->item(idx, 4)->setText(attribute->getDesc());
        }
    }
}

void MainWindow::showAttributeArray(void) {
    QStringList Headers;
    this->clearArray();
    Headers << "Attribute Name" << "Attribute Id (0x)" << "Optional?" << "Data Type" << "Description";
    ui->twDescTableView->setRowCount(1);
    ui->twDescTableView->setColumnCount(5);
    ui->twDescTableView->setHorizontalHeaderLabels(Headers);
    this->fillCellWidgets(0);
}

QList<QTreeWidgetItem *> MainWindow::getTreeItemChildren(QTreeWidgetItem *parentTreeItem) {
   QList<QTreeWidgetItem *> childrenList = QList<QTreeWidgetItem *>();
   QTreeWidgetItem *currentChild;

   if (parentTreeItem != nullptr) {
      for (int idx = 0; idx < parentTreeItem->childCount(); idx++) {
         currentChild = parentTreeItem->child(idx);
         childrenList.append(currentChild);
      }
   }
   return childrenList;
}

QTreeWidgetItem *MainWindow::getTreeItemChild(QList<QTreeWidgetItem *> parentTreeItemChildren, QString childName) {
   QTreeWidgetItem *treeChild = nullptr;
   QTreeWidgetItem *currentChild;

   for (int idx = 0; idx < parentTreeItemChildren.size(); idx++) {
      currentChild = parentTreeItemChildren.at(idx);
      if (currentChild->text(0).compare(childName) == 0) {
         treeChild = currentChild;
         break;
      }
   }
   return treeChild;
}

void MainWindow::deleteNoLongerExistingTreeItemChildren(QTreeWidgetItem *parentTreeItem, QList<QTreeWidgetItem *> noLongerExistingChildren) {
   if (parentTreeItem != nullptr) {
      for (QTreeWidgetItem *treeItemchild : noLongerExistingChildren) {
         parentTreeItem->removeChild(treeItemchild);
         delete treeItemchild;
      }
   }
}

void MainWindow::updateDescTreeCmd(void) {
   QTreeWidgetItem *parentTreeItem = ui->twDescTreeView->topLevelItem(0);
   QStringList childName;
   QList<QTreeWidgetItem *> currentChildrenList = this->getTreeItemChildren(parentTreeItem);

   for (Command *command : this->m_cmdArray) {
      if (command->getHasAtt()) {
         QString cmdName = command->getName() + attArraySuffix;
         QTreeWidgetItem *existingChild = this->getTreeItemChild(currentChildrenList, cmdName);
         if (existingChild != nullptr) {
            // Remove pre-existing children from list
            currentChildrenList.removeAll(existingChild);
         } else {
            // Add children
            childName.clear();
            childName.append(cmdName);
            QTreeWidgetItem *newChild = new QTreeWidgetItem(childName,0);
            parentTreeItem->addChild(newChild);
            // Update sub-children if needed
            if (command->getAttArray().size() > 0) {
               this->updateDescTreeCmdAtt(command->getName());
            }
         }
      }
   }
   // Delete removed children
   this->deleteNoLongerExistingTreeItemChildren(parentTreeItem, currentChildrenList);
   ui->twDescTreeView->expandItem(parentTreeItem);
}

void MainWindow::updateDescTreeCmdAtt(QString cmdName) {
    QTreeWidgetItem *parentTreeItem = this->findTreeWidgetItem(cmdName + attArraySuffix);
    QStringList childName;
    Command *command = Command::findCmdAddr(cmdName, this->m_cmdArray);
    if ((parentTreeItem == nullptr) || (command == nullptr)) {
        qDebug() << "Null pointers, couldn't update tree\n";
        return;
    }
    QList<QTreeWidgetItem *> currentChildrenList = this->getTreeItemChildren(parentTreeItem);
    QList<Attribute *> commandAttArray = command->getAttArray();

    for (Attribute *attribute : commandAttArray) {
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            QString attName = attribute->getName() + attArraySuffix;
            QTreeWidgetItem *existingChild = this->getTreeItemChild(currentChildrenList, attName);
            if (existingChild != nullptr) {
                // Remove pre-existing children from list
                currentChildrenList.removeAll(existingChild);
            } else {
                // Add children
                childName.clear();
                childName.append(attName);
                QTreeWidgetItem *newChild = new QTreeWidgetItem(childName,0);
                parentTreeItem->addChild(newChild);
                // Note pointer to item
                attribute->setObjPtr(newChild);
                // Update sub-children if needed
                if (attribute->getSubAttArray().size() > 0) {
                    this->updateDescTreeAtt(newChild);
                }
            }
        }
    }
    // Delete removed children
    this->deleteNoLongerExistingTreeItemChildren(parentTreeItem, currentChildrenList);
    ui->twDescTreeView->expandItem(parentTreeItem);
}

void MainWindow::updateDescTreeAtt(QTreeWidgetItem *parentTreeItem) {
    QStringList childName;
    Attribute *pAttStorage = this->findAttStorage(parentTreeItem);

    if ((parentTreeItem == nullptr) || (pAttStorage == nullptr)) {
        qDebug() << "Null pointers, couldn't update tree\n";
        return;
    }
    QList<QTreeWidgetItem *> currentChildrenList = this->getTreeItemChildren(parentTreeItem);
    QList<Attribute *> attSubAttArray = pAttStorage->getSubAttArray();

    for (Attribute *attribute : attSubAttArray) {
        QString attName = attribute->getName() + attArraySuffix;
        QTreeWidgetItem *existingChild = this->getTreeItemChild(currentChildrenList, attName);
        if (attribute->getDataType() == NS_AttDataType::SUB_ATTRIBUTES) {
            if (existingChild != nullptr) {
                // Remove pre-existing children
                currentChildrenList.removeAll(existingChild);
            } else {
                // Add children
                childName.clear();
                childName.append(attName);
                QTreeWidgetItem *newChild = new QTreeWidgetItem(childName,0);
                parentTreeItem->addChild(newChild);
                // Note pointer to item
                attribute->setObjPtr(newChild);
                // Update sub-children if needed
                if (attribute->getSubAttArray().size() > 0) {
                    this->updateDescTreeAtt(newChild);
                }
            }
        }
    }
    // Delete removed children
    this->deleteNoLongerExistingTreeItemChildren(parentTreeItem, currentChildrenList);
    ui->twDescTreeView->expandItem(parentTreeItem);
}

bool MainWindow::saveCurrentDescTable(QTreeWidgetItem *item) {
    QString tableName = item->text(0);

    if (tableName.compare(cmdArrayName) == 0) {
        if (this->saveCommandTable()) {
            this->updateDescTreeCmd();
            return true;
        }
    } else if (tableName.contains(attArraySuffix)) {
        QString parentName = "";
        QString trimmedTableName = tableName;
        trimmedTableName.remove(tableName.length() - attArraySuffix.length(), attArraySuffix.length());
        if (item == nullptr) {
            qDebug() << "Null item, couldn't save table\n";
            return false;
        }
        if (item->parent() == nullptr) {
            qDebug() << "Null parent, couldn't save table\n";
            return false;
        }
        parentName = item->parent()->text(0);

        if (parentName.compare(cmdArrayName) == 0) {
           if (this->saveCmdAttTable(trimmedTableName)) {
               this->updateDescTreeCmdAtt(trimmedTableName);
               return true;
           }
        } else if (parentName.contains(attArraySuffix)) {
           if (this->saveAttTable(item)) {
               this->updateDescTreeAtt(item);
               return true;
           }
        } else {
           qDebug() << "Wrong parent table name, couldn't save table\n";
           return false;
        }
    } else {
        qDebug() << "Wrong table name, couldn't save table\n";
        return false;
    }
    return false;
}

NS_DirectionType::T_DirectionType MainWindow::getDirectionTypeFromComboBox(QComboBox *comboBox) {
   NS_DirectionType::T_DirectionType directionType = NS_DirectionType::UNKNOWN;
   if (comboBox != nullptr) {
      for (int idx = 0; idx < NS_DirectionType::SL_DirectionType.size(); idx++) {
         if (comboBox->currentText().compare(NS_DirectionType::SL_DirectionType.at(idx)) == 0) {
            directionType = NS_DirectionType::SLDirectionType2Enum[idx];
            break;
         }
      }
   }
   return directionType;
}

QStringList MainWindow::getTableNames(void) {
    QStringList tableNames = QStringList();

    for (int idx = 0; idx < ui->twDescTableView->rowCount(); idx++) {
        if (ui->twDescTableView->item(idx, 0) != nullptr) {
            QString input = ui->twDescTableView->item(idx, 0)->text();
            QString corrInput = CheckAndCorrectInputString(input);
            // Fix to avoid empty names
            ui->twDescTableView->item(idx, 0)->setText(corrInput);
            tableNames.append(corrInput);
        } else {
            break;
        }
    }
    return tableNames;
}

QList<short> MainWindow::getTableCodes(void) {
   QList<short> tableCodes = QList<short>();

   for (int idx = 0; idx < ui->twDescTableView->rowCount(); idx++) {
      if (ui->twDescTableView->item(idx, 1) != nullptr) {
         tableCodes.append(ui->twDescTableView->item(idx, 1)->text().toShort(nullptr, 16));
      } else {
         break;
      }
   }
   return tableCodes;
}

bool MainWindow::saveCommandTable(void) {
    QStringList prevExistingCmdNames = Command::getListCmdNames(this->m_cmdArray);
    QStringList noLongerExistingCmdNames = prevExistingCmdNames;
    QStringList newCmdNames = this->getTableNames();
    QList<short> newCmdCodes = this->getTableCodes();

    for (int idx = 0; idx < ui->twDescTableView->rowCount(); idx++) {
        // Empty line
        if ((ui->twDescTableView->item(idx, 0) == nullptr) && (ui->twDescTableView->item(idx, 1) == nullptr)) {
            qDebug() << "Empty line detected" << Qt::endl;
            return false;
        }
        // Missing parameters
        if ((ui->twDescTableView->item(idx, 0) == nullptr) || (ui->twDescTableView->item(idx, 1) == nullptr) || (ui->twDescTableView->cellWidget(idx, 2) == nullptr)
        || (ui->twDescTableView->cellWidget(idx, 3) == nullptr)) {
            QMessageBox::warning(this,"Warning", "Missing parameters on line n°: " + QString::number(idx + 1));
            return false;
        }
        QString cmdName = ui->twDescTableView->item(idx, 0)->text();
        short cmdCode = ui->twDescTableView->item(idx, 1)->text().toShort(nullptr, 16);
        QString cmdDesc;

        if (ui->twDescTableView->item(idx, 4) == nullptr) {
            cmdDesc = "";
        } else {
            cmdDesc = ui->twDescTableView->item(idx, 4)->text();
        }
        QComboBox *hasAttComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
        bool cmdHasAtt = (hasAttComboBox->currentText().compare("Yes") == 0) ? true : false ;
        QComboBox *directionComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
        NS_DirectionType::T_DirectionType cmdDirection = getDirectionTypeFromComboBox(directionComboBox);

        if (cmdDirection == NS_DirectionType::UNKNOWN) {
            QMessageBox::warning(this,"Warning", "Unknown data type");
            return false;
        }
        if (newCmdNames.count(cmdName) > 1) {
            QMessageBox::warning(this,"Warning", "Multiple definition of command name: " + cmdName + " detected");
            return false;
        }
        if (newCmdCodes.count(cmdCode) > 1) {
            QMessageBox::warning(this,"Warning", "Multiple definition of command id: 0x" + QString::number(cmdCode, 16) + " detected");
            return false;
        }
        // Edit or add commands
        if (idx < this->m_cmdArray.size()) {
            QString prevCmdName = prevExistingCmdNames.at(idx);
            qDebug() << " Command modification: " << prevCmdName << ": " << cmdName << ", " << cmdCode << ", " << cmdHasAtt << ", " << cmdDirection << ", " << cmdDesc;
            Command *currentCmd = this->m_cmdArray.at(idx);
            currentCmd->editParameters(cmdName, cmdCode, cmdHasAtt, cmdDirection, cmdDesc);
            if (!cmdHasAtt) {
                currentCmd->clearAttArray();
            }
            // Remove non-longer existing commands
            noLongerExistingCmdNames.removeAll(prevCmdName);
        } else {
            qDebug() << "Command creation: " << cmdName << ", " << cmdCode << ", " << cmdHasAtt << ", " << cmdDirection << ", " << cmdDesc;
            this->m_cmdArray.append(new Command(cmdName, cmdCode, cmdHasAtt, cmdDirection, cmdDesc));
        }
    }
    for (QString noLongerExistingCmdName : noLongerExistingCmdNames) {
        this->deleteCommand(noLongerExistingCmdName);
    }
    return true;
}

NS_AttDataType::T_AttDataType MainWindow::getAttDataTypeFromComboBox(QComboBox *comboBox) {
   NS_AttDataType::T_AttDataType dataType = NS_AttDataType::UNKNOWN;
   if (comboBox != nullptr) {
      for (int idx = 0; idx < NS_AttDataType::SL_DocAttDataType.size(); idx++) {
         if (comboBox->currentText().compare(NS_AttDataType::SL_DocAttDataType.at(idx)) == 0) {
            dataType = NS_AttDataType::SLAttDataType2Enum[idx];
            break;
         }
      }
   }
   return dataType;
}

bool MainWindow::saveCmdAttTable(QString cmdName) {
    Command *command = Command::findCmdAddr(cmdName, this->m_cmdArray);
    QStringList newAttNames = this->getTableNames();
    QList<short> newAttCodes = this->getTableCodes();

    if (command != nullptr) {
        QStringList prevExistingAttNames = command->getAttNamesList();
        QStringList noLongerExistingAttNames = prevExistingAttNames;

        for (int idx = 0; idx < ui->twDescTableView->rowCount(); idx++) {
            // Empty line
            if ((ui->twDescTableView->item(idx, 0) == nullptr) && (ui->twDescTableView->item(idx, 1) == nullptr)) {
                qDebug() << "Empty line detected" << Qt::endl;
                return false;
            }
            // Missing parameters
            if ((ui->twDescTableView->item(idx, 0) == nullptr) || (ui->twDescTableView->item(idx, 1) == nullptr) || (ui->twDescTableView->cellWidget(idx, 2) == nullptr)
            || (ui->twDescTableView->cellWidget(idx, 3) == nullptr)) {
                QMessageBox::warning(this,"Warning", "Missing parameters on line n°: " + QString::number(idx + 1));
                return false;
            }
            QString attName = ui->twDescTableView->item(idx, 0)->text();
            QString attDesc;

            if (ui->twDescTableView->item(idx, 4) == nullptr) {
                attDesc = "";
            } else {
                attDesc = ui->twDescTableView->item(idx, 4)->text();
            }

            short attCode = ui->twDescTableView->item(idx, 1)->text().toShort(nullptr, 16);
            QComboBox *isOptionalComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
            bool attIsOptional = (isOptionalComboBox->currentText().compare("Yes") == 0) ? true : false ;
            QComboBox *dataTypeComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
            NS_AttDataType::T_AttDataType attDataType = getAttDataTypeFromComboBox(dataTypeComboBox);

            if (attDataType == NS_AttDataType::UNKNOWN) {
                QMessageBox::warning(this,"Warning", "Unknown data type");
                return false;
            }
            if (newAttNames.count(attName) > 1) {
                QMessageBox::warning(this,"Warning", "Multiple definition of attribute id: 0x" + QString::number(attCode, 16) + " detected");
                return false;
            }
            if (newAttCodes.count(attCode) > 1) {
                QMessageBox::warning(this,"Warning", "Multiple definition of attribute name: " + attName + " detected");
                return false;
            }
            // Edit or create attribute
            if (idx < command->getAttArray().size()) {
                QString prevAttName = prevExistingAttNames.at(idx);
                qDebug() << "Attribute modification: " << prevAttName << ": "
                << attName << ", " << attCode << ", " << attIsOptional << ", " << NS_AttDataType::SL_AttDataType.at(attDataType) << ", " << attDesc;
                Attribute *currentAtt = command->getAttArray().at(idx);
                currentAtt->editParams(attName, attCode, attIsOptional, attDataType, attDesc);

                if (attDataType != NS_AttDataType::SUB_ATTRIBUTES) {
                    currentAtt->clearArray_rec();
                }
                noLongerExistingAttNames.removeAll(prevAttName);
            } else {
                qDebug() << "Attribute creation: " << attName << ", " << attCode << ", " << attIsOptional << ", "
                << NS_AttDataType::SL_AttDataType.at(attDataType) << ", " << attDesc;
                command->addAttribute(new Attribute(attName, attCode, attIsOptional, attDataType, attDesc));
            }
        }
        for (QString noLongerExistingAttName : noLongerExistingAttNames) {
            this->deleteCommand(noLongerExistingAttName);
        }
    } else if (this->m_cmdArray.size() > 0) {
        qDebug() << "Command array empty, couldn't save table\n";
        return false;
    }
    return true;
}

QTreeWidgetItem *MainWindow::findTreeWidgetItem_REC(QTreeWidgetItem *parentTreeItem, QString tableName) {
   QTreeWidgetItem *treeWidgetItem = nullptr;
   QTreeWidgetItem *currentTreeWidgetItem = nullptr;

   if (parentTreeItem != nullptr) {
      for (int idx = 0; idx < parentTreeItem->childCount(); idx++) {
         currentTreeWidgetItem = parentTreeItem->child(idx);
         if (currentTreeWidgetItem != nullptr) {
            QString treeItemText = currentTreeWidgetItem->text(0);
            if (treeItemText.compare(tableName) == 0) {
               treeWidgetItem = currentTreeWidgetItem;
               break;
            } else if (currentTreeWidgetItem->childCount() > 0) {
               treeWidgetItem = this->findTreeWidgetItem_REC(currentTreeWidgetItem, tableName);
            }
         }
      }
   }
   return treeWidgetItem;
}

QTreeWidgetItem *MainWindow::findTreeWidgetItem(QString tableName) {
   QTreeWidgetItem *treeWidgetItem = nullptr;
   QTreeWidgetItem *currentTreeWidgetItem = nullptr;
   QTreeWidgetItem *topTreeWidgetItem = ui->twDescTreeView->topLevelItem(0);

   if (topTreeWidgetItem != nullptr) {
      int topChildCount = topTreeWidgetItem->childCount();
      for (int idx = 0; idx < topChildCount; idx++) {
         currentTreeWidgetItem = topTreeWidgetItem->child(idx);
         if (currentTreeWidgetItem != nullptr) {
            int currentChildCount = currentTreeWidgetItem->childCount();
            QString treeItemText = currentTreeWidgetItem->text(0);
            if (treeItemText.compare(tableName) == 0) {
               treeWidgetItem = currentTreeWidgetItem;
               break;
            } else if (currentChildCount > 0) {
               treeWidgetItem = this->findTreeWidgetItem_REC(currentTreeWidgetItem, tableName);
               if(treeWidgetItem != nullptr) {
                   break;
               }
            }
         }
      }
   }
   return treeWidgetItem;
}

Attribute *MainWindow::findAttStorage_REC(QTreeWidgetItem *item, QList<Attribute *> attList) {
    Attribute *pAttribute = nullptr;
    for(Attribute *att: attList) {
        // Check if attribute corresponds
        if (att->getObjPtr() == item) {
            pAttribute = att;
            break;
        }
        // Parse sub-attribute list
        QList<Attribute *> subAttList = att->getSubAttArray();
        pAttribute = findAttStorage_REC(item, subAttList);
        if (pAttribute != nullptr) {
            break;
        }
    }
    return pAttribute;
}

Attribute *MainWindow::findAttStorage(QTreeWidgetItem *item) {
    Attribute *pAttribute = nullptr;

    if (item != nullptr) {
        for(Command *cmd: this->m_cmdArray) {
            QList<Attribute *> attList = cmd->getAttArray();
            pAttribute = findAttStorage_REC(item, attList);
            if (pAttribute != nullptr) {
                break;
            }
        }
    }
    return pAttribute;
}

bool MainWindow::saveAttTable(QTreeWidgetItem *item) {
    Attribute *pAttStorage = this->findAttStorage(item);
    QStringList newAttNames = this->getTableNames();
    QList<short> newAttCodes = this->getTableCodes();

    if (pAttStorage != nullptr) {
        QStringList prevExistingAttNames = pAttStorage->getSubAttNamesList();
        QStringList noLongerExistingAttNames = prevExistingAttNames;

        for (int idx = 0; idx < ui->twDescTableView->rowCount(); idx++) {
            // Empty line
            if ((ui->twDescTableView->item(idx, 0) == nullptr) && (ui->twDescTableView->item(idx, 1) == nullptr)) {
                qDebug() << "Empty line detected" << Qt::endl;
                return false;
            }
            // Missing parameters
            if ((ui->twDescTableView->item(idx, 0) == nullptr) || (ui->twDescTableView->item(idx, 1) == nullptr) || (ui->twDescTableView->cellWidget(idx, 2) == nullptr)
            || (ui->twDescTableView->cellWidget(idx, 3) == nullptr)) {
                QMessageBox::warning(this,"Warning", "Missing parameters on line n°: " + QString::number(idx + 1));
                return false;
            }
            QString attName = ui->twDescTableView->item(idx, 0)->text();
            QString attDesc;

            if (ui->twDescTableView->item(idx, 4) == nullptr) {
                attDesc = "";
            } else {
                attDesc = ui->twDescTableView->item(idx, 4)->text();
            }
            short attCode = ui->twDescTableView->item(idx, 1)->text().toShort(nullptr, 16);
            QComboBox *isOptionalComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 2));
            bool attIsOptional = (isOptionalComboBox->currentText().compare("Yes") == 0) ? true : false ;
            QComboBox *dataTypeComboBox = static_cast<QComboBox *>(ui->twDescTableView->cellWidget(idx, 3));
            NS_AttDataType::T_AttDataType attDataType = getAttDataTypeFromComboBox(dataTypeComboBox);

            if (attDataType == NS_AttDataType::UNKNOWN) {
                QMessageBox::warning(this,"Warning", "Unknown data type!");
                return false;
            }
            if (newAttNames.count(attName) > 1) {
                QMessageBox::warning(this,"Warning", "Multiple definition of attribute name: " + attName + " detected");
                return false;
            }
            if (newAttCodes.count(attCode) > 1) {
                QMessageBox::warning(this,"Warning", "Multiple definition of attribute id: 0x" + QString::number(attCode, 16) + " detected");
                return false;
            }
            // Edit or add attribute
            if (idx < pAttStorage->getSubAttArray().size()) {
                QString prevAttName = prevExistingAttNames.at(idx);
                qDebug() << "Sub-attribute modification: " << prevAttName << ": "
                << attName << ", " << attCode << ", " << attIsOptional << ", " << NS_AttDataType::SL_AttDataType.at(attDataType) << ", " << attDesc;
                Attribute *currentAtt = pAttStorage->getSubAttArray().at(idx);
                currentAtt->editParams(attName, attCode, attIsOptional, attDataType, attDesc);
                if (attDataType != NS_AttDataType::SUB_ATTRIBUTES) {
                    currentAtt->clearArray_rec();
                }
                noLongerExistingAttNames.removeAll(prevAttName);
            } else {
                qDebug() << "Sub-attribute creation: "
                << attName << ", " << attCode << ", " << attIsOptional << ", " << NS_AttDataType::SL_AttDataType.at(attDataType) << ", " << attDesc;
                pAttStorage->addSubAtt(new Attribute(attName, attCode, attIsOptional, attDataType, attDesc));
            }
        }
        for (QString noLongerExistingAttName : noLongerExistingAttNames) {
            this->deleteCommand(noLongerExistingAttName);
        }
    } else if (this->m_cmdArray.size() > 0) {
        qDebug() << "Empty command table, couldn't save table\n";
        return false;
    }
    return true;
}

void MainWindow::on_pbSaveTable_clicked(void) {
   if (ui->twDescTreeView->currentItem() != nullptr) {
      if (this->saveCurrentDescTable(ui->twDescTreeView->currentItem())) {
         QMessageBox::information(this,"Info", "Table saved");
      }
   }
}

void MainWindow::preFillCmdIdxCell(int lineIdx) {
    int cmdIdx = 0;
    // Find previous idx
    if (lineIdx > 0) {
        int prevLineIdx = lineIdx - 1;
        QTableWidgetItem *prevCell = ui->twDescTableView->item(prevLineIdx, 1);
        // Increment if possible
        if (prevCell != nullptr) {
            cmdIdx = prevCell->text().toShort(nullptr, 16) + 1;
        }
    }
    // Fill idx
    QTableWidgetItem *newCell = ui->twDescTableView->item(lineIdx, 1);
    if (newCell != nullptr) {
        newCell->setText(QString::number(cmdIdx, 16).rightJustified(2, '0'));
    }
}

void MainWindow::on_pbAddTableLine_clicked(void) {
   int newLineIdx = ui->twDescTableView->rowCount();
   ui->twDescTableView->insertRow(newLineIdx);
   this->fillCellItems(newLineIdx);
   this->fillCellWidgets(newLineIdx);
   this->preFillCmdIdxCell(newLineIdx);
}

void MainWindow::deleteCommand(QString cmdName) {
   int cmdIdx = Command::findCmdIdx(cmdName, this->m_cmdArray);
   if (cmdIdx >= 0) {
      qDebug() << "Command deleted: " << cmdName;
      this->m_cmdArray.at(cmdIdx)->clearAttArray();
      delete this->m_cmdArray.takeAt(cmdIdx);
      this->updateDescTreeCmd();
   } else {
      qDebug() << "Unknown command: " << cmdName;
   }
}

void MainWindow::deleteCommandAtt(QString cmdName, QString attName) {
   Command *command = Command::findCmdAddr(cmdName.remove(cmdName.length() - attArraySuffix.length(), attArraySuffix.length()), this->m_cmdArray);
   if (command != nullptr) {
      qDebug() << "Attribute deleted: " << attName;
      command->removeAttByName(attName);
      this->updateDescTreeCmdAtt(cmdName);
   }
}

void MainWindow::deleteAttribute(QString attName, QTreeWidgetItem *attItem) {
   Attribute *attribute = this->findAttStorage(attItem);
   if (attribute != nullptr) {
      qDebug() << "Attribute deleted: " << attName;
      attribute->removeAtt(attName);
      this->updateDescTreeAtt(attItem);
   }
}

// Delete Line button action
void MainWindow::on_pbDeleteTableLine_clicked(void) {
   int questionAnswer;
   QString objectName;
   bool deleteOk = false;

   if (ui->twDescTableView->rowCount() > 0) {
      QTreeWidgetItem *currentItem = ui->twDescTreeView->currentItem();

      if (currentItem != nullptr) {
         if (ui->twDescTableView->currentRow() >= 0) {
            QString msgString = "Do you want to delete line n°" + QString::number(ui->twDescTableView->currentRow() + 1) + "?";
            questionAnswer = QMessageBox::question(this, "Warning", msgString, QMessageBox::Yes | QMessageBox::No);

            if (questionAnswer == QMessageBox::Yes) {
               if (ui->twDescTableView->item(ui->twDescTableView->currentRow(), 0) != nullptr) {
                  deleteOk = true;
                  objectName = ui->twDescTableView->item(ui->twDescTableView->currentRow(), 0)->text();
                  qDebug() << "Attribute or command deleted: " << objectName;
               }
               ui->twDescTableView->removeRow(ui->twDescTableView->currentRow());
            }
         } else {
            QString msgString = "Do you want to delete line n°" + QString::number(ui->twDescTableView->rowCount()) + "?";
            questionAnswer = QMessageBox::question(this, "Warning", msgString, QMessageBox::Yes | QMessageBox::No);

            if (questionAnswer == QMessageBox::Yes) {
               if (ui->twDescTableView->item(ui->twDescTableView->rowCount() - 1 , 0) != nullptr) {
                  deleteOk = true;
                  objectName = ui->twDescTableView->item(ui->twDescTableView->rowCount() - 1, 0)->text();
                  qDebug() << "Command deleted: " << objectName;
               }
               ui->twDescTableView->removeRow(ui->twDescTableView->rowCount() - 1);
            }
         }
         if (deleteOk) {
            if (currentItem->parent() == nullptr) {
               // Commands
               this->deleteCommand(objectName);
            } else if (currentItem->parent()->parent() == nullptr) {
               // Commands attributes
               QString parentObjectName =  currentItem->text(0);
               this->deleteCommandAtt(parentObjectName, objectName);
            } else {
               // Sub-attributes
               this->deleteAttribute(objectName, currentItem);
            }
         }
      }
   } else {
      QMessageBox::warning(this,"Warning", "No line to delete!");
   }
}

// Refresh Table button action
void MainWindow::on_pbRefreshTable_clicked(void) {
    QTreeWidgetItem *current = ui->twDescTreeView->currentItem();

    if (current == nullptr) {
        return;
    }
    QString msgString = "This will reload the table (unsaved work will be lost), continue?";
    int questionAnswer = QMessageBox::question(this, "Question", msgString, QMessageBox::Yes | QMessageBox::No);

    if (questionAnswer == QMessageBox::No) {
        return;
    }
    QString tableName = current->text(0);
    ui->twDescTableView->clear();

    if (tableName.compare(cmdArrayName) == 0) {
        this->showCommandArray();
        if (this->m_cmdArray.size() > 0) {
            this->loadCommandArray();
        }
    } else if (tableName.contains(attArraySuffix)) {
        this->showAttributeArray();
        if (current->parent() == nullptr) {
            return;
        }
        if (current->parent()->text(0).compare(cmdArrayName) == 0) {
            this->loadCommandAttArray(tableName.remove(tableName.length() - attArraySuffix.length(), attArraySuffix.length()));
        } else {
            this->loadAttArray(current);
        }
    }
}

void MainWindow::on_pbSortTable_clicked(void) {
    QTreeWidgetItem *currentTreeItem = ui->twDescTreeView->currentItem();

    if (currentTreeItem != nullptr) {
       QString widgetName = currentTreeItem->text(0);
       QString tableName = currentTreeItem->text(0);
       QString msgString = "Do you want to sort table: " + tableName + " in id ascending order?";
       int questionAnswer = QMessageBox::question(this, "Question", msgString, QMessageBox::Yes | QMessageBox::No);

       if (questionAnswer == QMessageBox::Yes) {
          if (tableName.compare(cmdArrayName) == 0) {
             this->saveCurrentDescTable(currentTreeItem);
             this->showCommandArray();

             if (this->m_cmdArray.size() > 0) {
                this->m_cmdArray = Command::sortListById(this->m_cmdArray);
                this->loadCommandArray();
             }
          } else if (tableName.contains(attArraySuffix)) {
             this->saveCurrentDescTable(currentTreeItem);
             this->showAttributeArray();

             if (currentTreeItem->parent() != nullptr) {
                if (currentTreeItem->parent()->text(0).compare(cmdArrayName) == 0) {
                   QString cmdName = tableName.remove(tableName.length() - attArraySuffix.length(), attArraySuffix.length());
                   Command *command = Command::findCmdAddr(cmdName, this->m_cmdArray);

                   if (command != nullptr) {
                      command->sortAttArrayById();
                      this->loadCommandAttArray(cmdName);
                   }
                } else {
                   Attribute *attribute = this->findAttStorage(currentTreeItem);
                   if (attribute != nullptr) {
                      attribute->sortArrayById();
                      this->loadAttArray(currentTreeItem);
                   }
                }
             }
          }
       }
       this->clearTree();
       this->updateDescTreeCmd();
       ui->twDescTreeView->setCurrentItem(this->findTreeWidgetFromName(widgetName));
    }
}

// Clear all data
void MainWindow::clearData(void) {
   // Reset protocolName
   ui->leProtocolName->setText(defProtocolName);
   // Clear protocolNId field
   ui->leProtocolId->clear();
   // Clear code extractor
   this->m_codeextractA = CodeExtractor();
   this->m_codeextractB = CodeExtractor();
   // Clear button states
   QPalette pal;
   pal.setColor(QPalette::Button, QColor(Qt::white));
   ui->pbImportDescA->setPalette(pal);
   ui->pbImportDescB->setPalette(pal);
   // Clear command array
   for (Command *command : this->m_cmdArray) {
      command->clearAttArray();
   }
   while (!this->m_cmdArray.isEmpty()) {
       delete this->m_cmdArray.takeFirst();
   }
   // Clear ui Tree
   this->clearTree();
   // Clear ui Table
   ui->twDescTableView->clearContents();
   // Reset Table
   this->showCommandArray();
   // Reset directories paths
   outAPath = defoutAPath;
   outBPath = defoutBPath;
}

void MainWindow::clearTree(void) {
    treeIsCleared = true;
    ui->twDescTreeView->setCurrentItem(ui->twDescTreeView->topLevelItem(0));
    QTreeWidgetItem *topItem = ui->twDescTreeView->topLevelItem(0);
    while (topItem->childCount() > 0) {
        delete topItem->child(topItem->childCount() - 1);
    }
}

// Import Desc A button action
void MainWindow::on_pbImportDescA_clicked(void) {
    QString protocolName(ui->leProtocolName->text());

    if (protocolName.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol name is empty!");
        return;
    }
    protocolName = CheckAndCorrectInputString(protocolName);
    QDir dir(outAPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filterName = protocolName + "_Main_a.c";
    QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", "./", filterName);
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", file.errorString());
        return;
    }
    QTextStream inStream(&file);
    this->m_codeextractA = CodeExtractor();
    if (this->m_codeextractA.extractFromSourceFile(protocolName, &inStream, this->m_cmdArray)) {
      QMessageBox::information(nullptr, "Info", "Code successfully imported!");
    } else {
      QMessageBox::warning(nullptr, "Error", "Code importation failed!");
    }
    file.close();
    // Note destination path
    QFileInfo fileInfo(file);
    outAPath = fileInfo.absoluteDir().absolutePath();
    // Change button state to indicate importation
    QPalette pal;
    pal.setColor(QPalette::Button, QColor(Qt::green));
    ui->pbImportDescA->setPalette(pal);
}

// Import Desc B button action
void MainWindow::on_pbImportDescB_clicked(void) {
    QString protocolName(ui->leProtocolName->text());

    if (protocolName.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol name is empty!");
        return;
    }
    protocolName = CheckAndCorrectInputString(protocolName);
    QDir dir(outBPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QString filterName = protocolName + "_Main_b.c";
    QString fileName = QFileDialog::getOpenFileName(this, "Choose file to load", "./", filterName);
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", file.errorString());
        return;
    }
    QTextStream inStream(&file);
    this->m_codeextractB = CodeExtractor();
    if (this->m_codeextractB.extractFromSourceFile(protocolName, &inStream, this->m_cmdArray)) {
      QMessageBox::information(nullptr, "Info", "Code successfully imported!");
    } else {
      QMessageBox::warning(nullptr, "Error", "Code importation failed!");
    }
    file.close();
    // Note destination path
    QFileInfo fileInfo(file);
    outBPath = fileInfo.absoluteDir().absolutePath();
    // Change button state to indicate importation
    QPalette pal;
    pal.setColor(QPalette::Button, QColor(Qt::green));
    ui->pbImportDescB->setPalette(pal);
}

// Generate Desc button action
void MainWindow::on_pbGenerateDesc_clicked(void) {
    QString protocolName(ui->leProtocolName->text());
    const QString protocolId(ui->leProtocolId->text());

    if (ui->twDescTreeView->currentItem() != nullptr) {
       this->saveCurrentDescTable(ui->twDescTreeView->currentItem());
    }
    if (protocolName.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol name is empty!");
        return;
    }
    protocolName = CheckAndCorrectInputString(protocolName);

    if (protocolId.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol id is empty!");
        return;
    }
    if (this->m_cmdArray.size() <= 0) {
        QMessageBox::warning(nullptr, "Warning", "Protocol has no command!");
        return;
    }
    QString dupName = CheckAttNameDuplicate();
    if (dupName.size() > 0) {
        QMessageBox::warning(nullptr, "Warning", "Duplicate complex attribute name: '"+ dupName +"' found, this is not supported !");
        return;
    }

    // Generate "A" files
    this->m_codegen.generateMainHeader(protocolName, this->m_cmdArray, this->m_codeextractA, outAPath);
    this->m_codegen.generateMain(protocolName, this->m_cmdArray, this->m_codeextractA, true, outAPath);
    this->m_codegen.generateBridgeHeader(protocolName, protocolId, this->m_cmdArray, outAPath);
    this->m_codegen.generateBridge(protocolName, this->m_cmdArray, true, outAPath);
    this->m_codegen.generateDescription(protocolName, this->m_cmdArray, outAPath);

    this->m_rustgen.generateMain(protocolName, this->m_cmdArray, true, rustoutAPath);
    this->m_rustgen.generateBridge(protocolName, protocolId, this->m_cmdArray, true, rustoutAPath);

    // Generate "B" files
    this->m_codegen.generateMainHeader(protocolName, this->m_cmdArray, this->m_codeextractB, outBPath);
    this->m_codegen.generateMain(protocolName, this->m_cmdArray, this->m_codeextractB, false, outBPath);
    this->m_codegen.generateBridgeHeader(protocolName, protocolId, this->m_cmdArray, outBPath);
    this->m_codegen.generateBridge(protocolName, this->m_cmdArray, false, outBPath);
    this->m_codegen.generateDescription(protocolName, this->m_cmdArray, outBPath);

    this->m_rustgen.generateMain(protocolName, this->m_cmdArray, false, rustoutBPath);
    this->m_rustgen.generateBridge(protocolName, protocolId, this->m_cmdArray, false, rustoutBPath);

    QMessageBox::information(nullptr, "Info", "C code has been generated in A: " + outAPath + ", B: " + outBPath 
        + "\nRust code has been generated in A: " + rustoutAPath + ", B: " + rustoutBPath);
}

// Close event action
void MainWindow::closeEvent(QCloseEvent *event) {
   int questionAnswer = QMessageBox::question(this, "Question", "Do you want to leave (unsaved work will be lost)?", QMessageBox::Yes | QMessageBox::No);

   if (questionAnswer == QMessageBox::Yes) {
      event->accept();
   } else {
      event->ignore();
   }
}

// Generate Wiki button action
void MainWindow::on_pbGenerateDoc_clicked(void) {
    QString protocolName(ui->leProtocolName->text());

    if (protocolName.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "Protocol name is empty!");
        return;
    }
    protocolName = CheckAndCorrectInputString(protocolName);

    QString protocolId(ui->leProtocolId->text().trimmed());
    QString protocolDesc(ui->leProtocolDesc->text());

    if (this->m_cmdArray.size() <= 0) {
        QMessageBox::warning(nullptr, "Warning", "Protocol has no command!");
        return;
    }
    QString dupName = CheckAttNameDuplicate();
    if (dupName.size() > 0) {
        QMessageBox::warning(nullptr, "Warning", "Duplicate complex attribute name: '"+ dupName +"' found, this is not supported !");
        return;
    }
    this->m_docgen.generateWikiTable(protocolName, protocolId, protocolDesc, this->m_cmdArray, exportDirPath);
    this->m_docgen.generateDokuWikiTable(protocolName, protocolId, protocolDesc, this->m_cmdArray, exportDirPath);
    this->m_docgen.generateMkdownTable(protocolName, protocolId, protocolDesc, this->m_cmdArray, exportDirPath);
    QMessageBox::information(nullptr, "Info","Documentation was created at: "+ exportDirPath);
}

void MainWindow::on_twDescTableView_cellChanged(int row, int column) {
    if (column == 0) {
        QTableWidgetItem *item = ui->twDescTableView->item(row, column);
        if (item == nullptr) {
            return;
        }
        // Check if valid table name
        QString cellText(item->text());
        if (!cellText.isEmpty()) {
            const QString correctedCellText = CheckAndCorrectInputString(cellText);
            ui->twDescTableView->item(row, column)->setText(correctedCellText);
        }
    }
}

void MainWindow::on_twDescTableView_itemChanged(QTableWidgetItem *item) {
   bool WeHaveEncounteredAnError(false);
   const int ItemColumn(item->column());

   if (ItemColumn > 1) {
      return;
   }
   const QString ItemText(item->text());
   if (ItemText.isEmpty()) {
      return;
   }
   // Check for duplicates on first 2 columns
   for (int RowIndex = 0; RowIndex<ui->twDescTableView->rowCount(); ++RowIndex) {
      QTableWidgetItem *ItemCell(ui->twDescTableView->item(RowIndex, ItemColumn));

      if (item == ItemCell) {
         continue;
      }
      if (ItemCell == nullptr) {
        continue;
      }
      const QString RowItemText(ItemCell->text());

      if ((ItemColumn == 0) && (ItemText != RowItemText)) {
        continue;
      }
      if ((ItemColumn == 1) && (ItemText.toShort(nullptr, 16) != RowItemText.toShort(nullptr, 16))) {
        continue;
      }
      // Indicates error if duplicate
      item->setBackground(Qt::yellow);
      WeHaveEncounteredAnError = true;
   }
   if (!WeHaveEncounteredAnError) {
      // Reinitialize table view
      item->setBackground(Qt::white);
   }
}
