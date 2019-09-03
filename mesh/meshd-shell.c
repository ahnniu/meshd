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
#include "src/shared/queue.h"
#include "src/shared/shell.h"
#include "mesh/dbus-server.h"
#include "mesh/meshd-shell.h"

static DBusMessage *exec_menu_list(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const struct queue *submenus;
	const struct queue_entry *submenu;

	bt_shell_printf("[dbus][%s][%s] %s.%s()\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg));

	submenus = bt_shell_get_submenus();

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
		DBUS_TYPE_STRING_AS_STRING, &array);

	for (submenu = queue_get_entries((struct queue *)submenus); submenu;
					submenu = submenu->next) {
		struct bt_shell_menu *menu = submenu->data;

		dbus_message_iter_append_basic(&array, DBUS_TYPE_STRING, &menu->name);
	}

	dbus_message_iter_close_container(&iter, &array);

	return reply;
}

static DBusMessage *exec_menu_submenu(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *name;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &name,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		name);

	bt_shell_manual_input_fmt("menu %s", name);

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_menu_back(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;

	bt_shell_printf("[dbus][%s][%s] %s.%s()\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg));

	bt_shell_manual_input_fmt("back");

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static DBusMessage *exec_cmd(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter iter, array;
	const char *cmd;
	int error;

	if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &cmd,
			DBUS_TYPE_INVALID) == FALSE)
		return meshd_error_invalid_args(msg);

	bt_shell_printf("[dbus][%s][%s] %s.%s(%s)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		cmd);

	error = bt_shell_manual_input_fmt("%s", cmd);

	reply = dbus_message_new_method_return(msg);

	return reply;
}

static const GDBusMethodTable shell_methods[] = {
	{
		GDBUS_METHOD("MenuList",
		NULL,
		GDBUS_ARGS({ "name", "as" }),
		exec_menu_list)
	},
	{
		GDBUS_METHOD("Menu",
		GDBUS_ARGS({ "name", "s" }),
		NULL,
		exec_menu_submenu)
	},
	{
		GDBUS_METHOD("MenuBack",
		NULL,
		NULL,
		exec_menu_back)
	},
	{
		GDBUS_METHOD("Exec",
		GDBUS_ARGS({ "cmd", "s" }),
		NULL,
		exec_cmd)
	},
	{ }
};

static const GDBusSignalTable shell_signals[] = {
	{ GDBUS_SIGNAL(MESHCTLD_SIGNAL_CMD_EXEC_DONE, GDBUS_ARGS({ "result", "a{sv}" })) },
	{ }
};

int meshd_shell_register()
{
	gboolean status;
	DBusConnection *dbus_conn;

	dbus_conn = meshd_get_dbus_connection();

	status = g_dbus_register_interface(dbus_conn,
		MESHCTLD_OBJECT_PATH_SHELL, MESHCTLD_DBUS_MESH_INTERFACE,
		shell_methods, shell_signals,
		NULL, NULL, NULL);

	if(!status) return -EINVAL;

	return 0;
}
