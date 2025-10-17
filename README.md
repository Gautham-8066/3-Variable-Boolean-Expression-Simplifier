# 3-Variable Boolean Expression Simplifier

A robust tool that simplifies 3-variable Boolean expressions using Karnaugh Map (K-map) logic. This project provides two separate implementations: one for the **Arduino platform** with a serial interface, and another for an **8051 (AT89S52) microcontroller** with a keypad for input and an LCD for output.

![8051 Proteus Simulation GIF](https://i.imgur.com/your-gif-url.gif)

---

## 📚 Table of Contents

* [About The Project](#about-the-project)
* [Features](#features)
* [Project Structure](#project-structure)
* [Implementations](#implementations)
    * [1. Arduino Version](#1-arduino-version)
    * [2. 8051 (AT89S52) Version](#2-8051-at89s52-version)
* [How It Works](#how-it-works)
* [License](#license)

---

## 🎯 About The Project

This project was developed to provide a practical demonstration of digital logic simplification on embedded systems. It takes a set of minterms for a 3-variable (A, B, C) Boolean function and outputs the minimized Sum-of-Products (SOP) expression.

The core logic is based on the Karnaugh Map method, systematically identifying groups of 4 (quads), 2 (pairs), and 1 (singletons) to find the most simplified logical expression.

---

## ✨ Features

* **K-Map Logic**: Implements the simplification algorithm by finding essential prime implicants.
* **Dual Implementations**:
    * **Arduino**: Easy-to-use interface via the Serial Monitor. Displays the full truth table, original SOP, and simplified SOP.
    * **8051**: Standalone embedded system using a 4x3 keypad and a 16x2 LCD.
* **Handles All Cases**: Correctly simplifies any combination of minterms from 0 to 7, including the edge cases for functions that are always `0` or `1`.
* **Proteus Simulation**: Includes a complete Proteus simulation file for the 8051 circuit, allowing for virtual testing without physical hardware.

---



---

## 🚀 Implementations

This repository contains two versions of the project.

### 1. Arduino Version

This version is designed for quick testing and detailed output. It communicates with a computer via a USB connection and the Arduino IDE's Serial Monitor.

#### Hardware Requirements
* An Arduino board (e.g., Arduino Uno, Nano)
* USB Cable

#### Software Requirements
* [Arduino IDE](https://www.arduino.cc/en/software)

#### How to Use
1.  Open the `arduino_simplifier/arduino_simplifier.ino` file in the Arduino IDE.
2.  Select your board and port from the **Tools** menu.
3.  Upload the sketch to your Arduino.
4.  Open the **Serial Monitor** (Ctrl+Shift+M) and set the baud rate to **9600**.
5.  Enter the minterms you want to simplify, separated by spaces (e.g., `1 3 4 5 7`), and press Enter.
6.  The Arduino will print the truth table, the full SOP expression, and the final simplified expression.

### 2. 8051 (AT89S52) Version

This is a standalone hardware implementation that does not require a computer to operate once programmed. It's ideal for a dedicated lab tool.



#### Hardware Requirements
* AT89S52 Microcontroller
* 16x2 LCD Display
* 4x3 Matrix Keypad
* 11.0592 MHz Crystal Oscillator with two 33pF capacitors
* Power supply components (capacitors, resistors)

#### Software Requirements
* [Keil µVision IDE](https://www.keil.com/demo/eval/c51.htm) (for compiling the C code)
* [Proteus Design Suite](https://www.labcenter.com/) (for running the simulation)
* An 8051 programmer (if building physical hardware)

#### How to Use (Proteus Simulation)
1.  Open the `8051_simplifier/proteus_simulation/embedded.pdsprj` file in Proteus.
2.  Double-click the AT89S52 microcontroller component.
3.  In the properties window, click the folder icon next to "Program File" and navigate to the `8051_simplifier/boolean_simplifier.hex` file.
4.  Click **OK** to load the hex file.
5.  Run the simulation (▶️ button).
6.  Use the virtual keypad to enter minterms (0-7).
7.  Press the '#' key to calculate. The simplified expression will be displayed on the LCD.

---

## 🧠 How It Works

The simplification logic for both versions relies on the same core algorithm:

1.  **Minterm Masking**: User inputs (e.g., `1 3 7`) are converted into a single 8-bit integer (a bitmask). For the input `1 3 7`, the mask would be `10001010` in binary (bits 1, 3, and 7 are set).
2.  **Hierarchical Grouping**: The algorithm checks for groups in a specific order to ensure the largest possible groups are found first (as per K-map rules).
    * **Quads (Groups of 4)**: It first scans for all possible 4-minterm groups. If a quad is found, its corresponding single-literal term (e.g., `A`, `B'`, `C`) is added to the result, and its minterms are marked as "covered."
    * **Pairs (Groups of 2)**: Next, it scans for all possible 2-minterm groups among the remaining uncovered minterms. If a pair is found, its two-literal term (e.g., `A'B`, `BC'`) is added, and its minterms are covered.
    * **Singletons (Individual Minterms)**: Finally, any minterms that could not be grouped are treated as individuals and their full three-literal term (e.g., `A'B'C'`) is added to the result.
3.  **Output Formatting**: The resulting terms are concatenated with `+` signs to form the final Sum-of-Products expression.

---

## 📜 License

This project is licensed under the MIT License. See the `LICENSE` file for details.

*(Suggestion: Create a file named `LICENSE` and add the MIT License text to it.)*
