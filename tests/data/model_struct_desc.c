/**
 * \file LCSF_Desc_StructTest.c
 * \brief StructTest LCSF descriptor
 * \author LCSF Generator v1.5
 *
 */

// *** Libraries include ***
// Standard lib
// Custom lib
#include "LCSF_Bridge_StructTest.h"
#include <LCSF_Config.h>

// *** Definitions ***
// --- Private Constants ---

// Sub-attribute array descriptor of attribute cfg
static const lcsf_attribute_desc_t LCSF_StructTest_cfg_AttDescArray[LCSF_STRUCTTEST_ATT_CFG_SUBATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_CFG_ATT_ID_LINK, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_CFG_ATT_ID_LINKS, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_CFG_ATT_ID_OPT_STATUS, 0, NULL},
    {false, LCSF_UINT8, LCSF_STRUCTTEST_CFG_ATT_ID_STEP, 0, NULL},
    {true, LCSF_UINT64, LCSF_STRUCTTEST_CFG_ATT_ID_EPOCH, 0, NULL},
};

// Attribute array descriptor of command get_status
static const lcsf_attribute_desc_t LCSF_StructTest_get_status_AttDescArray[LCSF_STRUCTTEST_CMD_GET_STATUS_ATT_NB] = {
    {false, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_STATUS, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_LINKS, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_OPT_STATUS, 0, NULL},
    {true, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_EXT_STATUS, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_COUNTERS, 0, NULL},
    {false, LCSF_BYTE_ARRAY, LCSF_STRUCTTEST_GET_STATUS_ATT_ID_LINK_COUNTERS, 0, NULL},
};

// Attribute array descriptor of command set_cfg
static const lcsf_attribute_desc_t LCSF_StructTest_set_cfg_AttDescArray[LCSF_STRUCTTEST_CMD_SET_CFG_ATT_NB] = {
    {false, LCSF_UINT8, LCSF_STRUCTTEST_SET_CFG_ATT_ID_MODE, 0, NULL},
    {false, LCSF_SUB_ATTRIBUTES, LCSF_STRUCTTEST_SET_CFG_ATT_ID_CFG, LCSF_STRUCTTEST_ATT_CFG_SUBATT_NB, LCSF_StructTest_cfg_AttDescArray},
    {false, LCSF_UINT16, LCSF_STRUCTTEST_SET_CFG_ATT_ID_TRIM, 0, NULL},
    {true, LCSF_UINT32, LCSF_STRUCTTEST_SET_CFG_ATT_ID_BIAS, 0, NULL},
};

// Command array descriptor
static const lcsf_command_desc_t LCSF_StructTest_CmdDescArray[LCSF_STRUCTTEST_CMD_NB] = {
    {LCSF_STRUCTTEST_CMD_ID_GET_STATUS, LCSF_STRUCTTEST_CMD_GET_STATUS_ATT_NB, LCSF_StructTest_get_status_AttDescArray},
    {LCSF_STRUCTTEST_CMD_ID_SET_CFG, LCSF_STRUCTTEST_CMD_SET_CFG_ATT_NB, LCSF_StructTest_set_cfg_AttDescArray},
};

// --- Public Constants ---

// Protocol descriptor
static const lcsf_protocol_desc_t LCSF_StructTest_Desc = {.CmdNb = LCSF_STRUCTTEST_CMD_NB, .pCmdDescArray = LCSF_StructTest_CmdDescArray};
const lcsf_validator_protocol_desc_t LCSF_StructTest_ProtDesc = {.ProtId = LCSF_STRUCTTEST_PROTOCOL_ID, .ProtVer = LCSF_STRUCTTEST_PROTOCOL_VERSION, .pProtDesc = &LCSF_StructTest_Desc, .pFnInterpretMsg = LCSF_Bridge_StructTestReceive};
