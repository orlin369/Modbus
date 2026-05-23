/*
  WriteRelayCoils.ino

  Toggle the first four coils of a Modbus relay or digital output module.

  Target slave vendor: ComWinTop
  Target slave model: CWT-MB308V
  Register map note: this example assumes the first relay outputs are exposed
  as coils starting at address 0. Confirm the coil map before connecting loads.
*/

#include <ModbusMaster.h>

const uint8_t SLAVE_ID = 1;
const uint32_t MODBUS_BAUD = 9600;
const uint16_t FIRST_COIL = 0x0000;
const uint8_t COIL_COUNT = 4;

const char DEVICE_VENDOR[] = "ComWinTop";
const char DEVICE_MODEL[] = "CWT-MB308V";

ModbusMaster node;
bool state = false;

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
  for (uint8_t i = 0; i < COIL_COUNT; i++)
  {
    uint8_t result = node.writeSingleCoil(FIRST_COIL + i, state);

    Serial.print("Coil ");
    Serial.print(FIRST_COIL + i);
    Serial.print(state ? " ON: " : " OFF: ");
    Serial.println(result == node.ku8MBSuccess ? "ok" : "failed");

    delay(100);
  }

  state = !state;
  delay(2000);
}
