#pragma once

#include <QList>
#include <QString>
#include "command.h"

// Model protocol info
extern QString protocol_name;
extern QString protocol_id;

// Command list
extern const QList<Command *> cmd_list;

// Extractor data
extern const QString unk_inc;
extern const QString unk_def;
extern const QString unk_pri_fct;
extern const QString unk_pub_fct;
extern const QString unk_pub_hdr;
extern const QString def_cmd_hdl;
extern const QStringList cmd_fct;

// File test functions
bool openFile(QFile *pFile, QString filedir, QString fileName);
QStringList readFileContent(QFile *pFile);