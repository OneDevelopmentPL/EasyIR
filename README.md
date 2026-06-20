# EasyIR

Prosta i dokładna biblioteka do pracy z podczerwienią (IR) dla Arduino, ESP8266 i ESP32.

Funkcje:
- Odbieranie surowych impulsów (tablica czasów w mikrosekundach)
- Dekodowanie protokołu NEC (często używany w pilotach)
- Wysyłanie pakietów NEC (generacja nośnej ~38kHz)

Instalacja (Arduino IDE):
1. Skopiuj folder `EasyIR` do `Documents/Arduino/libraries/`.
2. Uruchom Arduino IDE, otwórz przykład `SimpleReceiver` lub `SimpleSender`.

Użycie:
- `EasyIR ir; ir.begin(rxPin);` — inicjalizacja odbioru
- `int n = ir.readRaw(buf, 100);` — odczyt surowych impulsów do `buf`
- `ir.decodeNEC(buf, n, code)` — dekoduje NEC do 32-bit `code`
- `ir.beginTx(txPin); ir.sendNEC(code, txPin);` — nadaje NEC

Uwaga:
- Biblioteka jest prosta i ma na celu szybko rozpocząć pracę z IR. Dla bardziej zaawansowanych zastosowań (lepsza tolerancja czasowa, inne protokoły, sprzętowe PWM) rozważ użycie bibliotek takich jak `IRremote`.
