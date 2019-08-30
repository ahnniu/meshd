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

static const GDBusMethodTable prov_methods[] = {
	{
		GDBUS_METHOD("DiscoverUnprovisioned",
			GDBUS_ARGS({ "onoff", "s" }),
			NULL,
			exec_discover_unprovisioned)
	},
	{ }
};

static void prov_free(gpointer user_data)
{

}

static const GDBusSignalTable prov_signals[] = {
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED,
			GDBUS_ARGS({ "uuid", "s" }))
	},
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

int prov_register()
{
	gboolean status;
	char* path = MESHCTLD_OBJECT_PATH_PROVISIONER;
	DBusConnection *dbus_conn;

	dbus_conn = meshd_get_dbus_connection();

	status = g_dbus_register_interface(dbus_conn,
		path, MESHCTLD_DBUS_MESH_INTERFACE,
		prov_methods, prov_signals,
		NULL, NULL,
		prov_free);

	if(!status) return -EINVAL;

	return 0;
}
