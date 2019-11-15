#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/uio.h>
#include <wordexp.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <glib.h>

#include "src/shared/shell.h"
#include "src/shared/util.h"
#include "mesh/mesh-net.h"
#include "mesh/keys.h"
#include "mesh/net.h"
#include "mesh/node.h"
#include "mesh/prov-db.h"
#include "mesh/util.h"
#include "mesh/onoff-model.h"

#include "mesh/dbus-server.h"
#include "mesh/meshd-shell.h"
#include "mesh/meshd-model.h"
#include "mesh/cli-model.h"
#include "mesh/meshd-sensor-model.h"
#include "mesh/sensor-model.h"


static void cmd_sensor_x_get(uint16_t opcode, const char* messge, int argc, char *argv[])
{
	uint16_t target = 0;
	uint16_t prop_id = 0;
	uint16_t n;
	uint8_t msg[32];

	switch (argc) {
	case 3:
		sscanf(argv[1], "%x", &prop_id);
	case 2:
		sscanf(argv[2], "%x", &target);
		break;

	default:
		bt_shell_printf("Bad arguments\n");
		sensor_emit_cmd_failed(argv[0], -EINVAL, "Bad arguments");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
		break;
	}

	if (!target) {
		bt_shell_printf("Invalid target address\n");
		sensor_emit_cmd_failed(argv[0], -EINVAL, "Invalid target address");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	n = mesh_opcode_set(opcode, msg);
	if (prop_id) {
		put_le16(prop_id, msg + n);
		n += 2;
	}

	if (!send_cmd(target, msg, n)) {
		bt_shell_printf("Failed to send \"GENERIC ON/OFF SET\"\n");
		sensor_emit_cmd_failed(argv[0], -EFAULT,
			"Failed to send '%s'",
			messge);
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	return bt_shell_noninteractive_quit(EXIT_SUCCESS);
}

static void cmd_sensor_desc_get(int argc, char *argv[])
{
	cmd_sensor_x_get(BT_MESH_MODEL_OP_SENS_DESC_GET,
		MESH_MODEL_OPCODE_STRING(BT_MESH_MODEL_OP_SENS_DESC_GET),
		argc, argv);
}

static void cmd_sensor_get(int argc, char *argv[])
{
	cmd_sensor_x_get(BT_MESH_MODEL_OP_SENS_GET,
		MESH_MODEL_OPCODE_STRING(BT_MESH_MODEL_OP_SENS_GET),
		argc, argv);
}

static const struct bt_shell_menu sensor_menu = {
	.name = "sensor",
	.desc = "Sensor Model Submenu",
	.entries = {
		{ "sensor_desc_get", "<addr> [prop_id]", cmd_sensor_desc_get, "Sensor Descriptor Get" },
		{ "sensor_get",      "<addr> [prop_id]", cmd_sensor_get, "Sensor Get" },
		{ }
	},
};

static bool sensor_desc_status_recvd(uint16_t src, uint16_t dst,
 						uint8_t *data, uint16_t len, void *user_data)
{
	int i, k;
	struct sensor_desciptor *desc;
	int per_desc_size;
	uint8_t *buf;
	uint8_t jbuf[200];
	uint16_t jbuf_len;

	per_desc_size = sizeof(struct sensor_desciptor);

	k = len / per_desc_size;

	if (!k || (len % per_desc_size)) {
		return false;
	}

	bt_shell_printf("Sensor Desciptor Status with %d properties recived\n", k);

	jbuf_len = 200;
	memset(jbuf, 0, jbuf_len);

	for (i = 0, buf = data; i < k; i++, buf += per_desc_size) {
		desc = (struct sensor_desciptor*)buf;
		bt_shell_printf("\t[%d] prop_id = %4.4x \n",
			i, desc->prop_id);
		snprintf(jbuf, jbuf_len,
			"{"
				"\"positive_tolerance\":\"%x\","
				"\"negative_tolerance\":\"%x\","
				"\"sampling_function\":\"%x\","
				"\"measurement_period\":\"%x\","
				"\"update_interval\":\"%x\""
			"}",
			desc->positive_tolerance,
			desc->negative_tolerance,
			desc->sampling_function,
			desc->measurement_period,
			desc->update_interval
		);

		bt_shell_printf("\t\t%s\n", jbuf);
		sensor_emit_sensor_desc_status(src, dst, desc->prop_id, jbuf);
	}

	return true;
}

static bool sensor_status_recvd(uint16_t src, uint16_t dst,
 						uint8_t *data, uint16_t len, void *user_data)
{
	int i, j, n;
	uint8_t *buf;
	uint16_t tlv_len, raw_len;
	uint16_t prop_id;
	struct sensor_tlv_a *a;
	struct sensor_tlv_b *b;
	uint8_t *jbuf;
	uint16_t jbuf_len;


	if (!len) {
		return false;
	}

	bt_shell_printf("Sensor Status recived\n");

	// ["%2x", ...]
	jbuf_len = len * 5;
	jbuf = g_malloc(jbuf_len);
	if (!jbuf) {
		return false;
	}

	for (i = 0, buf = data; buf < data + len; i++) {
		if (SENSOR_IS_TLV_A(buf[0])) {
			a = (struct sensor_tlv_a*)buf;
			tlv_len = SENSOR_TLV_A_SIZE;
			raw_len = a->length;
			prop_id = a->prop_id;
		} else {
			b = (struct sensor_tlv_b*)buf;
			tlv_len = SENSOR_TLV_B_SIZE;
			raw_len = b->length;
			prop_id = b->prop_id;
		}
		bt_shell_printf("\t [%d]: prop_id = %4.4x, raw_len = %d\n", i, prop_id, raw_len);
		buf += tlv_len;
		jbuf[0] = '[';
		n = 1;
		for (j = 0; j < raw_len; j++) {
			n += snprintf(&jbuf[n], jbuf_len, "\"%2.2x\",", buf[j]);
		}
		if (raw_len > 0) {
			n--;
		}
		jbuf[n++] = ']';
		jbuf[n] = 0;
		buf += raw_len;
		bt_shell_printf("\t\tRAW: %s", jbuf);
		sensor_emit_sensor_status(src, dst, prop_id, raw_len, jbuf);
	}

	g_free(jbuf);
	return true;
}

static struct mesh_opcode_op sensor_cli_op[] = {
	MESH_OPCODE_OP(BT_MESH_MODEL_OP_SENS_DESC_STATUS, sensor_desc_status_recvd, NULL),
	MESH_OPCODE_OP(BT_MESH_MODEL_OP_SENS_STATUS, sensor_status_recvd, NULL),
	MESH_OPCODE_OP_END
};

bool sensor_client_init()
{
	if(!node_remote_opcode_register(sensor_cli_op))
		return false;

	bt_shell_add_submenu(&sensor_menu);

	return true;
}
