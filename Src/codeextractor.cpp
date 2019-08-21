#include "codeextractor.h"

CodeExtractor::CodeExtractor() {
    this->m_extractionComplete = false;
    this->m_state = EXTRACT_INCLUDES;
    this->m_unknownIncludes = QString();
    this->m_unknownDefinitions = QString();
    this->m_unknownPrivateFunctions = QString();
    this->m_unknownPublicFunctions = QString();
    this->m_unknownPublicFunctionsHeaders = QString();
    this->m_commandFunctions = QStringList();
}

void CodeExtractor::extractIncludes(QTextStream *pInStream) {

    while (!pInStream->atEnd()) {
        QString currentLine = pInStream->readLine();
        this->m_unknownIncludes.append(currentLine + "\n");

        if (currentLine.contains("Definitions")) {
            this->m_state = EXTRACT_DEFINITIONS;
            break;
        }
    }
}

void CodeExtractor::extractDefinitions(QTextStream *pInStream) {

    while (!pInStream->atEnd()) {
        QString currentLine = pInStream->readLine();
        this->m_unknownIncludes.append(currentLine + "\n");

        if (currentLine.contains("Private Functions")) {
            this->m_state = EXTRACT_PRIVATE_FUNCTIONS;
            break;
        }
    }
}

void CodeExtractor::extractPrivateFunctions(QString protocolName, QTextStream *pInStream, QList<Command *> cmdList) {

    for (int idx = 0; idx < cmdList.size(); idx++) {
        this->m_commandFunctions.append("");
    }
    while (!pInStream->atEnd()) {
        QString currentLine = pInStream->readLine();
        bool functionFinished = false;
        bool functionBegan = false;
        bool isCommandFunction = false;
        QString functionBuffer = "";
        int bracketCounter = 0;
        int cmdIdx = -1;

        if (currentLine.trimmed().compare("") == 0) {
            continue;
        }
        while (!pInStream->atEnd() && !functionFinished) {
            functionBuffer.append(currentLine + "\n");

            if (currentLine.contains("Public Functions")) {
                functionBuffer = "";
                isCommandFunction = false;
                this->m_state = EXTRACT_PUBLIC_FUNCTIONS;
                break;
            }
            if ((currentLine.contains(protocolName + "Execute")) && !currentLine.contains("\\fn")) {
                for (int idx = 0; idx < cmdList.size(); idx++) {
                    QString commandName = cmdList.at(idx)->getName();

                    if (currentLine.contains(commandName)) {
                        cmdIdx = idx;
                        isCommandFunction = true;
                        break;
                    }
                }
            }
            if (currentLine.contains("{")) {
                if (bracketCounter == 0) {
                    functionBegan = true;
                }
                bracketCounter++;
            }
            if (currentLine.contains("}")) {
                bracketCounter--;
            }
            if ((bracketCounter == 0) && functionBegan) {
                functionFinished = true;
                break;
            } else if(bracketCounter < 0) {
                qDebug() << "Error: Negative bracket number!";
                functionBuffer = "";
                return;
            }
            currentLine = pInStream->readLine();
        }
        if (isCommandFunction && (cmdIdx >= 0)) {
            this->m_commandFunctions.replace(cmdIdx, functionBuffer + "\n");
        } else if (functionBuffer.compare("") != 0) {
            this->m_unknownPrivateFunctions.append(functionBuffer + "\n");
        } else {
            break;
        }
    }
}

void CodeExtractor::extractDefaultCommandHandler(QTextStream *pInStream) {

    this->m_defaultCommandHandler = "";
    int bracketCounter = 1;
    bool functionBegan = false;

    while (!pInStream->atEnd()) {
        QString currentLine = pInStream->readLine();

        if (currentLine.contains("{")) {
            if (bracketCounter == 0) {
                functionBegan = true;
            }
            bracketCounter++;
        }
        if (currentLine.contains("}")) {
            bracketCounter--;
        }
        if((bracketCounter <= 0) && functionBegan) {
            return;
        }
        if (currentLine.contains("default:")) {
            while (!pInStream->atEnd()) {
                currentLine = pInStream->readLine();

                if (currentLine.contains("break;")) {
                    break;
                } else {
                    this->m_defaultCommandHandler.append(currentLine + "\n");
                }
            }
        }
    }
}

