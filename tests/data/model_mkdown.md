# Test protocol tables

Test protocol id: 55

## Commands table

| Name | Id | Direction | Description | Attributes? |
|:----:|:--:|:---------:|:-----------:|:------------:|
| SC1 | `0x00` | `A to B` | Simple command A to B | No |
| SC2 | `0x01` | `B to A` | Simple command B to A | No |
| SC3 | `0x02` | `Bidirectional` | Simple command bidirectional | No |
| CC1 | `0x03` | `A to B` | Complex command no sub-attributes A to B | Yes |
| CC2 | `0x04` | `B to A` | Complex command no sub-attributes B to A | Yes |
| CC3 | `0x05` | `Bidirectional` | Complex command no sub-attributes bidirectional | Yes |
| CC4 | `0x06` | `A to B` | Complex command with sub-attributes A to B | Yes |
| CC5 | `0x07` | `B to A` | Complex command with sub-attributes B to A | Yes |
| CC6 | `0x08` | `Bidirectional` | Complex command with sub-attributes bidirectional | Yes |

## CC1 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute |
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute |
| SA3 | `0x02` | No | `(u)int32` | Non optional uint32 simple attribute |
| SA4 | `0x03` | No | `byte array` | Non optional byte array simple attribute |
| SA5 | `0x04` | No | `string` | Non optional string simple attribute |
| SA6 | `0x05` | Yes | `(u)int8` | Optional uint8 simple attribute |
| SA7 | `0x06` | Yes | `(u)int16` | Optional uint16 simple attribute |
| SA8 | `0x07` | Yes | `(u)int32` | Optional uint32 simple attribute |
| SA9 | `0x08` | Yes | `byte array` | Optional byte array simple attribute |
| SA10 | `0x09` | Yes | `string` | Optional string simple attribute |

## CC2 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute |
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute |
| SA3 | `0x02` | No | `(u)int32` | Non optional uint32 simple attribute |
| SA4 | `0x03` | No | `byte array` | Non optional byte array simple attribute |
| SA5 | `0x04` | No | `string` | Non optional string simple attribute |
| SA6 | `0x05` | Yes | `(u)int8` | Optional uint8 simple attribute |
| SA7 | `0x06` | Yes | `(u)int16` | Optional uint16 simple attribute |
| SA8 | `0x07` | Yes | `(u)int32` | Optional uint32 simple attribute |
| SA9 | `0x08` | Yes | `byte array` | Optional byte array simple attribute |
| SA10 | `0x09` | Yes | `string` | Optional string simple attribute |

## CC3 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute |
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute |
| SA3 | `0x02` | No | `(u)int32` | Non optional uint32 simple attribute |
| SA4 | `0x03` | No | `byte array` | Non optional byte array simple attribute |
| SA5 | `0x04` | No | `string` | Non optional string simple attribute |
| SA6 | `0x05` | Yes | `(u)int8` | Optional uint8 simple attribute |
| SA7 | `0x06` | Yes | `(u)int16` | Optional uint16 simple attribute |
| SA8 | `0x07` | Yes | `(u)int32` | Optional uint32 simple attribute |
| SA9 | `0x08` | Yes | `byte array` | Optional byte array simple attribute |
| SA10 | `0x09` | Yes | `string` | Optional string simple attribute |

## CC4 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute |
| CA1 | `0x0a` | No | `sub-attributes` | Non optional complex attribute |
| CA2 | `0x0b` | Yes | `sub-attributes` | Optional complex attribute |

## CC5 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute |
| CA5 | `0x0a` | No | `sub-attributes` | Non optional complex attribute |
| CA6 | `0x0b` | Yes | `sub-attributes` | Optional complex attribute |

## CC6 attributes table

| Name | Id | Optional? | Data type | Attribute Description |
|:----:|:--:|:----------:|:---------:|:---------------------:|
| SA4 | `0x03` | No | `byte array` | Non optional byte array simple attribute |
| CA9 | `0x0a` | No | `sub-attributes` | Non optional complex attribute |
| CA10 | `0x0b` | Yes | `sub-attributes` | Optional complex attribute |

## CA1 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute
| SA3 | `0x02` | Yes | `(u)int32` | Optional uint32 simple attribute

## CA10 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA11 | `0x0b` | No | `sub-attributes` | Non-optional complex attribute

## CA11 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA12 | `0x0a` | No | `sub-attributes` | Non-optional complex attribute

## CA12 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA4 | `0x03` | No | `byte array` | Non-optional byte array simple attribute

## CA2 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA3 | `0x0b` | No | `sub-attributes` | Non-optional complex attribute

## CA3 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA4 | `0x0a` | No | `sub-attributes` | Non-optional complex attribute

## CA4 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA4 | `0x03` | No | `byte array` | Non-optional byte array simple attribute

## CA5 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute
| SA3 | `0x02` | Yes | `(u)int32` | Optional uint32 simple attribute

## CA6 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA7 | `0x0b` | No | `sub-attributes` | Non-optional complex attribute

## CA7 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | Yes | `(u)int8` | Optional uint8 simple attribute
| CA8 | `0x0a` | No | `sub-attributes` | Non-optional complex attribute

## CA8 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA4 | `0x03` | No | `byte array` | Non-optional byte array simple attribute

## CA9 sub-attributes table

| Name | Id | Optional? | Data type | Description |
|:----:|:--:|:----------:|:---------:|:-----------:|
| SA1 | `0x00` | No | `(u)int8` | Non optional uint8 simple attribute
| SA2 | `0x01` | No | `(u)int16` | Non optional uint16 simple attribute
| SA3 | `0x02` | Yes | `(u)int32` | Optional uint32 simple attribute

