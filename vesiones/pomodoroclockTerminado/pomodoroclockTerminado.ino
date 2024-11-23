#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define CLK_PIN D2               // Pin del CLK del display
#define DIO_PIN D3               // Pin del DIO del display
#define POT_PIN A0               // Pin del potenciómetro en NodeMCU
#define CHANGESTATE_BTN D0       // Botón para cambiar configuraciones dentro del modo Pomodoro
#define CHANGEMODES_BTN D5       // Botón para cambiar entre mostrar hora y modo Pomodoro
//#define SIMULATED_BTN D6  // Pin que usas solo desde software
//#define SIMULATED_BTN2 D4  // Pin que usas solo desde software


#define BUZZER_PIN D7            // Pin del buzzer

// WiFi & MQTT
const char* ssid = "Galaxy S10111b";                // Cambia a tu SSID
const char* password = "ncgx1560";        // Cambia a tu contraseña
const char* mqtt_server = "test.mosquitto.org";       // Cambia a la dirección IP del brc:\Users\Emilio\Documents\Arduino\pomodoroclokc_esp82pansa\pomodoroclokc_esp82pansa.inooker MQTT

WiFiClient espClient;
PubSubClient client(espClient);



TM1637Display display(CLK_PIN, DIO_PIN);

int cambiodemodos = 0;           // Variable para alternar entre hora y Pomodoro
int modohoraOminuto = 0;         // Variable para alternar dentro del Pomodoro
int pansa = 0;                   // Variable para alternar entre numCycles restMinutes WorkMinutes
int numCycles = 1;               // Número de ciclos Pomodoro (por defecto 1)
int restMinutes = 1;             // Tiempo de descanso (por defecto 1 minuto)
int workMinutes = 10;            // Tiempo de trabajo (por defecto 10 minutos)

int currentCycle = 0;            // Ciclo actual del Pomodoro
int minutes = 0;                 // Minutos restantes en la cuenta regresiva
int seconds = 0;                 // Segundos restantes en la cuenta regresiva

int minutesClock = 0;
int hoursClock = 0;

bool isWorkPeriod = true;        // Bandera para alternar entre trabajo y descanso
bool configCompleted = false;    // Indica si la configuración ha terminado
bool configClockCompleted = false;

unsigned long lastUpdate = 0;
unsigned long lastUpdateClock = 0;
int lastPotValue = 0;

unsigned long lastDebounceTime = 0;  // Tiempo de la última pulsación
unsigned long debounceDelay = 50;    // Tiempo mínimo para considerar una nueva pulsación

//int sinoMqtt = 0;



int CHANGEstate = 0;
int CHANGEmode = 0;
int potValueMQTT = 0;

int MODOMQTT = false;
int confirmRandom = 0;
// Función para conectar WiFi

/*
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
*/
void setup_wifi() {
  delay(10);
  Serial.print("Conectando al WiFi...");
  WiFi.begin(ssid, password);

  // Intentar conectar a WiFi con un límite de intentos
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) { // Intentar durante 15 segundos
    delay(10);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado al WiFi");
  } else {
    Serial.println("No se pudo conectar al WiFi");
  }
}

// Función para manejar mensajes MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    char message[50];  // Buffer para almacenar el mensaje
    if (length >= sizeof(message)) {
        length = sizeof(message) - 1;  // Limitar longitud para evitar desbordamientos
    }

    memcpy(message, payload, length);  // Copiar mensaje al buffer
    message[length] = '\0';  // Asegurar terminación de cadena

    if (strcmp(topic, "pomodoro/mode") == 0) {
        
      CHANGEmode = atoi(message);

    } 
    else if (strcmp(topic, "Slider/pomo") == 0) {
      //MODOMQTT = true;
      if(confirmRandom != 1){
        potValueMQTT = atoi(message); 
      }
      
            
    } 
    else if (strcmp(topic, "change/configpomodoro") == 0) {
        CHANGEstate = atoi(message);
    } 
    else if (strcmp(topic, "confirm/some") == 0) {
        confirmRandom = atoi(message);
    }else if (strcmp(topic, "modo/wifi") == 0) {
        if(atoi(message) == 1){
          MODOMQTT = true;
        }else if(atoi(message) == 0){
          MODOMQTT = false;
        }
    }  
    c:\Users\Emilio\Documents\Arduino\producto_casi_final\producto_casi_final.ino
}


