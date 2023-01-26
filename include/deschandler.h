/**
 * \file deschandler.h
 * \brief Description file handler module
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

#include <QFile>
#include "command.h"

class DescHandler {
private:

public:
    DescHandler();
    static void load_desc(QFile& file, QList<Command *>& cmdArray, QString& protocolName, QString& protocolId);
    static bool save_desc(QString filename, QList<Command *> cmdArray, QString protocolName, QString protocolId);
};