# DBus Interface - Onoff

## DBus Object

| Dest       | Interface             | Object Path            |
| ---------- | --------------------- | ---------------------- |
| org.embest | org.embest.mesh.Model | org/embest/model/onoff |

## DBus Method

| Member | Arguments In | Arguments Out | Description              |
| ------ | ------------ | ------------- | ------------------------ |
| get    | a{sv}        | null          | Get an onoff model state |
| set    | a{sv}        | null          | Set an onoff model state |

### get

Get an onoff model state, A signal with the new state will be emitted then.

| Argument In | Type  | Description |
| ----------- | ----- | ----------- |
|             | a{sv} |             |

| Dict Key | Dict Variant Type | Description            |
| -------- | ----------------- | ---------------------- |
| "addr"   | q                 | Should be a group addr |

### set

Set an onoff model state

| Argument In | Type  | Description |
| ----------- | ----- | ----------- |
| new state   | a{sv} |             |

| Dict Key | Dict Variant Type | Description            |
| -------- | ----------------- | ---------------------- |
| "addr"   | q                 | Should be a group addr |
| "state"  | q                 | 0 / 1                  |

## DBus Signals

| Member | Arguments | Description              |
| ------ | --------- | ------------------------ |
| status | a{sv}     | The new state of a model |

### status

| Dict Key | Dict Variant Type | Description                                                             |
| -------- | ----------------- | ----------------------------------------------------------------------- |
| "object" | s                 | Object Path, will be 'org/embest/model/onoff'                           |
| "mod_id" | q                 | Model Id, it will be '1000', Generic OnOff Server                       |
| "src"    | q                 | The state from which element model, it is the unicast addr of the model |
| "dst"    | q                 | The msg send to                                                         |
| "state"  | q                 | onoff state, 0 / 1                                                      |
