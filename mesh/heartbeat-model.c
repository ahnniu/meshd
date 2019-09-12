
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/uio.h>
#include <wordexp.h>

#include <glib.h>

#include "src/shared/shell.h"
#include "src/shared/util.h"
#include "mesh/mesh-net.h"
#include "mesh/keys.h"
#include "mesh/net.h"
#include "mesh/node.h"
#include "mesh/prov-db.h"
#include "mesh/util.h"
#include "mesh/config-model.h"

#include "mesh/dbus-server.h"
#include "mesh/meshd-shell.h"
#include "mesh/meshd-config.h"
#include "foundation-models.h"
#include "meshd-foundation-models.h"

void heartbeat_rxed(uint16_t net_idx, uint8_t ttl,
								uint16_t src, uint16_t dst, uint16_t features)
{
	uint16_t relay, proxy, friend, lpn;

	relay = (features & MESH_FEATURE_RELAY) ? 1 : 0;
	proxy = (features & MESH_FEATURE_PROXY) ? 1 : 0;
	friend = (features & MESH_FEATURE_FRIEND) ? 1 : 0;
	lpn = (features & MESH_FEATURE_LPN) ? 1 : 0;

	heartbeat_emit_rxed(net_idx, src, dst,
		relay, proxy, friend, lpn);
}

bool heartbeat_init()
{
	net_mesh_heartbeat_set_rxed_callback(heartbeat_rxed);
	return true;
}