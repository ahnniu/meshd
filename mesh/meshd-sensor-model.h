#ifndef _MESHD_SENSOR_MODEL_H
#define _MESHD_SENSOR_MODEL_H


#define sensor_emit_status(name, src, dst, dict, ...) \
				model_emit_status(MESHCTLD_OBJECT_PATH_MODEL_SENSOR, \
					BT_MESH_MODEL_ID_SENSOR_SETUP_SRV, src, dst, \
					"name=%s, " \
					dict, \
					name, \
					## __VA_ARGS__)

#define sensor_emit_sensor_desc_status(src, dst, prop_id, desc) \
				sensor_emit_status("sensor_desc", src, dst, \
					"prop_id=%q, desc=%s", \
					prop_id, desc)

#define sensor_emit_sensor_status(src, dst, prop_id, raw_len, raw) \
				sensor_emit_status("sensor", src, dst, \
					"prop_id=%q, raw_len=%q, raw=%s", \
					prop_id, raw_len, raw)

#define sensor_emit_cmd_failed(method, result, error, ...) \
				shell_emit_cmd_failed(MESHCTLD_OBJECT_PATH_MODEL_SENSOR, \
					method, result, error, ## __VA_ARGS__)


int meshd_sensor_register();

#endif