#include <Arduino.h>
#include <WiFi.h>
#include "Firebase_ESP_Client.h"
#define FIREBASE_HOST "https://iot2025-demo-ca19a-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "bkm9lpbHNRr8bN5t3jEdY8HOjYKDWydmKntBfgxv"
#define LED_PIN 21

// #define WIFI_SSID "CE–IOT"
// #define WIFI_PASSWORD "CE-1OT@!"

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

FirebaseData fbdo;
FirebaseConfig fbConfig;
FirebaseData fbdoStream;
void WifiConnect();
void Firebase_Init(const String &streamPath);
void onFirebaseStream(FirebaseStream data);

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    Serial.println("Booting...");
    WifiConnect();
    Serial.println("Connecting to Firebase...");
    Firebase_Init("cmd");
    Serial.println("System ready.");
}

void loop()
{
    Firebase.RTDB.setInt(&fbdo, "/data", millis());
    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    delay(1000);
}

void Firebase_Init(const String &streamPath)
{
    FirebaseAuth fbAuth;
    fbConfig.host = FIREBASE_HOST;
    fbConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&fbConfig, &fbAuth);
    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(1024);
    Firebase.RTDB.setwriteSizeLimit(&fbdo, "small");
    while (!Firebase.ready())
    {
        Serial.println("Connecting to firebase...");
        delay(1000);
    }
    String path = streamPath;
    if (Firebase.RTDB.beginStream(&fbdoStream, path.c_str()))
    {
        Serial.println("Firebase stream on " + path);
        Firebase.RTDB.setStreamCallback(&fbdoStream,
                                        onFirebaseStream, 0);
    }
    else
        Serial.println(String("Firebase stream failed: ") +
                       fbdoStream.errorReason());
}

void onFirebaseStream(FirebaseStream data)
{
 Serial.printf("onFirebaseStream: %s %s %s %s\n", data.streamPath().c_str(),
 data.dataPath().c_str(), data.dataType().c_str(),
 data.stringData().c_str());
}

const char* wifiStatusToString(wl_status_t status) {
  switch (status) {
    case WL_IDLE_STATUS:
      return "IDLE_STATUS";
    case WL_NO_SSID_AVAIL:
      return "NO_SSID_AVAILABLE";
    case WL_SCAN_COMPLETED:
      return "SCAN_COMPLETED";
    case WL_CONNECTED:
      return "CONNECTED";
    case WL_CONNECT_FAILED:
      return "CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "CONNECTION_LOST";
    case WL_DISCONNECTED:
      return "DISCONNECTED";
    default:
      return "UNKNOWN";
  }
}
void WifiConnect()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Connecting to WiFi %s ...\r\n", WIFI_SSID);
  wl_status_t status;
  do 
  {
    status =  WiFi.status();
    Serial.print("WiFi Status: ");
    Serial.print(wifiStatusToString(status));
    Serial.print(" (");
    Serial.print(status);
    Serial.println(")");
    delay(1000);
  } while (status != WL_CONNECTED);
  Serial.println("Connected to WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}