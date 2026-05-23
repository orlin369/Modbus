/**
@file
Arduino Modbus RTU slave support.
*/

#include "ModbusSlave.h"

ModbusSlave::ModbusSlave() = default;

void ModbusSlave::begin(uint8_t slave, Stream &serial)
{
  _u8MBSlave = slave;
  _serial = &serial;
}

void ModbusSlave::preTransmission(Callback preTransmission)
{
  _preTransmission = preTransmission;
}

void ModbusSlave::postTransmission(Callback postTransmission)
{
  _postTransmission = postTransmission;
}

void ModbusSlave::setFrameTimeout(uint16_t milliseconds)
{
  _u16FrameTimeout = milliseconds;
}

void ModbusSlave::configureCoils(bool *values, uint16_t count, uint16_t startAddress)
{
  _coils.values = values;
  _coils.count = count;
  _coils.startAddress = startAddress;
}

void ModbusSlave::configureDiscreteInputs(bool *values, uint16_t count, uint16_t startAddress)
{
  _discreteInputs.values = values;
  _discreteInputs.count = count;
  _discreteInputs.startAddress = startAddress;
}

void ModbusSlave::configureHoldingRegisters(uint16_t *values, uint16_t count, uint16_t startAddress)
{
  _holdingRegisters.values = values;
  _holdingRegisters.count = count;
  _holdingRegisters.startAddress = startAddress;
}

void ModbusSlave::configureInputRegisters(uint16_t *values, uint16_t count, uint16_t startAddress)
{
  _inputRegisters.values = values;
  _inputRegisters.count = count;
  _inputRegisters.startAddress = startAddress;
}

bool ModbusSlave::getCoil(uint16_t address) const
{
  if (!isAddressInMap(_coils, address, 1))
  {
    return false;
  }

  return _coils.values[mapIndex(_coils, address)];
}

uint8_t ModbusSlave::setCoil(uint16_t address, bool value)
{
  if (!isAddressInMap(_coils, address, 1))
  {
    return ku8MBIllegalDataAddress;
  }

  _coils.values[mapIndex(_coils, address)] = value;
  return ku8MBSuccess;
}

uint16_t ModbusSlave::getHoldingRegister(uint16_t address) const
{
  if (!isAddressInMap(_holdingRegisters, address, 1))
  {
    return 0;
  }

  return _holdingRegisters.values[mapIndex(_holdingRegisters, address)];
}

uint8_t ModbusSlave::setHoldingRegister(uint16_t address, uint16_t value)
{
  if (!isAddressInMap(_holdingRegisters, address, 1))
  {
    return ku8MBIllegalDataAddress;
  }

  _holdingRegisters.values[mapIndex(_holdingRegisters, address)] = value;
  return ku8MBSuccess;
}

uint8_t ModbusSlave::poll()
{
  if (_serial == nullptr || !_serial->available())
  {
    return ku8MBNoRequest;
  }

  uint8_t request[ku8MaxADUSize];
  uint8_t requestSize = 0;
  const uint8_t status = readRequest(request, requestSize);
  if (status != ku8MBSuccess)
  {
    return status;
  }

  return handleRequest(request, requestSize);
}

bool ModbusSlave::isAddressInMap(const BitMap &map, uint16_t address, uint16_t quantity) const
{
  return map.values != nullptr &&
    quantity > 0 &&
    address >= map.startAddress &&
    quantity <= map.count &&
    (address - map.startAddress) <= (map.count - quantity);
}

bool ModbusSlave::isAddressInMap(const RegisterMap &map, uint16_t address, uint16_t quantity) const
{
  return map.values != nullptr &&
    quantity > 0 &&
    address >= map.startAddress &&
    quantity <= map.count &&
    (address - map.startAddress) <= (map.count - quantity);
}

uint16_t ModbusSlave::mapIndex(const BitMap &map, uint16_t address) const
{
  return address - map.startAddress;
}

uint16_t ModbusSlave::mapIndex(const RegisterMap &map, uint16_t address) const
{
  return address - map.startAddress;
}

uint8_t ModbusSlave::readRequest(uint8_t *adu, uint8_t &aduSize)
{
  uint32_t lastByteTime = millis();

  while ((millis() - lastByteTime) <= _u16FrameTimeout)
  {
    while (_serial->available())
    {
      if (aduSize >= ku8MaxADUSize)
      {
        while (_serial->read() != -1);
        return ku8MBFrameTooLarge;
      }

      adu[aduSize++] = static_cast<uint8_t>(_serial->read());
      lastByteTime = millis();
    }
  }

  if (aduSize < 4)
  {
    return ku8MBNoRequest;
  }

  if (!validCrc(adu, aduSize))
  {
    return ku8MBInvalidCRC;
  }

  return ku8MBSuccess;
}

