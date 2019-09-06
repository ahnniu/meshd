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
	DBusMessageIter iter;
	dbus_uint16_t next_idx;
	int error;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT16, &next_idx,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%d)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		next_idx);

	error = bt_shell_manual_input_fmt("connect %d", next_idx);

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
			GDBUS_ARGS({ "net_idx", "q" }),
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
