#ifndef _CLI_MODEL_H
#define _CLI_MODEL_H

#define CONFIG_DEFAULT_APP_IDX 1

bool gateway_send_cmd(uint16_t target, uint16_t app_idx, uint8_t *buf, uint16_t len);
bool send_cmd(uint16_t target, uint8_t *buf, uint16_t len);

#endif