bool ModbusSlave::validCrc(const uint8_t *adu, uint8_t aduSize) const
{
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < aduSize - 2; i++)
  {
    crc = crc16_update(crc, adu[i]);
  }

  return lowByte(crc) == adu[aduSize - 2] && highByte(crc) == adu[aduSize - 1];
}

void ModbusSlave::appendCrc(uint8_t *adu, uint8_t &aduSize) const
{
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < aduSize; i++)
  {
    crc = crc16_update(crc, adu[i]);
  }

  adu[aduSize++] = lowByte(crc);
  adu[aduSize++] = highByte(crc);
}

void ModbusSlave::sendResponse(uint8_t *adu, uint8_t aduSize)
{
  appendCrc(adu, aduSize);

  if (_preTransmission)
  {
    _preTransmission();
  }

  for (uint8_t i = 0; i < aduSize; i++)
  {
    _serial->write(adu[i]);
  }
  _serial->flush();

  if (_postTransmission)
  {
    _postTransmission();
  }
}

void ModbusSlave::sendException(uint8_t functionCode, uint8_t exceptionCode)
{
  uint8_t response[5];
  uint8_t responseSize = 0;
  response[responseSize++] = _u8MBSlave;
  response[responseSize++] = functionCode | 0x80;
  response[responseSize++] = exceptionCode;
  sendResponse(response, responseSize);
}

uint8_t ModbusSlave::handleRequest(const uint8_t *request, uint8_t requestSize)
{
  const uint8_t requestSlave = request[0];
  const bool broadcast = requestSlave == 0;
  if (requestSlave != _u8MBSlave && !broadcast)
  {
    return ku8MBNoRequest;
  }

  const uint8_t functionCode = request[1];
  if (requestSize < 8)
  {
    if (!broadcast)
    {
      sendException(functionCode, ku8MBIllegalDataValue);
    }
    return ku8MBIllegalDataValue;
  }

  const uint16_t address = word(request[2], request[3]);
  const uint16_t valueOrQuantity = word(request[4], request[5]);
  uint8_t status = ku8MBIllegalFunction;

  if (broadcast &&
    (functionCode == ku8MBReadCoils ||
     functionCode == ku8MBReadDiscreteInputs ||
     functionCode == ku8MBReadHoldingRegisters ||
     functionCode == ku8MBReadInputRegisters))
  {
    return ku8MBSuccess;
  }

  switch (functionCode)
  {
    case ku8MBReadCoils:
      status = handleReadBits(_coils, functionCode, address, valueOrQuantity);
      break;

    case ku8MBReadDiscreteInputs:
      status = handleReadBits(_discreteInputs, functionCode, address, valueOrQuantity);
      break;

    case ku8MBReadHoldingRegisters:
      status = handleReadRegisters(_holdingRegisters, functionCode, address, valueOrQuantity);
      break;

    case ku8MBReadInputRegisters:
      status = handleReadRegisters(_inputRegisters, functionCode, address, valueOrQuantity);
      break;

    case ku8MBWriteSingleCoil:
      status = handleWriteSingleCoil(address, valueOrQuantity, broadcast);
      break;

    case ku8MBWriteSingleRegister:
      status = handleWriteSingleRegister(address, valueOrQuantity, broadcast);
      break;

    case ku8MBWriteMultipleCoils:
      status = handleWriteMultipleCoils(request, requestSize, address, valueOrQuantity, broadcast);
      break;

    case ku8MBWriteMultipleRegisters:
      status = handleWriteMultipleRegisters(request, requestSize, address, valueOrQuantity, broadcast);
      break;
  }

  if (status != ku8MBSuccess && !broadcast)
  {
    sendException(functionCode, status);
  }

  return status;
}

uint8_t ModbusSlave::handleReadBits(const BitMap &map, uint8_t functionCode, uint16_t address, uint16_t quantity)
{
  if (quantity > 2000 || !isAddressInMap(map, address, quantity))
  {
    return ku8MBIllegalDataAddress;
  }

  uint8_t response[ku8MaxADUSize];
  uint8_t responseSize = 0;
  const uint8_t byteCount = (quantity + 7) / 8;
  response[responseSize++] = _u8MBSlave;
  response[responseSize++] = functionCode;
  response[responseSize++] = byteCount;

  for (uint8_t i = 0; i < byteCount; i++)
  {
    response[responseSize++] = 0;
  }

  for (uint16_t i = 0; i < quantity; i++)
  {
    if (map.values[mapIndex(map, address + i)])
    {
      response[3 + (i / 8)] |= 1 << (i % 8);
    }
  }

  sendResponse(response, responseSize);
  return ku8MBSuccess;
}

