/*
  ModbusScanner.ino

  Scan a Modbus RTU bus for responding slave IDs.

  Target slave vendor: Any Modbus RTU vendor
  Target slave model: Any Modbus RTU slave device
  Use case: discover unknown slave IDs before moving to a device-specific
  example.
*/

#include <ModbusMaster.h>

#define MAX485_DE      3
#define MAX485_RE_NEG  2

const uint32_t MODBUS_BAUD = 9600;
const uint16_t PROBE_REGISTER = 0x0000;

const char DEVICE_VENDOR[] = "Any Modbus RTU vendor";
const char DEVICE_MODEL[] = "Any Modbus RTU slave";

ModbusMaster node;

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, HIGH);
  digitalWrite(MAX485_DE, HIGH);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);

  Serial.begin(115200);
  Serial1.begin(MODBUS_BAUD);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.print("Target vendor: ");
  Serial.println(DEVICE_VENDOR);
  Serial.print("Target model: ");
  Serial.println(DEVICE_MODEL);
  Serial.println("Scanning Modbus RTU slave IDs...");
}

void loop()
{
  for (uint8_t slave = 1; slave < 248; slave++)
  {
    node.begin(slave, Serial1);

    uint8_t result = node.readHoldingRegisters(PROBE_REGISTER, 1);
    if (result == node.ku8MBSuccess)
    {
      Serial.print("Found slave ");
      Serial.print(slave);
      Serial.print(" value=");
      Serial.println(node.getResponseBuffer(0));
    }

    delay(50);
  }

  Serial.println("Scan complete.");
  delay(5000);
}
