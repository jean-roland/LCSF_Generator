#pragma once

#include <QFile>
#include "command.h"

class DescHandler {
private:

public:
    DescHandler();
    static void load_desc(QFile& file, QList<Command *>& cmdArray, QString& protocolName, QString& protocolId);
    static bool save_desc(QString filename, QList<Command *>, QString protocolName, QString protocolId);
};
