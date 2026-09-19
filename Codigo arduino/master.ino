/*
  MASTER - Bus I2C con 2 esclavos
  ---------------------------------
  Lee un potenciometro en A0 (0 a 1023) y envia ese valor COMPLETO
  a los dos slaves al mismo tiempo (no elige entre uno u otro).

  Como I2C manda datos de a 1 byte (maximo 255), el valor de hasta
  1023 se parte en 2 bytes: byte alto y byte bajo.

  Conexiones del master:
   - A0  -> pata central del potenciometro
   - A4  -> SDA del bus I2C (comun con los 2 slaves)
   - A5  -> SCL del bus I2C (comun con los 2 slaves)
   - 5V  -> riel de 5V de la protoboard (alimenta las resistencias pull-up)
   - GND -> GND comun con los 2 slaves
*/

#include <Wire.h>

const int PIN_POT = A0;
const byte DIR_SLAVE1 = 0x08;  // LED RGB
const byte DIR_SLAVE2 = 0x09;  // Buzzer

const int MARGEN_TOLERANCIA = 3;  // Ignora cambios menores a esto (ruido electrico)

int ultimoValorProcesado = -1;  // -1 fuerza que la primera lectura siempre se procese

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  int valorPot = analogRead(PIN_POT);  // 0 a 1023

  if (abs(valorPot - ultimoValorProcesado) > MARGEN_TOLERANCIA) {
    ultimoValorProcesado = valorPot;

    // Partimos el valor de 16 bits (0-1023) en 2 bytes de 8 bits
    byte byteAlto = highByte(valorPot);
    byte byteBajo = lowByte(valorPot);

    Serial.println("----------------------------");
    Serial.print("Potenciometro leyo: ");
    Serial.println(valorPot);
    Serial.print("Enviando byteAlto=");
    Serial.print(byteAlto);
    Serial.print(" byteBajo=");
    Serial.println(byteBajo);

    // Enviamos el mismo valor a AMBOS slaves
    Wire.beginTransmission(DIR_SLAVE1);
    Wire.write(byteAlto);
    Wire.write(byteBajo);
    byte resultado1 = Wire.endTransmission();

    Wire.beginTransmission(DIR_SLAVE2);
    Wire.write(byteAlto);
    Wire.write(byteBajo);
    byte resultado2 = Wire.endTransmission();

    if (resultado1 == 0) {
      Serial.println("Envio exitoso al Slave 1 (LED)");
    } else {
      Serial.print("ERROR enviando a Slave 1, codigo: ");
      Serial.println(resultado1);
    }

    if (resultado2 == 0) {
      Serial.println("Envio exitoso al Slave 2 (Buzzer)");
    } else {
      Serial.print("ERROR enviando a Slave 2, codigo: ");
      Serial.println(resultado2);
    }
  }
}
