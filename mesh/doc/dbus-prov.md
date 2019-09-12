# DBus Interface - Provisioner

## DBus Object

| Dest       | Interface                | Object Path            |
| ---------- | ------------------------ | ---------------------- |
| org.embest | org.embest.MeshInterface | org/embest/provisioner |

## DBus Method

| Member                 | Arguments In | Arguments Out | Description                                                 |
| ---------------------- | ------------ | ------------- | ----------------------------------------------------------- |
| discover_unprovisioned | s            | null          | Start / stop scanning unprovisioned devices                 |
| provision              | s            | null          | Start to provision a new unprovisioned device with its uuid |
| enter_key              | s            | null          | Enter the key when provsioning                              |
| connect                | a{sv}        | null          | Connect to a mesh net or mesh node                          |
| disconnect             | null         | null          | Disconnect from the connected net / device                  |
| mesh_info              | null         | null          | Get the mesh database to sync                               |

### discover_unprovisioned

To start / stop discovering unprovisioned devices, when a new devices is discovered, a signal(unprovisioned_device_discovered(a{sv})) will be emitted with new device's uuid and name.

| Argument In | Type | Description  |
| ----------- | ---- | ------------ |
| onoff       | s    | "on" / "off" |


#### Example

To start discovering unprovisioned devices

```bash
$ sudo dbus-send --system --type=method_call --print-reply \
--dest=org.embest /org/embest/provisioner \
org.embest.MeshInterface.discover_unprovisioned \
string:"on"
```

### provision

```bash
$ sudo dbus-send --system --type=method_call --print-reply \
--dest=org.embest /org/embest/provisioner \
org.embest.MeshInterface.provision \
string:"dd000000000000000000000000000000"
```

### connect

You can connect to a net or a node:

- You must connect to a node first when you want to config it
- You must connect to a net when you want to listen / publish data to mesh net

Then you can subscribe the 'connecting' signal to check the connected status

| Dict Key    | Dict Variant Type | Description                                          |
| ----------- | ----------------- | ---------------------------------------------------- |
| "net_idx"   | q                 | the net index that you want to connect to            |
| "node_addr" | q                 | the node unicast address that you want to connect to |

#### connect to a net

| Dict Key    | Dict Variant Type | Description                                   |
| ----------- | ----------------- | --------------------------------------------- |
| "net_idx"   | q                 | net index, 0 for example                      |
| "node_addr" | q                 | not pass this argument(default 0) or set to 0 |

#### connect to a node

If you want to connect to a node, you should make the node entering configuration mode(This will be 'Enable advertising with Node Identity' in mesh) first. Usually, you should press a button or reset / re-power the device.

| Dict Key    | Dict Variant Type | Description                                  |
| ----------- | ----------------- | -------------------------------------------- |
| "net_idx"   | q                 | net index the node belongs to, 0 for example |
| "node_addr" | q                 | node unicast address                         |

### disconnect

Disconnect from a net or node.

## DBus Signals

| Member                          | Arguments | Description                                  |
| ------------------------------- | --------- | -------------------------------------------- |
| unprovisioned_device_discovered | a{sv}     | A new unprovisioned device is discovered     |
| db                              | a{sv}     | Provisioner database info                    |
| provisioning                    | a{sv}     | Events when try to provision a new device    |
| connecting                      | a{sv}     | Events when try to connect to a mesh network |

### unprovisioned_device_discovered

| Dict Key | Dict Variant Type | Description                               |
| -------- | ----------------- | ----------------------------------------- |
| "uuid"   | s                 | the uuid of this new unprovisioned device |
| "name"   | s                 | the name of this new unprovisioned devcie |
| "oob"    | q                 | Not used now                              |

### db

| Dict Key | Dict Variant Type | Description                                              |
| -------- | ----------------- | -------------------------------------------------------- |
| "name"   | s                 | should be "mesh" or "local" indicates which info this is |
| "info"   | s                 | json string                                              |

### provisioning

| Dict Key | Dict Variant Type | Description                       |
| -------- | ----------------- | --------------------------------- |
| "action" | s                 | should be "request_key" or "done" |

#### when "action"="request_key"

| Dict Key | Dict Variant Type | Description                             |
| -------- | ----------------- | --------------------------------------- |
| "type"   | s                 | should be "DECIMAL" or "HEX" or "ASCII" |
| "size"   | q                 | max length                              |

#### when "action"="done"

| Dict Key  | Dict Variant Type | Description                                          |
| --------- | ----------------- | ---------------------------------------------------- |
| "status"  | i                 | indicates the provisioning result, 0 is success      |
| "unicast" | q                 | The unicast address that the provisioner distributes |
| "error"   | s                 | error messages                                       |

### connecting

| Dict Key    | Dict Variant Type | Description                                       |
| ----------- | ----------------- | ------------------------------------------------- |
| "connected" | i                 | 1 indicates connected, 0 indicate connection lost |

#### when "connected"=1

| Dict Key  | Dict Variant Type | Description                    |
| --------- | ----------------- | ------------------------------ |
| "type"    | s                 | should be "net" or "identity". |
| "net_idx" | q                 | when type=net                  |
| "unicast" | q                 | when type= identity            |

## How to

### discover

- Call the discover_unprovisioned DBus method
- Listening to the unprovisioned_device_discovered signal

### provision

- Call the provision DBus method
- Listening the provisioning signal

### connect

- Call the connect DBus method
- Listening to the connecting signal

> Note: You should conect to the net first.