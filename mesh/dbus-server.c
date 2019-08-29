#include <errno.h>
#include <glib.h>
#include <dbus/dbus.h>

#include "gdbus/gdbus.h"
#include "src/log.h"
#include "src/shared/mainloop.h"
#include "dbus-server.h"

#define MESHCTLD_DBUS_NAME "org.embest"


static DBusConnection *connection = NULL;

static void meshd_set_dbus_connection(DBusConnection *conn)
{
	connection = conn;
}

DBusConnection *meshd_get_dbus_connection(void)
{
	return connection;
}

void meshd_disconnected_dbus(DBusConnection *conn, void *data)
{
	mainloop_quit();
}

int meshd_connect_dbus(void)
{
  DBusConnection *conn;
	DBusError err;

	dbus_error_init(&err);

	conn = g_dbus_setup_bus(DBUS_BUS_SYSTEM, MESHCTLD_DBUS_NAME, &err);
	if (!conn) {
		if (dbus_error_is_set(&err)) {
			g_printerr("D-Bus setup failed: %s\n", err.message);
			dbus_error_free(&err);
			return -EIO;
		}
		return -EALREADY;
	}

	meshd_set_dbus_connection(conn);

	g_dbus_set_disconnect_function(conn, meshd_disconnected_dbus, NULL, NULL);
	g_dbus_attach_object_manager(conn);

	return 0;
}

DBusMessage *meshd_error_invalid_args(DBusMessage *msg)
{
	return meshd_error_invalid_args_str(msg,
					"Invalid arguments in method call");
}

DBusMessage *meshd_error_invalid_args_str(DBusMessage *msg, const char *str)
{
	return g_dbus_create_error(msg, MESHCTLD_DBUS_ERROR_INTERFACE ".InvalidArguments",
					"%s", str);
}

