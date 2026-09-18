/*
  SLAVE 2 - Direccion I2C 0x09
  ---------------------------------
  Recibe un byte (0-255) del master. Cada vez que el valor CAMBIA
  (es decir, se movio el potenciometro), hace un beep corto y
  luego se calla. Si el potenciometro se queda quieto, el buzzer
  permanece en silencio.

  Conexiones del slave 2:
   - Pin 8   -> terminal positivo del buzzer pasivo
   - GND     -> terminal negativo del buzzer
   - A4  -> SDA del bus I2C (comun con el master y el slave 1)
   - A5  -> SCL del bus I2C (comun con el master y el slave 1)
   - GND -> GND comun
*/

#include <Wire.h>

const byte DIR_SLAVE2 = 0x09;
const int PIN_BUZZER = 8;

const int FRECUENCIA_BEEP = 1000;   // Tono del beep en Hz
const unsigned long DURACION_BEEP = 150; // Duracion del beep en ms

volatile byte valorRecibido = 0;
volatile bool huboCambio = false;

byte ultimoValorProcesado = 0;
bool sonando = false;
unsigned long momentoInicioBeep = 0;

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);

  Wire.begin(DIR_SLAVE2);           // Se une al bus con direccion fija 0x09
  Wire.onReceive(alRecibirDatos);   // Funcion que se ejecuta al llegar datos

  Serial.begin(9600);
}

void loop() {
  // Si llego un valor distinto al ultimo procesado, arrancamos un beep nuevo
  if (huboCambio) {
    huboCambio = false;

    if (valorRecibido != ultimoValorProcesado) {
      ultimoValorProcesado = valorRecibido;
      tone(PIN_BUZZER, FRECUENCIA_BEEP);
      sonando = true;
      momentoInicioBeep = millis();

      Serial.print("Beep - nuevo valor: ");
      Serial.println(valorRecibido);
    }
  }

  // Apagamos el beep despues de la duracion definida, sin usar delay()
  if (sonando && (millis() - momentoInicioBeep >= DURACION_BEEP)) {
    noTone(PIN_BUZZER);
    sonando = false;
  }
}

// Se ejecuta automaticamente cuando el master envia datos a esta direccion
void alRecibirDatos(int cantidadBytes) {
  if (Wire.available()) {
    valorRecibido = Wire.read();
    huboCambio = true;
  }
}
