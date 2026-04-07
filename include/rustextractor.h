/**
 * \file rustextractor.h
 * \brief Rust code extractor module
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

#ifndef RUSTEXTRACTOR_H
#define RUSTEXTRACTOR_H

#include "command.h"
#include <QDebug>
#include <QList>
#include <QString>
#include <QTextStream>

class RustExtractor {
  private:
    bool m_extractionComplete;
    QString m_extraUses;
    QString m_customDefinitions;
    QStringList m_commandFunctions;
    QString m_executeCmdFunction;
    QString m_customPublicFunctions;
    QString m_trailingContent;

    QString extractFunction(QString firstLine, QTextStream *pIn);
    bool extractUses(QTextStream *pIn);
    bool extractDefinitions(QTextStream *pIn, QString &firstExecLine);
    void extractExecuteFunctions(QTextStream *pIn, QString firstLine, QList<Command *> cmdList);
    void extractPublicFunctions(QTextStream *pIn);

  public:
    RustExtractor();
    bool extractFromSourceFile(QString protocolName, QTextStream *pIn, QList<Command *> cmdList);
    bool getExtractionComplete() const;
    QString getExtraUses() const;
    QString getCustomDefinitions() const;
    QStringList getCommandFunctions() const;
    QString getExecuteCmdFunction() const;
    QString getCustomPublicFunctions() const;
    QString getTrailingContent() const;
};

#endif // RUSTEXTRACTOR_H
