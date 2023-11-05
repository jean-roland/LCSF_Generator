/**
 * \file test_data.cpp
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

#include <iostream>
#include <QFile>
#include <QTextStream>

#include "enumtype.h"
#include "attribute.h"
#include "test_data.h"

// File test functions
bool openFile(QFile *pFile, QString filedir, QString fileName) {
    pFile->setFileName(filedir + fileName);

    if(!pFile->open(QIODevice::ReadOnly)) {
        std::cout << " Couldn't open " + fileName.toStdString() + ": " + pFile->errorString().toStdString();
        return false;
    }
    return true;
}

QStringList readFileContent(QFile *pFile) {
    QTextStream file_content(pFile);
    QStringList output;

    while (!file_content.atEnd()) {
        QString line = file_content.readLine();
        output.append(line);
    }
    return output;
}

// Model protocol info
QString protocol_name = "Test";
QString protocol_id = "55";
QString protocol_desc = "The test protocol used to test/bench the lcsf ecosystem.";

// Model command/attributes
Command SC1("SC1", 0, false, NS_DirectionType::A_TO_B, "Simple command A to B");
Command SC2("SC2", 1, false, NS_DirectionType::B_TO_A, "Simple command B to A");
Command SC3("SC3", 2, false, NS_DirectionType::BIDIRECTIONAL, "Simple command bidirectional");

// CC1
Attribute CC1_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CC1_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CC1_SA3("SA3", 2, false, NS_AttDataType::UINT32, "Non optional uint32 simple attribute");
Attribute CC1_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non optional byte array simple attribute");
Attribute CC1_SA5("SA5", 4, false, NS_AttDataType::STRING, "Non optional string simple attribute");
Attribute CC1_SA6("SA6", 5, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CC1_SA7("SA7", 6, true, NS_AttDataType::UINT16, "Optional uint16 simple attribute");
Attribute CC1_SA8("SA8", 7, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");
Attribute CC1_SA9("SA9", 8, true, NS_AttDataType::BYTE_ARRAY, "Optional byte array simple attribute");
Attribute CC1_SA10("SA10", 9, true, NS_AttDataType::STRING, "Optional string simple attribute");
Attribute CC1_SA11("SA11", 10, false, NS_AttDataType::UINT64, "Non optional uint64 simple attribute");
Attribute CC1_SA12("SA12", 11, false, NS_AttDataType::FLOAT32, "Non optional float32 simple attribute");
Attribute CC1_SA13("SA13", 12, false, NS_AttDataType::FLOAT64, "Non optional float64 simple attribute");
QList<Attribute *> CC1_att_array = {&CC1_SA1, &CC1_SA2, &CC1_SA3, &CC1_SA4, &CC1_SA5, &CC1_SA6, &CC1_SA7, &CC1_SA8, &CC1_SA9, &CC1_SA10, &CC1_SA11, &CC1_SA12, &CC1_SA13};
Command CC1("CC1", 3, true, NS_DirectionType::A_TO_B, CC1_att_array, "Complex command no sub-attributes A to B");

// CC2
Attribute CC2_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CC2_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CC2_SA3("SA3", 2, false, NS_AttDataType::UINT32, "Non optional uint32 simple attribute");
Attribute CC2_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non optional byte array simple attribute");
Attribute CC2_SA5("SA5", 4, false, NS_AttDataType::STRING, "Non optional string simple attribute");
Attribute CC2_SA6("SA6", 5, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CC2_SA7("SA7", 6, true, NS_AttDataType::UINT16, "Optional uint16 simple attribute");
Attribute CC2_SA8("SA8", 7, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");
Attribute CC2_SA9("SA9", 8, true, NS_AttDataType::BYTE_ARRAY, "Optional byte array simple attribute");
Attribute CC2_SA10("SA10", 9, true, NS_AttDataType::STRING, "Optional string simple attribute");
Attribute CC2_SA11("SA11", 10, false, NS_AttDataType::UINT64, "Non optional uint64 simple attribute");
Attribute CC2_SA12("SA12", 11, false, NS_AttDataType::FLOAT32, "Non optional float32 simple attribute");
Attribute CC2_SA13("SA13", 12, false, NS_AttDataType::FLOAT64, "Non optional float64 simple attribute");
QList<Attribute *> CC2_att_array = {&CC2_SA1, &CC2_SA2, &CC2_SA3, &CC2_SA4, &CC2_SA5, &CC2_SA6, &CC2_SA7, &CC2_SA8, &CC2_SA9, &CC2_SA10, &CC2_SA11, &CC2_SA12, &CC2_SA13};
Command CC2("CC2", 4, true, NS_DirectionType::B_TO_A, CC2_att_array, "Complex command no sub-attributes B to A");

// CC3
Attribute CC3_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CC3_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CC3_SA3("SA3", 2, false, NS_AttDataType::UINT32, "Non optional uint32 simple attribute");
Attribute CC3_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non optional byte array simple attribute");
Attribute CC3_SA5("SA5", 4, false, NS_AttDataType::STRING, "Non optional string simple attribute");
Attribute CC3_SA6("SA6", 5, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CC3_SA7("SA7", 6, true, NS_AttDataType::UINT16, "Optional uint16 simple attribute");
Attribute CC3_SA8("SA8", 7, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");
Attribute CC3_SA9("SA9", 8, true, NS_AttDataType::BYTE_ARRAY, "Optional byte array simple attribute");
Attribute CC3_SA10("SA10", 9, true, NS_AttDataType::STRING, "Optional string simple attribute");
Attribute CC3_SA11("SA11", 10, false, NS_AttDataType::UINT64, "Non optional uint64 simple attribute");
Attribute CC3_SA12("SA12", 11, false, NS_AttDataType::FLOAT32, "Non optional float32 simple attribute");
Attribute CC3_SA13("SA13", 12, false, NS_AttDataType::FLOAT64, "Non optional float64 simple attribute");
QList<Attribute *> CC3_att_array = {&CC3_SA1, &CC3_SA2, &CC3_SA3, &CC3_SA4, &CC3_SA5, &CC3_SA6, &CC3_SA7, &CC3_SA8, &CC3_SA9, &CC3_SA10, &CC3_SA11, &CC3_SA12, &CC3_SA13};
Command CC3("CC3", 5, true, NS_DirectionType::BIDIRECTIONAL, CC3_att_array, "Complex command no sub-attributes bidirectional");

// CC4
Attribute CC4_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CA1_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CA1_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CA1_SA3("SA3", 2, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");

QList<Attribute *> CA1_subatt_array = {&CA1_SA1, &CA1_SA2, &CA1_SA3};
Attribute CC4_CA1("CA1", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA1_subatt_array, "Non optional complex attribute");

Attribute CA2_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA3_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA4_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non-optional byte array simple attribute");

QList<Attribute *> CA4_subatt_array = {&CA4_SA4};
Attribute CA3_CA4("CA4", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA4_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA3_subatt_array = {&CA3_SA1, &CA3_CA4};
Attribute CA2_CA3("CA3", 11, false, NS_AttDataType::SUB_ATTRIBUTES, CA3_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA2_subatt_array = {&CA2_SA1, &CA2_CA3};
Attribute CC4_CA2("CA2", 11, true, NS_AttDataType::SUB_ATTRIBUTES, CA2_subatt_array, "Optional complex attribute");

QList<Attribute *> CC4_att_array = {&CC4_SA1, &CC4_CA1, &CC4_CA2};
Command CC4("CC4", 6, true, NS_DirectionType::A_TO_B, CC4_att_array, "Complex command with sub-attributes A to B");

// CC5
Attribute CC5_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CA5_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CA5_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CA5_SA3("SA3", 2, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");

QList<Attribute *> CA5_subatt_array = {&CA5_SA1, &CA5_SA2, &CA5_SA3};
Attribute CC5_CA5("CA5", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA5_subatt_array, "Non optional complex attribute");

Attribute CA6_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA7_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA8_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non-optional byte array simple attribute");

QList<Attribute *> CA8_subatt_array = {&CA8_SA4};
Attribute CA7_CA8("CA8", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA8_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA7_subatt_array = {&CA7_SA1, &CA7_CA8};
Attribute CA6_CA7("CA7", 11, false, NS_AttDataType::SUB_ATTRIBUTES, CA7_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA6_subatt_array = {&CA6_SA1, &CA6_CA7};
Attribute CC5_CA6("CA6", 11, true, NS_AttDataType::SUB_ATTRIBUTES, CA6_subatt_array, "Optional complex attribute");

QList<Attribute *> CC5_att_array = {&CC5_SA2, &CC5_CA5, &CC5_CA6};
Command CC5("CC5", 7, true, NS_DirectionType::B_TO_A, CC5_att_array, "Complex command with sub-attributes B to A");

// CC6
Attribute CC6_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non optional byte array simple attribute");
Attribute CA9_SA1("SA1", 0, false, NS_AttDataType::UINT8, "Non optional uint8 simple attribute");
Attribute CA9_SA2("SA2", 1, false, NS_AttDataType::UINT16, "Non optional uint16 simple attribute");
Attribute CA9_SA3("SA3", 2, true, NS_AttDataType::UINT32, "Optional uint32 simple attribute");

QList<Attribute *> CA9_subatt_array = {&CA9_SA1, &CA9_SA2, &CA9_SA3};
Attribute CC6_CA9("CA9", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA9_subatt_array, "Non optional complex attribute");

Attribute CA10_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA11_SA1("SA1", 0, true, NS_AttDataType::UINT8, "Optional uint8 simple attribute");
Attribute CA12_SA4("SA4", 3, false, NS_AttDataType::BYTE_ARRAY, "Non-optional byte array simple attribute");

QList<Attribute *> CA12_subatt_array = {&CA12_SA4};
Attribute CA11_CA12("CA12", 10, false, NS_AttDataType::SUB_ATTRIBUTES, CA12_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA11_subatt_array = {&CA11_SA1, &CA11_CA12};
Attribute CA10_CA11("CA11", 11, false, NS_AttDataType::SUB_ATTRIBUTES, CA11_subatt_array, "Non-optional complex attribute");

QList<Attribute *> CA10_subatt_array = {&CA10_SA1, &CA10_CA11};
Attribute CC6_CA10("CA10", 11, true, NS_AttDataType::SUB_ATTRIBUTES, CA10_subatt_array, "Optional complex attribute");

QList<Attribute *> CC6_att_array = {&CC6_SA4, &CC6_CA9, &CC6_CA10};
Command CC6("CC6", 8, true, NS_DirectionType::BIDIRECTIONAL, CC6_att_array, "Complex command with sub-attributes bidirectional");

// Command list
const QList<Command *> cmd_list = {&SC1, &SC2, &SC3, &CC1, &CC2, &CC3, &CC4, &CC5, &CC6};

// Extractor A data
const QString a_unk_inc = R"(/**
 * \file Test_Main.c
 * \brief Test protocol module (A)
 * \author LCSF Generator v1.3
 *
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_Test.h"
#include "Test_Main.h"
#include <LCSF_Config.h>

// *** Definitions ***
)";

const QString a_unk_def = R"(// --- Private Macros ---
#define TEST_ARRAY_SIZE 5

// --- Private Types ---
typedef struct _test_info {
    uint8_t *pSendBuffer;
    uint16_t buffSize;
    test_cmd_payload_t SendCmdPayload;
    bool hasPattern;
    const uint8_t *pattern;
} test_info_t;

// --- Private Function Prototypes ---
// Generated functions
static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload);
static bool TestExecuteSC2(void);
static bool TestExecuteSC3(void);
static bool TestExecuteCC2(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC5(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static test_info_t TestInfo;

// *** End Definitions ***

// *** Private Functions ***
)";

const QString a_unk_pri_fct = R"(/**
 * \fn static void Test_FillArray(uint8_t *pArray, bool incr)
 * \brief Fill an array with data
 *
 * \param pArray pointer to array
 * \param incr indicates if function increments or decrements if no pattern
 */
