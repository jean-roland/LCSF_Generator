#pragma once

#include <QList>
#include <QString>
#include "enumtype.h"
#include "Command.h"
#include "Attribute.h"

// Model protocol info
QString protocol_name = "Test";
QString protocol_id = "55";

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
QList<Attribute *> CC1_att_array = {&CC1_SA1, &CC1_SA2, &CC1_SA3, &CC1_SA4, &CC1_SA5, &CC1_SA6, &CC1_SA7, &CC1_SA8, &CC1_SA9, &CC1_SA10};
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
QList<Attribute *> CC2_att_array = {&CC2_SA1, &CC2_SA2, &CC2_SA3, &CC2_SA4, &CC2_SA5, &CC2_SA6, &CC2_SA7, &CC2_SA8, &CC2_SA9, &CC2_SA10};
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
QList<Attribute *> CC3_att_array = {&CC3_SA1, &CC3_SA2, &CC3_SA3, &CC3_SA4, &CC3_SA5, &CC3_SA6, &CC3_SA7, &CC3_SA8, &CC3_SA9, &CC3_SA10};
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