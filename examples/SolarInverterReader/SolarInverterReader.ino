/*
  SolarInverterReader.ino

  Example layout for reading common solar-inverter style input registers.
  Replace the register addresses and scale factors with the device manual.

  Target slave vendor: MarsRock
  Target slave model: G2 SUN Series grid-tie inverter
  Register map note: addresses below are placeholders for the telemetry shape.
  Replace them with the exact map for your inverter firmware before use.
*/

#include <ModbusMaster.h>

const uint8_t SLAVE_ID = 1;
const uint32_t MODBUS_BAUD = 9600;

const uint16_t REG_AC_POWER = 0x0000;
const uint16_t REG_AC_VOLTAGE = 0x0001;
const uint16_t REG_DC_VOLTAGE = 0x0002;
const uint16_t REG_TEMPERATURE = 0x0003;

const char DEVICE_VENDOR[] = "MarsRock";
const char DEVICE_MODEL[] = "G2 SUN Series grid-tie inverter";

ModbusMaster node;

float scaled(uint16_t value, float scale)
{
  return value * scale;
}

void setup()
{
  Serial.begin(115200);
  Serial1.begin(MODBUS_BAUD);

  node.begin(SLAVE_ID, Serial1);

  Serial.print("Target vendor: ");
  Serial.println(DEVICE_VENDOR);
  Serial.print("Target model: ");
  Serial.println(DEVICE_MODEL);
}

void loop()
{
  uint8_t result = node.readInputRegisters(REG_AC_POWER, 4);

  if (result == node.ku8MBSuccess)
  {
    Serial.print("AC power W: ");
    Serial.println(scaled(node.getResponseBuffer(REG_AC_POWER - REG_AC_POWER), 1.0f));

    Serial.print("AC voltage V: ");
    Serial.println(scaled(node.getResponseBuffer(REG_AC_VOLTAGE - REG_AC_POWER), 0.1f));

    Serial.print("DC voltage V: ");
    Serial.println(scaled(node.getResponseBuffer(REG_DC_VOLTAGE - REG_AC_POWER), 0.1f));

    Serial.print("Temperature C: ");
    Serial.println(scaled(node.getResponseBuffer(REG_TEMPERATURE - REG_AC_POWER), 0.1f));
  }
  else
  {
    Serial.print("Inverter read failed: 0x");
    Serial.println(result, HEX);
  }

  delay(2000);
}
