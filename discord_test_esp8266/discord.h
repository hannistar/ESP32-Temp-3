#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "WiFiClientSecureAxTLS.h"

const char ssid[] = SECRET_SSID;    // Network SSID (name)
const char pass[] = SECRET_PASS;    // Network password (use for WPA, or use as key for WEP)
const String discord_webhook = SECRET_WEBHOOK;
const String discord_tts = SECRET_TTS;
ESP8266WiFiMulti WiFiMulti;
WiFiClientSecure client;

void connectWIFI() {
  WiFiMulti.addAP(ssid, pass);
  WiFi.mode(WIFI_STA);
  client.setInsecure();
  Serial.print("[WiFi] Connecting to: ");
  Serial.println(ssid);
  // wait for WiFi connection
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.println("[WiFi] Connected");
}

void sendDiscord(String content) {
  HTTPClient https;
  Serial.println("[HTTP] Connecting to Discord...");
  Serial.println("[HTTP] Message: " + content);
  Serial.println("[HTTP] TTS: " + discord_tts);
  if (https.begin(client, discord_webhook)) {  // HTTPS
    // start connection and send HTTP header
    https.addHeader("Content-Type", "application/json");
    int httpCode = https.POST("{\"content\":\"" + content + "\",\"tts\":" + discord_tts + "}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.print("[HTTP] Status code: ");
      Serial.println(httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        String payload = https.getString();
        Serial.print("[HTTP] Response: ");
        Serial.println(payload);
      }
    } else {
      Serial.print("[HTTP] Post... failed, error: ");
      Serial.println(https.errorToString(httpCode).c_str());
    }

    https.end();
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }

  // End extra scoping block
}
