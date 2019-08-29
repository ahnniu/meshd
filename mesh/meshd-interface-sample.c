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
#include "dbus-server.h"

static char hello_str[50];

// Here is a test use dbus-send command
// sudo dbus-send --system --type=method_call --print-reply --dest=org.embest /org/embest/mesh/example/1 org.embest.MeshInterface.SayHello string:"paul"
static DBusMessage *sample_say_hello(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *name;
	char* ret;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &name,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		name);

	snprintf(hello_str, 50, "Hello %s, this is meshctld", name);

	// Note: cannot use hello_str directly in dbus_message_iter_append_basic
	// reference: https://lists.freedesktop.org/archives/dbus/2013-December/015873.html
	// The pointer is only valid within the sending application's address space;
	// D-Bus doesn't know what your pointer points to, how to serialize it,
	// or whether it can even be serialized.
	ret = hello_str;
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
		DBUS_TYPE_STRING_AS_STRING, &array);

	dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &ret);
	dbus_message_iter_close_container(&iter, &array);

	return reply;
}


static const GDBusMethodTable sample_methods[] = {
	{
		GDBUS_METHOD("SayHello",
		GDBUS_ARGS({ "name", "s" }),
		GDBUS_ARGS({ "hello", "as" }),
		sample_say_hello)
	},
	{ }
};

static void sample_free(gpointer user_data)
{

}

int sample_register()
{
	gboolean status;
	char* path = "/org/embest/mesh/example/1";
	DBusConnection *dbus_conn;

	dbus_conn = meshd_get_dbus_connection();

	status = g_dbus_register_interface(dbus_conn,
		path, MESHCTLD_DBUS_MESH_INTERFACE,
		sample_methods, NULL,
		NULL, NULL,
		sample_free);

	if(!status) return -EINVAL;

	return 0;
}