static void Test_FillArray(uint8_t *pArray, bool incr) {
    if (TestInfo.hasPattern) {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            pArray[idx] = TestInfo.pattern[idx];
        }
    } else {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            if (incr) {
                pArray[idx] += 1;
            } else {
                pArray[idx] -= 1;
            }
        }
    }
}

/**
 * \fn static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload)
 * \brief Send a command
 *
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not=
 * \return bool: true if operation was a success=
 */
static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload) {
    if (cmdName >= TEST_CMD_COUNT) {
        return false;
    }
    int msgSize = 0;
    if (hasPayload) {
        test_cmd_payload_t *pCmdPayload = &TestInfo.SendCmdPayload;
        msgSize = LCSF_Bridge_TestEncode(cmdName, pCmdPayload, TestInfo.pSendBuffer, TestInfo.buffSize);
    } else {
        msgSize = LCSF_Bridge_TestEncode(cmdName, NULL, TestInfo.pSendBuffer, TestInfo.buffSize);
    }
    if (msgSize <= 0) {
        return false;
    }
    // TODO Pass buffer to send function e.g: return DummySend(ExampleInfo.sendBuffer, (size_t)msgSize);
    return true;
}

)";

const QString a_unk_pub_fct = R"(/**
 * \fn bool Test_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool Test_MainInit(uint8_t *pBuffer, size_t buffSize) {
    if (pBuffer == NULL) {
        return false;
    }
    // Note infos
    TestInfo.pSendBuffer = pBuffer;
    TestInfo.buffSize = buffSize;
    return true;
}

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern) {
    if (pattern == NULL) {
        return;
    }
    TestInfo.hasPattern = true;
    TestInfo.pattern = pattern;
}

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void) {
    TestInfo.hasPattern = false;
    TestInfo.pattern = NULL;
}

)";

