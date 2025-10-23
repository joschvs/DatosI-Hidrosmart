#ifndef RAINGUARD_H
#define RAINGUARD_H

#include <Arduino.h>
#include <WiFiS3.h>
#include <ArduinoJson.h>

class RainGuard {
  float   _lat;
  float   _lon;
  uint8_t _threshold;
  bool    _cachedDecision;
  uint8_t _cachedProb;
  uint16_t _lastY; uint8_t _lastM; uint8_t _lastD;

  static bool waitHeaders(WiFiSSLClient& c, uint32_t timeoutMs) {
    uint32_t t0 = millis(); String line;
    while (millis() - t0 < timeoutMs) {
      while (c.available()) {
        char ch = c.read(); line += ch;
        if (line.endsWith("\r\n\r\n")) return true;
      }
    }
    return false;
  }

  // FIX: Limpia los tamaños de chunk (hex) y deja solo el JSON
  static String readBody(WiFiSSLClient& c, uint32_t timeoutMs) {
    uint32_t t0 = millis();
    String out;
    String line;
    while (millis() - t0 < timeoutMs) {
      while (c.available()) {
        char ch = c.read();
        if (ch == '\n') {
          line.trim();
          // Si la línea es un número hexadecimal, ignorarla
          if (line.length() > 0 && strspn(line.c_str(), "0123456789abcdefABCDEF") == line.length()) {
            line = "";
            continue;
          }
          out += line + "\n";
          line = "";
        } else if (ch != '\r') {
          line += ch;
        }
        t0 = millis();
      }
    }
    return out;
  }

  static void parseISO(const char* iso, uint16_t& y, uint8_t& m, uint8_t& d) {
    if (!iso || strlen(iso) < 10) { y=0; m=0; d=0; return; }
    y = (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0')*10 + (iso[3]-'0');
    m = (iso[5]-'0')*10 + (iso[6]-'0');
    d = (iso[8]-'0')*10 + (iso[9]-'0');
  }

public:
  RainGuard(float lat, float lon, uint8_t thresholdPct = 70)
  : _lat(lat), _lon(lon), _threshold(thresholdPct),
    _cachedDecision(false), _cachedProb(0), _lastY(0), _lastM(0), _lastD(0) {}

  void setLocation(float lat, float lon) { _lat = lat; _lon = lon; }
  void setThreshold(uint8_t pct) { _threshold = pct; }
  uint8_t cachedProbability() const { return _cachedProb; }
  bool shouldBlockToday() const { return _cachedDecision; }

  bool refresh() {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("RainGuard: WiFi no conectado.");
      return false;
    }

    WiFiSSLClient client;
    if (!client.connect("api.open-meteo.com", 443)) {
      Serial.println("RainGuard: No se pudo conectar al servidor.");
      return false;
    }

    String url = String("/v1/forecast?latitude=") + String(_lat, 4) +
                 "&longitude=" + String(_lon, 4) +
                 "&daily=precipitation_probability_max&timezone=auto";

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: api.open-meteo.com\r\n" +
                 "User-Agent: Arduino/1.0\r\n" +
                 "Connection: close\r\n\r\n");

    if (!waitHeaders(client, 6000)) {
      Serial.println("RainGuard: Timeout esperando headers.");
      client.stop();
      return false;
    }

    String body = readBody(client, 8000);
    client.stop();

    if (body.length() == 0) {
      Serial.println("RainGuard: Respuesta vacía.");
      return false;
    }

    Serial.println("RainGuard: Respuesta recibida:");
    Serial.println(body);

    StaticJsonDocument<4096> doc;
    auto err = deserializeJson(doc, body);
    if (err) {
      Serial.print("RainGuard: Error al parsear JSON: ");
      Serial.println(err.c_str());
      return false;
    }

    JsonObject daily = doc["daily"];
    if (daily.isNull()) {
      Serial.println("RainGuard: Campo 'daily' no encontrado.");
      return false;
    }

    JsonArray timeArr = daily["time"].as<JsonArray>();
    JsonArray probArr = daily["precipitation_probability_max"].as<JsonArray>();
    if (timeArr.isNull() || probArr.isNull() || timeArr.size() == 0 || probArr.size() == 0) {
      Serial.println("RainGuard: Datos incompletos.");
      return false;
    }

    const char* iso0 = timeArr[0] | "";
    parseISO(iso0, _lastY, _lastM, _lastD);

    _cachedProb = probArr[0] | 0;
    _cachedDecision = (_cachedProb >= _threshold);

    Serial.print("RainGuard: Probabilidad de lluvia = ");
    Serial.print(_cachedProb);
    Serial.print("% → ");
    Serial.println(_cachedDecision ? "Bloquear riego" : "Permitir riego");

    return true;
  }
};

#endif