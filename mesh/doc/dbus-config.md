# DBus - Config

Config a node such as publication set, subscribe add, appkey add and etc.

## DBus Object

| Dest       | Interface                   | Object Path       |
| ---------- | --------------------------- | ----------------- |
| org.embest | org.embest.mesh.Provisioner | org/embest/config |

## DBus Method

| Member          | Arguments In | Arguments Out | Description                          |
| --------------- | ------------ | ------------- | ------------------------------------ |
| target          | q            | null          | Set the target node device to config |
| appkey_add      | q            | null          | Add a app key to the node            |
| bind            | a{sv}        | null          | Bind a app key to a model            |
| pub_set         | a{sv}        | null          | Publication set                      |
| sub_add         | a{sv}        | null          | Add a subscribe                      |
| composition_get | null         | null          | Get the composition data of a node   |
| hb_pub_set      | a{sv}        | null          | Heartbeat Publication set            |

> Note: You should enter the "config" menu first.

### target

You should set the target node device first.

### bind

| Dict Key  | Dict Variant Type | Description                               |
| --------- | ----------------- | ----------------------------------------- |
| "ele_idx" | q                 | element index                             |
| "app_idx" | q                 | app key index, usually be 1               |
| "mod_id"  | q                 | Bluetooth Mesh Model Id, for example 1000 |

### pub_set

| Dict Key   | Dict Variant Type | Description                 |
| ---------- | ----------------- | --------------------------- |
| "ele_addr" | q                 | element unicast addr        |
| "pub_addr" | q                 | pub group addr              |
| "app_idx"  | q                 | app key index, usually be 1 |
| "period"   | q                 | pub period, default 0       |

### sub_add

| Dict Key   | Dict Variant Type | Description                 |
| ---------- | ----------------- | --------------------------- |
| "ele_addr" | q                 | element unicast addr        |
| "sub_addr" | q                 | group addr to subscribe     |
| "app_idx"  | q                 | app key index, usually be 1 |

### hb_pub_set

This is used to set heartbeat. Then you can subscribe the signal "heartbeat".


| Dict Key   | Dict Variant Type | Description                                         |
| ---------- | ----------------- | --------------------------------------------------- |
| "pub_addr" | q                 | pub group addr, 0 will stop heartbeat pub           |
| "net_idx"  | q                 | net index                                           |
| "period"   | q                 | 2^(period - 1) in second. 0 will stop heartbeat pub |

## DBus Signal

| Member    | Arguments | Description             |
| --------- | --------- | ----------------------- |
| status    | a{sv}     | Config command response |
| heartbeat | a{sv}     | Heartbeat               |

### status

| Dict Key | Dict Variant Type | Description                                     |
| -------- | ----------------- | ----------------------------------------------- |
| "name"   | s                 | config command status                           |
| "status" | i                 | 0 indicates success, values < 0 indicate errors |
| "error"  | s                 | error messages                                  |

#### "name"=?

| Request(DBus method)    | Response name | Description                                    |
| ----------------------- | ------------- | ---------------------------------------------- |
| composition_get         | composition   | Node composition json string with node unicast |
| appkey_add / appkey_del | appkey        | node app key status                            |
| bind                    | bind          | bind status                                    |
| pub_set                 | pub           | publication status                             |
| sub_add                 | sub           | subscribe status                               |

### heartbeat

After setting heartbeat pub, If a node is online, it will pub heart msg periodically. If timeout, it indicates that the node is offline.

| Dict Key        | Dict Variant Type | Description                                  |
| --------------- | ----------------- | -------------------------------------------- |
| "src"           | q                 | The node unicast                             |
| "dst"           | q                 | The msg send to (pub addr)                   |
| "net_idx"       | q                 | The net index the node belongs to            |
| "feature_relay" | q                 | 1 indicates the node supports relay feature  |
| "feature_proxy" | q                 | 1 indicates the node supports proxy feature  |
| "feature_relay" | q                 | 1 indicates the node supports friend feature |
| "feature_lpn"   | q                 | 1 indicates the node supports lpn feature    |
