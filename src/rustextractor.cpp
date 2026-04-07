/**
 * \file rustextractor.cpp
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

#include "rustextractor.h"

RustExtractor::RustExtractor() {
    this->m_extractionComplete = false;
    this->m_extraUses = QString();
    this->m_customDefinitions = QString();
    this->m_commandFunctions = QStringList();
    this->m_defaultCommandHandler = QString();
    this->m_customPublicFunctions = QString();
    this->m_trailingContent = QString();
}

// Extract a complete brace-balanced function starting from firstLine.
// Note: brace counting is simple and does not handle braces inside string literals.
QString RustExtractor::extractFunction(QString firstLine, QTextStream *pIn) {
    QString buffer = firstLine + "\n";
    int braceCount = 0;
    bool functionBegan = false;

    for (QChar ch : firstLine) {
        if (ch == '{') { braceCount++; functionBegan = true; }
        else if (ch == '}') { braceCount--; }
    }

    while (!(functionBegan && braceCount == 0) && !pIn->atEnd()) {
        QString line = pIn->readLine();
        buffer.append(line + "\n");
        for (QChar ch : line) {
            if (ch == '{') { braceCount++; functionBegan = true; }
            else if (ch == '}') { braceCount--; }
        }
    }
    return buffer;
}

// Parse a full execute_cmd function string and extract the body of the "_ =>" arm.
void RustExtractor::extractDefaultHandler(const QString &fullFunction) {
    QStringList lines = fullFunction.split('\n');
    bool inDefaultArm = false;
    int armBraceDepth = 0;

    for (const QString &line : lines) {
        if (!inDefaultArm) {
            if (line.trimmed().startsWith("_ =>")) {
                inDefaultArm = true;
                for (QChar ch : line) {
                    if (ch == '{') armBraceDepth++;
                    else if (ch == '}') armBraceDepth--;
                }
                // armBraceDepth == 0 means single-line `_ => {}`, nothing to collect
            }
        } else {
            if (armBraceDepth <= 0) break;
            for (QChar ch : line) {
                if (ch == '{') armBraceDepth++;
                else if (ch == '}') armBraceDepth--;
            }
            if (armBraceDepth <= 0) {
                // This is the closing `}` of the arm — don't include it
                break;
            }
            this->m_defaultCommandHandler.append(line + "\n");
        }
    }
}

// Collect extra uses added by the user between "// --- Custom uses ---"
// and the "/// Command enum" doc comment.
bool RustExtractor::extractUses(QTextStream *pIn) {
    bool markerFound = false;

    while (!pIn->atEnd()) {
        QString line = pIn->readLine();
        if (!markerFound) {
            if (line == "// --- Custom uses ---") {
                markerFound = true;
            }
        } else {
            if (line.startsWith("/// Command enum")) {
                return true;
            }
            this->m_extraUses.append(line + "\n");
        }
    }
    return false; // Marker or CmdEnum not found
}

// Skip enum/struct definitions, then collect custom definitions added by the user
// between "// --- Custom definitions ---" and the first "fn execute_" function.
// Returns the first "fn execute_" line via firstExecLine.
bool RustExtractor::extractDefinitions(QTextStream *pIn, QString &firstExecLine) {
    bool markerFound = false;

    while (!pIn->atEnd()) {
        QString line = pIn->readLine();
        if (!markerFound) {
            if (line == "// --- Custom definitions ---") {
                markerFound = true;
            }
        } else {
            if (line.startsWith("fn execute_")) {
                firstExecLine = line;
                return true;
            }
            this->m_customDefinitions.append(line + "\n");
        }
    }
    return false; // Marker or first execute function not found
}

// Extract each "fn execute_X" function and match it to cmdList by name.
// When "fn execute_cmd" is found, extract it and parse the "_ =>" arm.
void RustExtractor::extractExecuteFunctions(QTextStream *pIn, QString firstLine, QList<Command *> cmdList) {
    QString currentLine = firstLine;
    const QString execPrefix = "fn execute_";

    while (true) {
        // Skip blank lines and doc comment lines to find the next function
        while (currentLine.trimmed().isEmpty() || currentLine.trimmed().startsWith("///")) {
            if (pIn->atEnd()) return;
            currentLine = pIn->readLine();
        }

        if (currentLine.startsWith("fn execute_cmd(")) {
            QString fullFunction = this->extractFunction(currentLine, pIn);
            this->extractDefaultHandler(fullFunction);
            return;
        }

        if (currentLine.startsWith(execPrefix)) {
            int prefixLen = execPrefix.length();
            int parenPos = currentLine.indexOf('(', prefixLen);
            if (parenPos > prefixLen) {
                QString execName = currentLine.mid(prefixLen, parenPos - prefixLen);
                QString fullFunction = this->extractFunction(currentLine, pIn);
                for (int idx = 0; idx < cmdList.size(); idx++) {
                    if (cmdList.at(idx)->getName().toLower() == execName) {
                        this->m_commandFunctions.replace(idx, fullFunction + "\n");
                        break;
                    }
                }
            }
            currentLine = pIn->atEnd() ? "" : pIn->readLine();
            continue;
        }

        // Unknown line — stop
        break;
    }
}

// Collect the custom public functions (init and process_cmd) between
// "// --- Custom public functions ---" and EOF (or "#[cfg(test)]").
// Anything from "#[cfg(test)]" onward is stored as trailing content.
void RustExtractor::extractPublicFunctions(QTextStream *pIn) {
    bool markerFound = false;

    while (!pIn->atEnd()) {
        QString line = pIn->readLine();
        if (!markerFound) {
            if (line == "// --- Custom public functions ---") {
                markerFound = true;
            }
        } else {
            if (line.startsWith("#[cfg(test)]")) {
                this->m_trailingContent.append(line + "\n");
                while (!pIn->atEnd()) {
                    this->m_trailingContent.append(pIn->readLine() + "\n");
                }
                return;
            }
            this->m_customPublicFunctions.append(line + "\n");
        }
    }
}

bool RustExtractor::extractFromSourceFile(QString protocolName, QTextStream *pIn, QList<Command *> cmdList) {
    if (pIn == nullptr) return false;

    // Pre-size command functions list with empty entries
    for (int i = 0; i < cmdList.size(); i++) {
        this->m_commandFunctions.append("");
    }

    if (!this->extractUses(pIn)) return false;

    QString firstExecLine;
    if (!this->extractDefinitions(pIn, firstExecLine)) return false;

    this->extractExecuteFunctions(pIn, firstExecLine, cmdList);
    this->extractPublicFunctions(pIn);

    this->m_extractionComplete = true;
    return true;
}

bool RustExtractor::getExtractionComplete() const {
    return this->m_extractionComplete;
}
QString RustExtractor::getExtraUses() const {
    return this->m_extraUses;
}
QString RustExtractor::getCustomDefinitions() const {
    return this->m_customDefinitions;
}
QStringList RustExtractor::getCommandFunctions() const {
    return this->m_commandFunctions;
}
QString RustExtractor::getDefaultCommandHandler() const {
    return this->m_defaultCommandHandler;
}
QString RustExtractor::getCustomPublicFunctions() const {
    return this->m_customPublicFunctions;
}
QString RustExtractor::getTrailingContent() const {
    return this->m_trailingContent;
}
