#ifndef _MESHD_CONFIG_H
#define _MESHD_CONFIG_H

extern int meshd_config_register();

#define config_emit_cmd_failed(method, result, error, ...) \
				shell_emit_cmd_failed(MESHCTLD_OBJECT_PATH_CONFIG, \
					method, result, error, ## __VA_ARGS__)

#define config_emit_target_failed(result, error, ...) \
				config_emit_cmd_failed("Target", result, error, ## __VA_ARGS__)

#define config_emit_new_status(name, result, dict, error, ...) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_CONFIG, MESHCTLD_SIGNAL_CONFIG_STATUS, \
				"name=%s, status=%i, " \
				dict \
				";" \
				error, \
				name, result, \
				## __VA_ARGS__)

#endif
