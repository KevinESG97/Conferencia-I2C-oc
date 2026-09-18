/*
  SLAVE 1 - Direccion I2C 0x08
  ---------------------------------
  Recibe un byte (0-255) del master y enciende el LED RGB en un
  color solido segun 3 rangos fijos (sin degradado):
   - 0 a 90    -> Azul
   - 91 a 170  -> Morado
   - 171 a 255 -> Verde

  Conexiones del slave 1:
   - Pin 9  -> resistencia 220 ohm -> pata ROJA del LED RGB
   - Pin 10 -> resistencia 220 ohm -> pata VERDE del LED RGB
   - Pin 11 -> resistencia 220 ohm -> pata AZUL del LED RGB
   - Catodo/anodo comun -> GND o 5V segun el tipo de LED (este codigo asume catodo comun)
   - A4  -> SDA del bus I2C (comun con el master y el slave 2)
   - A5  -> SCL del bus I2C (comun con el master y el slave 2)
   - GND -> GND comun
*/

#include <Wire.h>

const byte DIR_SLAVE1 = 0x08;

const int PIN_ROJO  = 9;
const int PIN_VERDE = 10;
const int PIN_AZUL  = 11;

volatile byte valorRecibido = 0;

void setup() {
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);

  Wire.begin(DIR_SLAVE1);           // Se une al bus con direccion fija 0x08
  Wire.onReceive(alRecibirDatos);   // Funcion que se ejecuta al llegar datos

  Serial.begin(9600);
}

void loop() {
  if (valorRecibido <= 90) {
    // Azul solido
    encenderColor(0, 0, 255);
  } else if (valorRecibido <= 170) {
    // Morado solido (rojo + azul)
    encenderColor(128, 0, 255);
  } else {
    // Verde solido
    encenderColor(0, 255, 0);
  }

  delay(50);
}

void encenderColor(int rojo, int verde, int azul) {
  analogWrite(PIN_ROJO, rojo);
  analogWrite(PIN_VERDE, verde);
  analogWrite(PIN_AZUL, azul);
}

// Se ejecuta automaticamente cuando el master envia datos a esta direccion
void alRecibirDatos(int cantidadBytes) {
  if (Wire.available()) {
    valorRecibido = Wire.read();
    Serial.print("Slave 1 recibio: ");
    Serial.println(valorRecibido);
  }
}
