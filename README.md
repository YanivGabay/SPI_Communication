# Arduino Interactions and SPI Protocol

### By Reut Michaeli & Yaniv Gabay

---

## Overview

This project explores Arduino's capabilities and the Serial Peripheral Interface (SPI) protocol through two key exercises. The goal was to design, simulate, and implement communication between multiple components using SPI while leveraging modular C++ classes.

---

## Understanding Arduino

Arduino is a popular and flexible microcontroller board widely used for electronics projects. Its simplicity, open-source nature, and modular design make it ideal for a variety of use cases, including:
- **Espresso Machines**
- **Parking Sensors**
- **LED Screens**

### Key Features:
- Integrated circuit with processor, memory, input/output peripherals on a single chip.
- Modular shields (add-ons) can be plugged directly into the board.

---

## SPI Protocol

The Serial Peripheral Interface (SPI) is a communication protocol used for data exchange between a **master** device and one or more **slave** devices.

### Key Components:
- **Master**: Initiates and controls communication, generates the clock signal, and selects slaves.
- **Slave**: Responds to master commands, operates only when selected.

### Communication Lines:
1. **MOSI** (Master Out, Slave In) - Data from master to slave.
2. **MISO** (Master In, Slave Out) - Data from slave back to master.
3. **SCLK** (Serial Clock) - Clock signal controlled by the master.
4. **CS** (Chip Select) - Activates individual slaves.

**Note**: All SPI communication is based on LOW and HIGH digital signal transitions (0s and 1s).

---

## Project Exercises

### Exercise 1: Simulating SPI Communication

**Goal**: Simulate SPI communication between two Arduino devices.

- **Challenge**: No physical hardware available.
- **Solution**: Used [Tinkercad](https://www.tinkercad.com/) to design and test virtually.
- **Setup**:
  - Two Arduinos connected with bulbs to visualize master-slave communication.
  - Written in C++.

---

### Exercise 2: Modular SPI Implementation in C++

1. **Class Design**:
   - **Analog to Digital Converter (ADC)**:
     Converts analog signals (e.g., sound, temperature) to digital signals.
   - **Digital to Analog Converter (DAC)**:
     Converts digital signals (binary) to analog signals.
   - **Potentiometer**:
     A class to write instructions/data to a digital potentiometer using SPI.

2. **User Interaction**:
   - Users interact with the master to choose operations (e.g., Read, Write) and components (e.g., DAC or ADC).
   - Example: `WR_ADC` = Write to ADC.

3. **Command Structure**:
   - SPI communication requires 16 bits:
     - **Command Byte (8 bits)**: Specifies the operation.
     - **Data Byte (8 bits)**: Contains the data.

---

## Tools Used
- **Arduino**: For simulation and SPI protocol implementation.
- **Tinkercad**: To design and test virtual electronics projects.
- **C++**: For class-based implementation of modular SPI.

---

## Acknowledgments

Thank you for your interest in the Arduino and SPI Protocol project!
