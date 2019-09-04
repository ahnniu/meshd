#ifndef _MESHD_PROV_H
#define _MESHD_PROV_H

#define prov_emit_new_device_discovered(uuid, name, oob) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_PROVISIONER, \
					MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED, \
					"uuid=%s, name=%s, oob=%q;", \
					uuid, name, oob)

#define prov_emit_provisioning(action, dict, error, ...) \
					meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_PROVISIONER, MESHCTLD_SIGNAL_PROVISIONING, \
						"action=%s, " \
						dict \
						";" \
						error, \
						action, \
						## __VA_ARGS__)

#define prov_emit_provisioning_request_key(type, size) \
				prov_emit_provisioning("request_key", \
					"type=%s, size=%q", \
					"", \
					type, size)

#define prov_emit_provisioning_done(result, unicast, error, ...) \
				prov_emit_provisioning("done", \
					"status=%i, unicast=%q", \
					error, \
					result, unicast, \
					## __VA_ARGS__)

#define prov_emit_provisioning_done_success(unicast) \
				prov_emit_provisioning_done(0, unicast, "")

#define prov_emit_provisioning_done_failed(result, error, ...) \
				prov_emit_provisioning_done(result, UNASSIGNED_ADDRESS, \
					error, ## __VA_ARGS__)

#define prov_emit_cmd_failed(method, result, error, ...) \
				shell_emit_cmd_failed(MESHCTLD_OBJECT_PATH_PROVISIONER, \
					method, result, error, ## __VA_ARGS__)

#define prov_emit_connecting(connected, dict, error, ...) \
					meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_PROVISIONER, MESHCTLD_SIGNAL_CONNECTING, \
						"connected=%i, " \
						dict \
						";" \
						error, \
						connected, \
						## __VA_ARGS__)

#define prov_emit_connecting_lost(error, ...) \
				prov_emit_connecting(0, "", error, ## __VA_ARGS__)

#define prov_emit_type_connected(type, dict, error, ...) \
				prov_emit_connecting(1, \
					"type=%s, " \
					dict, \
					error, \
					type, \
					## __VA_ARGS__)

#define prov_emit_net_connected(net_idx) \
				prov_emit_type_connected("net", \
					"net_idx=%q", \
					"", \
					net_idx)

#define prov_emit_identity_connected(unicast) \
				prov_emit_type_connected("net", \
					"unicast=%q", \
					"", \
					unicast)

extern int prov_register();

#endif
