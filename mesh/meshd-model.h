#ifndef _MESHD_MODEL_H
#define _MESHD_MODEL_H

#define model_emit_status(object, mod_id, src, dst, dict, ...) \
				meshd_interface_emit_signal_fmt(MESHCTLD_DBUS_MESH_MODEL_INTERFACE, \
					MESHCTLD_OBJECT_PATH_MODEL, MESHCTLD_SIGNAL_MESH_MODEL_STATUS, \
					"object=%s, mod_id=%q, src=%q, dst=%q, " \
					dict \
					";", \
					object, mod_id, src, dst, \
					## __VA_ARGS__)


#endif