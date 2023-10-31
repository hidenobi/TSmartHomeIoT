#include <DHT.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

// define DHT
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// define Firebase
#define API_KEY_FIREBASE "AIzaSyDVLnLiM30bqPBv925gTkXVHHuFaH7Q7Hw"
#define LINK_FIREBASE_DATABASE "https://t-smart-home-380e3-default-rtdb.asia-southeast1.firebasedatabase.app/"
// Define the Firebase Data object
FirebaseData firebaseData;
// Define the FirebaseAuth data for authentication data
FirebaseAuth auth;
// Define the FirebaseConfig data for config data
FirebaseConfig config;
#define USER_EMAIL "demoiot@hidenobi.com"
#define USER_PASSWORD "12345678"

// define wifi
#define WIFI_SSID "Al+HNO3=>Al(NO3)3+NO+N2O+N2+H2O"
#define WIFI_PASSWORD "726711113"

// define key firebase
#define KeyTemperature "KeyTemperature"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi();
  setUpFirebase();
}

void loop() {
  // put your main code here, to run repeatedly:
  readDataFromDHT11();
  delay(1000);
}
void readDataFromDHT11() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (!isnanf(humidity) && !isnanf(temperature)) {
    String temperatureDescription = "Nhiet do la: ";
    String humidityDescription = "Do am la: ";
    Serial.println(temperatureDescription + String(temperature));
    Serial.println(humidityDescription + String(humidity));
    // send temperature
    sendData(temperature, KeyTemperature);
  }
}

void connectWifi() {
  // Connect or reconnect to Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(String(WIFI_SSID));
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      Serial.print(".");
      delay(2000);
    }
    Serial.println("\nConnected.");
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
}
void setUpFirebase() {
  // Assign api key
  config.api_key = API_KEY_FIREBASE;
  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  /* Assign the RTDB URL (required) */
  config.database_url = LINK_FIREBASE_DATABASE;
  Firebase.begin(&config, &auth);
  // Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);
  Firebase.setMaxRetry(firebaseData, 3);
  Firebase.setMaxErrorQueue(firebaseData, 30);
  Firebase.enableClassicRequest(firebaseData, true);
  firebaseData.setBSSLBufferSize(4096, 1024);
  firebaseData.setResponseSize(2048);
}

void sendData(float data, String key) {
  if (Firebase.pushFloat(firebaseData, key, data)) {
    Serial.println("send data successful");
  } else {
    Serial.println("send data failure");
  }
}
