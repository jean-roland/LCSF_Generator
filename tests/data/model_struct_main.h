/**
 * \file StructTest_Main.h
 * \brief StructTest protocol module
 * \author LCSF Generator v1.4
 *
 */

#ifndef structtest_main_h
#define structtest_main_h

// *** Libraries include ***
// Standard lib
// Custom lib
#include <LCSF_Config.h>

// *** Definitions ***
// --- Public Types ---

// Command name enum
enum _structtest_cmd_names {
    STRUCTTEST_CMD_GET_STATUS,
    STRUCTTEST_CMD_SET_CFG,
    STRUCTTEST_CMD_COUNT,
};

// Attributes enums
enum _structtest_cfg_att_names {
    STRUCTTEST_CFG_ATT_LINK,
    STRUCTTEST_CFG_ATT_LINKS,
    STRUCTTEST_CFG_ATT_OPT_STATUS,
};

enum _structtest_get_status_att_names {
    STRUCTTEST_GET_STATUS_ATT_STATUS,
    STRUCTTEST_GET_STATUS_ATT_LINKS,
    STRUCTTEST_GET_STATUS_ATT_OPT_STATUS,
    STRUCTTEST_GET_STATUS_ATT_EXT_STATUS,
    STRUCTTEST_GET_STATUS_ATT_COUNTERS,
    STRUCTTEST_GET_STATUS_ATT_LINK_COUNTERS,
};

enum _structtest_set_cfg_att_names {
    STRUCTTEST_SET_CFG_ATT_MODE,
    STRUCTTEST_SET_CFG_ATT_CFG,
};

// Struct types
#define STRUCTTEST_STATUS_SIZE 38
#define STRUCTTEST_STATUS_FIELD_NB 9
#define STRUCTTEST_STATUS_ARRAY_SIZE(nb) (1 + (nb) * STRUCTTEST_STATUS_SIZE)
typedef struct _structtest_status {
    uint64_t uptime;
    int16_t temp;
    float ratio;
    uint8_t flags;
    int32_t offset;
    double drift;
    int8_t delta;
    int64_t big;
    uint16_t count;
} structtest_status_t;

#define STRUCTTEST_LINK_SIZE 12
#define STRUCTTEST_LINK_FIELD_NB 3
#define STRUCTTEST_LINK_ARRAY_SIZE(nb) ((nb) * STRUCTTEST_LINK_SIZE)
typedef struct _structtest_link {
    uint32_t sent;
    uint32_t lost;
    uint32_t crc;
} structtest_link_t;

// counters has variable-length fields, its sizes are maximums
#define STRUCTTEST_COUNTERS_SIZE 37
#define STRUCTTEST_COUNTERS_FIELD_NB 7
#define STRUCTTEST_COUNTERS_ARRAY_SIZE(nb) ((nb) * (1 + STRUCTTEST_COUNTERS_SIZE))
typedef struct _structtest_counters {
    uint32_t rx;
    uint16_t err;
    uint64_t uptime;
    uint8_t crc;
    int16_t temp;
    int32_t drift;
    int64_t offset;
} structtest_counters_t;

// Attribute with sub-attributes structures
#define STRUCTTEST_CFG_ATT_LINKS_FLAG (1 << 0)
#define STRUCTTEST_CFG_ATT_OPT_STATUS_FLAG (1 << 1)
typedef struct _structtest_set_cfg_att_cfg_att_payload {
    uint8_t optAttFlagsBitfield;
    uint16_t linkFieldNb;
    structtest_link_t link;
    uint32_t linksSize;
    uint8_t *p_links;
    uint16_t opt_statusFieldNb;
    structtest_status_t opt_status;
} structtest_set_cfg_att_cfg_att_payload_t;

// Commands data structures
#define STRUCTTEST_GET_STATUS_ATT_OPT_STATUS_FLAG (1 << 0)
#define STRUCTTEST_GET_STATUS_ATT_EXT_STATUS_FLAG (1 << 1)
typedef struct _structtest_get_status_att_payload {
    uint8_t optAttFlagsBitfield;
    uint16_t statusFieldNb;
    structtest_status_t status;
    uint32_t linksSize;
    uint8_t *p_links;
    uint16_t opt_statusFieldNb;
    structtest_status_t opt_status;
    uint32_t ext_statusSize;
    uint8_t *p_ext_status;
    uint16_t countersFieldNb;
    structtest_counters_t counters;
    uint32_t link_countersSize;
    uint8_t *p_link_counters;
} structtest_get_status_att_payload_t;

typedef struct _structtest_set_cfg_att_payload {
    uint8_t mode;
    structtest_set_cfg_att_cfg_att_payload_t cfg_payload;
} structtest_set_cfg_att_payload_t;

// Command payload union
typedef union _structtest_cmd_payload {
    structtest_get_status_att_payload_t get_status_payload;
    structtest_set_cfg_att_payload_t set_cfg_payload;
} structtest_cmd_payload_t;

// --- Public Function Prototypes ---

/**
 * \fn bool StructTest_MainInit(uint8_t *pBuffer, size_t buffSize)
 * \brief Initialize the module
 *
 * \param pBuffer pointer to send buffer
 * \param buffSize buffer size
 * \return bool: true if operation was a success
 */
bool StructTest_MainInit(uint8_t *pBuffer, size_t buffSize);

/**
 * \fn bool StructTest_MainExecute(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload)
 * \brief Execute a command
 *
 * \param cmdName name of the command
 * \param pCmdPayload pointer to command payload
 * \return bool: true if operation was a success
 */
bool StructTest_MainExecute(uint_fast16_t cmdName, structtest_cmd_payload_t *pCmdPayload);

// *** End Definitions ***
#endif // structtest_Main_h
