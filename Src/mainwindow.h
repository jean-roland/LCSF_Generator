#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt include
#include <QMainWindow>
#include <QTreeWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QTableWidgetItem>
// Custom include
#include "command.h"
#include "enumtype.h"
#include "codegenerator.h"
#include "codeextractor.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QList<Command *> m_cmdArray;
    CodeGenerator m_codegen;
    CodeExtractor m_codeextractA;
    CodeExtractor m_codeextractB;
    QString outAPath;
    QString outBPath;
    bool treeIsCleared;
    const QString defProtocolName = "Default";
    const QString defoutAPath = "./OutputA";
    const QString defoutBPath = "./OutputB";
    const QString cmdArrayName = "Commands";
    const QString attArraySuffix = "_Attributes";
    const QString cmdDescParentCmdName = "NONE";
    const QString descDirPath = "./Description";
    const QString exportDirPath = "./Export";
    const QString baseTableLabel = "Current table - ";

    bool CheckInputString(QString input);
    QString CorrectInputString(QString input);
    QString CheckAndCorrectInputString(QString input);
    QTreeWidgetItem *findTreeWidgetFromName_Rec(QTreeWidgetItem *parentWidget, QString treeWidgetName);
    QTreeWidgetItem *findTreeWidgetFromName(QString treeWidgetName);
    void clearArray(void);
    void showCommandArray(void);
    void loadCommandArray(void);
    void loadCommandAttArray(QString cmdName);
    void loadAttArray(QTreeWidgetItem *item);
    void showAttributeArray(void);
    void fillCellItems(int rowIdx);
    void fillCellWidgets(int rowIdx);
    QList<QTreeWidgetItem *> getTreeItemChildren(QTreeWidgetItem *parentTreeItem);
    QTreeWidgetItem *getTreeItemChild(QList<QTreeWidgetItem *> parentTreeItemChildren, QString childName);
    void deleteNoLongerExistingTreeItemChildren(QTreeWidgetItem *parentTreeItem, QList<QTreeWidgetItem *> noLongerExistingChildren);
    void updateDescTreeCmd(void);
    void updateDescTreeCmdAtt(QString cmdName);
    void updateDescTreeAtt(QTreeWidgetItem *parentTreeItem);
    bool saveCurrentDescTable(QTreeWidgetItem *item);
    QStringList getTableNames(void);
    QList<short> getTableCodes(void);
    NS_DirectionType::T_DirectionType getDirectionTypeFromComboBox(QComboBox *comboBox);
    bool saveCommandTable(void);
    NS_AttDataType::T_AttDataType getAttDataTypeFromComboBox(QComboBox *comboBox);
    bool saveCmdAttTable(QString cmdName);
    QTreeWidgetItem *findTreeWidgetItem_REC(QTreeWidgetItem *parentTreeItem, QString tableName);
    QTreeWidgetItem *findTreeWidgetItem(QString tableName);
    Attribute *findAttStorage_REC(QTreeWidgetItem *item, QList<Attribute *> attList);
    Attribute *findAttStorage(QTreeWidgetItem *item);
    bool saveAttTable(QTreeWidgetItem *item);
    void deleteCommand(QString cmdName);
    void deleteCommandAtt(QString cmdName, QString attName);
    void deleteAttribute(QString attName, QTreeWidgetItem *attItem);
    void saveDesc_REC_Json(QJsonArray &attributes, QList<Attribute *> attList2Save);
    void loadAtt_REC_Json(Command *pParentCmd, Attribute *pParentAtt, const QJsonObject &attribut);
    void clearData(void);
    void clearTree(void);

private slots:
    void on_actionNew_protocol_triggered(void);
    void on_actionSave_protocol_triggered(void);
    void on_actionLoad_protocol_triggered(void);
    void on_actionDocumentation_triggered(void);
    void on_actionAbout_triggered(void);
    void on_twDescTreeView_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_pbSaveTable_clicked(void);
    void on_pbAddTableLine_clicked(void);
    void on_pbDeleteTableLine_clicked(void);
    void on_pbRefreshTable_clicked(void);
    void on_pbSortTable_clicked(void);
    void on_pbGenerateDesc_clicked(void);
    void on_pbImportDescA_clicked(void);
    void on_pbImportDescB_clicked(void);
    void on_pbGenerateDoc_clicked(void);
    void on_twDescTableView_cellChanged(int row, int column);
    void on_twDescTableView_itemChanged(QTableWidgetItem *item);
    void closeEvent(QCloseEvent *event);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif // MAINWINDOW_H
