#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PubSubClient.h>

#define OLED_SDA 5
#define OLED_SCL 4
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BUTTON 0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid = "Redmi Wifi"; // Ganti dengan SSID Wi-Fi Anda
const char* password = "indonesia"; // Ganti dengan password Wi-Fi Anda
const char* mqttServer = "36.92.168.180"; // Alamat broker MQTT
const int mqttPort = 7483; // Port broker MQTT
const char* test_url = "http://www.google.com"; // Url tes Ping
const char* subscribeTopic = "/sensor/suhu/subscribe"; // Topik subscribe

unsigned long count = 0;
int suhu = 0;
int error = 0;
bool internet;
bool tombol;
bool fitur;
String message;
String status;

unsigned long buttonPressStart = 0;  // Waktu ketika tombol mulai ditekan
bool buttonPressed = false;          // Status tombol (apakah ditekan atau tidak)
bool readSuhuEnabled = false;        // Status fitur baca suhu
unsigned long previousMillis = 0;  
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0; 

// Fungsi callback untuk menangani pesan yang diterima dari broker MQTT
  void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima dari topic: ");
  Serial.println(topic);
  message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i]; // Membuat pesan dari payload
  }
  Serial.print("Pesan: ");
  Serial.println(message);
}

void setup() {
  Serial.begin(115200);
  
  //Setting MQTT & WIFI
  connectToWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // Set callback untuk menangani pesan dari MQTT
  
  //Inisiasi OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  pinMode(BUTTON, INPUT);
  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Jangan lanjutkan, loop selamanya
  }
}

void loop() {
  //Memeriksa koneksi Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
    reconnectMQTT();
  }

  //Memeriksa koneksi MQTT
  if (!client.connected() && internet==1) {
    reconnectMQTT();
  }
  client.loop(); // Memanggil fungsi ini untuk menangani komunikasi MQTT

  //Mengirim data Suhu & Counter tiap 1 detik
  unsigned long currentMillis = millis();    
  if (currentMillis - previousMillis >= 1000) {
    if(fitur==1){
      suhu = temperatureRead();
      status = "ON";
    } else if(fitur==0){
      suhu = 0;
      status = "OFF";
    }
    count++;
    if (internet==1) {
      String payload = "{\"suhu\": " + String(suhu) + ", \"count\": " + String(count) + "}";
      client.publish("/sensor/suhu/test", payload.c_str());
    } 
    previousMillis = currentMillis;  
  }

  //Memeriksa koneksi internet tiap 15 detik
  unsigned long currentMillis2 = millis(); 
  if (currentMillis - previousMillis2 >= 17000){
    if (cekInternet()) {
      Serial.println("ESP32 terhubung ke internet!");
      internet = 1;
    } else {
      Serial.println("Tidak ada koneksi internet.");
      internet = 0;
    }
    previousMillis2 = currentMillis2; 
  }

  //Update tampilan OLED tiap 10 detik
  if (currentMillis - previousMillis3 >= 10) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("Sensor");
    display.setCursor(0, 5);
    display.print("Suhu");
    display.setCursor(90, 5);
    display.print("Count");
    display.setCursor(0, 20);
    display.print(suhu);
    display.setCursor(40, 20);
    display.print(status);
    display.setCursor(90, 20);
    display.print(count);
    display.setCursor(0, 30);
    display.print("Error code : ");
    display.setCursor(90, 30);
    display.print(error);
    display.setCursor(0, 40);
    display.print("Status Tombol : ");
    display.setCursor(90, 40);
    display.print(tombol);
    display.setCursor(0, 50);
    display.print(message);
    display.display();
    previousMillis3 = currentMillis;  
  }

  //Update status tombol & Error code
  GPIO0();
  ERROR();
}

// Menghubungkan ke Wi-Fi
void connectToWiFi() {
  Serial.println("Menghubungkan ke Wi-Fi...");
  WiFi.begin(ssid, password);
}

// Menghubungkan ke broker MQTT
void reconnectMQTT() {
  Serial.println("Mencoba menghubungkan ke MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println(" terhubung!");
      if(internet==1){
        client.subscribe(subscribeTopic);
      }
    } else {
      Serial.println("MQTT GAGAL");
    }
}    

// Memeriksa koneksi internet
bool cekInternet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(1000);
    http.begin(test_url);  // Memulai koneksi ke URL
    int httpCode = http.GET();  // Mengirim permintaan HTTP GET
    if (httpCode > 0) {
      Serial.printf("Kode HTTP: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        http.end();  // Menutup koneksi
        return true;  // Terhubung ke internet
      }
    } else {
      Serial.printf("Permintaan gagal, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();  // Menutup koneksi
  } else {
    Serial.println("Tidak terhubung ke WiFi.");
  }  
  return false;  // Tidak ada koneksi internet
}