const QString a_unk_pub_hdr = R"(/**
 * \fn bool Test_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool Test_MainInit(uint8_t *pBuffer, size_t buffSize);

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern);

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void);

)";

const QString a_def_cmd_hdl = "            return false;\n";

const QString a_fct_sc2 = R"(/**
 * \fn static bool TestExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool TestExecuteX(test_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool TestExecuteSC2(void) {
    return TestSendCommand(TEST_CMD_SC1, false);
}

)";

const QString a_fct_sc3 = R"(static bool TestExecuteSC3(void) {
    return TestSendCommand(TEST_CMD_SC3, false);
}

)";

const QString a_fct_cc2 = R"(static bool TestExecuteCC2(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cc2_sa1 = 0;
    uint16_t m_cc2_sa2 = 0;
    uint32_t m_cc2_sa3 = 0;
    uint8_t *m_cc2_sa4 = NULL;
    char *m_cc2_sa5 = NULL;
    bool m_cc2_sa6_isHere = false;
    uint8_t m_cc2_sa6 = 0;
    bool m_cc2_sa7_isHere = false;
    uint16_t m_cc2_sa7 = 0;
    bool m_cc2_sa8_isHere = false;
    uint32_t m_cc2_sa8 = 0;
    bool m_cc2_sa9_isHere = false;
    uint8_t *m_cc2_sa9 = NULL;
    bool m_cc2_sa10_isHere = false;
    char *m_cc2_sa10 = NULL;
    uint64_t m_cc2_sa11 = 0;
    float m_cc2_sa12 = 0.0;
    double m_cc2_sa13 = 0.0;
    // Retrieve attributes data
    m_cc2_sa1 = pCmdPayload->cc2_payload.sa1;
    m_cc2_sa2 = pCmdPayload->cc2_payload.sa2;
    m_cc2_sa3 = pCmdPayload->cc2_payload.sa3;
    m_cc2_sa4 = pCmdPayload->cc2_payload.p_sa4;
    m_cc2_sa5 = pCmdPayload->cc2_payload.p_sa5;
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA6_FLAG) != 0) {
        m_cc2_sa6 = pCmdPayload->cc2_payload.sa6;
        m_cc2_sa6_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA7_FLAG) != 0) {
        m_cc2_sa7 = pCmdPayload->cc2_payload.sa7;
        m_cc2_sa7_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA8_FLAG) != 0) {
        m_cc2_sa8 = pCmdPayload->cc2_payload.sa8;
        m_cc2_sa8_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA9_FLAG) != 0) {
        m_cc2_sa9 = pCmdPayload->cc2_payload.p_sa9;
        m_cc2_sa9_isHere = true;
    }
    if ((pCmdPayload->cc2_payload.optAttFlagsBitfield & TEST_CC2_ATT_SA10_FLAG) != 0) {
        m_cc2_sa10 = pCmdPayload->cc2_payload.p_sa10;
        m_cc2_sa10_isHere = true;
    }
    m_cc2_sa11 = pCmdPayload->cc2_payload.sa11;
    m_cc2_sa12 = pCmdPayload->cc2_payload.sa12;
    m_cc2_sa13 = pCmdPayload->cc2_payload.sa13;
    // Init payload optAttFlagsBitfield
    TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc1_payload.sa1 = m_cc2_sa1 + 1;
    TestInfo.SendCmdPayload.cc1_payload.sa2 = m_cc2_sa2 + 1;
    TestInfo.SendCmdPayload.cc1_payload.sa3 = m_cc2_sa3 + 1;
    TestInfo.SendCmdPayload.cc1_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc1_payload.p_sa4 = m_cc2_sa4;
    TestInfo.SendCmdPayload.cc1_payload.sa11 = m_cc2_sa11 + 1;
    TestInfo.SendCmdPayload.cc1_payload.sa12 = m_cc2_sa12 + 1.0;
    TestInfo.SendCmdPayload.cc1_payload.sa13 = m_cc2_sa13 + 1.0;
    Test_FillArray(TestInfo.SendCmdPayload.cc1_payload.p_sa4, true);
    TestInfo.SendCmdPayload.cc1_payload.p_sa5 = m_cc2_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc2_sa5); idx++) {
        TestInfo.SendCmdPayload.cc1_payload.p_sa5[idx] += 1;
    }
    if (m_cc2_sa6_isHere) {
        TestInfo.SendCmdPayload.cc1_payload.sa6 = m_cc2_sa6 + 1;
        TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA6_FLAG;
    }
    if (m_cc2_sa7_isHere) {
        TestInfo.SendCmdPayload.cc1_payload.sa7 = m_cc2_sa7 + 1;
        TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA7_FLAG;
    }
    if (m_cc2_sa8_isHere) {
        TestInfo.SendCmdPayload.cc1_payload.sa8 = m_cc2_sa8 + 1;
        TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA8_FLAG;
    }
    if (m_cc2_sa9_isHere) {
        TestInfo.SendCmdPayload.cc1_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc1_payload.p_sa9 = m_cc2_sa9;
        Test_FillArray(TestInfo.SendCmdPayload.cc1_payload.p_sa9, true);
        TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA9_FLAG;
    }
    if (m_cc2_sa10_isHere) {
        TestInfo.SendCmdPayload.cc1_payload.p_sa10 = m_cc2_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc2_sa10); idx++) {
            TestInfo.SendCmdPayload.cc1_payload.p_sa10[idx] += 1;
        }
        TestInfo.SendCmdPayload.cc1_payload.optAttFlagsBitfield |= TEST_CC1_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC1, true);
}

)";

