# DBus - Config

Config a node such as publication set, subscribe add, appkey add and etc.

## DBus Object

| Dest       | Interface                | Object Path       |
| ---------- | ------------------------ | ----------------- |
| org.embest | org.embest.MeshInterface | org/embest/config |

## DBus Method

| Member          | Arguments In | Arguments Out | Description                          |
| --------------- | ------------ | ------------- | ------------------------------------ |
| target          | q            | null          | Set the target node device to config |
| appkey_add      | q            | null          | Add a app key to the node            |
| bind            | a{sv}        | null          | Bind a app key to a model            |
| pub_set         | a{sv}        | null          | Publication set                      |
| sub_add         | a{sv}        | null          | Add a subscribe                      |
| composition_get | null         | null          | Get the composition data of a node   |

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

## DBus Signal

| Member | Arguments | Description             |
| ------ | --------- | ----------------------- |
| status | a{sv}     | Config command response |

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
