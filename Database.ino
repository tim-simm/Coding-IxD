#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "see";
const char* password = "KHB-Computerstudio";
const char* server = "lamit03.userpage.fu-berlin.de/database.php";

String survey_data;

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Verbindung zum WiFi wird hergestellt...");
    }
    Serial.println("Verbunden mit dem WiFi.");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        //Serial.println("Verbunden");
        // Server-URL für die HTTP-Anfrage
        String url = "https://" + String(server);

        http.begin(url);

        int httpCode = http.GET();
        if (httpCode > 0) {
            if (httpCode == HTTP_CODE_OK) {
                //Serial.println("Abfrage");
                String payload = http.getString();
                if (survey_data != payload) {
                survey_data = payload;
                Serial.println(survey_data);
                }
            }
        } else {
            Serial.println("Fehler beim Senden der HTTP-Anfrage.");
        }

        http.end();
    }

    delay(5000);  // Warten Sie 5 Sekunden, bevor Sie die nächste Anfrage senden
}