# Renix/Bendix ECU Diagnostic Framework

A C++ based diagnostic and reverse-engineering suite for Renault/Bendix Electronic Control Units. While specifically tuned for the **R19 (F3N-740) Single Point Injection (SPI)**, the project is an extensible framework for any ECU utilizing similar serial diagnostic frames.

## Architecture & Extensibility

The project is designed with a modular, object-oriented architecture that separates the diagnostic logic from the data transmission. Adding support for a new ECU model is streamlined through a **base class interface** and **virtual member functions**.

### Adding New Models
To integrate a new vehicle or ECU variant:
1.  **Duplicate** an existing model folder within the `/models` directory.
2.  **Modify** the three core files (header, implementation, and index definitions) to match the new frame's structure.
3.  **Register** the model by adding its specific `struct` to the global model table.

This allows the same codebase to handle varied frame lengths, byte orders (Big/Little Endian), and scaling formulas without modifying the core logger or filter logic.

## Operational Modes

*   **`model_f3n_740`**: Fully mapped for the Bendix SPI (29-byte frame). Includes scaling for RPM, MAP, Injection, and signed adaptation offsets.
*   **`model_experimental`**: An "Exploded View" that expands all flag bytes into individual signal lines (up to 90+ lines) for bit-level discovery.
*   **`model_raw`**: A baseline mode providing the raw Hexadecimal data-frame in Line 1. Essential for capturing data from unknown models to establish a new mapping.

## Data Analysis Workflow

The tool is optimized for a **"Capture Once, Analyze Often"** workflow:
*   **Raw Preservation**: Every log entry contains the full hex string. If a mapping is updated in the code, old logs can be re-piped to reveal new insights without re-driving the car.
*   **Signed Normalization**: "Correction" values (Adaptations/Regulation) are centered at zero to clearly visualize the ECU's "effort" to maintain stoichiometry.
*   **High-Speed Filtering**: The PC tool supports block-skipping and range-selection to keep Gnuplot performance high during long (15+ min) drive cycles.

## Diagnostic Logic: The F3N-740 Case Study
The framework identifies mechanical failures by monitoring logical "Floor" and "Ceiling" limits. A **Vacuum Leak** is confirmed in the data when:
1.  **Idle Regulation (Byte 27)** hits its minimum "floor" of 10.
2.  **Idle Richness Adaptation (Byte 26)** hits a "ceiling" (e.g., +92 / 220 raw) to compensate for unmetered air.
3.  **Idle Switch (Flags0)** remains active while RPM "ripples" or "hunts."

---
*Disclaimer: This is an open-source reverse-engineering project. Always cross-reference data with original Bendix/Renix Monopoint service manuals.*


----
The following part of this Readme is human-written:

## Building the Software
* the ESP32 firmware is an ESP-IDF project and should be build with "idf.py" the normal way from root folder.
* the tools for converting from bin or hex to human/gnuplot-friendly output are build with cmake from main folder
* the graphs are generated from the output of the tools or the firmware using some .gp files. these files handle the prefiltering using standard tools like grep and awk.

## Using the Software
* The ESP32 has to bei bluetooth-paired with the smart-phone or other devices which run the bluetooth terminal. Bluetooth Classic SPP is currently used.  BLE may be added later. You can switch models and filter output-lines interactively in the bluetooth-terminal app.
* the tools have some commandline options. call them with --help option to learn more.
* the gnuplot files can take the data from standard input. there are several options depending on the .gp file, for data-thinning (skip), range-selection (start, span, end). The .gp file for the "exp" model can take two line numbers (line_a, line_b) which will produce a diagram with 2 graphs.  the line number for the exp model are 1:1 the byte index in the frame (byte 0 is the program_version byte, the start bytes 0xff,0x00 are not counted). >t starts with line numer 2 and byte 2. the line number 1 is, like always the hex-string of the complete data frame.
  
## Building the Hardware
* This is optional. The ESP32 is there to capture data from the diagnose port, format it into live view data which is then passed to a bluetooth terminal.
*  If you already have a means to capture the binary diagnose data sent from your ECU, then you can pipe this data to the gnuplot files by using the tool xr25-bin2human on your PC.
 
```
build/xr25-bin2human --infile "YourData.bin" --model "x53b-740" | gnuplot -e "skip=100" -p gp/x53b-1.gp
```
* The ESP32 hardware is just a normal development board with a standard ESP32 (the first version).  Then build the well known interface between the car and the ESP32 UART2-RX pin. this is usual one transistor, one LED, a diode and some resistors.
* buy an adaptor cable "renault-12pin to obd2-16 pin" and let the obd2 connector hang out at the passenger foot well, if the 12pin connector is located there.  also buy an adaptor obd2 to open wires which you can solder to the universal circuit board which contains your esp32 board and the logic-level-adapter transistor circuit. You can get the  supply voltage for the ESP32 from an USB cable to cigarette-lighter adaptor or you take the 12 Volt from the diagnose port and use a buck-stepdown convertor to generate the 5V from it.