#ifndef _DBUS_SERVER_H
#define _DBUS_SERVER_H

#define MESHCTLD_DBUS_MESH_INTERFACE	"org.embest.MeshInterface"
#define MESHCTLD_DBUS_ERROR_INTERFACE	"org.embest.Error"

#define MESHCTLD_OBJECT_PATH_PROVISIONER "/org/embest/Provisioner"

#define MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED  "UnprovisionedDeviceDiscovered"
#define MESHCTLD_SIGNAL_REQUEST_KEY "RequestKey"
#define MESHCTLD_SIGNAL_PROVISION_DONE "ProvisionDone"

extern int meshd_connect_dbus(void);
extern void meshd_disconnected_dbus(DBusConnection *conn, void *data);
extern DBusConnection *meshd_get_dbus_connection(void);

extern DBusMessage *meshd_error_invalid_args(DBusMessage *msg);
extern DBusMessage *meshd_error_invalid_args_str(DBusMessage *msg, const char *str);

#endif
