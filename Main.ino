


void setup() {
  Serial.begin(9600);
  ZonaDeRiego zona1(A0, 8, 300, 700);
  Serial.println("=== Sistema de Riego Inteligente ===");
  Serial.println("1. Ver estado de zonas");
  Serial.println("2. Configurar humedad");
  Serial.println("Ingrese una opción:");
}

void loop() {
  if (Serial.available()) {
    char opcion = Serial.read();

    switch (opcion) {
      case '1':
        Serial.println("Zona 1: Humedad actual: 450");
        Serial.println("Zona 2: Humedad actual: 600");
        break;

      case '2':
        Serial.println("Ingrese nueva humedad mínima:");
        int minima = Serial.read();
        Serial.println("Ingrese nueva humedad Máxima:");
        int maxima = Serial.read();

        break;

      default:
        Serial.println("Opción inválida");
        break;
    }

    delay(1000); // Para evitar lectura repetida
  }
}