// Configuración inicial
void setup() {
  display.setBrightness(0x0f);     // Máximo brillo
  pinMode(CHANGESTATE_BTN, INPUT_PULLDOWN_16);
  pinMode(CHANGEMODES_BTN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
 // pinMode(SIMULATED_BTN, OUTPUT); // boton mqtt 1
  //pinMode(SIMULATED_BTN2, OUTPUT); // boton mqtt 2
  //digitalWrite(SIMULATED_BTN, LOW);  // Asegúrate de que inicien en LOW
 // digitalWrite(SIMULATED_BTN2, LOW); 

  setup_wifi();  // Intentar conexión WiFi
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Si WiFi está conectado, intentamos conectarnos a MQTT (si falla, no pasa nada)
  if (WiFi.status() == WL_CONNECTED) {
    reconnect();  // Intentamos la conexión MQTT, pero no bloqueamos el programa
  } else {
    Serial.println("No hay conexión WiFi, MQTT no será utilizado.");
  }
}

// Reconexión MQTT si la conexión se pierde
void reconnect() {
  // Intentar reconectar al broker MQTT solo si no estamos conectados
  if (!client.connected()) {
    Serial.print("Intentando conectar a MQTT...");

    // Intentar conectar al cliente MQTT con un nombre único
    if (client.connect("NodeMCUClient")) {
      Serial.println("Conectado a MQTT");

      // Suscribirse a los tópicos relevantes
      client.subscribe("pomodoro/mode");
      client.subscribe("Slider/pomo");
      client.subscribe("change/configpomodoro");
      client.subscribe("confirm/some");
      client.subscribe("modo/wifi");

    } else {
      // Si no se puede conectar, mostrar el estado del error y esperar
      Serial.print("Error al conectar a MQTT: ");
      Serial.println(client.state());
      delay(1);  // Esperar antes de intentar de nuevo
    }
  }
}



void loop() {



  int potValue = leerPotenciometroSuavizado();
  //int buttonState = digitalRead(CHANGEMODES_BTN);
  Serial.println("Cambiodemodos: " + String(cambiodemodos));
  Serial.println("NumCycles: " + String(numCycles));
  Serial.println("WorkMinutes: " + String(workMinutes));

  switch (cambiodemodos) {
    case 0:  // Modo de visualización de la hora
      if (digitalRead(CHANGEMODES_BTN) == HIGH || CHANGEmode == 1) {
        emitirSonidoBuzzer(50);  // Sonido breve al presionar el botón
        cambiodemodos = 1;       // Alterna entre modos
        lastDebounceTime = millis();
        configCompleted = false;
        if (configClockCompleted) {
          MantenerHora();
        }
        CHANGEmode = 0;
        //digitalWrite(SIMULATED_BTN, LOW); 
        delay(200);
      }

      if (!configClockCompleted) {
        ConfigHora();
      }
      if (configClockCompleted) {
        MantenerHora();
        MostrarHora(hoursClock, minutesClock);
      }
      break;

    case 1:  // Modo Pomodoro
      if ((digitalRead(CHANGEMODES_BTN) == HIGH && !configCompleted) || (CHANGEmode == 1 && !configCompleted)) {
        emitirSonidoBuzzer(50);  // Sonido breve al presionar el botón
        CHANGEmode = 0;
        cambiodemodos = 0;
        //digitalWrite(SIMULATED_BTN, LOW); 
        delay(200);
      }
      if (configCompleted) {
        emitirSonidoBuzzer(50);  // Sonido breve al finalizar la configuración
        cambiodemodos = 2;
        delay(200);
      }
      ConfigurarPomodoro(potValue);
      break;

    case 2:  // Ejecución del Pomodoro
      if (digitalRead(CHANGEMODES_BTN) == HIGH || CHANGEmode == 1) {
        emitirSonidoBuzzer(50);  // Sonido breve al presionar el botón
        cambiodemodos = 0;
        CHANGEmode = 0;
        configCompleted = false;
        //digitalWrite(SIMULATED_BTN, LOW); 
        delay(200);
      }
      IniciarPomodoro();
      break;
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // Procesa los mensajes MQTT
  

}

void emitirSonidoBuzzer(int duracion) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duracion);
  digitalWrite(BUZZER_PIN, LOW);
}

void alarmaHastaBoton() {
  while (digitalRead(CHANGESTATE_BTN) != HIGH) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
  digitalWrite(BUZZER_PIN, LOW);  // Asegúrate de apagar el buzzer
  delay(200);  // Evita lecturas múltiples del botón
}

/////////////////////////
int leerPotenciometroSuavizado() {
  static float filteredValue = 0;  // Valor suavizado
  const float alpha = 0.1;         // Factor de suavizado (ajustable entre 0 y 1)
  int rawValue = analogRead(POT_PIN);

  // Fórmula del filtro exponencial
  filteredValue = alpha * rawValue + (1 - alpha) * filteredValue;

  return (int)filteredValue;
}