const QString a_fct_cc3 = R"(static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cc3_sa1 = 0;
    uint16_t m_cc3_sa2 = 0;
    uint32_t m_cc3_sa3 = 0;
    uint8_t *m_cc3_sa4 = NULL;
    char *m_cc3_sa5 = NULL;
    bool m_cc3_sa6_isHere = false;
    uint8_t m_cc3_sa6 = 0;
    bool m_cc3_sa7_isHere = false;
    uint16_t m_cc3_sa7 = 0;
    bool m_cc3_sa8_isHere = false;
    uint32_t m_cc3_sa8 = 0;
    bool m_cc3_sa9_isHere = false;
    uint8_t *m_cc3_sa9 = NULL;
    bool m_cc3_sa10_isHere = false;
    char *m_cc3_sa10 = NULL;
    uint64_t m_cc3_sa11 = 0;
    float m_cc3_sa12 = 0.0;
    double m_cc3_sa13 = 0.0;
    // Retrieve attributes data
    m_cc3_sa1 = pCmdPayload->cc3_payload.sa1;
    m_cc3_sa2 = pCmdPayload->cc3_payload.sa2;
    m_cc3_sa3 = pCmdPayload->cc3_payload.sa3;
    m_cc3_sa4 = pCmdPayload->cc3_payload.p_sa4;
    m_cc3_sa5 = pCmdPayload->cc3_payload.p_sa5;
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA6_FLAG) != 0) {
        m_cc3_sa6 = pCmdPayload->cc3_payload.sa6;
        m_cc3_sa6_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA7_FLAG) != 0) {
        m_cc3_sa7 = pCmdPayload->cc3_payload.sa7;
        m_cc3_sa7_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA8_FLAG) != 0) {
        m_cc3_sa8 = pCmdPayload->cc3_payload.sa8;
        m_cc3_sa8_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA9_FLAG) != 0) {
        m_cc3_sa9 = pCmdPayload->cc3_payload.p_sa9;
        m_cc3_sa9_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA10_FLAG) != 0) {
        m_cc3_sa10 = pCmdPayload->cc3_payload.p_sa10;
        m_cc3_sa10_isHere = true;
    }
    m_cc3_sa11 = pCmdPayload->cc3_payload.sa11;
    m_cc3_sa12 = pCmdPayload->cc3_payload.sa12;
    m_cc3_sa13 = pCmdPayload->cc3_payload.sa13;
    // Init payload optAttFlagsBitfield
    TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc3_payload.sa1 = m_cc3_sa1 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa2 = m_cc3_sa2 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa3 = m_cc3_sa3 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc3_payload.p_sa4 = m_cc3_sa4;
    TestInfo.SendCmdPayload.cc3_payload.sa11 = m_cc3_sa11 + 1;
    TestInfo.SendCmdPayload.cc3_payload.sa12 = m_cc3_sa12 + 1.0;
    TestInfo.SendCmdPayload.cc3_payload.sa13 = m_cc3_sa13 + 1.0;
    Test_FillArray(TestInfo.SendCmdPayload.cc3_payload.p_sa4, false);
    TestInfo.SendCmdPayload.cc3_payload.p_sa5 = m_cc3_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc3_sa5); idx++) {
        TestInfo.SendCmdPayload.cc3_payload.p_sa5[idx] -= 1;
    }
    if (m_cc3_sa6_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa6 = m_cc3_sa6 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA6_FLAG;
    }
    if (m_cc3_sa7_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa7 = m_cc3_sa7 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA7_FLAG;
    }
    if (m_cc3_sa8_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa8 = m_cc3_sa8 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA8_FLAG;
    }
    if (m_cc3_sa9_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc3_payload.p_sa9 = m_cc3_sa9;
        Test_FillArray(TestInfo.SendCmdPayload.cc3_payload.p_sa9, false);
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA9_FLAG;
    }
    if (m_cc3_sa10_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.p_sa10 = m_cc3_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc3_sa10); idx++) {
            TestInfo.SendCmdPayload.cc3_payload.p_sa10[idx] -= 1;
        }
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC3, true);
}

)";

const QString a_fct_cc5 = R"(static bool TestExecuteCC5(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint16_t m_cc5_sa2 = 0;
    uint8_t m_ca5_sa1 = 0;
    uint16_t m_ca5_sa2 = 0;
    bool m_ca5_sa3_isHere = false;
    uint32_t m_ca5_sa3 = 0;
    bool m_cc5_ca6_isHere = false;
    bool m_ca6_sa1_isHere = false;
    uint8_t m_ca6_sa1 = 0;
    bool m_ca7_sa1_isHere = false;
    uint8_t m_ca7_sa1 = 0;
    uint8_t *m_ca8_sa4 = NULL;
    // Retrieve attributes data
    m_cc5_sa2 = pCmdPayload->cc5_payload.sa2;
    m_ca5_sa1 = pCmdPayload->cc5_payload.ca5_payload.sa1;
    m_ca5_sa2 = pCmdPayload->cc5_payload.ca5_payload.sa2;
    if ((pCmdPayload->cc5_payload.ca5_payload.optAttFlagsBitfield & TEST_CA5_ATT_SA3_FLAG) != 0) {
        m_ca5_sa3 = pCmdPayload->cc5_payload.ca5_payload.sa3;
        m_ca5_sa3_isHere = true;
    }
    if ((pCmdPayload->cc5_payload.optAttFlagsBitfield & TEST_CC5_ATT_CA6_FLAG) != 0) {
        m_cc5_ca6_isHere = true;
        if ((pCmdPayload->cc5_payload.ca6_payload.optAttFlagsBitfield & TEST_CA6_ATT_SA1_FLAG) != 0) {
            m_ca6_sa1 = pCmdPayload->cc5_payload.ca6_payload.sa1;
            m_ca6_sa1_isHere = true;
        }
        if ((pCmdPayload->cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield & TEST_CA7_ATT_SA1_FLAG) != 0) {
            m_ca7_sa1 = pCmdPayload->cc5_payload.ca6_payload.ca7_payload.sa1;
            m_ca7_sa1_isHere = true;
        }
        m_ca8_sa4 = pCmdPayload->cc5_payload.ca6_payload.ca7_payload.ca8_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.SendCmdPayload.cc4_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc4_payload.ca1_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc4_payload.ca2_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc4_payload.sa1 = (uint8_t)m_cc5_sa2 + 1;
    TestInfo.SendCmdPayload.cc4_payload.ca1_payload.sa1 = m_ca5_sa1 + 1;
    TestInfo.SendCmdPayload.cc4_payload.ca1_payload.sa2 = m_ca5_sa2 + 1;
    // Optional attributes
    if (m_ca5_sa3_isHere) {
        TestInfo.SendCmdPayload.cc4_payload.ca1_payload.sa3 = m_ca5_sa3;
        TestInfo.SendCmdPayload.cc4_payload.ca1_payload.optAttFlagsBitfield |= TEST_CA1_ATT_SA3_FLAG;
    }
    if (m_cc5_ca6_isHere) {
        TestInfo.SendCmdPayload.cc4_payload.optAttFlagsBitfield |= TEST_CC4_ATT_CA2_FLAG;

        if (m_ca6_sa1_isHere) {
            TestInfo.SendCmdPayload.cc4_payload.ca2_payload.sa1 = m_ca6_sa1 + 1;
            TestInfo.SendCmdPayload.cc4_payload.ca2_payload.optAttFlagsBitfield |= TEST_CA2_ATT_SA1_FLAG;
        }
        if (m_ca7_sa1_isHere) {
            TestInfo.SendCmdPayload.cc4_payload.ca2_payload.ca3_payload.sa1 = m_ca7_sa1 + 1;
            TestInfo.SendCmdPayload.cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield |= TEST_CA3_ATT_SA1_FLAG;
        }
        TestInfo.SendCmdPayload.cc4_payload.ca2_payload.ca3_payload.ca4_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc4_payload.ca2_payload.ca3_payload.ca4_payload.p_sa4 = m_ca8_sa4;
        Test_FillArray(m_ca8_sa4, true);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC4, true);
}

)";

