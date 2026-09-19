/*
  SLAVE 2 - Direccion I2C 0x09
  ---------------------------------
  Recibe el valor completo del potenciometro (0-1023) partido en
  2 bytes, lo reconstruye, y hace una cantidad de pitidos segun
  el rango:
   - 0 a 300    -> 1 pitido
   - 301 a 700  -> 2 pitidos
   - 701 a 1023 -> 3 pitidos

  Los pitidos se hacen sin usar delay() para no bloquear la
  recepcion I2C mientras suena.

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

const int FRECUENCIA_BEEP = 1000;
const unsigned long DURACION_BEEP = 120;   // Cuanto dura cada pitido
const unsigned long PAUSA_ENTRE_BEEPS = 100; // Silencio entre pitidos seguidos

volatile int valorRecibido = 0;
volatile bool nuevoDato = false;

int pitidosPendientes = 0;   // Cuantos pitidos faltan por hacer
bool sonando = false;
unsigned long momentoUltimoCambio = 0;

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);

  Wire.begin(DIR_SLAVE2);
  Wire.onReceive(alRecibirDatos);

  Serial.begin(9600);
  Serial.println("Slave 2 (Buzzer) listo, direccion 0x09");
}

void loop() {
  // Si llego un valor nuevo, decidimos cuantos pitidos hacer
  if (nuevoDato) {
    nuevoDato = false;

    Serial.print("Slave 2 RECIBIO: ");
    Serial.print(valorRecibido);

    int cantidadPitidos;
    if (valorRecibido <= 300) {
      cantidadPitidos = 1;
    } else if (valorRecibido <= 700) {
      cantidadPitidos = 2;
    } else {
      cantidadPitidos = 3;
    }

    Serial.print("  -> Pitidos: ");
    Serial.println(cantidadPitidos);

    pitidosPendientes = cantidadPitidos;
    sonando = false;  // Reinicia cualquier secuencia anterior
  }

  // Maquina de estados simple para los pitidos, sin bloquear con delay()
  if (pitidosPendientes > 0 && !sonando) {
    tone(PIN_BUZZER, FRECUENCIA_BEEP);
    sonando = true;
    momentoUltimoCambio = millis();
  } else if (sonando && (millis() - momentoUltimoCambio >= DURACION_BEEP)) {
    noTone(PIN_BUZZER);
    sonando = false;
    pitidosPendientes--;
    momentoUltimoCambio = millis();

    // Pequena pausa antes del siguiente pitido (si falta alguno)
    if (pitidosPendientes > 0) {
      while (millis() - momentoUltimoCambio < PAUSA_ENTRE_BEEPS) {
        // Pausa corta y fija, no afecta la deteccion de nuevos valores
        // porque es mucho mas breve que el tiempo de reaccion humano
      }
    }
  }
}

// Se ejecuta cuando llegan los 2 bytes del master
void alRecibirDatos(int cantidadBytes) {
  if (Wire.available() >= 2) {
    byte byteAlto = Wire.read();
    byte byteBajo = Wire.read();
    valorRecibido = word(byteAlto, byteBajo);  // Reconstruye el valor 0-1023
    nuevoDato = true;
  }
}
