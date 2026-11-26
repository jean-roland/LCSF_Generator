/**
 * \file StructTest_Main_a.c
 * \brief StructTest protocol module (A)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
// Custom lib
#include "LCSF_Bridge_StructTest.h"
#include "StructTest_Main.h"
#include <LCSF_Config.h>

// *** Definitions ***
// --- Private Macros ---
// --- Private Types ---
typedef struct _structtest_info {
    uint8_t *pSendBuffer;
    uint16_t buffSize;
    structtest_cmd_payload_t SendCmdPayload;
} structtest_info_t;

// --- Private Function Prototypes ---
// Generated functions
static bool StructTestSendCommand(uint_fast16_t cmdName, bool hasPayload);
static bool StructTestExecuteget_status(structtest_cmd_payload_t *pCmdPayload);
static bool StructTestExecuteset_cfg(structtest_cmd_payload_t *pCmdPayload);

// --- Private Variables ---
static structtest_info_t StructTestInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static bool StructTestSendCommand(uint_fast16_t cmdName, bool hasPayload)
 * \brief Send a command
 *=
 * \param cmdName name of the command to send
 * \param hasPayload indicates if command has a payload or not
 * \return bool: true if operation was a success
 */
static bool StructTestSendCommand(uint_fast16_t cmdName, bool hasPayload) {
    if (cmdName >= STRUCTTEST_CMD_COUNT) {
        return false;
    }
    int msgSize = 0;
    if (hasPayload) {
        structtest_cmd_payload_t *pCmdPayload = &StructTestInfo.SendCmdPayload;
        msgSize = LCSF_Bridge_StructTestEncode(cmdName, pCmdPayload, StructTestInfo.pSendBuffer, StructTestInfo.buffSize);
    } else {
        msgSize = LCSF_Bridge_StructTestEncode(cmdName, NULL, StructTestInfo.pSendBuffer, StructTestInfo.buffSize);
    }
    if (msgSize <= 0) {
        return false;
    }
    // TODO Pass buffer to send function
    return true;
}

/**
 * \fn static bool StructTestExecuteX(void)
 * \brief Execute command X (no payload)
 *
 * \return bool: true if operation was a success
 */

/**
 * \fn static bool StructTestExecuteX(structtest_cmd_payload_t *pCmdPayload)
 * \brief Execute command X (with payload)
 *
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */

static bool StructTestExecuteget_status(structtest_cmd_payload_t *pCmdPayload) {
    // Declare attributes
    structtest_status_t m_get_status_status = {0};
    uint8_t *m_get_status_links = NULL;
    uint32_t m_get_status_linksSize = 0;
    bool m_get_status_opt_status_isHere = false;
    structtest_status_t m_get_status_opt_status = {0};
    bool m_get_status_ext_status_isHere = false;
    uint8_t *m_get_status_ext_status = NULL;
    uint32_t m_get_status_ext_statusSize = 0;
    structtest_counters_t m_get_status_counters = {0};
    uint8_t *m_get_status_link_counters = NULL;
    uint32_t m_get_status_link_countersSize = 0;
    // Retrieve attributes data
    m_get_status_status = pCmdPayload->get_status_payload.status;
    m_get_status_links = pCmdPayload->get_status_payload.p_links;
    m_get_status_linksSize = pCmdPayload->get_status_payload.linksSize;
    if ((pCmdPayload->get_status_payload.optAttFlagsBitfield & STRUCTTEST_GET_STATUS_ATT_OPT_STATUS_FLAG) != 0) {
        m_get_status_opt_status = pCmdPayload->get_status_payload.opt_status;
        m_get_status_opt_status_isHere = true;
    }
    if ((pCmdPayload->get_status_payload.optAttFlagsBitfield & STRUCTTEST_GET_STATUS_ATT_EXT_STATUS_FLAG) != 0) {
        m_get_status_ext_status = pCmdPayload->get_status_payload.p_ext_status;
        m_get_status_ext_statusSize = pCmdPayload->get_status_payload.ext_statusSize;
        m_get_status_ext_status_isHere = true;
    }
    m_get_status_counters = pCmdPayload->get_status_payload.counters;
    m_get_status_link_counters = pCmdPayload->get_status_payload.p_link_counters;
    m_get_status_link_countersSize = pCmdPayload->get_status_payload.link_countersSize;
    // Process data
#pragma GCC warning "#warning TODO: function to implement"
    return true;
}

static bool StructTestExecuteset_cfg(structtest_cmd_payload_t *pCmdPayload) {
    // Declare attributes
    uint8_t m_set_cfg_mode = 0;
    structtest_link_t m_cfg_link = {0};
    bool m_cfg_links_isHere = false;
    uint8_t *m_cfg_links = NULL;
    uint32_t m_cfg_linksSize = 0;
    bool m_cfg_opt_status_isHere = false;
    structtest_status_t m_cfg_opt_status = {0};
    int8_t m_cfg_step = 0;
    bool m_cfg_epoch_isHere = false;
    int64_t m_cfg_epoch = 0;
    int16_t m_set_cfg_trim = 0;
    bool m_set_cfg_bias_isHere = false;
    int32_t m_set_cfg_bias = 0;
    // Retrieve attributes data
    m_set_cfg_mode = pCmdPayload->set_cfg_payload.mode;
    m_cfg_link = pCmdPayload->set_cfg_payload.cfg_payload.link;
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_LINKS_FLAG) != 0) {
        m_cfg_links = pCmdPayload->set_cfg_payload.cfg_payload.p_links;
        m_cfg_linksSize = pCmdPayload->set_cfg_payload.cfg_payload.linksSize;
        m_cfg_links_isHere = true;
    }
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_OPT_STATUS_FLAG) != 0) {
        m_cfg_opt_status = pCmdPayload->set_cfg_payload.cfg_payload.opt_status;
        m_cfg_opt_status_isHere = true;
    }
    m_cfg_step = pCmdPayload->set_cfg_payload.cfg_payload.step;
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_EPOCH_FLAG) != 0) {
        m_cfg_epoch = pCmdPayload->set_cfg_payload.cfg_payload.epoch;
        m_cfg_epoch_isHere = true;
    }
    m_set_cfg_trim = pCmdPayload->set_cfg_payload.trim;
    if ((pCmdPayload->set_cfg_payload.optAttFlagsBitfield & STRUCTTEST_SET_CFG_ATT_BIAS_FLAG) != 0) {
        m_set_cfg_bias = pCmdPayload->set_cfg_payload.bias;
        m_set_cfg_bias_isHere = true;
    }
    // Process data
#pragma GCC warning "#warning TODO: function to implement"
    return true;
}

// *** Public Functions ***

/**
 * \fn bool StructTest_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool StructTest_MainInit(uint8_t *pBuffer, size_t buffSize) {
    if (pBuffer == NULL) {
        return false;
    }
    // Note infos
    StructTestInfo.pSendBuffer = pBuffer;
    StructTestInfo.buffSize = buffSize;
    return true;
}

// Place custom public functions here

/**
 * \fn bool StructTest_MainExecute(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool StructTest_MainExecute(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case STRUCTTEST_CMD_GET_STATUS:
            return StructTestExecuteget_status(pCmdPayload);

        case STRUCTTEST_CMD_SET_CFG:
            return StructTestExecuteset_cfg(pCmdPayload);

        default:
            // This case can be customized (e.g to send an error command)
            return false;
    }
}
