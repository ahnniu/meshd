/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2017  Intel Corporation. All rights reserved.
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

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
#include "mesh/meshd-onoff-model.h"

static uint8_t trans_id;
static uint16_t onoff_app_idx = APP_IDX_INVALID;

static bool client_onoff_status_msg_recvd(uint16_t src, uint16_t dst,
 						uint8_t *data, uint16_t len, void *user_data);

static int client_bind(uint16_t app_idx, int action)
{
	if (action == ACTION_ADD) {
		if (onoff_app_idx != APP_IDX_INVALID) {
			return MESH_STATUS_INSUFF_RESOURCES;
		} else {
			onoff_app_idx = app_idx;
			bt_shell_printf("On/Off client model: new binding"
					" %4.4x\n", app_idx);
		}
	} else {
		if (onoff_app_idx == app_idx)
			onoff_app_idx = APP_IDX_INVALID;
	}
	return MESH_STATUS_SUCCESS;
}

static int print_remaining_time(uint8_t remaining_time)
{
	uint8_t step = (remaining_time & 0xc0) >> 6;
	uint8_t count = remaining_time & 0x3f;
	int secs = 0, msecs = 0, minutes = 0, hours = 0;

	switch (step) {
	case 0:
		msecs = 100 * count;
		secs = msecs / 1000;
		msecs -= (secs * 1000);
		break;
	case 1:
		secs = 1 * count;
		minutes = secs / 60;
		secs -= (minutes * 60);
		break;

	case 2:
		secs = 10 * count;
		minutes = secs / 60;
		secs -= (minutes * 60);
		break;
	case 3:
		minutes = 10 * count;
		hours = minutes / 60;
		minutes -= (hours * 60);
		break;

	default:
		break;
	}

	bt_shell_printf("\n\t\tRemaining time: %d hrs %d mins %d secs %d"
			" msecs\n", hours, minutes, secs, msecs);

	return hours * 3600000 + minutes * 60000 + secs * 1000 + msecs;
}

static bool client_msg_recvd(uint16_t src, uint8_t *data,
				uint16_t len, void *user_data)
{
	uint32_t opcode;
	int n;
	uint16_t local_node_unicast;

	local_node_unicast = node_get_primary(node_get_local_node());

	if (mesh_opcode_get(data, len, &opcode, &n)) {
		len -= n;
		data += n;
	} else
		return false;

	bt_shell_printf("On Off Model Message received (%d) opcode %x\n",
								len, opcode);
	print_byte_array("\t",data, len);

	switch (opcode & ~OP_UNRELIABLE) {
	default:
		return false;

	case OP_GENERIC_ONOFF_STATUS:
		return client_onoff_status_msg_recvd(src, local_node_unicast,
				data, len, NULL);
		break;
	}

	return true;
}


static uint32_t parms[8];

static uint32_t read_input_parameters(int argc, char *argv[])
{
	uint32_t i;

	if (!argc)
		return 0;

	--argc;
	++argv;

	if (!argc || argv[0][0] == '\0')
		return 0;

	memset(parms, 0xff, sizeof(parms));

	for (i = 0; i < sizeof(parms)/sizeof(parms[0]) && i < (unsigned) argc;
									i++) {
		sscanf(argv[i], "%x", &parms[i]);
		if (parms[i] == 0xffffffff)
			break;
	}

	return i;
}

static bool send_cmd(uint16_t target, uint8_t *buf, uint16_t len)
{
	struct mesh_node *node = node_get_local_node();
	uint8_t ttl;

	if(!node)
		return false;

	ttl = node_get_default_ttl(node);

	return net_access_layer_send(ttl, node_get_primary(node),
					target, onoff_app_idx, buf, len);
}

