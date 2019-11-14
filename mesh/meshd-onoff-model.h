#ifndef _MESHD_ONOFF_MODEL_H
#define _MESHD_ONOFF_MODEL_H

#define onoff_emit_new_state_with_remaining(src, dst, state, remaining_time) \
				model_emit_status(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, \
					BT_MESH_MODEL_ID_GEN_ONOFF_SRV, src, dst, \
					"state=%q, remaining=%i", \
					state, remaining_time)

#define onoff_emit_new_state(src, dst, state) \
				model_emit_status(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, \
					BT_MESH_MODEL_ID_GEN_ONOFF_SRV, src, dst, \
					"state=%q", \
					state)

#define onoff_emit_cmd_failed(method, result, error, ...) \
				shell_emit_cmd_failed(MESHCTLD_OBJECT_PATH_MODEL_ONOFF, \
					method, result, error, ## __VA_ARGS__)


int meshd_onoff_register();

#endif