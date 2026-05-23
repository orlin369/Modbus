/*
  ModbusSlaveSimulator.ino

  Serve local Arduino data as a Modbus RTU slave.

  Target slave vendor: This Arduino sketch
  Target slave model: Modbus RTU simulator
  Use case: test a Modbus RTU master without a physical industrial device.
*/

#include <ModbusSlave.h>

#define MAX485_DE      3
#define MAX485_RE_NEG  2

const uint8_t SLAVE_ID = 10;
const uint32_t MODBUS_BAUD = 9600;

bool coils[8];
bool discreteInputs[8];
uint16_t holdingRegisters[16];
uint16_t inputRegisters[16];

ModbusSlave slave;

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

  holdingRegisters[0] = 1234;
  holdingRegisters[1] = 5678;
  inputRegisters[0] = 3300;
  inputRegisters[1] = 250;

  slave.begin(SLAVE_ID, Serial1);
  slave.preTransmission(preTransmission);
  slave.postTransmission(postTransmission);
  slave.configureCoils(coils, 8, 0);
  slave.configureDiscreteInputs(discreteInputs, 8, 0);
  slave.configureHoldingRegisters(holdingRegisters, 16, 0);
  slave.configureInputRegisters(inputRegisters, 16, 0);

  Serial.println("Modbus RTU slave simulator ready.");
}

void loop()
{
  discreteInputs[0] = digitalRead(4);
  inputRegisters[2] = analogRead(A0);

  slave.poll();
}