uint8_t ModbusSlave::handleReadRegisters(const RegisterMap &map, uint8_t functionCode, uint16_t address, uint16_t quantity)
{
  if (quantity > 125 || !isAddressInMap(map, address, quantity))
  {
    return ku8MBIllegalDataAddress;
  }

  uint8_t response[ku8MaxADUSize];
  uint8_t responseSize = 0;
  response[responseSize++] = _u8MBSlave;
  response[responseSize++] = functionCode;
  response[responseSize++] = quantity * 2;

  for (uint16_t i = 0; i < quantity; i++)
  {
    const uint16_t value = map.values[mapIndex(map, address + i)];
    response[responseSize++] = highByte(value);
    response[responseSize++] = lowByte(value);
  }

  sendResponse(response, responseSize);
  return ku8MBSuccess;
}

uint8_t ModbusSlave::handleWriteSingleCoil(uint16_t address, uint16_t value, bool broadcast)
{
  if (value != 0x0000 && value != 0xFF00)
  {
    return ku8MBIllegalDataValue;
  }

  if (!isAddressInMap(_coils, address, 1))
  {
    return ku8MBIllegalDataAddress;
  }

  _coils.values[mapIndex(_coils, address)] = value == 0xFF00;

  if (!broadcast)
  {
    uint8_t response[8] = {_u8MBSlave, ku8MBWriteSingleCoil, highByte(address), lowByte(address), highByte(value), lowByte(value)};
    uint8_t responseSize = 6;
    sendResponse(response, responseSize);
  }
  return ku8MBSuccess;
}

uint8_t ModbusSlave::handleWriteSingleRegister(uint16_t address, uint16_t value, bool broadcast)
{
  if (!isAddressInMap(_holdingRegisters, address, 1))
  {
    return ku8MBIllegalDataAddress;
  }

  _holdingRegisters.values[mapIndex(_holdingRegisters, address)] = value;

  if (!broadcast)
  {
    uint8_t response[8] = {_u8MBSlave, ku8MBWriteSingleRegister, highByte(address), lowByte(address), highByte(value), lowByte(value)};
    uint8_t responseSize = 6;
    sendResponse(response, responseSize);
  }
  return ku8MBSuccess;
}

uint8_t ModbusSlave::handleWriteMultipleCoils(const uint8_t *request, uint8_t requestSize, uint16_t address, uint16_t quantity, bool broadcast)
{
  const uint8_t byteCount = request[6];
  if (quantity > 1968 || byteCount != (quantity + 7) / 8 || requestSize < byteCount + 9)
  {
    return ku8MBIllegalDataValue;
  }

  if (!isAddressInMap(_coils, address, quantity))
  {
    return ku8MBIllegalDataAddress;
  }

  for (uint16_t i = 0; i < quantity; i++)
  {
    _coils.values[mapIndex(_coils, address + i)] = bitRead(request[7 + (i / 8)], i % 8);
  }

  if (!broadcast)
  {
    uint8_t response[8] = {_u8MBSlave, ku8MBWriteMultipleCoils, highByte(address), lowByte(address), highByte(quantity), lowByte(quantity)};
    uint8_t responseSize = 6;
    sendResponse(response, responseSize);
  }
  return ku8MBSuccess;
}

uint8_t ModbusSlave::handleWriteMultipleRegisters(const uint8_t *request, uint8_t requestSize, uint16_t address, uint16_t quantity, bool broadcast)
{
  const uint8_t byteCount = request[6];
  if (quantity > 123 || byteCount != quantity * 2 || requestSize < byteCount + 9)
  {
    return ku8MBIllegalDataValue;
  }

  if (!isAddressInMap(_holdingRegisters, address, quantity))
  {
    return ku8MBIllegalDataAddress;
  }

  for (uint16_t i = 0; i < quantity; i++)
  {
    _holdingRegisters.values[mapIndex(_holdingRegisters, address + i)] = word(request[7 + 2 * i], request[8 + 2 * i]);
  }

  if (!broadcast)
  {
    uint8_t response[8] = {_u8MBSlave, ku8MBWriteMultipleRegisters, highByte(address), lowByte(address), highByte(quantity), lowByte(quantity)};
    uint8_t responseSize = 6;
    sendResponse(response, responseSize);
  }
  return ku8MBSuccess;
}
