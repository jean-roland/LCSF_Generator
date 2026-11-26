/**
 * \file LCSF_Bridge_StructTest.h
 * \brief StructTest LCSF bridge module
 * \author LCSF Generator v1.4
 *
 */

#ifndef Lcsf_bridge_structtest_h
#define Lcsf_bridge_structtest_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include "StructTest_Main.h"
#include <LCSF_Config.h>
#include <lib/LCSF_Validator.h>

// *** Definitions ***
// --- Public Types ---

// Command identifier enum
enum _lcsf_structtest_cmd_id {
    LCSF_STRUCTTEST_CMD_ID_GET_STATUS = 0x0,
    LCSF_STRUCTTEST_CMD_ID_SET_CFG = 0x1,
};

// Attribute identifier enums
enum _lcsf_structtest_cfg_att_id {
    LCSF_STRUCTTEST_CFG_ATT_ID_LINK = 0x0,
    LCSF_STRUCTTEST_CFG_ATT_ID_LINKS = 0x1,
    LCSF_STRUCTTEST_CFG_ATT_ID_OPT_STATUS = 0x2,
    LCSF_STRUCTTEST_CFG_ATT_ID_STEP = 0x3,
    LCSF_STRUCTTEST_CFG_ATT_ID_EPOCH = 0x4,
};

enum _lcsf_structtest_get_status_att_id {
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_STATUS = 0x0,
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_LINKS = 0x1,
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_OPT_STATUS = 0x2,
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_EXT_STATUS = 0x3,
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_COUNTERS = 0x4,
    LCSF_STRUCTTEST_GET_STATUS_ATT_ID_LINK_COUNTERS = 0x5,
};

enum _lcsf_structtest_set_cfg_att_id {
    LCSF_STRUCTTEST_SET_CFG_ATT_ID_MODE = 0x0,
    LCSF_STRUCTTEST_SET_CFG_ATT_ID_CFG = 0x1,
    LCSF_STRUCTTEST_SET_CFG_ATT_ID_TRIM = 0x2,
    LCSF_STRUCTTEST_SET_CFG_ATT_ID_BIAS = 0x3,
};

// --- Public Constants ---

