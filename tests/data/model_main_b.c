/**
 * \file Test_Main_b.c
 * \brief Test protocol module (B)
 * \author LCSF Generator v1.4
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
// --- Private Macros ---
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

/**
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

/**
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

static bool TestExecuteSC3(void) {
    return TestSendCommand(TEST_CMD_SC3, false);
}

static bool TestExecuteCC1(test_cmd_payload_t *pCmdPayload) {
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

static bool TestExecuteCC3(test_cmd_payload_t *pCmdPayload) {
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

static bool TestExecuteCC4(test_cmd_payload_t *pCmdPayload) {
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

static bool TestExecuteCC6(test_cmd_payload_t *pCmdPayload) {
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

// *** Public Functions ***

/**
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

/**
 * \fn bool Test_MainExecute(uint_fast16_t cmdName, test_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool Test_MainExecute(uint_fast16_t cmdName, test_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case TEST_CMD_SC1:
            return TestExecuteSC1();

        case TEST_CMD_SC3:
            return TestExecuteSC3();

        case TEST_CMD_CC1:
            return TestExecuteCC1(pCmdPayload);

        case TEST_CMD_CC3:
            return TestExecuteCC3(pCmdPayload);

        case TEST_CMD_CC4:
            return TestExecuteCC4(pCmdPayload);

        case TEST_CMD_CC6:
            return TestExecuteCC6(pCmdPayload);

        default:
            // Custom default handler
            return TestSendCommand(TEST_CMD_SC2, false);
    }
}
