#ifndef _SENSOR_MODEL_H
#define _SENSOR_MODEL_H

#include "mesh/model.h"

enum sensor_data_format {
	SENSOR_DATA_FORMAT_A = 0,
	SENSOR_DATA_FORMAT_B = 1
};

#define SENSOR_IS_TLV_A(tag) ((tag & 0x01) == SENSOR_DATA_FORMAT_A)
#define SENSOR_TLV_A_SIZE (sizeof(struct sensor_tlv_a))
#define SENSOR_TLV_B_SIZE (sizeof(struct sensor_tlv_b))

struct sensor_tlv_a {
	uint16_t format:1;
	uint16_t length:4;
	uint16_t prop_id:11;
} __attribute__ ((packed));

struct sensor_tlv_b {
	uint8_t format:1;
	uint8_t length:7;
	uint16_t prop_id:11;
} __attribute__ ((packed));

struct sensor_desciptor {
	uint16_t prop_id;
	uint16_t positive_tolerance:12;
	uint16_t negative_tolerance:12;
	uint8_t sampling_function;
	uint8_t measurement_period;
	uint8_t update_interval;
} __attribute__ ((packed));

#define BT_MESH_MODEL_OP_SENS_DESC_GET		BT_MESH_MODEL_OP_2(0x82, 0x30)
#define BT_MESH_MODEL_OP_SENS_DESC_GET_AS_STRING "Sensor Descriptor Get"

#define BT_MESH_MODEL_OP_SENS_GET		BT_MESH_MODEL_OP_2(0x82, 0x31)
#define BT_MESH_MODEL_OP_SENS_GET_AS_STRING "Sensor Get"

#define BT_MESH_MODEL_OP_SENS_COL_GET		BT_MESH_MODEL_OP_2(0x82, 0x32)
#define BT_MESH_MODEL_OP_SENS_SERIES_GET	BT_MESH_MODEL_OP_2(0x82, 0x33)

#define BT_MESH_MODEL_OP_SENS_DESC_STATUS	BT_MESH_MODEL_OP_1(0x51)
#define BT_MESH_MODEL_OP_SENS_DESC_STATUS_AS_STRING "Sensor Descriptor Status"

#define BT_MESH_MODEL_OP_SENS_STATUS		BT_MESH_MODEL_OP_1(0x52)
#define BT_MESH_MODEL_OP_SENS_STATUS_AS_STRING "Sensor Status"

bool sensor_client_init();

#endif