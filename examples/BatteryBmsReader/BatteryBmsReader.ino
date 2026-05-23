/*
  BatteryBmsReader.ino

  Example layout for reading a Modbus RTU battery/BMS register block.
  Replace the register addresses and scale factors with the BMS manual.

  Target slave vendor: JKESS / Jikong
  Target slave model: JK200-MBS / JK-PB series BMS
  Register map note: addresses below are placeholders for the telemetry shape.
  Replace them with the exact map for your BMS firmware before use.
*/

#include <ModbusMaster.h>

const uint8_t SLAVE_ID = 1;
const uint32_t MODBUS_BAUD = 9600;

const uint16_t REG_PACK_VOLTAGE = 0x0000;
const uint16_t REG_PACK_CURRENT = 0x0001;
const uint16_t REG_STATE_OF_CHARGE = 0x0002;
const uint16_t REG_MAX_CELL_MV = 0x0003;
const uint16_t REG_MIN_CELL_MV = 0x0004;

const char DEVICE_VENDOR[] = "JKESS / Jikong";
const char DEVICE_MODEL[] = "JK200-MBS / JK-PB series BMS";

ModbusMaster node;

int16_t signedRegister(uint16_t value)
{
  return (int16_t)value;
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
  uint8_t result = node.readHoldingRegisters(REG_PACK_VOLTAGE, 5);

  if (result == node.ku8MBSuccess)
  {
    Serial.print("Pack voltage V: ");
    Serial.println(node.getResponseBuffer(REG_PACK_VOLTAGE - REG_PACK_VOLTAGE) * 0.1f);

    Serial.print("Pack current A: ");
    Serial.println(signedRegister(node.getResponseBuffer(REG_PACK_CURRENT - REG_PACK_VOLTAGE)) * 0.1f);

    Serial.print("State of charge %: ");
    Serial.println(node.getResponseBuffer(REG_STATE_OF_CHARGE - REG_PACK_VOLTAGE));

    Serial.print("Max cell V: ");
    Serial.println(node.getResponseBuffer(REG_MAX_CELL_MV - REG_PACK_VOLTAGE) * 0.001f);

    Serial.print("Min cell V: ");
    Serial.println(node.getResponseBuffer(REG_MIN_CELL_MV - REG_PACK_VOLTAGE) * 0.001f);
  }
  else
  {
    Serial.print("BMS read failed: 0x");
    Serial.println(result, HEX);
  }

  delay(2000);
}
