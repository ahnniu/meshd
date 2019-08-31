#ifndef _MESHD_PROV_H
#define _MESHD_PROV_H

extern int prov_register();
extern void prov_emit_new_device_discovered(char *uuid);
extern void prov_emit_request_key(const char* type, uint16_t max_len);
extern void prov_emit_provision_failed(int result, const char *error, ...);
extern void prov_emit_provision_success(uint16_t unicast_addr);

#endif
