/**
 * \file test_data.h
 * \brief Common test data variables
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

#pragma once

#include "command.h"
#include <QList>
#include <QString>

// Paths, tests should be run from build/bin/ for these paths to work
#define MODEL_MAIN_A_PATH "../../tests/data/model_main_a.c"
#define MODEL_MAIN_B_PATH "../../tests/data/model_main_b.c"
#define MODEL_DIR_PATH "../../tests/data/"

// Model protocol info
extern QString protocol_name;
extern QString protocol_id;
extern QString protocol_desc;

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