const QString a_fct_cc6 = R"(static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t *m_cc6_sa4 = NULL;
    uint8_t m_ca9_sa1 = 0;
    uint16_t m_ca9_sa2 = 0;
    bool m_ca9_sa3_isHere = false;
    uint32_t m_ca9_sa3 = 0;
    bool m_cc6_ca10_isHere = false;
    bool m_ca10_sa1_isHere = false;
    uint8_t m_ca10_sa1 = 0;
    bool m_ca11_sa1_isHere = false;
    uint8_t m_ca11_sa1 = 0;
    uint8_t *m_ca12_sa4 = NULL;
    // Retrieve attributes data
    m_cc6_sa4 = pCmdPayload->cc6_payload.p_sa4;
    m_ca9_sa1 = pCmdPayload->cc6_payload.ca9_payload.sa1;
    m_ca9_sa2 = pCmdPayload->cc6_payload.ca9_payload.sa2;
    if ((pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield & TEST_CA9_ATT_SA3_FLAG) != 0) {
        m_ca9_sa3 = pCmdPayload->cc6_payload.ca9_payload.sa3;
        m_ca9_sa3_isHere = true;
    }
    if ((pCmdPayload->cc6_payload.optAttFlagsBitfield & TEST_CC6_ATT_CA10_FLAG) != 0) {
        m_cc6_ca10_isHere = true;
        if ((pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield & TEST_CA10_ATT_SA1_FLAG) != 0) {
            m_ca10_sa1 = pCmdPayload->cc6_payload.ca10_payload.sa1;
            m_ca10_sa1_isHere = true;
        }
        if ((pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield & TEST_CA11_ATT_SA1_FLAG) != 0) {
            m_ca11_sa1 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1;
            m_ca11_sa1_isHere = true;
        }
        m_ca12_sa4 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.SendCmdPayload.cc6_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca10_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc6_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc6_payload.p_sa4 = m_cc6_sa4;
    Test_FillArray(m_cc6_sa4, false);
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa1 = m_ca9_sa1 - 1;
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa2 = m_ca9_sa2 - 1;
    // Optional attributes
    if (m_ca9_sa3_isHere) {
        TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa3 = m_ca9_sa3 - 1;
        TestInfo.SendCmdPayload.cc6_payload.ca9_payload.optAttFlagsBitfield |= TEST_CA9_ATT_SA3_FLAG;
    }
    if (m_cc6_ca10_isHere) {
        TestInfo.SendCmdPayload.cc6_payload.optAttFlagsBitfield |= TEST_CC6_ATT_CA10_FLAG;

        if (m_ca10_sa1_isHere) {
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.sa1 = m_ca10_sa1 - 1;
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.optAttFlagsBitfield |= TEST_CA10_ATT_SA1_FLAG;
        }
        if (m_ca11_sa1_isHere) {
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.sa1 = m_ca11_sa1 - 1;
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield |= TEST_CA11_ATT_SA1_FLAG;
        }
        TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.ca12_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4 = m_ca12_sa4;
        Test_FillArray(m_ca12_sa4, false);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC6, true);
}

)";

const QStringList a_cmd_fct = {"", a_fct_sc2, a_fct_sc3, "", a_fct_cc2, a_fct_cc3, "", a_fct_cc5, a_fct_cc6};

// Ectractor B data

const QString b_unk_inc = R"(/**
 * \file Test_Main.c
 * \brief Test protocol module (B)
 * \author LCSF Generator v1.3
 *
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_Test.h"
#include "Test_Main.h"
#include <LCSF_Config.h>

// *** Definitions ***
)";

const QString b_unk_def = R"(// --- Private Macros ---
#define TEST_ARRAY_SIZE 5

// --- Private Types ---
typedef struct _test_info {
    uint8_t *pSendBuffer;
    uint16_t buffSize;
    test_cmd_payload_t SendCmdPayload;
    bool hasPattern;
    const uint8_t *pattern;
} test_info_t;

// --- Private Function Prototypes ---
// Generated functions
static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload);
static bool TestExecuteSC1(void);
static bool TestExecuteSC3(void);
static bool TestExecuteCC1(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC4(test_cmd_payload_t *pCmdPayload);
static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static test_info_t TestInfo;

// *** End Definitions ***

// *** Private Functions ***
)";

const QString b_unk_pri_fct = R"(/**
 * \fn static void Test_FillArray(uint8_t *pArray, bool incr)
 * \brief Fill an array with data
 *
 * \param pArray pointer to array
 * \param incr indicates if function increments or decrements if no pattern
 */
static void Test_FillArray(uint8_t *pArray, bool incr) {
    if (TestInfo.hasPattern) {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            pArray[idx] = TestInfo.pattern[idx];
        }
    } else {
        for (uint8_t idx = 0; idx < TEST_ARRAY_SIZE; idx++) {
            if (incr) {
                pArray[idx] += 1;
            } else {
                pArray[idx] -= 1;
            }
        }
    }
}

/**
 * \fn static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload)
 * \brief Send a command
 *
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not
 * \return bool: true if operation was a success
 */
static bool TestSendCommand(uint_fast16_t cmdName, bool hasPayload) {
    if (cmdName >= TEST_CMD_COUNT) {
        return false;
    }
    int msgSize = 0;
    if (hasPayload) {
        test_cmd_payload_t *pCmdPayload = &TestInfo.SendCmdPayload;
        msgSize = LCSF_Bridge_TestEncode(cmdName, pCmdPayload, TestInfo.pSendBuffer, TestInfo.buffSize);
    } else {
        msgSize = LCSF_Bridge_TestEncode(cmdName, NULL, TestInfo.pSendBuffer, TestInfo.buffSize);
    }
    if (msgSize <= 0) {
        return false;
    }
    // TODO Pass buffer to send function e.g: return DummySend(ExampleInfo.sendBuffer, (size_t)msgSize);
    return true;
}

)";

