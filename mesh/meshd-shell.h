#ifndef _MESHD_SHELL_H
#define _MESHD_SHELL_H

extern int meshd_shell_register();
extern void shell_emit_menu_exec_invalid_cmd(int argc, char *argv[]);

#define shell_emit_cmd_failed(object_path, method_name, result, fmt, ...) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_SHELL, MESHCTLD_SIGNAL_CMD_EXEC_DONE, \
						"object=%o, method=%s, status=%i; " \
						fmt, object_path, method_name, result, \
						## __VA_ARGS__)

#define shell_emit_cmd_success(object_path, method_name) \
				shell_emit_cmd_failed(object_path, method_name, 0, "")

#endif
