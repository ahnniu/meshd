#ifndef _MESHD_ONOFF_MODEL_H
#define _MESHD_ONOFF_MODEL_H

#define onoff_emit_new_state_with_remaining(src, dst, state, remaining_time) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, "status", \
				"src=%q, dst=%q, state=%q, remaining=%i;", \
				src, dst, state, remaining_time)

#define onoff_emit_new_state(src, dst, state) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, "status", \
				"src=%q, dst=%q, state=%q;", \
				src, dst, state)

#define onoff_emit_cmd_failed(method, result, error, ...) \
				shell_emit_cmd_failed(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, \
					method, result, error, ## __VA_ARGS__)


int meshd_onoff_register();

#endif