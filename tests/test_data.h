#pragma once

#include <QList>
#include <QString>
#include "command.h"

// Model protocol info
extern QString protocol_name;
extern QString protocol_id;

// Command list
extern const QList<Command *> cmd_list;

// Extractor A data
extern const QString a_unk_inc;
extern const QString a_unk_def;
extern const QString a_unk_pri_fct;
extern const QString a_unk_pub_fct;
extern const QString a_unk_pub_hdr;
extern const QString a_def_cmd_hdl;
extern const QStringList a_cmd_fct;

// Extractor B data
extern const QString b_unk_inc;
extern const QString b_unk_def;
extern const QString b_unk_pri_fct;
extern const QString b_unk_pub_fct;
extern const QString b_unk_pub_hdr;
extern const QString b_def_cmd_hdl;
extern const QStringList b_cmd_fct;

// File test functions
bool openFile(QFile *pFile, QString filedir, QString fileName);
QStringList readFileContent(QFile *pFile);