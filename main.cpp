#include <Arduino.h>
#include "ZonesManager.h"
#include <TimeLib.h>
#include <WiFiS3.h>

const char* ssid = "Keisi";
const char* password = "Kmsr141206";

//ZonaManager(pinValvula, pinSensor, humedadMin, humedadMax)
ZonaManager zona1(A5, A0, 20, 100); // sensor en A5, válvula en A0
ZonaManager zona2(A4, A1, 0, 100); // sensor en A5, válvula en A1
ZonaManager zona3(A3, A2, 0, 100); // sensor en A5, válvula en A2

//Aqui tenemos que configurar la hora actual ya que no se cuenta con modulo para hacerlo automatico
const int horaInicial = 5;
const int minutoInicial = 15;
const int diaInicial = 23;
const int mesInicial = 10;
const int yearInicial = 2025;

unsigned long ultimoRiego = 0;
const unsigned long intervaloRiego = 5000;

void mostrarMenu() {
  Serial.println("=== Sistema de Riego Inteligente ===");
  Serial.println("1. Ver estado de zonas");
  Serial.println("2. Configurar Zona 1");
  Serial.println("3. Configurar Zona 2");
  Serial.println("4. Configurar Zona 3");
  Serial.println("5. Ver probabilidad de lluvia");
  Serial.println("6. Salir");
}

void setup() {
  Serial.begin(9600);
  setTime(horaInicial, minutoInicial, 0, diaInicial, mesInicial, yearInicial);
  mostrarMenu();

  Serial.print("Conectando a WiFi...");
  WiFi.begin(ssid, password);

  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 10000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nNo se pudo conectar a WiFi.");
  }
}
void loop() {
  
  //Serial.println("Loop activo"); // Verifica que el loop corre
  uint32_t diaActual = day();
  uint16_t minutosActuales = hour() * 60 + minute();
  static uint16_t lastDay = 0;
  
 // Actualizar RainGuard solo una vez al día
  if (diaActual != lastDay) {
    if (zona1.refreshRainGuard()) {
      Serial.print("RainGuard actualizado. Probabilidad: ");
      Serial.print(zona1.obtenerProbabilidadLluvia());
      Serial.println("%");
    } else {
      Serial.println("Error al actualizar RainGuard.");
    }
    lastDay = diaActual;
  }

  zona1.verificarYRegar(diaActual, minutosActuales);
  zona2.verificarYRegar(diaActual, minutosActuales);
  zona3.verificarYRegar(diaActual, minutosActuales);
  if (Serial.available()) {
    int opcion = Serial.parseInt();
    switch (opcion) {
      case 1: {
        Serial.print("Zona 1: Humedad actual: ");
        Serial.println(zona1.obtenerHumedad());
        Serial.print("Zona 2: Humedad actual: ");
        Serial.println(zona2.obtenerHumedad());
        Serial.print("Zona 3: Humedad actual: ");
        Serial.println(zona3.obtenerHumedad());
        break;
      }
      case 2:
      case 3:
      case 4: {
        ZonaManager* zona = (opcion == 2) ? &zona1 : (opcion == 3) ? &zona2 : &zona3;
        Serial.print("Configurando Zona ");
        Serial.println(opcion - 1);

        Serial.println("Ingrese humedad mínima:");
        while (!Serial.available()) {}
        int minima = Serial.parseInt();
        Serial.print("→ Humedad mínima: ");
        Serial.println(minima);

        Serial.println("Ingrese humedad máxima:");
        while (!Serial.available()) {}
        int maxima = Serial.parseInt();
        Serial.print("→ Humedad máxima: ");
        Serial.println(maxima);

        Serial.println("¿Quieres configurar el horario de riego automático? (si/no)");
        while (!Serial.available()) {}
        String condicion = Serial.readString();
        condicion.toLowerCase();
        condicion.trim();

        if (condicion == "si") {
          Serial.println("Ingrese hora de inicio:");
          while (!Serial.available()) {}
          int hIni = Serial.parseInt();
          Serial.println(hIni);

          Serial.println("Ingrese minutos de inicio:");
          while (!Serial.available()) {}
          int mIni = Serial.parseInt();
          Serial.println(mIni);

          Serial.println("Ingrese hora de fin:");
          while (!Serial.available()) {}
          int hFin = Serial.parseInt();
          Serial.println(hFin);

          Serial.println("Ingrese minutos de fin:");
          while (!Serial.available()) {}
          int mFin = Serial.parseInt();
          Serial.println(mFin);

          Serial.println("Ingrese intervalo de días (1, 2, ...7):");
          while (!Serial.available()) {}
          int intervalo = Serial.parseInt();
          Serial.println(intervalo);

          zona->configurarHorario(hIni, mIni, hFin, mFin, intervalo);
        }

        zona->configurarHumedad(minima, maxima);
        Serial.print("Zona ");
        Serial.print(opcion - 1);
        Serial.println(" configurada correctamente.");
        break;
      }
      case 5: {
       Serial.print("Probabilidad de lluvia (Zona 1): ");
        Serial.print(zona1.obtenerProbabilidadLluvia());
        Serial.println("%");
        break;
      }

      case 6: {
        Serial.println("Saliendo del sistema...");
        return;
      }

      default: {
        Serial.println("Opción inválida");
        break;
      }
    }

    delay(1000);
    mostrarMenu();
  }
}


/*


int waterPumpPin = A1; // Pin donde está conectada la bomba
int waterPumpPin2 = A0;
int waterPumpPin3 = A2;
int sensorPin = A5;
int sensorValue = 0;

void setup() {
  pinMode(waterPumpPin, OUTPUT);
  pinMode(waterPumpPin2, OUTPUT);
  pinMode(waterPumpPin3, OUTPUT);
  digitalWrite(waterPumpPin, LOW); 
  digitalWrite(waterPumpPin2, LOW); 
  digitalWrite(waterPumpPin3, LOW); 
  Serial.begin(9600); 
  analogReadResolution(12);
  Serial.println("Escribe 1 para encender la bomba o 0 para apagarla");
}

void loop() {
  sensorValue = analogRead(sensorPin);
  int humedad = map(sensorValue, 3400, 1200, 0, 100);
  humedad = constrain(humedad, 0, 100);

  if (Serial.available()) {
    int num = Serial.parseInt();

    if (num == 1) {
      digitalWrite(waterPumpPin, HIGH);
      digitalWrite(waterPumpPin2, HIGH);
      digitalWrite(waterPumpPin3, HIGH);
      Serial.println("Bomba encendida");
    } 
    else if (num == 0) {
      digitalWrite(waterPumpPin, LOW);
      digitalWrite(waterPumpPin2, LOW);
      digitalWrite(waterPumpPin3, LOW);
      Serial.println("Bomba apagada");
    }
  }

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println("%");

  delay(1000);
}
*/