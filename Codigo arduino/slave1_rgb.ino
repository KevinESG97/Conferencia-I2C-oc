/*
  SLAVE 1 - Direccion I2C 0x08
  ---------------------------------
  Recibe el valor completo del potenciometro (0-1023) partido en
  2 bytes, lo reconstruye, y enciende el LED RGB en un color solido:
   - 0 a 300    -> Azul
   - 301 a 700  -> Morado
   - 701 a 1023 -> Verde

  Conexiones del slave 1 (catodo comun):
   - Pin 9  -> resistencia 220 ohm -> pata VERDE del LED RGB
   - Pin 10 -> resistencia 220 ohm -> pata AZUL del LED RGB
   - Pin 11 -> resistencia 220 ohm -> pata ROJA del LED RGB
   - Catodo comun -> GND
   - A4  -> SDA del bus I2C (comun con el master y el slave 2)
   - A5  -> SCL del bus I2C (comun con el master y el slave 2)
   - GND -> GND comun
*/

#include <Wire.h>

const byte DIR_SLAVE1 = 0x08;

const int PIN_VERDE = 9;
const int PIN_AZUL  = 10;
const int PIN_ROJO  = 11;

volatile int valorRecibido = 0;
volatile bool nuevoDato = false;

void setup() {
  pinMode(PIN_ROJO, OUTPUT);
  pinMode(PIN_VERDE, OUTPUT);
  pinMode(PIN_AZUL, OUTPUT);

  Wire.begin(DIR_SLAVE1);
  Wire.onReceive(alRecibirDatos);

  Serial.begin(9600);
  Serial.println("Slave 1 (LED RGB) listo, direccion 0x08");
}

void loop() {
  if (nuevoDato) {
    nuevoDato = false;

    Serial.print("Slave 1 RECIBIO: ");
    Serial.print(valorRecibido);

    if (valorRecibido <= 300) {
      Serial.println("  -> Color: AZUL");
      encenderColor(0, 0, 255);
    } else if (valorRecibido <= 700) {
      Serial.println("  -> Color: MORADO");
      encenderColor(128, 0, 255);
    } else {
      Serial.println("  -> Color: VERDE");
      encenderColor(0, 255, 0);
    }
  }
}

void encenderColor(int rojo, int verde, int azul) {
  analogWrite(PIN_ROJO, rojo);
  analogWrite(PIN_VERDE, verde);
  analogWrite(PIN_AZUL, azul);
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
