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

	error = bt_shell_manual_input_fmt("discover-unprovisioned %s", onoff);

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

static const GDBusMethodTable prov_methods[] = {
	{
		GDBUS_METHOD("DiscoverUnprovisioned",
			GDBUS_ARGS({ "onoff", "s" }),
			NULL,
			exec_discover_unprovisioned)
	},
	{
		GDBUS_METHOD("Provision",
			GDBUS_ARGS({ "uuid", "s" }),
			NULL,
			exec_start_prov)
	},
	{
		GDBUS_METHOD("EnterKey",
			GDBUS_ARGS({ "key", "s" }),
			NULL,
			exec_enter_key)
	},
	{
		GDBUS_METHOD("Connect",
			GDBUS_ARGS({ "net_idx", "q" }),
			NULL,
			exec_start_connect)
	},
	{
		GDBUS_METHOD("Disconnect",
			NULL,
			NULL,
			exec_start_disconnect)
	},
	{ }
};

static const GDBusSignalTable prov_signals[] = {
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED,
			GDBUS_ARGS({ "uuid", "s" }))
	},
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_REQUEST_KEY, GDBUS_ARGS({ "type", "a{sv}" })) },
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_PROVISION_DONE, GDBUS_ARGS({ "result", "a{sv}" })) },
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_CONNECT_DONE, GDBUS_ARGS({ "result", "a{sv}" })) },
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_CONNECTION_LOST, NULL) },
	{ }
};

// sudo dbus-monitor --system "type='signal',sender='org.embest',interface='org.embest.MeshInterface'"
void prov_emit_new_device_discovered(char *uuid)
{
	DBusConnection *dbus_conn;
	DBusMessage *signal;
	DBusMessageIter iter, array;
	char *arg_uuid = uuid;

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(MESHCTLD_OBJECT_PATH_PROVISIONER,
					MESHCTLD_DBUS_MESH_INTERFACE,
					MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED);
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &arg_uuid);

	dbus_connection_send(dbus_conn, signal, NULL);
	dbus_message_unref(signal);
}

void prov_emit_request_key(const char* type, uint16_t max_len)
{
	DBusConnection *dbus_conn;
	DBusMessage *signal;
	DBusMessageIter iter, dict;
	const char *arg_type = type;
	uint16_t arg_max_len = max_len;

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(MESHCTLD_OBJECT_PATH_PROVISIONER,
					MESHCTLD_DBUS_MESH_INTERFACE,
					MESHCTLD_SIGNAL_REQUEST_KEY);
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	g_dbus_dict_append_entry(&dict, "type", DBUS_TYPE_STRING,
				&arg_type);
	g_dbus_dict_append_entry(&dict, "size", DBUS_TYPE_UINT16,
				&arg_max_len);
	dbus_message_iter_close_container(&iter, &dict);

	dbus_connection_send(dbus_conn, signal, NULL);
	dbus_message_unref(signal);
}

static void prov_emit_provision_done_with_error(int result, uint16_t unicast_addr, const char *error)
{
	DBusConnection *dbus_conn;
	DBusMessage *signal;
	DBusMessageIter iter, dict;
	const char *arg_error = error;
	int arg_result = result;
	uint16_t arg_unicast_addr = unicast_addr;

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(MESHCTLD_OBJECT_PATH_PROVISIONER,
					MESHCTLD_DBUS_MESH_INTERFACE,
					MESHCTLD_SIGNAL_PROVISION_DONE);
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	g_dbus_dict_append_entry(&dict, "status", DBUS_TYPE_INT32,
				&arg_result);
		g_dbus_dict_append_entry(&dict, "unicast", DBUS_TYPE_UINT16,
				&arg_unicast_addr);
	g_dbus_dict_append_entry(&dict, "error", DBUS_TYPE_STRING,
				&arg_error);
	dbus_message_iter_close_container(&iter, &dict);

	dbus_connection_send(dbus_conn, signal, NULL);
	dbus_message_unref(signal);
}

void prov_emit_provision_failed(int result, const char *error, ...)
{
	va_list args;
	char str[200];

	if (error)
		vsnprintf(str, sizeof(str), error, args);
	else
		str[0] = '\0';

	return prov_emit_provision_done_with_error(result, 0, str);
}

void prov_emit_provision_success(uint16_t unicast_addr)
{
	prov_emit_provision_done_with_error(0, unicast_addr, "");
}

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

static void prov_emit_connect_done_with_error(int result, const char *error)
{
	DBusConnection *dbus_conn;
	DBusMessage *signal;
	DBusMessageIter iter, dict;
	const char *arg_error = error;
	int arg_result = result;

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(MESHCTLD_OBJECT_PATH_PROVISIONER,
					MESHCTLD_DBUS_MESH_INTERFACE,
					MESHCTLD_SIGNAL_CONNECT_DONE);
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	g_dbus_dict_append_entry(&dict, "status", DBUS_TYPE_INT32,
				&arg_result);
	g_dbus_dict_append_entry(&dict, "error", DBUS_TYPE_STRING,
				&arg_error);
	dbus_message_iter_close_container(&iter, &dict);

	dbus_connection_send(dbus_conn, signal, NULL);
	dbus_message_unref(signal);
}

void prov_emit_connect_failed(int result, const char *error, ...)
{
	va_list args;
	char str[200];

	if (error)
		vsnprintf(str, sizeof(str), error, args);
	else
		str[0] = '\0';

	return prov_emit_connect_done_with_error(result, str);
}

void prov_emit_connect_success()
{
	prov_emit_connect_done_with_error(0, "");
}

void prov_emit_connection_lost()
{
	DBusConnection *dbus_conn;
	DBusMessage *signal;

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(MESHCTLD_OBJECT_PATH_PROVISIONER,
					MESHCTLD_DBUS_MESH_INTERFACE,
					MESHCTLD_SIGNAL_CONNECTION_LOST);
	if (signal == NULL)
		return;

	dbus_connection_send(dbus_conn, signal, NULL);
	dbus_message_unref(signal);
}