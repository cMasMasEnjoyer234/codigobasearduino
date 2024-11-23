#include <TM1637Display.h>

#define CLK 2       // Pin del reloj del TM1637
#define DIO 3       // Pin de datos del TM1637
#define MINUTE_BTN 4  // Pin del botón para ajustar minutos
#define SECOND_BTN 5  // Pin del botón para ajustar segundos

TM1637Display display(CLK, DIO);

int minutes = 0;
int seconds = 0;
unsigned long lastUpdate = 0;
unsigned long blinkTime = 0;
bool showDisplay = true;  // Variable para alternar entre mostrar y ocultar

void setup() {
  pinMode(MINUTE_BTN, INPUT_PULLUP);  // Configurar el botón de minutos
  pinMode(SECOND_BTN, INPUT_PULLUP);  // Configurar el botón de segundos
  display.setBrightness(5);  // Ajusta el brillo (0-7)
}

void printTime() {
  int timeToDisplay = (minutes * 100) + seconds;  // Formato MM:SS
  display.showNumberDecEx(timeToDisplay, 0x40, true);  // Mostrar con ":" en MM:SS
}

void loop() {
  // Incrementar minutos al presionar el botón de minutos
  if (digitalRead(MINUTE_BTN) == HIGH) {
    minutes++;
    if (minutes == 24) {
      minutes = 0;
    }
      blinkTime = millis();
      showDisplay = !showDisplay;  // Alterna entre mostrar y ocultar
      
      if (showDisplay) {
        printTime();  // Mostrar el tiempo
      } else {
        display.clear();  // Ocultar el tiempo
      }
    if (millis() - blinkTime >= 1) {
      blinkTime = millis();
      showDisplay = !showDisplay;  // Alterna entre mostrar y ocultar
      
      if (showDisplay) {
        printTime();  // Mostrar el tiempo
      } else {
        display.clear();  // Ocultar el tiempo
      }
    }
    delay(200);  // Retraso para evitar rebotes
  }

  // Incrementar segundos al presionar el botón de segundos
  if (digitalRead(SECOND_BTN) == HIGH) {
    seconds++;
    if (seconds == 60) {
      seconds = 0;
      minutes++;
      if (minutes == 60) {
        minutes = 0;
      }
    }
      blinkTime = millis();
      showDisplay = !showDisplay;  // Alterna entre mostrar y ocultar
      
      if (showDisplay) {
        printTime();  // Mostrar el tiempo
      } else {
        display.clear();  // Ocultar el tiempo
      }
    if (millis() - blinkTime >= 1) {
      blinkTime = millis();
      showDisplay = !showDisplay;  // Alterna entre mostrar y ocultar
      
      if (showDisplay) {
        printTime();  // Mostrar el tiempo
      } else {
        display.clear();  // Ocultar el tiempo
      }
    }
    delay(200);  // Retraso para evitar rebotes
  }

  // Actualizar cada segundo para incrementar el tiempo automáticamente
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    seconds++;
    
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      
      if (minutes >= 24) {
        minutes = 0;  // Reinicia a 00:00 después de 59:59
      }
    }
  }


  printTime();

}