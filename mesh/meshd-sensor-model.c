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
#include "mesh/meshd-model.h"
#include "mesh/meshd-sensor-model.h"

static DBusMessage *exec_sensor_x_get(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	DBusMessage *reply;
	DBusMessageIter array, dict, entry, variant;
	int error;
	int i, array_len, int_value;
	int variant_type;
	const char* key;
	uint16_t addr = 0;
	uint16_t prop_id = 0;

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
			if(!strcmp(key, "addr")) {
				addr = int_value & 0xFFFF;
			} else if(!strcmp(key, "prop_id")) {
				prop_id = int_value & 0xFFFF;
			}
		}
		dbus_message_iter_next(&dict);
	}

	bt_shell_printf("[dbus][%s][%s] %s.%s"
		"(addr = %4.4x, prop_id = %4.4x)\n",
		dbus_message_get_destination(msg),
		dbus_message_get_path(msg),
		dbus_message_get_interface(msg),
		dbus_message_get_member(msg),
		addr, prop_id);

	if (prop_id) {
		error = bt_shell_manual_input_fmt("sensor.%s %4.4x %4.4x",
							dbus_message_get_member(msg), addr, prop_id);
	} else {
		error = bt_shell_manual_input_fmt("sensor.%s %4.4x",
							dbus_message_get_member(msg), addr);
	}

	reply = dbus_message_new_method_return(msg);

	return reply;
}


static DBusMessage *exec_sensor_desc_get(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	return exec_sensor_x_get(conn, msg, user_data);
}

static DBusMessage *exec_sensor_get(DBusConnection *conn,
					DBusMessage *msg, void *user_data)
{
	return exec_sensor_x_get(conn, msg, user_data);
}

static const GDBusMethodTable sensor_methods[] = {
	{
		GDBUS_METHOD("sensor_desc_get",
		GDBUS_ARGS({ "model_info", "a{sv}" }),
		NULL,
		exec_sensor_desc_get)
	},
	{
		GDBUS_METHOD("sensor_get",
		GDBUS_ARGS({ "model_info", "a{sv}" }),
		NULL,
		exec_sensor_get)
	},
	{ }
};

int meshd_sensor_register()
{
	gboolean status;
	DBusConnection *dbus_conn;

	dbus_conn = meshd_get_dbus_connection();

	status = g_dbus_register_interface(dbus_conn,
		MESHCTLD_OBJECT_PATH_MODEL_SENSOR, MESHCTLD_DBUS_MESH_MODEL_INTERFACE,
		sensor_methods, NULL,
		NULL, NULL, NULL);

	if(!status) return -EINVAL;

	return 0;
}
