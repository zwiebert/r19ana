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