static void cmd_get_status(int argc, char *argv[])
{
	uint16_t target;
	uint16_t n;
	uint8_t msg[32];
	struct mesh_node *node;

	if ((read_input_parameters(argc, argv) != 1)) {
		bt_shell_printf("Bad arguments\n");
		onoff_emit_cmd_failed(argv[0], -EINVAL, "Bad arguments");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	target = parms[0];

	if (IS_UNASSIGNED(target)) {
		bt_shell_printf("Invalid target address\n");
		onoff_emit_cmd_failed(argv[0], -EINVAL, "Invalid target address");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	if (IS_UNICAST(target)) {
		node = node_find_by_addr(target);

		if (!node) {
			bt_shell_printf("Invalid target address\n");
			onoff_emit_cmd_failed(argv[0], -EINVAL, "Invalid target address");
			return bt_shell_noninteractive_quit(EXIT_FAILURE);
		}
	}

	n = mesh_opcode_set(OP_GENERIC_ONOFF_GET, msg);

	if (!send_cmd(target, msg, n)) {
		bt_shell_printf("Failed to send \"GENERIC ON/OFF GET\"\n");
		onoff_emit_cmd_failed(argv[0], -EFAULT, "Failed to send GENERIC ON/OFF GET");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	return bt_shell_noninteractive_quit(EXIT_SUCCESS);
}

static void cmd_set(int argc, char *argv[])
{
	uint16_t target;
	uint16_t n;
	uint8_t msg[32];
	struct mesh_node *node;

	if ((read_input_parameters(argc, argv) != 2) &&
					parms[1] != 0 && parms[1] != 1) {
		bt_shell_printf("Bad arguments: Expecting \"0\" or \"1\"\n");
		onoff_emit_cmd_failed(argv[0], -EINVAL, "Bad arguments");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	target = parms[0];

	if (IS_UNASSIGNED(target)) {
		bt_shell_printf("Invalid target address\n");
		onoff_emit_cmd_failed(argv[0], -EINVAL, "Invalid target address");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	if (IS_UNICAST(target)) {
		node = node_find_by_addr(target);

		if (!node) {
			bt_shell_printf("Invalid target address\n");
			onoff_emit_cmd_failed(argv[0], -EINVAL, "Invalid target address");
			return bt_shell_noninteractive_quit(EXIT_FAILURE);
		}
	}

	n = mesh_opcode_set(OP_GENERIC_ONOFF_SET, msg);
	msg[n++] = parms[1];
	msg[n++] = trans_id++;

	if (!send_cmd(target, msg, n)) {
		bt_shell_printf("Failed to send \"GENERIC ON/OFF SET\"\n");
		onoff_emit_cmd_failed(argv[0], -EFAULT, "Failed to send GENERIC ON/OFF SET");
		return bt_shell_noninteractive_quit(EXIT_FAILURE);
	}

	return bt_shell_noninteractive_quit(EXIT_SUCCESS);
}

static const struct bt_shell_menu onoff_menu = {
	.name = "onoff",
	.desc = "On/Off Model Submenu",
	.entries = {
	{"get",			"<ele_addr>",				cmd_get_status,
						"Get ON/OFF status"},
	{"set",		"<addr> <0/1>",			cmd_set,
						"Send \"SET ON/OFF\" command"},
	{} },
};

static struct mesh_model_ops client_cbs = {
	client_msg_recvd,
	client_bind,
	NULL,
	NULL
};

static bool client_onoff_status_msg_recvd(uint16_t src, uint16_t dst,
 						uint8_t *data, uint16_t len, void *user_data)
{
	uint8_t state;
	int remaining_ms = -1;

	if (len != 1 && len != 3) {
		return false;
	}

	state = data[0];
	bt_shell_printf("Element %4.4x: Off Status present = %s",
					src, state ? "ON" : "OFF");

	if (len == 3) {
		bt_shell_printf(", target = %s",
				data[1] ? "ON" : "OFF");
		remaining_ms = print_remaining_time(data[2]);
	} else
		bt_shell_printf("\n");

	if(remaining_ms < 0) {
		onoff_emit_new_state(src, dst, state);
	} else {
		onoff_emit_new_state_with_remaining(src, dst, state, remaining_ms);
	}
	return true;
}

static struct mesh_opcode_ops client_onoff_status_cbs = {
	client_onoff_status_msg_recvd
};

bool onoff_client_init(uint8_t ele)
{
	if (!node_local_model_register(ele, GENERIC_ONOFF_CLIENT_MODEL_ID,
					&client_cbs, NULL))
		return false;

	if(!node_remote_opcode_register("Generic OnOff Status", OP_GENERIC_ONOFF_STATUS,
					&client_onoff_status_cbs, NULL))
		return false;

	bt_shell_add_submenu(&onoff_menu);

	return true;
}
