# Changelog

This changelog is reconstructed from the local Git history.

## Unreleased

- Converted the repository to a minimal modern Arduino library layout.
- Removed legacy Ruby, Travis CI, Doxygen, generated reference, and packaging
  files from the distributed library tree.
- Converted legacy `.pde` examples to `.ino`.
- Simplified README and `.gitignore` for Arduino library use.
- Cleaned unused internal pointers and initialized internal state in
  `ModbusMaster`.
- Fixed `requestFrom()` so it no longer returns an uninitialized value.
- Fixed the RS485 half-duplex example's load-voltage register index and 32-bit
  power expression.
- Added more practical Modbus RTU example sketches.
- Added slave vendor/model metadata to the examples and README example table.
- Modernized the library source for C++11-style Arduino cores.
- Added `ModbusSlave` for serving local coils and registers as a Modbus RTU
  slave.

## 2.0.1 - 2016-09-23

- Bumped the library version to 2.0.1.

## 2.0.0 - 2016-09-23

- Reorganized the project for Arduino IDE 1.5 library specification layout.
- Added `library.properties` metadata, including the `includes` field.
- Moved library source to `src/`.
- Updated build and documentation references for the new layout.
- Removed deprecated Arduino version conditionals and old layout files.
- Added PlatformIO-based multi-board example builds.
- Refreshed README installation guidance.

## 1.0.0 - 2016-09-11

- Switched the project license to Apache License 2.0.
- Added half-duplex RS485 support callbacks.
- Added the RS485 half-duplex example.
- Added continuous integration support.
- Disabled internal debug mode by default.
- Added project support files including contribution guidance, issue templates,
  pull request template, code of conduct, and style notes.
- Updated generated documentation cross-references.

## 0.11.0 - 2015-05-22

- Updated architecture selection to match Arduino conventions.
- Adjusted Modbus response timeout behavior.

## 0.10.3 - 2015-05-22

- Cleaned Doxygen comments.
- Replaced C-style macros with inline utility functions.

## 0.10.2 - 2015-05-22

- Added processor-independent CRC16 support for SAM3X8E boards.
- Added receive-buffer flushing before transmit.
- Increased response timeout to 2000 ms.
- Updated pointer style and documentation file names.

## 0.9.1 and Earlier

- Early ModbusMaster releases with the core Modbus RTU master functionality.