void ConfigurarPomodoro(int potValue) {
  if(configClockCompleted){
    MantenerHora();
  }
   if (digitalRead(CHANGESTATE_BTN) == HIGH || CHANGEstate== 1) {
      emitirSonidoBuzzer(50);
      pansa++;
      if (pansa > 2) pansa = 0;
      //lastDebounceTime = millis();
      //digitalWrite(SIMULATED_BTN2, LOW);
      delay(200);
   }
// && (millis() - lastDebounceTime) > debounceDelay
  //MostrarTiempo(0, 0);



  if(!MODOMQTT){
    if (abs(potValue - lastPotValue) > 10) {
      
      if(pansa == 0){
          numCycles = map(potValue, 0, 1023, 1, 10);
          MostrarTiempo(numCycles, 0);
      }else if (pansa == 1){
          restMinutes = map(potValue, 0, 1023, 1, 15);
          MostrarTiempo(restMinutes, 0);
      }else if (pansa == 2){
          workMinutes = map(potValue, 0, 1023, 10, 59);
          MostrarTiempo(workMinutes, 0);
      }
      

      lastPotValue = potValue;
    }
  }else{
      if(pansa == 0 && confirmRandom ==0){
          numCycles = map(potValueMQTT, 0, 59, 1, 10);
          MostrarTiempo(numCycles, 0);
      }else if (pansa == 1 && confirmRandom ==0){
          restMinutes = map(potValueMQTT, 0, 59, 1, 15);
          MostrarTiempo(restMinutes, 0);
      }else if (pansa == 2 && confirmRandom ==0){
          workMinutes = map(potValueMQTT, 0, 59, 10, 59);
          MostrarTiempo(workMinutes, 0);
      }
      lastPotValue = potValue;
  }



  if ((pansa == 2 && digitalRead(CHANGESTATE_BTN) == HIGH) || (pansa == 2 && confirmRandom == 1))  { //                  WIP
    configCompleted = true;
    currentCycle = 0;
    minutes = workMinutes;
    seconds = 0;
    pansa = 0;
    delay(200);
  }
  CHANGEstate = 0; 
  // && (millis() - lastDebounceTime) > debounceDelay
}

void IniciarPomodoro() {
   if (currentCycle < numCycles) {
      if (millis() - lastUpdate >= 1000) {
         lastUpdate = millis();
         seconds--;

         if (seconds < 0) {
            seconds = 59;
            minutes--;

            if (minutes < 0) {
               isWorkPeriod = !isWorkPeriod;
               minutes = isWorkPeriod ? workMinutes : restMinutes;
               seconds = 0;

               if (isWorkPeriod) currentCycle++;
               alarmaHastaBoton();
            }
         }
        if(configClockCompleted){
          MantenerHora();
        }          
         MostrarTiempo(minutes, seconds);
      }
   } else {
      cambiodemodos = 0; // Vuelve al modo de reloj una vez terminado
      currentCycle = 0;  // Reinicia ciclo
      configCompleted = false; // Reinicia configuración para el próximo Pomodoro
   }
}


void MostrarTiempo(int minutes, int seconds) {
  int timeToDisplay = (minutes * 100) + seconds;
  display.showNumberDecEx(timeToDisplay, 0b01000000, true);
}



void ConfigHora() {
  int potValue = leerPotenciometroSuavizado();
  if (digitalRead(CHANGESTATE_BTN) == HIGH || CHANGEstate == 1) {
    emitirSonidoBuzzer(50);
    modohoraOminuto++;
    if (modohoraOminuto > 1) modohoraOminuto = 0;
    
    delay(200);
    
  }
  if(MODOMQTT == false){
      if (abs(potValue - lastPotValue) > 10) {
        if (modohoraOminuto == 0) {
          minutesClock = map(potValue, 0, 1023, -5, 59);
        } else {
          hoursClock = map(potValue, 0, 1023, -2, 23);
        }
      }
  }else{
        if (modohoraOminuto == 0 && confirmRandom == 0) {
          minutesClock =map(potValueMQTT, 0, 59, 0, 59); //map(potValueMQTT, 0, 1023, -6, 59);
        } else if(modohoraOminuto == 1 && confirmRandom == 0){
          hoursClock = map(potValueMQTT, 0, 59, 0, 24);//map(potValueMQTT, 0, 1023, -3, 23);
        }
  }

  MostrarTiempo(hoursClock, minutesClock);

  if ((modohoraOminuto == 1 && digitalRead(CHANGESTATE_BTN) == HIGH) ||(CHANGEstate == 1 && confirmRandom == 1)) {
    configClockCompleted = true;
    modohoraOminuto = 0;
    delay(200);
  }
  CHANGEstate = 0;
}

void MantenerHora(){
  if (millis() - lastUpdateClock >= 60000) {
    lastUpdateClock = millis();
    minutesClock++;
    if (minutesClock  == 60) {
      minutesClock = 0;
      hoursClock ++;
      if (hoursClock == 24) hoursClock = 0;
    }
  } 
  
}

void MostrarHora(int hoursClock, int minutesClock) {
  MostrarTiempo(hoursClock, minutesClock);
}
