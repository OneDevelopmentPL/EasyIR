# EasyIR — Minimal IR library

EasyIR is a minimal, accurate, and easy-to-use infrared (IR) library for microcontrollers (Arduino/AVR, ESP8266, ESP32). It aims to be simpler than larger libraries (e.g. `IRremote`) while providing reliable NEC decoding and a simple NEC transmitter.

Goals and design
----------------
- Minimal API surface for quick integration.
- Small code size and low RAM usage for constrained MCUs.
- Clear wiring and usage patterns for beginners.
- Provide a solid NEC decoder and a simple transmitter; keep extension points for hardware PWM and extra protocols.

Contents
--------
- `src/EasyIR.h`, `src/EasyIR.cpp` — library implementation
- `examples/SimpleReceiver` — capture raw pulses and decode NEC
- `examples/SimpleSender` — send a NEC frame periodically
- `library.properties`, `keywords.txt`, `README.md`

Quickstart
----------
1. Copy the `EasyIR` folder into `Documents/Arduino/libraries/` or build with `arduino-cli` using `--libraries /path/to/EasyIR`.
2. Open `examples/SimpleReceiver` in Arduino IDE or compile with `arduino-cli`:

```bash
arduino-cli compile --fqbn arduino:avr:uno /path/to/EasyIR/examples/SimpleReceiver --libraries /path/to/EasyIR
```

Wiring (recommended)
---------------------
Receiver (e.g. TSOP38238):

		+5V  -----+         +--------------+
						 |         | TSOPxxxx     |
						[ ]  VCC   |  VCC   OUT  GND
						 |  module  |   |     |    |
		GND  ---+--------- GND |     |    +---- GND
													 +-----+     

Connect `OUT` to the Arduino digital input pin you choose (e.g. `2`). Use 3.3V Vcc for ESP boards when necessary.

Transmitter (IR LED with NPN transistor):

		+5V ---[resistor]---|>---+    (IR LED anode)
													|
											 IR LED
													|
										(IR LED cathode)
													+---- collector (NPN)
															emitter -> GND
															base <- Arduino pin via 1k resistor

Do not drive the IR LED directly from the MCU pin. Use a transistor driver and a suitable resistor.

API (detailed)
--------------
Header: `src/EasyIR.h`

- `EasyIR()`
	- Construct library object.

- `void begin(uint8_t rxPin)`
	- Configure `rxPin` as input and prepare receiver usage.

- `int readRaw(unsigned long *buffer, int maxLen, unsigned long timeout = 100000UL)`
	- Blocking capture of alternating mark/space durations (microseconds) into `buffer`.
	- Returns number of duration entries written (even count: mark,space,mark,space...).
	- `maxLen` is the maximum entries to store; choose buffer size accordingly (example uses 100).

- `bool decodeNEC(const unsigned long *buffer, int len, uint32_t &result)`
	- Attempt to decode NEC protocol from `buffer` of length `len`.
	- On success returns true and writes the 32-bit value into `result`.

- `void beginTx(uint8_t txPin)`
	- Configure transmitter pin as output. Use a transistor driver as shown above.

- `void sendNEC(uint32_t data, uint8_t txPin)`
	- Transmit `data` as a NEC frame using software carrier (~38 kHz). Simple and portable, but less precise than hardware PWM.

NEC protocol basics implemented
------------------------------
- Leader: mark ~= 9000 µs, space ~= 4500 µs
- Bit: mark ~= 560 µs, space ~= 560 µs (logical 0) or ~= 1690 µs (logical 1)
- 32 data bits transmitted MSB-first in our implementation

Timing table (typical values)
-----------------------------
- Leader mark: 9000 µs
- Leader space: 4500 µs
- Bit mark: 560 µs
- Bit space (0): 560 µs
- Bit space (1): 1690 µs

Examples
--------
Receiver (prints raw and attempts decode)
```cpp
#include <EasyIR.h>
EasyIR ir;
unsigned long buf[120];

void setup(){
	Serial.begin(115200);
	ir.begin(2); // attach TSOP OUT to digital pin 2
}

void loop(){
	int n = ir.readRaw(buf, 120, 200000UL);
	if(n>0){
		Serial.print("Captured entries: "); Serial.println(n);
		for(int i=0;i<n;i++){
			Serial.print(buf[i]); Serial.print(i+1<n?",":"\n");
		}
		uint32_t code;
		if(ir.decodeNEC(buf, n, code)){
			Serial.print("NEC: 0x"); Serial.println(code, HEX);
		} else {
			Serial.println("NEC decode failed");
		}
	}
	delay(200);
}
```

Sender
```cpp
#include <EasyIR.h>
EasyIR ir;
const uint8_t txPin = 3;

void setup(){
	ir.beginTx(txPin);
}

void loop(){
	// Example NEC code (Samsung/Pronto/etc.)
	ir.sendNEC(0x20DF10EFUL, txPin);
	delay(2000);
}
```

Building and testing
--------------------
arduino-cli example:
```bash
arduino-cli compile --fqbn arduino:avr:uno /path/to/EasyIR/examples/SimpleReceiver --libraries /path/to/EasyIR
```

PlatformIO example (`platformio.ini`):
```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
lib_extra_dirs = /path/to/EasyIR
```

Debugging tips
--------------
- If you get `EasyIR.h: No such file or directory`, either move the `EasyIR` folder into `Documents/Arduino/libraries/` or pass `--libraries /path/to/EasyIR` to `arduino-cli`.
- Use `SimpleReceiver` to print raw timings. Compare to the timing table above to tune `approx()` tolerances in `src/EasyIR.cpp`.
- If `decodeNEC` fails for a known remote, inspect the raw output. Some remotes use slight variations or different protocols.
- For unstable transmission use a transistor driver and verify the IR LED current and orientation.

Port-specific notes
-------------------
- AVR: `delayMicroseconds` and `digitalWrite` are used for software carrier. Timing is reasonable for NEC but not bit-perfect under heavy interrupts.
- ESP8266/ESP32: software timing may be less accurate due to WiFi/RTOS; consider using hardware PWM or dedicated libraries on these platforms for reliable transmission.

Next improvements (ideas)
------------------------
- Hardware PWM transmitter mode (AVR timer, ESP LEDC) selectable via API.
- Additional decoders (RC5, Sony) and repeat-code handling.
- Unit tests and CI with `arduino-cli` or PlatformIO.

Contributing
------------
- PRs welcome. Please follow small, focused commits and include tests or example sketches when adding functionality.

License
-------
See the `LICENSE` file included in this repository.


