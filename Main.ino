#include <LiquidCrystal.h>
#include "Dinosaurio.h"
#include "Cactus.h"
#include "Pterodactilo.h"
#include <stdlib.h>

LiquidCrystal lcd(22, 23, 5, 18, 19, 21);
Dinosaurio dino;
Pterodactilo pterodactilo(random(10, 12)); 
Cactus cactus(16);

byte dinoReposo[8] = {
    B00000,
    B00000,
    B00111,
    B00101,
    B10111,
    B10100,
    B01101,
    B01100
};
byte dinoCamina1[8] = {      
    B00000,
    B00111,
    B00101,
    B10111,
    B10100,
    B01110,
    B01100,
    B00010
};  
byte dinoCamina2[8] = {
    B00000,
    B00111,
    B00101,
    B10111,
    B10100,
    B01110,
    B01100,
    B10000
};
byte dinoSalta[8] = {      
    B00111,
    B00101,
    B10111,
    B10100,
    B01101,
    B01100,
    B10000,
    B00000
};
byte cactus1[8] = {      
    B00000,
    B00000,
    B00001,
    B10101,
    B01101,
    B00110,
    B00100,
    B00100
};
byte pterodactilo1[8] = {
    B00000,
    B00001,
    B11101,
    B11011,
    B11111,
    B01110,
    B00100,
    B00000
};

byte pterodactilo2[8] = {
    B00000,
    B00001,
    B11101,
    B11011,
    B11111,
    B01110,
    B00000,
    B00000
};

struct Button {
    const uint8_t PIN;
    bool pressed;
};

Button button1 = {4, false};

int velocidad = 300;
unsigned long tiempoInicio = 0;
bool gameOver = false;

// Variables de conteo
int cuentaCactus = 0;
int cuentaPterodactilo = 0;
int cuentaTotal = 0;

// Constante para el decremento de velocidad
const int decrementoVelocidad = 6;

void IRAM_ATTR isr() {  
    button1.pressed = true;  
}

void setup() {
    lcd.begin(16, 2);
    Serial.begin(115200);
    Serial.println("test");

    pinMode(button1.PIN, INPUT_PULLUP);
    attachInterrupt(button1.PIN, isr, FALLING);

    lcd.createChar(0, dinoReposo);
    lcd.createChar(1, dinoCamina1);
    lcd.createChar(2, dinoCamina2);
    lcd.createChar(3, dinoSalta);
    lcd.createChar(4, cactus1);
    lcd.createChar(5, pterodactilo1);
    lcd.createChar(6, pterodactilo2);

    lcd.setCursor(0, 1);
    lcd.write(byte(0));

    lcd.setCursor(0, 0);
    lcd.print("GO");
    delay(2000);

    tiempoInicio = millis();

    // Reiniciar contadores y velocidad
    cuentaCactus = 0;
    cuentaPterodactilo = 0;
    cuentaTotal = 0;
    velocidad = 300;
}

void loop() {
    lcd.clear();

    if (!gameOver) {
        if (button1.pressed) {
            dino.saltar();
            lcd.setCursor(0, 0);
            button1.pressed = false;
        } else {
            lcd.setCursor(0, 1);
            dino.caminar();
        }

        lcd.write(byte(dino.obtenerEstado()));

        pterodactilo.mover();
        if (pterodactilo.obtenerEstado() == 5) {
            lcd.setCursor(pterodactilo.obtenerPosicion(), 0);
            lcd.write(byte(5));
        } else {
            lcd.setCursor(pterodactilo.obtenerPosicion(), 0);
            lcd.write(byte(6));
        }
        pterodactilo.volar();

        cactus.mover();
        lcd.setCursor(cactus.obtenerPosicion(), 1);
        lcd.write(byte(4));

        // Verificar colisión con el cactus
        if (dino.obtenerPosicion() == 0 && cactus.obtenerPosicion() == 0) {
            gameOver = true;
        } else if (cactus.obtenerPosicion() == 0) {
            cuentaCactus++;
            cuentaTotal++;
            if (velocidad > decrementoVelocidad) {
                velocidad -= decrementoVelocidad;
            }
        }

        // Verificar colisión con el pterodáctilo
        if (dino.obtenerPosicion() == 1 && pterodactilo.obtenerPosicion() == 0) {
            gameOver = true;
        } else if (pterodactilo.obtenerPosicion() == 0) {
            cuentaPterodactilo++;
            cuentaTotal++;
            if (velocidad > decrementoVelocidad) {
                velocidad -= decrementoVelocidad;
            }
        }

        // Mostrar el tiempo transcurrido
        unsigned long tiempoTranscurrido = millis() - tiempoInicio;
        lcd.setCursor(8, 0);
        lcd.print("TIME:");
        lcd.print(tiempoTranscurrido / 1000);

        // Mostrar las cuentas
        lcd.setCursor(13, 1);
        lcd.print("");
        lcd.print(cuentaTotal);
    }

    if (gameOver) {
        lcd.setCursor(4, 0);
        lcd.print("GAME OVER");
        lcd.setCursor(4, 1);
        lcd.print("SCORE: ");
        if (cuentaTotal < 10) {
            lcd.print("0");
        }
        lcd.print(cuentaTotal);
        if (button1.pressed) {
            setup();
            gameOver = false;
        }
    }

    delay(velocidad);
}
