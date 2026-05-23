# Contributing

Contributions are welcome when they keep the library small, portable, and easy
to use from the Arduino IDE.

## License

This project is licensed under the Apache License, Version 2.0. By submitting a
pull request, patch, issue comment with code, or other intentional contribution,
you agree that your contribution is provided under the same Apache-2.0 terms
unless you clearly mark it as "Not a Contribution".

The license allows use, modification, redistribution, and sublicensing, but
contributors must preserve copyright, license, patent, trademark, and
attribution notices required by the license.

## What to Contribute

- Bug fixes in the Modbus RTU transaction code.
- Compatibility fixes for Arduino boards and cores.
- Small API improvements that do not break existing sketches.
- Practical examples for common RS232/RS485 Modbus devices.
- Documentation fixes that make wiring, register usage, or behavior clearer.

## Before Sending Changes

- Keep the modern Arduino library layout: `src/`, `examples/`,
  `library.properties`, `keywords.txt`, `README.md`, and `LICENSE`.
- Put each example in its own folder with a matching `.ino` file name.
- Avoid board-specific code in the library itself when a sketch can handle it.
- Keep examples explicit about baud rate, slave ID, serial port, and register
  addresses.
- Do not commit generated build output, IDE caches, or local environment files.

## Testing

At minimum, compile the changed examples in Arduino IDE or Arduino CLI for one
AVR board and one modern 32-bit board when available.

Useful checks:

```sh
arduino-cli compile --fqbn arduino:avr:uno examples/Basic
arduino-cli compile --fqbn arduino:avr:mega examples/RS485_HalfDuplex
```

If you cannot run a hardware test, describe what was tested and what was not.

## Example Guidelines

Good examples should:

- Define all device-specific register addresses near the top.
- Print decoded values with units.
- Check `ku8MBSuccess` before using response data.
- Show RS485 direction-control callbacks when half-duplex hardware is involved.
- Avoid hidden writes to real equipment unless the sketch name and comments make
  that behavior obvious.

## Pull Request Notes

Please include:

- The board or core used for compilation.
- The Modbus device or simulator used for runtime testing, if any.
- The serial settings: baud rate, parity, stop bits when relevant.
- A short description of user-visible behavior changes.
