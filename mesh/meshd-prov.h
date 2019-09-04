#ifndef _MESHD_PROV_H
#define _MESHD_PROV_H

#define prov_emit_new_device_discovered(uuid, name, oob) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_PROVISIONER, \
					MESHCTLD_SIGNAL_UNPROVISIONED_DEVICE_DISCOVERED, \
					"uuid=%s, name=%s, oob=%q;", \
					uuid, name, oob)

extern int prov_register();
extern void prov_emit_request_key(const char* type, uint16_t max_len);
extern void prov_emit_provision_failed(int result, const char *error, ...);
extern void prov_emit_provision_success(uint16_t unicast_addr);
extern void prov_emit_connect_failed(int result, const char *error, ...);
extern void prov_emit_connect_success();
extern void prov_emit_connection_lost();

#endif