// Bridge decoder lifo size
#define LCSF_BRIDGE_STRUCTTEST_LIFO_SIZE 9
// Lcsf protocol identifier
#define LCSF_STRUCTTEST_PROTOCOL_ID 0x56
// Lcsf protocol version
#define LCSF_STRUCTTEST_PROTOCOL_VERSION 1
// Command number
#define LCSF_STRUCTTEST_CMD_NB STRUCTTEST_CMD_COUNT
// Command attribute number
#define LCSF_STRUCTTEST_CMD_GET_STATUS_ATT_NB 6
#define LCSF_STRUCTTEST_CMD_SET_CFG_ATT_NB 4
// Attribute sub-attributes number
#define LCSF_STRUCTTEST_ATT_CFG_SUBATT_NB 5
// Bridge struct serialization buffer size
#define LCSF_BRIDGE_STRUCTTEST_STRUCT_BUFF_SIZE 113
// Struct types fit in an attribute payload
_Static_assert(STRUCTTEST_STATUS_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct status is over the attribute payload max size");
_Static_assert(STRUCTTEST_LINK_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct link is over the attribute payload max size");
_Static_assert(STRUCTTEST_COUNTERS_SIZE <= LCSF_ATT_PAYLOAD_MAX_SIZE, "struct counters is over the attribute payload max size");

// Protocol descriptor
extern const lcsf_validator_protocol_desc_t LCSF_StructTest_ProtDesc;

// --- Public Function Prototypes ---

/**
 * \fn bool LCSF_Bridge_StructTestInit(void)
 * \brief Initialize the module
 *
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_StructTestInit(void);

/**
 * \fn bool LCSF_Bridge_StructTestReceive(lcsf_valid_cmd_t *pValidCmd)
 * \brief Receive valid command from LCSF_Validator and transmit to StructTest_Main
 *
 * \param pValidCmd pointer to the valid command
 * \return bool: true if operation was a success
 */
bool LCSF_Bridge_StructTestReceive(lcsf_valid_cmd_t *pValidCmd);

/**
 * \fn int LCSF_Bridge_StructTestEncode(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize)
 * \brief Receive command from StructTest_Main and transmit to LCSF_Validator for encoding
 *
 * \param cmdName name of the command
 * \param pValidCmd pointer to the valid command
 * \param pBuffer pointer to the send buffer
 * \param buffSize buffer size
 * \return int: -1 if operation failed, encoded message size if success
 */
int LCSF_Bridge_StructTestEncode(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload, uint8_t *pBuffer, size_t buffSize);

/**
 * \fn size_t LCSF_Bridge_StructTeststatusSerialize(const structtest_status_t *pStruct, uint8_t *pBuffer)
 * \brief Serialize a status struct in STRUCTTEST_STATUS_SIZE bytes
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTeststatusSerialize(const structtest_status_t *pStruct, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTeststatusDeserialize(structtest_status_t *pStruct, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a status struct, missing trailing fields are zeroed
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of fields read
 */
uint_fast16_t LCSF_Bridge_StructTeststatusDeserialize(structtest_status_t *pStruct, const uint8_t *pBuffer, size_t dataSize);

/**
 * \fn size_t LCSF_Bridge_StructTeststatusArraySerialize(const structtest_status_t *pArray, uint_fast16_t nb, uint8_t *pBuffer)
 * \brief Serialize a status struct array in STRUCTTEST_STATUS_ARRAY_SIZE(nb) bytes
 *
 * \param pArray pointer to the struct array
 * \param nb number of structs
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTeststatusArraySerialize(const structtest_status_t *pArray, uint_fast16_t nb, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTeststatusArrayDeserialize(structtest_status_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a status struct array
 *
 * \param pArray pointer to the struct array
 * \param maxNb struct array capacity
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of structs read
 */
uint_fast16_t LCSF_Bridge_StructTeststatusArrayDeserialize(structtest_status_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize);

/**
 * \fn size_t LCSF_Bridge_StructTestlinkSerialize(const structtest_link_t *pStruct, uint8_t *pBuffer)
 * \brief Serialize a link struct in STRUCTTEST_LINK_SIZE bytes
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTestlinkSerialize(const structtest_link_t *pStruct, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTestlinkDeserialize(structtest_link_t *pStruct, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a link struct, missing trailing fields are zeroed
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of fields read
 */
uint_fast16_t LCSF_Bridge_StructTestlinkDeserialize(structtest_link_t *pStruct, const uint8_t *pBuffer, size_t dataSize);

/**
 * \fn size_t LCSF_Bridge_StructTestlinkArraySerialize(const structtest_link_t *pArray, uint_fast16_t nb, uint8_t *pBuffer)
 * \brief Serialize a link struct array in STRUCTTEST_LINK_ARRAY_SIZE(nb) bytes
 *
 * \param pArray pointer to the struct array
 * \param nb number of structs
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTestlinkArraySerialize(const structtest_link_t *pArray, uint_fast16_t nb, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTestlinkArrayDeserialize(structtest_link_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a link struct array
 *
 * \param pArray pointer to the struct array
 * \param maxNb struct array capacity
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of structs read
 */
uint_fast16_t LCSF_Bridge_StructTestlinkArrayDeserialize(structtest_link_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize);

/**
 * \fn size_t LCSF_Bridge_StructTestcountersSerialize(const structtest_counters_t *pStruct, uint8_t *pBuffer)
 * \brief Serialize a counters struct in up to STRUCTTEST_COUNTERS_SIZE bytes
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTestcountersSerialize(const structtest_counters_t *pStruct, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTestcountersDeserialize(structtest_counters_t *pStruct, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a counters struct, missing trailing fields are zeroed
 *
 * \param pStruct pointer to the struct
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of fields read
 */
uint_fast16_t LCSF_Bridge_StructTestcountersDeserialize(structtest_counters_t *pStruct, const uint8_t *pBuffer, size_t dataSize);

/**
 * \fn size_t LCSF_Bridge_StructTestcountersArraySerialize(const structtest_counters_t *pArray, uint_fast16_t nb, uint8_t *pBuffer)
 * \brief Serialize a counters struct array in up to STRUCTTEST_COUNTERS_ARRAY_SIZE(nb) bytes
 *
 * \param pArray pointer to the struct array
 * \param nb number of structs
 * \param pBuffer pointer to the buffer
 * \return size_t: number of bytes written
 */
size_t LCSF_Bridge_StructTestcountersArraySerialize(const structtest_counters_t *pArray, uint_fast16_t nb, uint8_t *pBuffer);

/**
 * \fn uint_fast16_t LCSF_Bridge_StructTestcountersArrayDeserialize(structtest_counters_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize)
 * \brief Deserialize a counters struct array
 *
 * \param pArray pointer to the struct array
 * \param maxNb struct array capacity
 * \param pBuffer pointer to the data
 * \param dataSize data size
 * \return uint_fast16_t: number of structs read
 */
uint_fast16_t LCSF_Bridge_StructTestcountersArrayDeserialize(structtest_counters_t *pArray, uint_fast16_t maxNb, const uint8_t *pBuffer, size_t dataSize);

// *** End Definitions ***
#endif // Lcsf_bridge_structtest_h
