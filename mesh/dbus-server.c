#include <errno.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <string.h>
#include <stdio.h>

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


void meshd_interface_emit_signal_fmt(const char *interface, const char *object_path,
						const char* signal_name, const char *fmt, ...)
{
	va_list ap;
	const char *p;
	char *k, *key;
	char keys[200];
	const char *str_val[10];
	char error[200], *perror;
	int32_t int32_val[10];
	int int_val_index, str_index;
	DBusConnection *dbus_conn;
	DBusMessage *signal;
	DBusMessageIter iter, dict;

	if(!interface || !object_path || !signal_name) {
		return;
	}

	dbus_conn = meshd_get_dbus_connection();

	signal = dbus_message_new_signal(object_path,
					interface,
					signal_name);
	if (signal == NULL)
		return;

	dbus_message_iter_init_append(signal, &iter);

	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY,
				DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
				DBUS_TYPE_STRING_AS_STRING
				DBUS_TYPE_VARIANT_AS_STRING
				DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

	memset(keys, sizeof(keys), 0);
	k = keys;
	key = keys;
	int_val_index = 0;
	str_index = 0;
	va_start(ap, fmt);
	for (p = fmt; *p; p++) {
		if(*p == ';' || *p == '.') {
			break;
		} else if (*p == '=') {
			*k++ = '\0';
			continue;
		} else if(*p == ' ' || *p == ',') {
			continue;
		} else if(*p == '%') {
			// Get args and perpare dict
			switch (*++p) {
				case 'n':
					int32_val[int_val_index] = va_arg(ap, int32_t);
					int32_val[int_val_index] &= 0xFFFF;
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_INT16,
						&int32_val[int_val_index]);
					int_val_index++;
					break;
				case 'q':
					int32_val[int_val_index] = va_arg(ap, int32_t);
					int32_val[int_val_index] &= 0xFFFF;
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_UINT16,
						&int32_val[int_val_index]);
					int_val_index++;
					break;
				case 'i':
					int32_val[int_val_index] = va_arg(ap, int32_t);
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_INT32,
						&int32_val[int_val_index]);
					int_val_index++;
					break;
				case 'u':
					int32_val[int_val_index] = va_arg(ap, int32_t);
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_UINT32,
						&int32_val[int_val_index]);
					int_val_index++;
					break;
				case 'o':
					str_val[str_index] = va_arg(ap, char *);
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_OBJECT_PATH,
						&str_val[str_index]);
					str_index++;
					break;
				case 's':
					str_val[str_index] = va_arg(ap, char *);
					g_dbus_dict_append_entry(&dict, key, DBUS_TYPE_STRING,
						&str_val[str_index]);
					str_index++;
					break;
			}

			key = 0;
		} else {
			key = key ? key : k;
			*k++ = *p;
			continue;
		}
	}

	// Skip ';', '.'
	if(*p++) {
		// Trim
		for(; *p !='\0' && *p == ' '; p++);
		if(*p) {
			memset(error, sizeof(error), 0);
			perror = error;
			vsnprintf(error, sizeof(error), p, ap);
			g_dbus_dict_append_entry(&dict, "error", DBUS_TYPE_STRING,
						&perror);
		}
	}

	dbus_message_iter_close_container(&iter, &dict);

	dbus_connection_send(dbus_conn, signal, NULL);
	va_end(ap);
	dbus_message_unref(signal);
}

