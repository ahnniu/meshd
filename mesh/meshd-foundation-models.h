#ifndef _MESHD_FOUNDATION_MODELS_H
#define _MESHD_FOUNDATION_MODELS_H

#define heartbeat_emit_rxed(net_idx, src, dst, feature_relay, feature_proxy, feature_friend, feature_lpn) \
				meshd_emit_signal_fmt(MESHCTLD_OBJECT_PATH_CONFIG, MESHCTLD_SIGNAL_CONFIG_HEARTBEAT, \
					"net_idx=%q, src=%q, dst=%q, " \
					"feature_relay=%q, feature_proxy=%q, feature_friend=%q, feature_lpn=%q" \
					";", \
					net_idx, src, dst, \
					feature_relay, feature_proxy, feature_friend, feature_lpn)

#endif