const QString b_unk_pub_fct = R"(/**
 * \fn bool Test_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool Test_MainInit(uint8_t *pBuffer, size_t buffSize) {
    if (pBuffer == NULL) {
        return false;
    }
    // Note infos
    TestInfo.pSendBuffer = pBuffer;
    TestInfo.buffSize = buffSize;
    return true;
}

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern) {
    if (pattern == NULL) {
        return;
    }
    TestInfo.hasPattern = true;
    TestInfo.pattern = pattern;
}

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void) {
    TestInfo.hasPattern = false;
    TestInfo.pattern = NULL;
}

)";

const QString b_unk_pub_hdr = R"(/**
 * \fn bool Test_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool Test_MainInit(uint8_t *pBuffer, size_t buffSize);

/**
 * \fn void Test_MainClearPattern(const uint8_t *pattern)
 * \brief Set a pattern
 *
 * \param pattern pointer to pattern
 */
void Test_MainSetPattern(const uint8_t *pattern);

/**
 * \fn void Test_MainClearPattern(void)
 * \brief Clear a pattern
 */
void Test_MainClearPattern(void);

)";

const QString b_def_cmd_hdl = "            // Custom default handler\n            return TestSendCommand(TEST_CMD_SC2, false);\n";

const QString b_fct_sc1 = R"(/**
 * \fn static bool TestExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool TestExecuteX(test_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool TestExecuteSC1(void) {
    return TestSendCommand(TEST_CMD_SC2, false);
}

)";

const QString b_fct_sc3 = R"(static bool TestExecuteSC3(void) {
    return TestSendCommand(TEST_CMD_SC3, false);
}

)";

const QString b_fct_cc1 = R"(static bool TestExecuteCC1(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cc1_sa1 = 0;
    uint16_t m_cc1_sa2 = 0;
    uint32_t m_cc1_sa3 = 0;
    uint8_t *m_cc1_sa4 = NULL;
    char *m_cc1_sa5 = NULL;
    bool m_cc1_sa6_isHere = false;
    uint8_t m_cc1_sa6 = 0;
    bool m_cc1_sa7_isHere = false;
    uint16_t m_cc1_sa7 = 0;
    bool m_cc1_sa8_isHere = false;
    uint32_t m_cc1_sa8 = 0;
    bool m_cc1_sa9_isHere = false;
    uint8_t *m_cc1_sa9 = NULL;
    bool m_cc1_sa10_isHere = false;
    char *m_cc1_sa10 = NULL;
    uint64_t m_cc1_sa11 = 0;
    float m_cc1_sa12 = 0.0;
    double m_cc1_sa13 = 0.0;
    // Retrieve attributes data
    m_cc1_sa1 = pCmdPayload->cc1_payload.sa1;
    m_cc1_sa2 = pCmdPayload->cc1_payload.sa2;
    m_cc1_sa3 = pCmdPayload->cc1_payload.sa3;
    m_cc1_sa4 = pCmdPayload->cc1_payload.p_sa4;
    m_cc1_sa5 = pCmdPayload->cc1_payload.p_sa5;
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA6_FLAG) != 0) {
        m_cc1_sa6 = pCmdPayload->cc1_payload.sa6;
        m_cc1_sa6_isHere = true;
    }
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA7_FLAG) != 0) {
        m_cc1_sa7 = pCmdPayload->cc1_payload.sa7;
        m_cc1_sa7_isHere = true;
    }
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA8_FLAG) != 0) {
        m_cc1_sa8 = pCmdPayload->cc1_payload.sa8;
        m_cc1_sa8_isHere = true;
    }
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA9_FLAG) != 0) {
        m_cc1_sa9 = pCmdPayload->cc1_payload.p_sa9;
        m_cc1_sa9_isHere = true;
    }
    if ((pCmdPayload->cc1_payload.optAttFlagsBitfield & TEST_CC1_ATT_SA10_FLAG) != 0) {
        m_cc1_sa10 = pCmdPayload->cc1_payload.p_sa10;
        m_cc1_sa10_isHere = true;
    }
    m_cc1_sa11 = pCmdPayload->cc1_payload.sa11;
    m_cc1_sa12 = pCmdPayload->cc1_payload.sa12;
    m_cc1_sa13 = pCmdPayload->cc1_payload.sa13;
    // Init payload optAttFlagsBitfield
    TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc2_payload.sa1 = m_cc1_sa1 + 1;
    TestInfo.SendCmdPayload.cc2_payload.sa2 = m_cc1_sa2 + 1;
    TestInfo.SendCmdPayload.cc2_payload.sa3 = m_cc1_sa3 + 1;
    TestInfo.SendCmdPayload.cc2_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc2_payload.p_sa4 = m_cc1_sa4;
    TestInfo.SendCmdPayload.cc2_payload.sa11 = m_cc1_sa11 + 1;
    TestInfo.SendCmdPayload.cc2_payload.sa12 = m_cc1_sa12 + 1.0;
    TestInfo.SendCmdPayload.cc2_payload.sa13 = m_cc1_sa13 + 1.0;
    Test_FillArray(TestInfo.SendCmdPayload.cc2_payload.p_sa4, true);
    TestInfo.SendCmdPayload.cc2_payload.p_sa5 = m_cc1_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc1_sa5); idx++) {
        TestInfo.SendCmdPayload.cc2_payload.p_sa5[idx] += 1;
    }
    if (m_cc1_sa6_isHere) {
        TestInfo.SendCmdPayload.cc2_payload.sa6 = m_cc1_sa6 + 1;
        TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA6_FLAG;
    }
    if (m_cc1_sa7_isHere) {
        TestInfo.SendCmdPayload.cc2_payload.sa7 = m_cc1_sa7 + 1;
        TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA7_FLAG;
    }
    if (m_cc1_sa8_isHere) {
        TestInfo.SendCmdPayload.cc2_payload.sa8 = m_cc1_sa8 + 1;
        TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA8_FLAG;
    }
    if (m_cc1_sa9_isHere) {
        TestInfo.SendCmdPayload.cc2_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc2_payload.p_sa9 = m_cc1_sa9;
        Test_FillArray(TestInfo.SendCmdPayload.cc2_payload.p_sa9, true);
        TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA9_FLAG;
    }
    if (m_cc1_sa10_isHere) {
        TestInfo.SendCmdPayload.cc2_payload.p_sa10 = m_cc1_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc1_sa10); idx++) {
            TestInfo.SendCmdPayload.cc2_payload.p_sa10[idx] += 1;
        }
        TestInfo.SendCmdPayload.cc2_payload.optAttFlagsBitfield |= TEST_CC2_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC2, true);
}

)";

