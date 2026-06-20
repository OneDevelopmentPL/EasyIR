# EasyIR

EasyIR is a small, easy-to-use infrared (IR) library for Arduino, ESP8266 and ESP32. It focuses on simplicity and accuracy for common IR tasks while being easier to use than larger libraries like `IRremote`.

Features
- Read raw IR pulses into a microsecond-duration array (`readRaw`).
- Decode NEC protocol signals to a 32-bit code (`decodeNEC`).
- Send NEC signals using software-generated ~38 kHz carrier (`sendNEC`).

Installation (Arduino IDE)
1. Copy the `EasyIR` folder into your `Documents/Arduino/libraries/` directory, or compile with `arduino-cli` using `--libraries /path/to/EasyIR`.
2. Restart Arduino IDE and open one of the examples: `SimpleReceiver` or `SimpleSender`.

Quick usage
- `EasyIR ir; ir.begin(rxPin);` — initialize receiver on `rxPin`.
- `int n = ir.readRaw(buf, 100);` — read raw pulse durations (microseconds) into `buf`.
- `if (ir.decodeNEC(buf, n, code))` — decode NEC into 32-bit `code`.
- `ir.beginTx(txPin); ir.sendNEC(code, txPin);` — initialize transmitter and send NEC code.

Notes and limitations
- This library is intentionally simple and uses software timing for the carrier when sending. For more robust timing or additional protocols, consider using hardware PWM or a dedicated library.
- The `readRaw` implementation uses `pulseIn` and simple blocking reads; it is suitable for basic use but not for high-performance or multitasking scenarios.

Examples
- `examples/SimpleReceiver` — captures raw pulses and attempts NEC decode.
- `examples/SimpleSender` — sends a test NEC code periodically.

Contributing and enhancements
- If you want hardware PWM support (AVR timers or ESP PWM) or additional protocol decoders (RC5, Sony), open an issue or pull request.

License
- See the repository `LICENSE` file for license details.

Detailed documentation

Overview
--------
EasyIR provides a minimal API to capture raw IR pulse timings, decode NEC protocol frames, and transmit NEC frames. The goal is to be simple to understand and use on small microcontrollers while providing reliable NEC decoding and sending for common remote-control tasks.

Supported platforms
-------------------
- AVR (Arduino Uno, Nano, etc.)
- ESP8266 (NodeMCU, Wemos)
- ESP32

Requirements
------------
- Arduino core for your board (installable via Arduino IDE or `arduino-cli`).

Installation
------------
Arduino IDE
1. Copy the `EasyIR` folder into `Documents/Arduino/libraries/`.
2. Restart Arduino IDE.
3. Open `File > Examples > EasyIR > SimpleReceiver` or open `examples/SimpleReceiver/SimpleReceiver.ino`.

Arduino Library Manager
- If the library is published to the Arduino Library Manager, you can install it from the IDE: `Sketch > Include Library > Manage Libraries...` then search for "EasyIR" and click `Install`.
- With `arduino-cli` you can also install from the Library Manager with:
```bash
arduino-cli lib install EasyIR
```
arduino-cli
1. Install `arduino-cli` (Homebrew: `brew install arduino-cli`).
2. Install cores you need, e.g. `arduino-cli core install arduino:avr`.
3. Compile examples with the `--libraries` flag pointing to the `EasyIR` path:
```bash
arduino-cli compile --fqbn arduino:avr:uno /path/to/EasyIR/examples/SimpleReceiver --libraries /path/to/EasyIR
```

PlatformIO
1. Optionally add this library path in `platformio.ini` as `lib_extra_dirs = /path/to/EasyIR` or copy the folder into the `lib/` directory of your project.

Wiring / Hardware
------------------
Receiver (typical TSOPxxxx)
- VCC -> 5V (or 3.3V for 3.3V modules)
- GND -> GND
- OUT -> digital input pin (e.g. `2`)

Transmitter (IR LED + transistor recommended)
- IR LED anode -> resistor (100-330Ω) -> +5V
- IR LED cathode -> collector of NPN transistor (e.g. 2N2222)
- Emitter -> GND
- Arduino TX pin -> base resistor (1kΩ) -> base of transistor
- Add a diode and current-limiting resistor as appropriate. Do not drive IR LED directly from a GPIO without a transistor.

API Reference
-------------
Class: `EasyIR`

- `EasyIR()`
	- Constructor.
- `void begin(uint8_t rxPin)`
	- Initialize receiver on pin `rxPin`. Sets pin mode to `INPUT`.
- `int readRaw(unsigned long *buffer, int maxLen, unsigned long timeout = 100000UL)`
	- Reads alternating HIGH/LOW pulse durations (microseconds) into `buffer` using `pulseIn`.
	- Returns number of entries written (each entry is a duration). `maxLen` must match buffer size.
	- `timeout` is the maximum wait time for pulses (microseconds).
- `bool decodeNEC(const unsigned long *buffer, int len, uint32_t &result)`
	- Attempts to decode NEC from the raw durations in `buffer` of length `len`.
	- On success returns `true` and writes the 32-bit NEC code into `result`.
- `void beginTx(uint8_t txPin)`
	- Initialize transmitter pin as `OUTPUT`.
- `void sendNEC(uint32_t data, uint8_t txPin)`
	- Send 32-bit `data` as a NEC frame via `txPin` using software carrier generation (~38 kHz).

Example usage (receiver)
```cpp
#include <EasyIR.h>
EasyIR ir;
unsigned long buf[100];

void setup(){
	Serial.begin(115200);
	ir.begin(2); // attach receiver to pin 2
}

void loop(){
	int n = ir.readRaw(buf, 100);
	if(n>0){
		uint32_t code;
		if(ir.decodeNEC(buf, n, code)){
			Serial.print("NEC: 0x"); Serial.println(code, HEX);
		}
	}
}
```

Example usage (sender)
```cpp
#include <EasyIR.h>
EasyIR ir;
void setup(){ ir.beginTx(3); }
void loop(){ ir.sendNEC(0x20DF10EFUL, 3); delay(1000); }
```

Build examples with arduino-cli
-----------------------------
```bash
arduino-cli compile --fqbn arduino:avr:uno /path/to/EasyIR/examples/SimpleReceiver --libraries /path/to/EasyIR
arduino-cli compile --fqbn arduino:avr:uno /path/to/EasyIR/examples/SimpleSender --libraries /path/to/EasyIR
```

PlatformIO example snippet (`platformio.ini`)
```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
lib_extra_dirs = /path/to/EasyIR
```

Timing and tolerances
---------------------
- The decoder uses approximate matching for NEC timings (leader ~9000/4500 µs, bit mark ~560 µs, space ~560/1690 µs). Tolerance values are conservative but may need adjustment for noisy receivers or different remotes.
- `readRaw` uses `pulseIn` which is blocking and has limits on minimum detectable pulse width; it's fine for simple use but not for timing-critical multi-tasking applications.

Troubleshooting
---------------
- `EasyIR.h` not found when compiling: either copy the library to `Documents/Arduino/libraries/` or use `--libraries /path/to/EasyIR` when compiling with `arduino-cli`.
- Decoder fails on some remotes: try increasing tolerance in `approx` or inspect raw timings printed from `SimpleReceiver`.
- Transmission unstable: use transistor driver or implement hardware PWM for carrier generation.

Contributing
------------
- Feel free to open issues or PRs for additional protocol decoders (RC5, Sony), hardware PWM support, or performance improvements.

License
-------
See the `LICENSE` file in the repository.

