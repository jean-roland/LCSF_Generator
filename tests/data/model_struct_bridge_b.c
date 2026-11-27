/**
 * \file LCSF_Bridge_StructTest_b.c
 * \brief StructTest LCSF bridge module (B)
 * \author LCSF Generator v1.4
 *
 */

// *** Libraries include ***
// Standard lib
#include <string.h>
// Custom lib
#include "LCSF_Bridge_StructTest.h"
#include <LCSF_Config.h>
#include <lib/LCSF_Transcoder.h>
#include <lib/Lifo.h>

// *** Definitions ***
// --- Private Types ---

static uint8_t LifoData[LCSF_BRIDGE_STRUCTTEST_LIFO_SIZE * sizeof(lcsf_valid_att_t)];

// Module information structure
typedef struct _lcsf_bridge_structtest_info {
    lifo_desc_t Lifo;
    structtest_cmd_payload_t CmdPayload;
    uint8_t StructBuff[LCSF_BRIDGE_STRUCTTEST_STRUCT_BUFF_SIZE];
} lcsf_bridge_structtest_info_t;

// --- Private Constants ---
// Struct types fit in an attribute payload
_Static_assert(STRUCTTEST_STATUS_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct status is over the attribute payload max size");
_Static_assert(STRUCTTEST_LINK_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct link is over the attribute payload max size");
_Static_assert(STRUCTTEST_COUNTERS_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct counters is over the attribute payload max size");
// Array to convert command name value to their lcsf command id
static const uint16_t LCSF_Bridge_StructTest_CMDNAME2CMDID[LCSF_STRUCTTEST_CMD_NB] = {
    LCSF_STRUCTTEST_CMD_ID_GET_STATUS,
    LCSF_STRUCTTEST_CMD_ID_SET_CFG,
};

// --- Private Function Prototypes ---
static uint16_t LCSF_Bridge_StructTest_CMDID2CMDNAME(uint_fast16_t cmdId);
static void LCSF_Bridge_StructTestget_statusGetData(lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_StructTestset_cfgGetData(lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload);
static void LCSF_Bridge_StructTestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_StructTestget_statusFillAtt(lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_StructTestset_cfgFillAtt(lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload);
static bool LCSF_Bridge_StructTestFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload);
static size_t LCSF_Bridge_StructTestVarUintEncode(uint64_t value, uint8_t *pBuffer);
static size_t LCSF_Bridge_StructTestVarUintDecode(uint64_t *pValue, uint64_t maxValue, const uint8_t *pBuffer, size_t dataSize);
static uint64_t LCSF_Bridge_StructTestZigZagEncode(int64_t value);
static int64_t LCSF_Bridge_StructTestZigZagDecode(uint64_t value);

// --- Private Variables ---
static lcsf_bridge_structtest_info_t LcsfBridgeStructTestInfo;

// *** End Definitions ***

// *** Private Functions ***

/**
 * \fn static uint16_t LCSF_Bridge_StructTest_CMDID2CMDNAME(uint_fast16_t cmdId)
 * \brief Translate an lcsf command id to its name value
 *
 * \param cmdId lcsf command identifier to translate
 * \return uint16_t: name value of the command
 */
static uint16_t LCSF_Bridge_StructTest_CMDID2CMDNAME(uint_fast16_t cmdId) {
    switch (cmdId) {
        default:
        case LCSF_STRUCTTEST_CMD_ID_GET_STATUS:
            return STRUCTTEST_CMD_GET_STATUS;
        case LCSF_STRUCTTEST_CMD_ID_SET_CFG:
            return STRUCTTEST_CMD_SET_CFG;
    }
}

/**
 * \fn static void LCSF_Bridge_StructTestXGetData(lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload)
 * \brief Retrieve data of command X from its valid attribute array and store it in a payload
 *
 * \param pAttArray pointer to the command attribute array
 * \param pCmdPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_StructTestget_statusGetData(lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload) {
    // Initialize optional attribute flags bitfield
    pCmdPayload->get_status_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute status
    pCmdPayload->get_status_payload.statusFieldNb = LCSF_Bridge_StructTeststatusDeserialize(&(pCmdPayload->get_status_payload.status), pAttArray[STRUCTTEST_GET_STATUS_ATT_STATUS].Payload.pData, pAttArray[STRUCTTEST_GET_STATUS_ATT_STATUS].PayloadSize);
    // Retrieve data of attribute links
    pCmdPayload->get_status_payload.p_links = pAttArray[STRUCTTEST_GET_STATUS_ATT_LINKS].Payload.pData;
    pCmdPayload->get_status_payload.linksSize = pAttArray[STRUCTTEST_GET_STATUS_ATT_LINKS].PayloadSize;
    // Retrieve data of attribute opt_status
    if (pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->get_status_payload.optAttFlagsBitfield |= STRUCTTEST_GET_STATUS_ATT_OPT_STATUS_FLAG;
        pCmdPayload->get_status_payload.opt_statusFieldNb = LCSF_Bridge_StructTeststatusDeserialize(&(pCmdPayload->get_status_payload.opt_status), pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].Payload.pData, pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].PayloadSize);
    }
    // Retrieve data of attribute ext_status
    if (pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->get_status_payload.optAttFlagsBitfield |= STRUCTTEST_GET_STATUS_ATT_EXT_STATUS_FLAG;
        pCmdPayload->get_status_payload.p_ext_status = pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].Payload.pData;
        pCmdPayload->get_status_payload.ext_statusSize = pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].PayloadSize;
    }
    // Retrieve data of attribute counters
    pCmdPayload->get_status_payload.countersFieldNb = LCSF_Bridge_StructTestcountersDeserialize(&(pCmdPayload->get_status_payload.counters), pAttArray[STRUCTTEST_GET_STATUS_ATT_COUNTERS].Payload.pData, pAttArray[STRUCTTEST_GET_STATUS_ATT_COUNTERS].PayloadSize);
    // Retrieve data of attribute link_counters
    pCmdPayload->get_status_payload.p_link_counters = pAttArray[STRUCTTEST_GET_STATUS_ATT_LINK_COUNTERS].Payload.pData;
    pCmdPayload->get_status_payload.link_countersSize = pAttArray[STRUCTTEST_GET_STATUS_ATT_LINK_COUNTERS].PayloadSize;
}

static void LCSF_Bridge_StructTestset_cfgGetData(lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload) {
    // Initialize optional attribute flags bitfield
    pCmdPayload->set_cfg_payload.optAttFlagsBitfield = 0;
    // Retrieve data of attribute mode
    memcpy(&(pCmdPayload->set_cfg_payload.mode), pAttArray[STRUCTTEST_SET_CFG_ATT_MODE].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_MODE].PayloadSize);
    // Retrieve data of attribute cfg
    // Initialize optional attribute flags bitfield
    pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield = 0;
    // Retrieve data of sub-attribute link
    pCmdPayload->set_cfg_payload.cfg_payload.linkFieldNb = LCSF_Bridge_StructTestlinkDeserialize(&(pCmdPayload->set_cfg_payload.cfg_payload.link), pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINK].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINK].PayloadSize);
    // Retrieve data of sub-attribute links
    if (pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].Payload.pData != NULL) {
        // Note presence of optional attribute links
        pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield |= STRUCTTEST_CFG_ATT_LINKS_FLAG;
        // Retrieve data of sub-attribute links
        pCmdPayload->set_cfg_payload.cfg_payload.p_links = pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].Payload.pData;
        pCmdPayload->set_cfg_payload.cfg_payload.linksSize = pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].PayloadSize;
    }
    // Retrieve data of sub-attribute opt_status
    if (pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].Payload.pData != NULL) {
        // Note presence of optional attribute opt_status
        pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield |= STRUCTTEST_CFG_ATT_OPT_STATUS_FLAG;
        // Retrieve data of sub-attribute opt_status
        pCmdPayload->set_cfg_payload.cfg_payload.opt_statusFieldNb = LCSF_Bridge_StructTeststatusDeserialize(&(pCmdPayload->set_cfg_payload.cfg_payload.opt_status), pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].PayloadSize);
    }
    // Retrieve data of sub-attribute step
    memcpy(&(pCmdPayload->set_cfg_payload.cfg_payload.step), pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_STEP].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_STEP].PayloadSize);
    // Retrieve data of sub-attribute epoch
    if (pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].Payload.pData != NULL) {
        // Note presence of optional attribute epoch
        pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield |= STRUCTTEST_CFG_ATT_EPOCH_FLAG;
        // Retrieve data of sub-attribute epoch
        memcpy(&(pCmdPayload->set_cfg_payload.cfg_payload.epoch), pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].PayloadSize);
    }
    // Retrieve data of attribute trim
    memcpy(&(pCmdPayload->set_cfg_payload.trim), pAttArray[STRUCTTEST_SET_CFG_ATT_TRIM].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_TRIM].PayloadSize);
    // Retrieve data of attribute bias
    if (pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].Payload.pData != NULL) {
        // Note presence of optional attribute
        pCmdPayload->set_cfg_payload.optAttFlagsBitfield |= STRUCTTEST_SET_CFG_ATT_BIAS_FLAG;
        memcpy(&(pCmdPayload->set_cfg_payload.bias), pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].Payload.pData, pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].PayloadSize);
    }
}

/**
 * \fn static void LCSF_Bridge_StructTestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload)
 * \brief Retrieve command data from its attribute array and store it in a payload
 *
 * \param cmdName name of the command
 * \param pAttArray pointer to the command attribute array
 * \param pPayload pointer to the payload to contain the command data
 * \return void
 */
static void LCSF_Bridge_StructTestGetCmdData(uint_fast16_t cmdName, lcsf_valid_att_t *pAttArray, structtest_cmd_payload_t *pCmdPayload) {
    if (pAttArray == NULL) {
	    return;
    }
    switch (cmdName) {
        case STRUCTTEST_CMD_GET_STATUS:
            LCSF_Bridge_StructTestget_statusGetData(pAttArray, pCmdPayload);
            break;

        case STRUCTTEST_CMD_SET_CFG:
            LCSF_Bridge_StructTestset_cfgGetData(pAttArray, pCmdPayload);
            break;

        default: // Commands that don't have payload
            return;
    }
}

/**
 * \fn static bool LCSF_Bridge_StructTestXFillAtt(lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload)
 * \brief Allocate and fill attribute array of command X from its payload
 *
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_StructTestget_statusFillAtt(lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!LifoGet(&LcsfBridgeStructTestInfo.Lifo, LCSF_STRUCTTEST_CMD_GET_STATUS_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    uint8_t *pStructBuff = LcsfBridgeStructTestInfo.StructBuff;
    // Fill data of attribute status
    pAttArray[STRUCTTEST_GET_STATUS_ATT_STATUS].PayloadSize = LCSF_Bridge_StructTeststatusSerialize(&(pCmdPayload->get_status_payload.status), pStructBuff);
    pAttArray[STRUCTTEST_GET_STATUS_ATT_STATUS].Payload.pData = pStructBuff;
    pStructBuff += STRUCTTEST_STATUS_SIZE;
    // Fill data of attribute links
    pAttArray[STRUCTTEST_GET_STATUS_ATT_LINKS].PayloadSize = pCmdPayload->get_status_payload.linksSize;
    pAttArray[STRUCTTEST_GET_STATUS_ATT_LINKS].Payload.pData = pCmdPayload->get_status_payload.p_links;
    // Fill data of attribute opt_status
    if ((pCmdPayload->get_status_payload.optAttFlagsBitfield & STRUCTTEST_GET_STATUS_ATT_OPT_STATUS_FLAG) != 0) {
        pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].PayloadSize = LCSF_Bridge_StructTeststatusSerialize(&(pCmdPayload->get_status_payload.opt_status), pStructBuff);
        pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].Payload.pData = pStructBuff;
        pStructBuff += STRUCTTEST_STATUS_SIZE;
    } else {
        pAttArray[STRUCTTEST_GET_STATUS_ATT_OPT_STATUS].Payload.pData = NULL;
    }
    // Fill data of attribute ext_status
    if ((pCmdPayload->get_status_payload.optAttFlagsBitfield & STRUCTTEST_GET_STATUS_ATT_EXT_STATUS_FLAG) != 0) {
        pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].PayloadSize = pCmdPayload->get_status_payload.ext_statusSize;
        pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].Payload.pData = pCmdPayload->get_status_payload.p_ext_status;
    } else {
        pAttArray[STRUCTTEST_GET_STATUS_ATT_EXT_STATUS].Payload.pData = NULL;
    }
    // Fill data of attribute counters
    pAttArray[STRUCTTEST_GET_STATUS_ATT_COUNTERS].PayloadSize = LCSF_Bridge_StructTestcountersSerialize(&(pCmdPayload->get_status_payload.counters), pStructBuff);
    pAttArray[STRUCTTEST_GET_STATUS_ATT_COUNTERS].Payload.pData = pStructBuff;
    pStructBuff += STRUCTTEST_COUNTERS_SIZE;
    // Fill data of attribute link_counters
    pAttArray[STRUCTTEST_GET_STATUS_ATT_LINK_COUNTERS].PayloadSize = pCmdPayload->get_status_payload.link_countersSize;
    pAttArray[STRUCTTEST_GET_STATUS_ATT_LINK_COUNTERS].Payload.pData = pCmdPayload->get_status_payload.p_link_counters;
    return true;
}

static bool LCSF_Bridge_StructTestset_cfgFillAtt(lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload) {
    if (pCmdPayload == NULL) {
        return false;
    }
    // Allocate attribute array
    if (!LifoGet(&LcsfBridgeStructTestInfo.Lifo, LCSF_STRUCTTEST_CMD_SET_CFG_ATT_NB, (void *)pAttArrayAddr)) {
        return false;
    }
    // Intermediary variable
    lcsf_valid_att_t *pAttArray = *pAttArrayAddr;
    lcsf_valid_att_t **pSubAttArray = NULL;
    uint8_t *pStructBuff = LcsfBridgeStructTestInfo.StructBuff;
    // Fill data of attribute mode
    pAttArray[STRUCTTEST_SET_CFG_ATT_MODE].PayloadSize = GetVLESize(pCmdPayload->set_cfg_payload.mode);
    pAttArray[STRUCTTEST_SET_CFG_ATT_MODE].Payload.pData = &(pCmdPayload->set_cfg_payload.mode);
    // Fill data of attribute cfg
    // Intermediary variable
    pSubAttArray = &(pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray);
    // Allocate sub-attribute array
    if (!LifoGet(&LcsfBridgeStructTestInfo.Lifo, LCSF_STRUCTTEST_ATT_CFG_SUBATT_NB, (void *)pSubAttArray)) {
        return false;
    }
    // Fill data of sub-attribute link
    pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINK].PayloadSize = LCSF_Bridge_StructTestlinkSerialize(&(pCmdPayload->set_cfg_payload.cfg_payload.link), pStructBuff);
    pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINK].Payload.pData = pStructBuff;
    pStructBuff += STRUCTTEST_LINK_SIZE;
    // Fill data of sub-attribute links
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_LINKS_FLAG) != 0) {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].PayloadSize = pCmdPayload->set_cfg_payload.cfg_payload.linksSize;
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].Payload.pData = pCmdPayload->set_cfg_payload.cfg_payload.p_links;
    } else {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_LINKS].Payload.pData = NULL;
    }
    // Fill data of sub-attribute opt_status
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_OPT_STATUS_FLAG) != 0) {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].PayloadSize = LCSF_Bridge_StructTeststatusSerialize(&(pCmdPayload->set_cfg_payload.cfg_payload.opt_status), pStructBuff);
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].Payload.pData = pStructBuff;
        pStructBuff += STRUCTTEST_STATUS_SIZE;
    } else {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_OPT_STATUS].Payload.pData = NULL;
    }
    // Fill data of sub-attribute step
    pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_STEP].PayloadSize = GetVLESize((uint8_t)pCmdPayload->set_cfg_payload.cfg_payload.step);
    pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_STEP].Payload.pData = &(pCmdPayload->set_cfg_payload.cfg_payload.step);
    // Fill data of sub-attribute epoch
    if ((pCmdPayload->set_cfg_payload.cfg_payload.optAttFlagsBitfield & STRUCTTEST_CFG_ATT_EPOCH_FLAG) != 0) {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].PayloadSize = GetVLESize((uint64_t)pCmdPayload->set_cfg_payload.cfg_payload.epoch);
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].Payload.pData = &(pCmdPayload->set_cfg_payload.cfg_payload.epoch);
    } else {
        pAttArray[STRUCTTEST_SET_CFG_ATT_CFG].Payload.pSubAttArray[STRUCTTEST_CFG_ATT_EPOCH].Payload.pData = NULL;
    }
    // Fill data of attribute trim
    pAttArray[STRUCTTEST_SET_CFG_ATT_TRIM].PayloadSize = GetVLESize((uint16_t)pCmdPayload->set_cfg_payload.trim);
    pAttArray[STRUCTTEST_SET_CFG_ATT_TRIM].Payload.pData = &(pCmdPayload->set_cfg_payload.trim);
    // Fill data of attribute bias
    if ((pCmdPayload->set_cfg_payload.optAttFlagsBitfield & STRUCTTEST_SET_CFG_ATT_BIAS_FLAG) != 0) {
        pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].PayloadSize = GetVLESize((uint32_t)pCmdPayload->set_cfg_payload.bias);
        pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].Payload.pData = &(pCmdPayload->set_cfg_payload.bias);
    } else {
        pAttArray[STRUCTTEST_SET_CFG_ATT_BIAS].Payload.pData = NULL;
    }
    return true;
}

/**
 * \fn static bool LCSF_Bridge_StructTestFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload)
 * \brief Fill the attribute array of a command from its payload
 *
 * \param cmdName name of the command
 * \param pAttArrayAddr pointer to contain the attribute array
 * \param pCmdPayload pointer to the command payload
 * \return bool: true if operation was a success
 */
static bool LCSF_Bridge_StructTestFillCmdAtt(uint_fast16_t cmdName, lcsf_valid_att_t **pAttArrayAddr, structtest_cmd_payload_t *pCmdPayload) {
    switch (cmdName) {
        case STRUCTTEST_CMD_GET_STATUS:
            return LCSF_Bridge_StructTestget_statusFillAtt(pAttArrayAddr, pCmdPayload);

        case STRUCTTEST_CMD_SET_CFG:
            return LCSF_Bridge_StructTestset_cfgFillAtt(pAttArrayAddr, pCmdPayload);

        default: // Commands that don't have attributes
            *pAttArrayAddr = NULL;
            return true;
    }
}

/**
 * \fn static size_t LCSF_Bridge_StructTestVarUintEncode(uint64_t value, uint8_t *pBuffer)
 * \brief Encode a value in unsigned LEB128, 7 bits per byte from the least significant, MSb set when more bytes follow
 *
 * \param value value to encode
 * \param pBuffer pointer to the buffer, up to 10 bytes are written
 * \return size_t: number of bytes written
 */
static size_t LCSF_Bridge_StructTestVarUintEncode(uint64_t value, uint8_t *pBuffer) {
    size_t idx = 0;
    while (value >= 0x80) {
        pBuffer[idx++] = (uint8_t)(value | 0x80);
        value >>= 7;
    }
    pBuffer[idx++] = (uint8_t)value;
    return idx;
}

/**
 * \fn static size_t LCSF_Bridge_StructTestVarUintDecode(uint64_t *pValue, uint64_t maxValue, const uint8_t *pBuffer, size_t dataSize)
 * \brief Decode an unsigned LEB128 value
 *
 * \param pValue pointer to contain the value
 * \param maxValue largest value of the field type
 * \param pBuffer pointer to the buffer
 * \param dataSize number of bytes left in the buffer
 * \return size_t: number of bytes read, 0 if the value is cut short, over 10 bytes or over maxValue
 */
static size_t LCSF_Bridge_StructTestVarUintDecode(uint64_t *pValue, uint64_t maxValue, const uint8_t *pBuffer, size_t dataSize) {
    uint64_t value = 0;
    // A uint64_t takes up to 10 bytes, the 10th holds bit 63 only
    for (size_t idx = 0; (idx < dataSize) && (idx < 10); idx++) {
        uint64_t group = pBuffer[idx] & 0x7F;
        if ((idx == 9) && (group > 1)) {
            return 0;
        }
        value |= group << (7 * idx);
        if ((pBuffer[idx] & 0x80) == 0) {
            if (value > maxValue) {
                return 0;
            }
            *pValue = value;
            return idx + 1;
        }
    }
    return 0;
}

/**
 * \fn static uint64_t LCSF_Bridge_StructTestZigZagEncode(int64_t value)
 * \brief Map a signed value to unsigned so small magnitudes stay small: 0, -1, 1, -2 give 0, 1, 2, 3
 *
 * \param value signed value
 * \return uint64_t: zigzag value
 */
static uint64_t LCSF_Bridge_StructTestZigZagEncode(int64_t value) {
    // Shifted as unsigned, right shifting a negative value is implementation defined
    uint64_t shifted = (uint64_t)value << 1;
    return (value < 0) ? ~shifted : shifted;
}

/**
 * \fn static int64_t LCSF_Bridge_StructTestZigZagDecode(uint64_t value)
 * \brief Map a zigzag value back to signed
 *
 * \param value zigzag value
 * \return int64_t: signed value
 */
static int64_t LCSF_Bridge_StructTestZigZagDecode(uint64_t value) {
    int64_t magnitude = (int64_t)(value >> 1);
    return ((value & 1) != 0) ? -magnitude - 1 : magnitude;
}

// *** Public Functions ***

bool LCSF_Bridge_StructTestInit(void) {
    return LifoInit(&LcsfBridgeStructTestInfo.Lifo, LifoData, LCSF_BRIDGE_STRUCTTEST_LIFO_SIZE, sizeof(lcsf_valid_att_t));
}

bool LCSF_Bridge_StructTestReceive(lcsf_valid_cmd_t *pValidCmd) {
    uint16_t cmdName = LCSF_Bridge_StructTest_CMDID2CMDNAME(pValidCmd->CmdId);
    structtest_cmd_payload_t *pCmdPayload = &LcsfBridgeStructTestInfo.CmdPayload;

    // Clear payload to remove stale VLE bytes
    memset(pCmdPayload, 0, sizeof(*pCmdPayload));
    LCSF_Bridge_StructTestGetCmdData(cmdName, pValidCmd->pAttArray, pCmdPayload);
    return StructTest_MainExecute(cmdName, pCmdPayload);
}

int LCSF_Bridge_StructTestEncode(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize) {
    lcsf_valid_cmd_t sendCmd;
    sendCmd.CmdId = LCSF_Bridge_StructTest_CMDNAME2CMDID[cmdName];
    LifoFreeAll(&LcsfBridgeStructTestInfo.Lifo);

    if (!LCSF_Bridge_StructTestFillCmdAtt(cmdName, &(sendCmd.pAttArray), pCmdPayload)) {
        return -1;
    }
    return LCSF_ValidatorEncode(LCSF_STRUCTTEST_PROTOCOL_ID, &sendCmd, pBuffer, buffSize);
}

size_t LCSF_Bridge_StructTeststatusSerialize(const structtest_status_t *pStruct, uint8_t *pBuffer) {
    memcpy(&pBuffer[0], &(pStruct->uptime), sizeof(uint64_t));
    memcpy(&pBuffer[8], &(pStruct->temp), sizeof(int16_t));
    memcpy(&pBuffer[10], &(pStruct->ratio), sizeof(float));
    memcpy(&pBuffer[14], &(pStruct->flags), sizeof(uint8_t));
    memcpy(&pBuffer[15], &(pStruct->offset), sizeof(int32_t));
    memcpy(&pBuffer[19], &(pStruct->drift), sizeof(double));
    memcpy(&pBuffer[27], &(pStruct->delta), sizeof(int8_t));
    memcpy(&pBuffer[28], &(pStruct->big), sizeof(int64_t));
    memcpy(&pBuffer[36], &(pStruct->count), sizeof(uint16_t));
    return STRUCTTEST_STATUS_SIZE;
}

uint_fast16_t LCSF_Bridge_StructTeststatusDeserialize(structtest_status_t *pStruct, const uint8_t *pBuffer, size_t dataSize) {
    memset(pStruct, 0, sizeof(*pStruct));
    if (dataSize < 8) {
        return 0;
    }
    memcpy(&(pStruct->uptime), &pBuffer[0], sizeof(uint64_t));
    if (dataSize < 10) {
        return 1;
    }
    memcpy(&(pStruct->temp), &pBuffer[8], sizeof(int16_t));
    if (dataSize < 14) {
        return 2;
    }
    memcpy(&(pStruct->ratio), &pBuffer[10], sizeof(float));
    if (dataSize < 15) {
        return 3;
    }
    memcpy(&(pStruct->flags), &pBuffer[14], sizeof(uint8_t));
    if (dataSize < 19) {
        return 4;
    }
    memcpy(&(pStruct->offset), &pBuffer[15], sizeof(int32_t));
    if (dataSize < 27) {
        return 5;
    }
    memcpy(&(pStruct->drift), &pBuffer[19], sizeof(double));
    if (dataSize < 28) {
        return 6;
    }
    memcpy(&(pStruct->delta), &pBuffer[27], sizeof(int8_t));
    if (dataSize < 36) {
        return 7;
    }
    memcpy(&(pStruct->big), &pBuffer[28], sizeof(int64_t));
    if (dataSize < 38) {
        return 8;
    }
    memcpy(&(pStruct->count), &pBuffer[36], sizeof(uint16_t));
    return STRUCTTEST_STATUS_FIELD_NB;
}

size_t LCSF_Bridge_StructTeststatusArraySerialize(const structtest_status_t *pArray, uint_fast16_t nb, uint8_t *pBuffer) {
    size_t idx = 0;
    pBuffer[idx++] = STRUCTTEST_STATUS_SIZE;
    for (uint_fast16_t elemIdx = 0; elemIdx < nb; elemIdx++) {
        idx += LCSF_Bridge_StructTeststatusSerialize(&(pArray[elemIdx]), &pBuffer[idx]);
    }
    return idx;
}

uint_fast16_t LCSF_Bridge_StructTeststatusArrayDeserialize(structtest_status_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize) {
    if (dataSize < 1) {
        return 0;
    }
    // Elements may be from another struct version
    size_t elemSize = pBuffer[0];
    if (elemSize == 0) {
        return 0;
    }
    pBuffer++;
    dataSize--;
    uint_fast16_t nb = (uint_fast16_t)(dataSize / elemSize);
    if (nb > maxNb) {
        nb = maxNb;
    }
    for (uint_fast16_t elemIdx = 0; elemIdx < nb; elemIdx++) {
        LCSF_Bridge_StructTeststatusDeserialize(&(pArray[elemIdx]), &pBuffer[elemIdx * elemSize], elemSize);
    }
    return nb;
}

size_t LCSF_Bridge_StructTestlinkSerialize(const structtest_link_t *pStruct, uint8_t *pBuffer) {
    memcpy(&pBuffer[0], &(pStruct->sent), sizeof(uint32_t));
    memcpy(&pBuffer[4], &(pStruct->lost), sizeof(uint32_t));
    memcpy(&pBuffer[8], &(pStruct->crc), sizeof(uint32_t));
    return STRUCTTEST_LINK_SIZE;
}

uint_fast16_t LCSF_Bridge_StructTestlinkDeserialize(structtest_link_t *pStruct, const uint8_t *pBuffer, size_t dataSize) {
    memset(pStruct, 0, sizeof(*pStruct));
    if (dataSize < 4) {
        return 0;
    }
    memcpy(&(pStruct->sent), &pBuffer[0], sizeof(uint32_t));
    if (dataSize < 8) {
        return 1;
    }
    memcpy(&(pStruct->lost), &pBuffer[4], sizeof(uint32_t));
    if (dataSize < 12) {
        return 2;
    }
    memcpy(&(pStruct->crc), &pBuffer[8], sizeof(uint32_t));
    return STRUCTTEST_LINK_FIELD_NB;
}

size_t LCSF_Bridge_StructTestlinkArraySerialize(const structtest_link_t *pArray, uint_fast16_t nb, uint8_t *pBuffer) {
    size_t idx = 0;
    for (uint_fast16_t elemIdx = 0; elemIdx < nb; elemIdx++) {
        idx += LCSF_Bridge_StructTestlinkSerialize(&(pArray[elemIdx]), &pBuffer[idx]);
    }
    return idx;
}

uint_fast16_t LCSF_Bridge_StructTestlinkArrayDeserialize(structtest_link_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize) {
    size_t elemSize = STRUCTTEST_LINK_SIZE;
    uint_fast16_t nb = (uint_fast16_t)(dataSize / elemSize);
    if (nb > maxNb) {
        nb = maxNb;
    }
    for (uint_fast16_t elemIdx = 0; elemIdx < nb; elemIdx++) {
        LCSF_Bridge_StructTestlinkDeserialize(&(pArray[elemIdx]), &pBuffer[elemIdx * elemSize], elemSize);
    }
    return nb;
}

size_t LCSF_Bridge_StructTestcountersSerialize(const structtest_counters_t *pStruct, uint8_t *pBuffer) {
    size_t idx = 0;
    idx += LCSF_Bridge_StructTestVarUintEncode(pStruct->rx, &pBuffer[idx]);
    idx += LCSF_Bridge_StructTestVarUintEncode(pStruct->err, &pBuffer[idx]);
    idx += LCSF_Bridge_StructTestVarUintEncode(pStruct->uptime, &pBuffer[idx]);
    memcpy(&pBuffer[idx], &(pStruct->crc), sizeof(uint8_t));
    idx += sizeof(uint8_t);
    idx += LCSF_Bridge_StructTestVarUintEncode(LCSF_Bridge_StructTestZigZagEncode(pStruct->temp), &pBuffer[idx]);
    idx += LCSF_Bridge_StructTestVarUintEncode(LCSF_Bridge_StructTestZigZagEncode(pStruct->drift), &pBuffer[idx]);
    idx += LCSF_Bridge_StructTestVarUintEncode(LCSF_Bridge_StructTestZigZagEncode(pStruct->offset), &pBuffer[idx]);
    return idx;
}

uint_fast16_t LCSF_Bridge_StructTestcountersDeserialize(structtest_counters_t *pStruct, const uint8_t *pBuffer, size_t dataSize) {
    size_t idx = 0;
    size_t fieldSize = 0;
    uint64_t value = 0;
    memset(pStruct, 0, sizeof(*pStruct));
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT32_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 0;
    }
    pStruct->rx = (uint32_t)value;
    idx += fieldSize;
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT16_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 1;
    }
    pStruct->err = (uint16_t)value;
    idx += fieldSize;
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT64_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 2;
    }
    pStruct->uptime = (uint64_t)value;
    idx += fieldSize;
    if (dataSize < idx + sizeof(uint8_t)) {
        return 3;
    }
    memcpy(&(pStruct->crc), &pBuffer[idx], sizeof(uint8_t));
    idx += sizeof(uint8_t);
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT16_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 4;
    }
    pStruct->temp = (int16_t)LCSF_Bridge_StructTestZigZagDecode(value);
    idx += fieldSize;
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT32_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 5;
    }
    pStruct->drift = (int32_t)LCSF_Bridge_StructTestZigZagDecode(value);
    idx += fieldSize;
    fieldSize = LCSF_Bridge_StructTestVarUintDecode(&value, UINT64_MAX, &pBuffer[idx], dataSize - idx);
    if (fieldSize == 0) {
        return 6;
    }
    pStruct->offset = (int64_t)LCSF_Bridge_StructTestZigZagDecode(value);
    return STRUCTTEST_COUNTERS_FIELD_NB;
}

size_t LCSF_Bridge_StructTestcountersArraySerialize(const structtest_counters_t *pArray, uint_fast16_t nb, uint8_t *pBuffer) {
    size_t idx = 0;
    for (uint_fast16_t elemIdx = 0; elemIdx < nb; elemIdx++) {
        size_t elemSize = LCSF_Bridge_StructTestcountersSerialize(&(pArray[elemIdx]), &pBuffer[idx + 1]);
        pBuffer[idx] = (uint8_t)elemSize;
        idx += 1 + elemSize;
    }
    return idx;
}

uint_fast16_t LCSF_Bridge_StructTestcountersArrayDeserialize(structtest_counters_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize) {
    size_t idx = 0;
    uint_fast16_t nb = 0;
    while ((nb < maxNb) && (idx < dataSize)) {
        // Elements may be from another struct version
        size_t elemSize = pBuffer[idx];
        if (elemSize >= dataSize - idx) {
            return nb;
        }
        LCSF_Bridge_StructTestcountersDeserialize(&(pArray[nb]), &pBuffer[idx + 1], elemSize);
        idx += 1 + elemSize;
        nb++;
    }
    return nb;
}