const QString b_fct_cc3 = R"(static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cc3_sa1 = 0;
    uint16_t m_cc3_sa2 = 0;
    uint32_t m_cc3_sa3 = 0;
    uint8_t *m_cc3_sa4 = NULL;
    char *m_cc3_sa5 = NULL;
    bool m_cc3_sa6_isHere = false;
    uint8_t m_cc3_sa6 = 0;
    bool m_cc3_sa7_isHere = false;
    uint16_t m_cc3_sa7 = 0;
    bool m_cc3_sa8_isHere = false;
    uint32_t m_cc3_sa8 = 0;
    bool m_cc3_sa9_isHere = false;
    uint8_t *m_cc3_sa9 = NULL;
    bool m_cc3_sa10_isHere = false;
    char *m_cc3_sa10 = NULL;
    uint64_t m_cc3_sa11 = 0;
    float m_cc3_sa12 = 0.0;
    double m_cc3_sa13 = 0.0;
    // Retrieve attributes data
    m_cc3_sa1 = pCmdPayload->cc3_payload.sa1;
    m_cc3_sa2 = pCmdPayload->cc3_payload.sa2;
    m_cc3_sa3 = pCmdPayload->cc3_payload.sa3;
    m_cc3_sa4 = pCmdPayload->cc3_payload.p_sa4;
    m_cc3_sa5 = pCmdPayload->cc3_payload.p_sa5;
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA6_FLAG) != 0) {
        m_cc3_sa6 = pCmdPayload->cc3_payload.sa6;
        m_cc3_sa6_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA7_FLAG) != 0) {
        m_cc3_sa7 = pCmdPayload->cc3_payload.sa7;
        m_cc3_sa7_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA8_FLAG) != 0) {
        m_cc3_sa8 = pCmdPayload->cc3_payload.sa8;
        m_cc3_sa8_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA9_FLAG) != 0) {
        m_cc3_sa9 = pCmdPayload->cc3_payload.p_sa9;
        m_cc3_sa9_isHere = true;
    }
    if ((pCmdPayload->cc3_payload.optAttFlagsBitfield & TEST_CC3_ATT_SA10_FLAG) != 0) {
        m_cc3_sa10 = pCmdPayload->cc3_payload.p_sa10;
        m_cc3_sa10_isHere = true;
    }
    m_cc3_sa11 = pCmdPayload->cc3_payload.sa11;
    m_cc3_sa12 = pCmdPayload->cc3_payload.sa12;
    m_cc3_sa13 = pCmdPayload->cc3_payload.sa13;
    // Init payload optAttFlagsBitfield
    TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc3_payload.sa1 = m_cc3_sa1 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa2 = m_cc3_sa2 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa3 = m_cc3_sa3 - 1;
    TestInfo.SendCmdPayload.cc3_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc3_payload.p_sa4 = m_cc3_sa4;
    TestInfo.SendCmdPayload.cc3_payload.sa11 = m_cc3_sa11 + 1;
    TestInfo.SendCmdPayload.cc3_payload.sa12 = m_cc3_sa12 + 1.0;
    TestInfo.SendCmdPayload.cc3_payload.sa13 = m_cc3_sa13 + 1.0;
    Test_FillArray(TestInfo.SendCmdPayload.cc3_payload.p_sa4, false);
    TestInfo.SendCmdPayload.cc3_payload.p_sa5 = m_cc3_sa5;
    for (uint8_t idx = 0; idx < strlen(m_cc3_sa5); idx++) {
        TestInfo.SendCmdPayload.cc3_payload.p_sa5[idx] -= 1;
    }
    if (m_cc3_sa6_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa6 = m_cc3_sa6 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA6_FLAG;
    }
    if (m_cc3_sa7_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa7 = m_cc3_sa7 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA7_FLAG;
    }
    if (m_cc3_sa8_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa8 = m_cc3_sa8 - 1;
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA8_FLAG;
    }
    if (m_cc3_sa9_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.sa9Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc3_payload.p_sa9 = m_cc3_sa9;
        Test_FillArray(TestInfo.SendCmdPayload.cc3_payload.p_sa9, false);
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA9_FLAG;
    }
    if (m_cc3_sa10_isHere) {
        TestInfo.SendCmdPayload.cc3_payload.p_sa10 = m_cc3_sa10;
        for (uint8_t idx = 0; idx < strlen(m_cc3_sa10); idx++) {
            TestInfo.SendCmdPayload.cc3_payload.p_sa10[idx] -= 1;
        }
        TestInfo.SendCmdPayload.cc3_payload.optAttFlagsBitfield |= TEST_CC3_ATT_SA10_FLAG;
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC3, true);
}

)";

