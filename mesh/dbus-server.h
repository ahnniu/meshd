#ifndef _DBUS_SERVER_H
#define _DBUS_SERVER_H

#define MESHCTLD_DBUS_MESH_INTERFACE	"org.embest.mesh.Provisioner"
#define MESHCTLD_DBUS_MESH_MODEL_INTERFACE	"org.embest.mesh.Model"
#define MESHCTLD_DBUS_ERROR_INTERFACE	"org.embest.mesh.Error"

#define MESHCTLD_OBJECT_PATH_PROVISIONER "/org/embest/provisioner"
#define MESHCTLD_OBJECT_PATH_SHELL "/org/embest/shell"
#define MESHCTLD_OBJECT_PATH_CONFIG "/org/embest/config"
#define MESHCTLD_OBJECT_PATH_MODEL "/org/embest/model"
#define MESHCTLD_OBJECT_PATH_MODEL_ONOFF "/org/embest/model/onoff"
#define MESHCTLD_OBJECT_PATH_MODEL_SENSOR "/org/embest/model/sensor"

#define MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED  "unprovisioned_device_discovered"
#define MESHCTLD_SIGNAL_PROVISIONING "provisioning"
#define MESHCTLD_SIGNAL_CONNECTING "connecting"
#define MESHCTLD_SIGNAL_PROV_DB_INFO "db"


#define MESHCTLD_SIGNAL_CONFIG_STATUS "status"
#define MESHCTLD_SIGNAL_CMD_EXECUTING "executing"
#define MESHCTLD_SIGNAL_CONFIG_HEARTBEAT "heartbeat"

// org.embest.mesh.Model.status
#define MESHCTLD_SIGNAL_MESH_MODEL_STATUS "status"


#define meshd_emit_signal_fmt(object_path, signal_name, fmt, ...) \
				meshd_interface_emit_signal_fmt(MESHCTLD_DBUS_MESH_INTERFACE, \
					object_path, signal_name, fmt, ## __VA_ARGS__)

extern int meshd_connect_dbus(void);
extern void meshd_disconnected_dbus(DBusConnection *conn, void *data);
extern DBusConnection *meshd_get_dbus_connection(void);

extern DBusMessage *meshd_error_invalid_args(DBusMessage *msg);
extern DBusMessage *meshd_error_invalid_args_str(DBusMessage *msg, const char *str);


/**
 * @brief Emit a dbus signal with format string to generate 'a{sv}' type dbus parameters.
 * The signal should register first.
 *
 * @param interface the interface name to emit the signal
 * @param object_path the object to emit the signal
 * @param signal_name the signal name
 * @param fmt the format string, for example: "key1=%t, key2=%t. Error messages with %d"
 * The first part before ';'/'.' is the key=value pairs. you can use below dbus base type:
 * - %n 16-bit signed integer
 * - %q 16-bit unsigned integer
 * - %i 32-bit signed integer
 * - %u 32-bit unsigned integer
 * - %o D-Bus Object Path string
 * - %s UTF-8 string
 * The second part after ';' / '.' is the error message. you can use it just like printf
 * @param ... the variadic aguments
 *
 * For example:
 * meshd_emit_signal_fmt("/org/embest/Object", "signal_name",
 * 		"status=%i, unicast=%q, name=%s. This is an error message with error code: %d",
 * 		1, 0x0107, -1);
 */
extern void meshd_interface_emit_signal_fmt(const char *interface, const char *object_path,
							const char* signal_name, const char *fmt, ...);

#endif
