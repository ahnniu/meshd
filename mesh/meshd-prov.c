#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <wordexp.h>

#include <inttypes.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dbus/dbus.h>
#include <string.h>

#include "gdbus/gdbus.h"
#include "src/shared/mainloop.h"
#include "src/shared/shell.h"
#include "mesh/dbus-server.h"
#include "mesh/meshd-prov.h"

// Here is a test use dbus-send command
// sudo dbus-send --system --type=method_call --print-reply --dest=org.embest /org/embest/Provisioner org.embest.MeshInterface.DiscoverUnprovisioned string:"on"
static DBusMessage *exec_discover_unprovisioned(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *onoff;
	int error;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &onoff,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		onoff);

	error = bt_shell_manual_input_fmt("discover_unprovisioned %s", onoff);

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_enter_key(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *key;
	int error;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &key,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		key);

	error = bt_shell_manual_input_fmt("%s", key);

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_start_prov(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *uuid;
	int error;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &uuid,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		uuid);

	error = bt_shell_manual_input_fmt("provision %s", uuid);

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_start_connect(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter array, dict, entry, variant;
	int error;
	int i, array_len, int_value;
	int variant_type;
	const char* key;
	uint16_t net_idx = 0, node_addr = 0;

	dbus_message_iter_init(msg, &array);

	if(strcmp(dbus_message_iter_get_signature(&array), "a{sv}")) {
		return meshd_error_invalid_args(msg);
	}
	array_len = dbus_message_iter_get_element_count(&array);
	dbus_message_iter_recurse(&array, &dict);

	for(i = 0; i < array_len; i++) {
		dbus_message_iter_recurse(&dict, &entry);
		dbus_message_iter_get_basic(&entry, &key);
		dbus_message_iter_next(&entry);
		dbus_message_iter_recurse(&entry, &variant);
		variant_type = dbus_message_iter_get_arg_type(&variant);
		if(variant_type == DBUS_TYPE_UINT16 || variant_type == DBUS_TYPE_UINT32 ||
				variant_type == DBUS_TYPE_INT16 || variant_type == DBUS_TYPE_INT32) {
			dbus_message_iter_get_basic(&variant, &int_value);
			if(!strcmp(key, "net_idx")) {
				net_idx = int_value & 0xFFFF;
			} else if(!strcmp(key, "node_addr")) {
				node_addr = int_value & 0xFFFF;
			}
		}
		dbus_message_iter_next(&dict);
	}

	bt_shell_printf("[dbus][%s][%s] %s.%s"
		"(net_idx = %x, addr = %4.4x)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		net_idx, node_addr);

	if(!node_addr) {
		// Connect to node to config
		error = bt_shell_manual_input_fmt("connect %x %4.4x",
							net_idx, node_addr);
	} else {
		// Connect to net
		error = bt_shell_manual_input_fmt("connect %x",
							net_idx);
	}

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_start_disconnect(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	int error;

	bt_shell_printf("[dbus][%s][%s] %s.%s()\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg));

	error = bt_shell_manual_input_fmt("disconnect");

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_start_mesh_info(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	int error;

	bt_shell_printf("[dbus][%s][%s] %s.%s()\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg));

	error = bt_shell_manual_input_fmt("mesh_info");

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static const GDBusMethodTable prov_methods[] = {
	{
		GDBUS_METHOD("discover_unprovisioned",
			GDBUS_ARGS({ "onoff", "s" }),
			NULL,
			exec_discover_unprovisioned)
	},
	{
		GDBUS_METHOD("provision",
			GDBUS_ARGS({ "uuid", "s" }),
			NULL,
			exec_start_prov)
	},
	{
		GDBUS_METHOD("enter_key",
			GDBUS_ARGS({ "key", "s" }),
			NULL,
			exec_enter_key)
	},
	{
		GDBUS_METHOD("connect",
			GDBUS_ARGS({ "net_idx", "a{sv}" }),
			NULL,
			exec_start_connect)
	},
	{
		GDBUS_METHOD("disconnect",
			NULL,
			NULL,
			exec_start_disconnect)
	},
	{
		GDBUS_METHOD("mesh_info",
			NULL,
			NULL,
			exec_start_mesh_info)
	},
	{ }
};

static const GDBusSignalTable prov_signals[] = {
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED,
			GDBUS_ARGS({ "device", "a{sv}" }))
	},
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_PROVISIONING, GDBUS_ARGS({ "result", "a{sv}" })) },
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_CONNECTING, GDBUS_ARGS({ "result", "a{sv}" })) },
	{ }
};

int prov_register()
{
	gboolean status;
	char* path = MESHCTLD_OBJECT_PATH_PROVISIONER;
	DBusConnection *dbus_conn;

	dbus_conn = meshd_get_dbus_connection();

	status = g_dbus_register_interface(dbus_conn,
		path, MESHCTLD_DBUS_MESH_INTERFACE,
		prov_methods, prov_signals,
		NULL, NULL, NULL);

	if(!status) return -EINVAL;

	return 0;
}