const QString b_fct_cc4 = R"(static bool TestExecuteCC4(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t m_cc4_sa1 = 0;
    uint8_t m_ca1_sa1 = 0;
    uint16_t m_ca1_sa2 = 0;
    bool m_ca1_sa3_isHere = false;
    uint32_t m_ca1_sa3 = 0;
    bool m_cc4_ca2_isHere = false;
    bool m_ca2_sa1_isHere = false;
    uint8_t m_ca2_sa1 = 0;
    bool m_ca3_sa1_isHere = false;
    uint8_t m_ca3_sa1 = 0;
    uint8_t *m_ca4_sa4 = NULL;
    // Retrieve attributes data
    m_cc4_sa1 = pCmdPayload->cc4_payload.sa1;
    m_ca1_sa1 = pCmdPayload->cc4_payload.ca1_payload.sa1;
    m_ca1_sa2 = pCmdPayload->cc4_payload.ca1_payload.sa2;
    if ((pCmdPayload->cc4_payload.ca1_payload.optAttFlagsBitfield & TEST_CA1_ATT_SA3_FLAG) != 0) {
        m_ca1_sa3 = pCmdPayload->cc4_payload.ca1_payload.sa3;
        m_ca1_sa3_isHere = true;
    }
    if ((pCmdPayload->cc4_payload.optAttFlagsBitfield & TEST_CC4_ATT_CA2_FLAG) != 0) {
        m_cc4_ca2_isHere = true;
        if ((pCmdPayload->cc4_payload.ca2_payload.optAttFlagsBitfield & TEST_CA2_ATT_SA1_FLAG) != 0) {
            m_ca2_sa1 = pCmdPayload->cc4_payload.ca2_payload.sa1;
            m_ca2_sa1_isHere = true;
        }
        if ((pCmdPayload->cc4_payload.ca2_payload.ca3_payload.optAttFlagsBitfield & TEST_CA3_ATT_SA1_FLAG) != 0) {
            m_ca3_sa1 = pCmdPayload->cc4_payload.ca2_payload.ca3_payload.sa1;
            m_ca3_sa1_isHere = true;
        }
        m_ca4_sa4 = pCmdPayload->cc4_payload.ca2_payload.ca3_payload.ca4_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.SendCmdPayload.cc5_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc5_payload.ca5_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc5_payload.ca6_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc5_payload.sa2 = m_cc4_sa1 + 1;
    TestInfo.SendCmdPayload.cc5_payload.ca5_payload.sa1 = m_ca1_sa1 + 1;
    TestInfo.SendCmdPayload.cc5_payload.ca5_payload.sa2 = m_ca1_sa2 + 1;
    // Optional attributes
    if (m_ca1_sa3_isHere) {
        TestInfo.SendCmdPayload.cc5_payload.ca5_payload.sa3 = m_ca1_sa3;
        TestInfo.SendCmdPayload.cc5_payload.ca5_payload.optAttFlagsBitfield |= TEST_CA5_ATT_SA3_FLAG;
    }
    if (m_cc4_ca2_isHere) {
        TestInfo.SendCmdPayload.cc5_payload.optAttFlagsBitfield |= TEST_CC5_ATT_CA6_FLAG;

        if (m_ca2_sa1_isHere) {
            TestInfo.SendCmdPayload.cc5_payload.ca6_payload.sa1 = m_ca2_sa1 + 1;
            TestInfo.SendCmdPayload.cc5_payload.ca6_payload.optAttFlagsBitfield |= TEST_CA6_ATT_SA1_FLAG;
        }
        if (m_ca3_sa1_isHere) {
            TestInfo.SendCmdPayload.cc5_payload.ca6_payload.ca7_payload.sa1 = m_ca3_sa1 + 1;
            TestInfo.SendCmdPayload.cc5_payload.ca6_payload.ca7_payload.optAttFlagsBitfield |= TEST_CA7_ATT_SA1_FLAG;
        }
        TestInfo.SendCmdPayload.cc5_payload.ca6_payload.ca7_payload.ca8_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc5_payload.ca6_payload.ca7_payload.ca8_payload.p_sa4 = m_ca4_sa4;
        Test_FillArray(m_ca4_sa4, true);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC5, true);
}

)";

const QString b_fct_cc6 = R"(static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Declare attributes
    uint8_t *m_cc6_sa4 = NULL;
    uint8_t m_ca9_sa1 = 0;
    uint16_t m_ca9_sa2 = 0;
    bool m_ca9_sa3_isHere = false;
    uint32_t m_ca9_sa3 = 0;
    bool m_cc6_ca10_isHere = false;
    bool m_ca10_sa1_isHere = false;
    uint8_t m_ca10_sa1 = 0;
    bool m_ca11_sa1_isHere = false;
    uint8_t m_ca11_sa1 = 0;
    uint8_t *m_ca12_sa4 = NULL;
    // Retrieve attributes data
    m_cc6_sa4 = pCmdPayload->cc6_payload.p_sa4;
    m_ca9_sa1 = pCmdPayload->cc6_payload.ca9_payload.sa1;
    m_ca9_sa2 = pCmdPayload->cc6_payload.ca9_payload.sa2;
    if ((pCmdPayload->cc6_payload.ca9_payload.optAttFlagsBitfield & TEST_CA9_ATT_SA3_FLAG) != 0) {
        m_ca9_sa3 = pCmdPayload->cc6_payload.ca9_payload.sa3;
        m_ca9_sa3_isHere = true;
    }
    if ((pCmdPayload->cc6_payload.optAttFlagsBitfield & TEST_CC6_ATT_CA10_FLAG) != 0) {
        m_cc6_ca10_isHere = true;
        if ((pCmdPayload->cc6_payload.ca10_payload.optAttFlagsBitfield & TEST_CA10_ATT_SA1_FLAG) != 0) {
            m_ca10_sa1 = pCmdPayload->cc6_payload.ca10_payload.sa1;
            m_ca10_sa1_isHere = true;
        }
        if ((pCmdPayload->cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield & TEST_CA11_ATT_SA1_FLAG) != 0) {
            m_ca11_sa1 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.sa1;
            m_ca11_sa1_isHere = true;
        }
        m_ca12_sa4 = pCmdPayload->cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4;
    }
    // Init payload optAttFlagsBitfields
    TestInfo.SendCmdPayload.cc6_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca10_payload.optAttFlagsBitfield = 0;
    TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield = 0;
    // Process data
    TestInfo.SendCmdPayload.cc6_payload.sa4Size = TEST_ARRAY_SIZE;
    TestInfo.SendCmdPayload.cc6_payload.p_sa4 = m_cc6_sa4;
    Test_FillArray(m_cc6_sa4, false);
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa1 = m_ca9_sa1 - 1;
    TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa2 = m_ca9_sa2 - 1;
    // Optional attributes
    if (m_ca9_sa3_isHere) {
        TestInfo.SendCmdPayload.cc6_payload.ca9_payload.sa3 = m_ca9_sa3 - 1;
        TestInfo.SendCmdPayload.cc6_payload.ca9_payload.optAttFlagsBitfield |= TEST_CA9_ATT_SA3_FLAG;
    }
    if (m_cc6_ca10_isHere) {
        TestInfo.SendCmdPayload.cc6_payload.optAttFlagsBitfield |= TEST_CC6_ATT_CA10_FLAG;

        if (m_ca10_sa1_isHere) {
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.sa1 = m_ca10_sa1 - 1;
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.optAttFlagsBitfield |= TEST_CA10_ATT_SA1_FLAG;
        }
        if (m_ca11_sa1_isHere) {
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.sa1 = m_ca11_sa1 - 1;
            TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.optAttFlagsBitfield |= TEST_CA11_ATT_SA1_FLAG;
        }
        TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.ca12_payload.sa4Size = TEST_ARRAY_SIZE;
        TestInfo.SendCmdPayload.cc6_payload.ca10_payload.ca11_payload.ca12_payload.p_sa4 = m_ca12_sa4;
        Test_FillArray(m_ca12_sa4, false);
    }
    // Send command
    return TestSendCommand(TEST_CMD_CC6, true);
}

)";

extern const QStringList b_cmd_fct = {b_fct_sc1, "", b_fct_sc3, b_fct_cc1, "", b_fct_cc3, b_fct_cc4, "", b_fct_cc6};