/*
  MASTER - Bus I2C con 2 esclavos
  ---------------------------------
  Lee un potenciometro en A0 (0 a 1023) y decide:
   - Si el valor esta en la mitad baja (0-511)  -> envia orden al Slave 1 (LED RGB, direccion 0x08)
   - Si el valor esta en la mitad alta (512-1023) -> envia orden al Slave 2 (Buzzer, direccion 0x09)

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

void setup() {
  Wire.begin();          // Master no necesita direccion propia
  Serial.begin(9600);
}

void loop() {
  int valorPot = analogRead(PIN_POT);  // 0 a 1023

  if (valorPot < 512) {
    // Mitad baja -> Slave 1 (LED RGB)
    // Escalamos 0-511 a 0-255 para mandar un solo byte
    byte valorEscalado = map(valorPot, 0, 511, 0, 255);

    Wire.beginTransmission(DIR_SLAVE1);
    Wire.write(valorEscalado);
    Wire.endTransmission();

    Serial.print("Enviado a Slave 1 (LED RGB): ");
    Serial.println(valorEscalado);
  } else {
    // Mitad alta -> Slave 2 (Buzzer)
    // Escalamos 512-1023 a 0-255 para mandar un solo byte
    byte valorEscalado = map(valorPot, 512, 1023, 0, 255);

    Wire.beginTransmission(DIR_SLAVE2);
    Wire.write(valorEscalado);
    Wire.endTransmission();

    Serial.print("Enviado a Slave 2 (Buzzer): ");
    Serial.println(valorEscalado);
  }

  delay(200);  // Pequena pausa para no saturar el bus
}
