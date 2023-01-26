/**
 * \file codeextractor.h
 * \brief Code extractor module
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

#ifndef CODEEXTRACTOR_H
#define CODEEXTRACTOR_H

#include <QString>
#include <QList>
#include <QTextStream>
#include <QDebug>
#include "command.h"

class CodeExtractor {
private:
    typedef enum {
        EXTRACT_INCLUDES,
        EXTRACT_DEFINITIONS,
        EXTRACT_PRIVATE_FUNCTIONS,
        EXTRACT_PUBLIC_FUNCTIONS,
        EXTRACT_END,
    }T_CodeExtractorStates;

    bool m_extractionComplete;
    T_CodeExtractorStates m_state;
    QString m_unknownIncludes;
    QString m_unknownDefinitions;
    QString m_unknownPrivateFunctions;
    QStringList m_commandFunctions;
    QString m_unknownPublicFunctions;
    QString m_unknownPublicFunctionsHeaders;
    QString m_defaultCommandHandler;

    void extractIncludes(QTextStream *pInStream);
    void extractDefinitions(QTextStream *pInStream);
    void extractPrivateFunctions(QString protocolName, QTextStream *pInStream , QList<Command *> cmdList);
    void extractPublicFunctionHeader(QString functionBuffer);
    void extractDefaultCommandHandler(QTextStream *pInStream);
    void extractPublicFunctions(QString protocolName, QTextStream *pInStream);
    bool isHeader(QString text);

public:
    CodeExtractor();
    bool extractFromSourceFile(QString protocolName, QTextStream *pInStream , QList<Command *> cmdList);
    bool getExtractionComplete();
    QString getUnknownIncludes();
    QString getUnknownDefinitions();
    QString getUnknownPrivateFunctions();
    QStringList getCommandFunctions();
    QString getUnknownPublicFunctions();
    QString getUnknownPublicFunctionsHeaders();
    QString getDefaultCommandHandler();
};

#endif // CODEEXTRACTOR_H
