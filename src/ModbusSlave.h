/**
@file
Arduino Modbus RTU slave support.
*/
/*

  ModbusSlave.h - Arduino library for serving Modbus RTU requests over
  RS232/485.

  Library:: ModbusMaster

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.

*/

#ifndef ModbusSlave_h
#define ModbusSlave_h

#include "Arduino.h"
#include "util/crc16.h"

class ModbusSlave
{
  public:
    using Callback = void (*)();

    static constexpr uint8_t ku8MBSuccess = 0x00;
    static constexpr uint8_t ku8MBIllegalFunction = 0x01;
    static constexpr uint8_t ku8MBIllegalDataAddress = 0x02;
    static constexpr uint8_t ku8MBIllegalDataValue = 0x03;
    static constexpr uint8_t ku8MBSlaveDeviceFailure = 0x04;

    static constexpr uint8_t ku8MBNoRequest = 0xF0;
    static constexpr uint8_t ku8MBInvalidCRC = 0xF1;
    static constexpr uint8_t ku8MBFrameTooLarge = 0xF2;

    ModbusSlave();

    void begin(uint8_t slave, Stream &serial);
    void preTransmission(Callback);
    void postTransmission(Callback);
    void setFrameTimeout(uint16_t milliseconds);

    void configureCoils(bool *values, uint16_t count, uint16_t startAddress = 0);
    void configureDiscreteInputs(bool *values, uint16_t count, uint16_t startAddress = 0);
    void configureHoldingRegisters(uint16_t *values, uint16_t count, uint16_t startAddress = 0);
    void configureInputRegisters(uint16_t *values, uint16_t count, uint16_t startAddress = 0);

    bool getCoil(uint16_t address) const;
    uint8_t setCoil(uint16_t address, bool value);
    uint16_t getHoldingRegister(uint16_t address) const;
    uint8_t setHoldingRegister(uint16_t address, uint16_t value);

    uint8_t poll();

  private:
    struct BitMap
    {
      bool *values = nullptr;
      uint16_t count = 0;
      uint16_t startAddress = 0;
    };

    struct RegisterMap
    {
      uint16_t *values = nullptr;
      uint16_t count = 0;
      uint16_t startAddress = 0;
    };

    static constexpr uint8_t ku8MBReadCoils = 0x01;
    static constexpr uint8_t ku8MBReadDiscreteInputs = 0x02;
    static constexpr uint8_t ku8MBReadHoldingRegisters = 0x03;
    static constexpr uint8_t ku8MBReadInputRegisters = 0x04;
    static constexpr uint8_t ku8MBWriteSingleCoil = 0x05;
    static constexpr uint8_t ku8MBWriteSingleRegister = 0x06;
    static constexpr uint8_t ku8MBWriteMultipleCoils = 0x0F;
    static constexpr uint8_t ku8MBWriteMultipleRegisters = 0x10;

    static constexpr uint8_t ku8MaxADUSize = 255;
    static constexpr uint16_t ku16DefaultFrameTimeout = 5;

    Stream *_serial = nullptr;
    uint8_t _u8MBSlave = 0;
    uint16_t _u16FrameTimeout = ku16DefaultFrameTimeout;
    Callback _preTransmission = nullptr;
    Callback _postTransmission = nullptr;

    BitMap _coils;
    BitMap _discreteInputs;
    RegisterMap _holdingRegisters;
    RegisterMap _inputRegisters;

    bool isAddressInMap(const BitMap &map, uint16_t address, uint16_t quantity) const;
    bool isAddressInMap(const RegisterMap &map, uint16_t address, uint16_t quantity) const;
    uint16_t mapIndex(const BitMap &map, uint16_t address) const;
    uint16_t mapIndex(const RegisterMap &map, uint16_t address) const;

    uint8_t readRequest(uint8_t *adu, uint8_t &aduSize);
    bool validCrc(const uint8_t *adu, uint8_t aduSize) const;
    void appendCrc(uint8_t *adu, uint8_t &aduSize) const;
    void sendResponse(uint8_t *adu, uint8_t aduSize);
    void sendException(uint8_t functionCode, uint8_t exceptionCode);

    uint8_t handleRequest(const uint8_t *request, uint8_t requestSize);
    uint8_t handleReadBits(const BitMap &map, uint8_t functionCode, uint16_t address, uint16_t quantity);
    uint8_t handleReadRegisters(const RegisterMap &map, uint8_t functionCode, uint16_t address, uint16_t quantity);
    uint8_t handleWriteSingleCoil(uint16_t address, uint16_t value, bool broadcast);
    uint8_t handleWriteSingleRegister(uint16_t address, uint16_t value, bool broadcast);
    uint8_t handleWriteMultipleCoils(const uint8_t *request, uint8_t requestSize, uint16_t address, uint16_t quantity, bool broadcast);
    uint8_t handleWriteMultipleRegisters(const uint8_t *request, uint8_t requestSize, uint16_t address, uint16_t quantity, bool broadcast);
};

#endif
