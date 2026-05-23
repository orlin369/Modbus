/*
  ReadHoldingRegisters.ino

  Poll a block of holding registers and print raw 16-bit values.

  Target slave vendor: Wiren Board
  Target slave model: WB-MIR v3
  Register map note: this is a raw register reader. Set START_REGISTER and
  REGISTER_COUNT from the WB-MIR v3 manual before using values in automation.
*/

#include <ModbusMaster.h>

const uint8_t SLAVE_ID = 1;
const uint32_t MODBUS_BAUD = 9600;
const uint16_t START_REGISTER = 0x0000;
const uint16_t REGISTER_COUNT = 8;

const char DEVICE_VENDOR[] = "Wiren Board";
const char DEVICE_MODEL[] = "WB-MIR v3";

ModbusMaster node;

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
  uint8_t result = node.readHoldingRegisters(START_REGISTER, REGISTER_COUNT);

  if (result == node.ku8MBSuccess)
  {
    for (uint8_t i = 0; i < REGISTER_COUNT; i++)
    {
      Serial.print("HR[");
      Serial.print(START_REGISTER + i);
      Serial.print("] = ");
      Serial.println(node.getResponseBuffer(i));
    }
  }
  else
  {
    Serial.print("Read failed: 0x");
    Serial.println(result, HEX);
  }

  delay(1000);
}
