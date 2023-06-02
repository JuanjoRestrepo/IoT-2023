#include <WiFi.h>
#include <PubSubClient.h>

//broker.hivemq.com
//test.mosquitto.org
// broker.emqx.io

#define LED 15

// Configuración de la conexión Wi-Fi
const char* WIFI_SSID = "miwifi";  // Ajusta el nombre de tu red Wi-Fi
const char* WIFI_PASSWORD = "jaja1234";  // Ajusta la contraseña de tu red Wi-Fi

// Configuración de la conexión MQTT
const char* MQTT_BROKER = "test.mosquitto.org";  // Ajusta la dirección IP del broker MQTT
const int MQTT_PORT = 1883;  // Puerto predeterminado para MQTT
const char* MQTT_TOPIC = "huerta/sensor1";  // Ajusta el tema MQTT según tus necesidades

// Callback que se ejecuta cuando se recibe un mensaje MQTT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensaje recibido: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    // Aquí puedes agregar la lógica para controlar los relés y los sensores de tu huerta
    if ( (char)payload[0] == '1' ){
      digitalWrite(LED, HIGH);
    }else if( (char)payload[0] == '0'){
      digitalWrite(LED, LOW);
    }
}

// Configuración del cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Conexión Wi-Fi y MQTT
void conectarWiFiYMqtt() {
    delay(10);
    Serial.println();
    Serial.print("Conectando a la red Wi-Fi: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("Conectado a la red Wi-Fi");

    client.setServer(MQTT_BROKER, MQTT_PORT);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.print("Conectando al broker MQTT: ");
        Serial.println(MQTT_BROKER);
        if (client.connect("ESP32Client")) {
            Serial.println("Conectado al broker MQTT");
            client.subscribe(MQTT_TOPIC);
        } else {
            Serial.print("Error al conectar al broker MQTT. Código de error: ");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

// Tarea principal del ESP32
void setup(){
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    conectarWiFiYMqtt();
}

void loop(){
    if (!client.connected()) {
        conectarWiFiYMqtt();
    }
    client.loop();
}
