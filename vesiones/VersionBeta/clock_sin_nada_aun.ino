#include <TM1637Display.h>

#define CLK 2  // Pin del reloj del TM1637
#define DIO 3  // Pin de datos del TM1637
TM1637Display display(CLK, DIO);

int minutes = 0;
int seconds = 0;
int temporizador = 4;
unsigned long blinkTime = 0;
bool showDisplay = true;  // Variable para alternar entre mostrar y ocultar
unsigned long lastUpdate = 0;

void setup() {
  display.setBrightness(5);  // Ajusta el brillo (0-7)
}

void printTime() {
  int timeToDisplay = (minutes * 100) + seconds;  // Formato MM:SS
  display.showNumberDecEx(timeToDisplay, 0x40, true);  // Mostrar con ":" en MM:SS
}

void loop() {
  // Actualización cada segundo
  if (millis() - lastUpdate >= 1000) {  // cada segundo
    lastUpdate = millis();
    seconds++;
    
    if (seconds == 60) {
      seconds = 0;
      minutes++;
      
      if (minutes == 60) {
        minutes = 0;  // Reinicia a 00:00 después de 59:59
      }
    }
  }

/*

  if (minutes == temporizador){
    if (millis() - blinkTime >= 500) {
      blinkTime = millis();
      showDisplay = !showDisplay;  // Alterna entre mostrar y ocultar
      
      if (showDisplay) {
        printTime();  // Mostrar el tiempo
      } else {
        display.clear();  // Ocultar el tiempo
      }
      temporizador += 4;
    }
    
  }
*/
  
  printTime();  // Actualizar el display con el tiempo actual (MM:SS)
}