void CodeExtractor::extractPublicFunctions(QString protocolName, QTextStream *pInStream) {

    while (!pInStream->atEnd()) {
        QString currentLine = pInStream->readLine();
        bool functionFinished = false;
        bool functionBegan = false;
        QString functionBuffer = "";
        int bracketCounter = 0;

        if (currentLine.trimmed().compare("") == 0) {
            continue;
        }
        while (!pInStream->atEnd() && !functionFinished) {
            if (currentLine.contains(protocolName + "_MainExecute") && !currentLine.contains("\\fn")) {
                this->extractDefaultCommandHandler(pInStream);
                functionBuffer = "";
                break;
            } else {
                functionBuffer.append(currentLine + "\n");

                if (currentLine.contains("{")) {
                    if (bracketCounter == 0) {
                        functionBegan = true;
                    }
                    bracketCounter++;
                }
                if (currentLine.contains("}")) {
                    bracketCounter--;
                }
            }
            if ((bracketCounter == 0) && functionBegan) {
                functionFinished = true;
                break;
            } else if(bracketCounter < 0) {
                qDebug() << "Error: Negative bracket number!";
                functionBuffer = "";
                return;
            }
            currentLine = pInStream->readLine();
        }
        if (functionBuffer.compare("") != 0) {
            this->extractPublicFunctionHeader(functionBuffer);
            this->m_unknownPublicFunctions.append(functionBuffer + "\n");
        }
    }
    this->m_state = EXTRACT_END;
}

bool CodeExtractor::isHeader(QString text) {
    QString trText = text.trimmed();
    if(trText.size() <= 2) {
        return true;
    }
    QChar firstCh = trText.at(0);
    return ((firstCh == '*') || (firstCh == '/'));
}

void CodeExtractor::extractPublicFunctionHeader(QString functionBuffer) {
    QStringList functionLinesBuffer = functionBuffer.split("\n");
    QString functionHeaderBuffer = QString();
    bool isHeaderStarted = false;
    bool isHeaderFinished = false;

    for (int i = 0; i < functionLinesBuffer.size(); i++) {
        QString currentLine = functionLinesBuffer.at(i);

        if (!isHeader(currentLine) && (!isHeaderStarted || (isHeaderStarted && isHeaderFinished))) {
            isHeaderStarted = false;
            isHeaderFinished = false;
            QRegExp re(R"([\{])");
            functionHeaderBuffer.append(currentLine.replace(re,"").trimmed() + ";\n\n");
            this->m_unknownPublicFunctionsHeaders.append(functionHeaderBuffer);
            break;
        } else if (currentLine.contains("/*")) {
            isHeaderStarted = true;
        }
        if (currentLine.contains("*/") && isHeaderStarted) {
            isHeaderFinished = true;
        }
        functionHeaderBuffer.append(currentLine + "\n");
    }
}

void CodeExtractor::extractFromSourceFile(QString protocolName, QTextStream *pInStream, QList<Command *> cmdList) {

    if (pInStream == nullptr) {
        QMessageBox::warning(nullptr, "Error", "Nothing to import!");
        return;
    }
    while ((!pInStream->atEnd()) && (this->m_state != EXTRACT_END)) {
        switch (this->m_state) {
            case EXTRACT_INCLUDES:
                this->extractIncludes(pInStream);
            break;

            case EXTRACT_DEFINITIONS:
                this->extractDefinitions(pInStream);
            break;

            case EXTRACT_PRIVATE_FUNCTIONS:
                this->extractPrivateFunctions(protocolName, pInStream, cmdList);
            break;

            case EXTRACT_PUBLIC_FUNCTIONS:
                this->extractPublicFunctions(protocolName, pInStream);
            break;

            case EXTRACT_END:
            break;
        }
    }
    this->m_extractionComplete = true;
    QString msgString = "Code successfully imported!";
    QMessageBox::information(nullptr,"Info", msgString);
}

bool CodeExtractor::getExtractionComplete() {
    return this->m_extractionComplete;
}
QString CodeExtractor::getUnknownIncludes() {
    return this->m_unknownIncludes;
}
QString CodeExtractor::getUnknownDefinitions() {
    return this->m_unknownDefinitions;
}
QString CodeExtractor::getUnknownPrivateFunctions() {
    return this->m_unknownPrivateFunctions;
}
QStringList CodeExtractor::getCommandFunctions() {
    return this->m_commandFunctions;
}
QString CodeExtractor::getUnknownPublicFunctions() {
    return this->m_unknownPublicFunctions;
}
QString CodeExtractor::getUnknownPublicFunctionsHeaders() {
    return this->m_unknownPublicFunctionsHeaders;
}
QString CodeExtractor::getDefaultCommandHandler() {
    return this->m_defaultCommandHandler;
}
