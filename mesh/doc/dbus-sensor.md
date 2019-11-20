# DBus Interface - Sensor

## DBus Object

| Dest       | Interface             | Object Path             |
| ---------- | --------------------- | ----------------------- |
| org.embest | org.embest.mesh.Model | org/embest/model/sensor |

## DBus Method


| Member          | Arguments In | Arguments Out | Description                                       |
| --------------- | ------------ | ------------- | ------------------------------------------------- |
| sensor_desc_get | a{sv}        | null          | Get one or all Sensor Descriptors                 |
| sensor_get      | a{sv}        | null          | Get the sensor data of one / all Sensor Proerties |

### sensor_desc_get

The Sensor Descriptor state represents the attributes describing the sensor data. One Sensor Property, One Sensor Descriptor. The Sensor Server model may support a couple of Sensor Properties or just one. Users can use `sensor_desc_get` to test if it support a sensor property or get all the sensor descriptors.

Currently, we don't use the attributes.

This is just a request, the sensor descriptors will be sent from a signal.

| Argument In | Type  | Description |
| ----------- | ----- | ----------- |
|             | a{sv} |             |

| Dict Key  | Dict Variant Type | Description                                        |
| --------- | ----------------- | -------------------------------------------------- |
| "addr"    | q                 | Should be a group addr(Sensor server model)        |
| "prop_id" | q                 | Which sensor property you want to get, 0 means all |


### sensor_get

Get the sensor data of a / all sensor properties.


| Argument In | Type  | Description |
| ----------- | ----- | ----------- |
|             | a{sv} |             |

| Dict Key  | Dict Variant Type | Description                                        |
| --------- | ----------------- | -------------------------------------------------- |
| "addr"    | q                 | Should be a group addr(Sensor server model)        |
| "prop_id" | q                 | Which sensor property you want to get, 0 means all |


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
| "name"   | s                 | status name, can be "sensor_desc" or "sensor"                           |

#### when "name" == "sensor_desc"

This is a sensor descriptor status. When a model support a couple of sensor descriptors, it will emit the descriptor one by one.

| Dict Key  | Dict Variant Type | Description                                 |
| --------- | ----------------- | ------------------------------------------- |
| "prop_id" | q                 | sensor property(device property) ID. uint16 |
| "desc"    | s                 | a json string, current doesn't use          |

```json
{
	"positive_tolerance":"a5",
	"negative_tolerance": "ff",
	"sampling_function":"23",
	"measurement_period": "cd",
	"update_interval":"12"
}
```

#### when "name" == "sensor"

This is a sensor raw value of a sensor property. When a model support a couple of sensor properties, it will emit the sensor raw data one by one.

raw_len=%q, raw=%s
| Dict Key  | Dict Variant Type | Description                                 |
| --------- | ----------------- | ------------------------------------------- |
| "prop_id" | q                 | sensor property(device property) ID. uint16 |
| "raw_len" | q                 | Data lenght in byte                         |
| "raw"     | s                 | a byte string array, but value -> hex       |

a "raw" example:

```json
["12", "ab", "ff"]
```
This is raw value only, For scalar values, the represented value is related to the Device Property Value by the following
equation, where the M, d, and b coefficients are defined for each field of the characteristic:

```c
R = raw * M * 10^d * 2^b;
```
Where: R = represented value C = raw value M = multiplier, positive or negative integer (between -10 and +10) d = decimal exponent, positive or negative integer b = binary exponent, positive or negative integer

Users can get the M / d / b from the xmls by prop_id.

## Device property

Sensor Property: Average Ambient Temperature In A Period Of Day, prop_id = 0x0001

```xml
<!--
Copyright 2017 Bluetooth SIG, Inc. All rights reserved.
-->
<Property name="Average Ambient Temperature In A Period Of Day" type="org.bluetooth.property.average_ambient_temperature_in_a_period_of_day" group="org.bluetooth.property_group.ambient_temperature" property-id="0x0001" last-modified="2017-07-11" approved="Yes">
  <Description>
This property represents an average ambient temperature measured over a period of time during the day. This property can be used to update a column in a histogram. The first value is the average ambient temperature as measured during a period of time, the second value is the start of the period, and the third value is the end of the period. A period of time is represented with a unit of Decihour (1/10th of a hour, or 6 minutes); a start time of 0 corresponds to midnight (local time), and the maximum value for the end time is 240. The values reported represent the most recent measured average values for the periods in the past 24 hours. For example: if the current time is 11:00, and an average is reported for a time period with a start value of 115 and with an end value of 116 (corresponding to a time period from 11:30 to 11:36) that measurement represents the average temperature of that time period during the previous day. This property does not distinguish between indoor and outdoor ambient temperatures. Typically the use context is derived from the device location properties.
  </Description>
  <ReferencedCharacteristic name="Temperature 8 In A Period Of Day" type="org.bluetooth.characteristic.temperature_8_in_a_period_of_day"/>
</Property>
```

Mesh Characteristic: Temperature 8 In A Period Of Day, with UUID: 2B0E

```xml
<!--
Copyright 2017 Bluetooth SIG, Inc. All rights reserved.
-->
<Characteristic xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://schemas.bluetooth.org/Documents/characteristic.xsd" name="Temperature 8 In A Period Of Day" type="org.bluetooth.characteristic.temperature_8_in_a_period_of_day" uuid="2B0E" last-modified="2017-07-11" approved="Yes">
  <InformativeText>
    <Abstract>
This characteristic aggregates the Temperature 8 characteristic, and two instances of the Time Decihour 8 characteristic, to represent a temperature value in a period of day.
    </Abstract>
  </InformativeText>
  <Value>
    <Field name="Temperature">
      <Reference>org.bluetooth.characteristic.temperature_8</Reference>
    </Field>
    <Field name="Start Time">
      <Reference>org.bluetooth.characteristic.time_decihour_8</Reference>
    </Field>
    <Field name="End Time">
      <Reference>org.bluetooth.characteristic.time_decihour_8</Reference>
    </Field>
  </Value>
</Characteristic>
```

Characteristic Time Decihour 8, uuid = 2B12
We can get M / D / B from this xml.

```xml
<!--
Copyright 2017 Bluetooth SIG, Inc. All rights reserved.
-->
<Characteristic xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://schemas.bluetooth.org/Documents/characteristic.xsd" name="Time Decihour 8" type="org.bluetooth.characteristic.time_decihour_8" uuid="2B12" last-modified="2017-07-11" approved="Yes">
  <InformativeText>
    <Abstract>
The Time Decihour 8 characteristic is used to represent a period of time in tenths of an hour.
    </Abstract>
  </InformativeText>
  <Value>
    <Field name="Time Decihour 8">
      <InformativeText>Unit is hour with a resolution of 0.1.</InformativeText>
      <Format>uint8</Format>
      <Unit>org.bluetooth.unit.time.hour</Unit>
      <Minimum>0.0</Minimum>
      <Maximum>24.0</Maximum>
      <DecimalExponent>-1</DecimalExponent>
      <BinaryExponent>0</BinaryExponent>
      <Multipler>1</Multipler>
      <Description>
A value of 0xFF represents 'value is not known'. All other values are Prohibited.
      </Description>
    </Field>
  </Value>
</Characteristic>
```