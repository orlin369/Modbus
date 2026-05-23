# ModbusMaster

Arduino library for communicating with Modbus RTU slaves over RS232 or RS485.
It can act as both a Modbus RTU master and a Modbus RTU slave.

## Features

- Read coils and discrete inputs.
- Read input and holding registers.
- Write single or multiple coils.
- Write single or multiple holding registers.
- Mask-write registers.
- Read/write multiple registers in one transaction.
- Optional callbacks for RS485 driver-enable and receiver-enable control.
- Serve local coils, discrete inputs, holding registers, and input registers as
  a Modbus RTU slave.

## Installation

Copy this folder into your Arduino `libraries` folder, or install it as a zip
library from the Arduino IDE with **Sketch > Include Library > Add .ZIP
Library...**.

The library uses the current Arduino library layout:

```text
ModbusMaster/
  library.properties
  keywords.txt
  src/
  examples/
```

## Basic Use

Master:

```cpp
#include <ModbusMaster.h>

ModbusMaster node;

void setup()
{
  Serial.begin(19200);
  node.begin(2, Serial);
}

void loop()
{
  uint8_t result = node.readHoldingRegisters(2, 6);

  if (result == node.ku8MBSuccess)
  {
    uint16_t value = node.getResponseBuffer(0);
  }

  delay(1000);
}
```

Slave:

```cpp
#include <ModbusSlave.h>

ModbusSlave slave;
bool coils[8];
uint16_t holdingRegisters[16];

void setup()
{
  Serial1.begin(9600);
  slave.begin(10, Serial1);
  slave.configureCoils(coils, 8);
  slave.configureHoldingRegisters(holdingRegisters, 16);
}

void loop()
{
  holdingRegisters[0]++;
  slave.poll();
}
```

See the sketches in `examples/` for complete usage.

## Examples

| Example | Slave vendor | Slave model | Purpose |
| --- | --- | --- | --- |
| `Basic` | Any Modbus RTU vendor | Test slave or simulator | Minimal read/write flow. |
| `RS485_HalfDuplex` | EPSolar / EPEver | LS2024B solar charge controller | MAX485-style direction control callbacks. |
| `ModbusScanner` | Any Modbus RTU vendor | Unknown slave device | Scan slave IDs for devices that answer a probe read. |
| `ModbusSlaveSimulator` | This Arduino sketch | Modbus RTU simulator | Serve local Arduino arrays as a Modbus RTU slave. |
| `ReadHoldingRegisters` | Wiren Board | WB-MIR v3 | Raw holding-register inspection template. |
| `WriteRelayCoils` | ComWinTop | CWT-MB308V | Relay/digital-output coil write template. |
| `SolarInverterReader` | MarsRock | G2 SUN Series grid-tie inverter | Inverter telemetry template. |
| `BatteryBmsReader` | JKESS / Jikong | JK200-MBS / JK-PB series BMS | Battery/BMS telemetry template. |
| `PhoenixContact_nanoLC` | Phoenix Contact | nanoLine / nanoLC controller | Legacy PLC-oriented register example. |

Device-specific examples include vendor and model information in the sketch
header and print it during startup. Template examples still require checking the
exact register map for the firmware and hardware revision you connect.

## License

Licensed under the Apache License, Version 2.0. See `LICENSE`.
