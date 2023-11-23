#include <DHT.h>
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <time.h>

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

// define led
#define LED_2 2
#define LED_RAIN 4

// define motion sensor
#define MOTION_SENSOR 13

// define rain sensor
#define RAIN_SENSOR 15

// define key firebase
#define MainKey "MainKey/"
#define KeyTemperature "KeyTemperature"
#define KeyHumidity "KeyHumidity"
#define KeyLed "KeyLed"
#define KeyMotion "KeyMotion"
#define KeyRain "KeyRain"
#define KeyLedRain "KeyLedRain"
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi();
  setUpFirebase();
  pinLed();
  pinMotionSensor();
  pinRainSensor();
}

void loop() {
  // put your main code here, to run repeatedly:
  readDataFromDHT11();
  getStatusOfListLed();
  readMotionSensor();
  readRainSensor();
  delay(2000);
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
    // send humidity
    sendData(humidity, KeyHumidity);
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

void getStatusOfListLed() {
  String mainKey = getMainKey(KeyLed);
  getStatusOfLed(mainKey, LED_2);
}

void getStatusOfLed(String mainKey, int id) {
  String key = mainKey + "/" + String(id);
  if (Firebase.getJSON(firebaseData, key)) {
    // Success, then try to read the JSON payload value
    FirebaseJson &json = firebaseData.jsonObject();
    FirebaseJsonData data;
    json.get(data, "/status");
    bool status = false;
    if (data.success) {
      if (data.typeNum == FirebaseJson::JSON_BOOL) {
        status = data.boolValue;
      }
    }
    Serial.println("id: " + String(id) + " status: " + String(status));
    if (status == true) {
      digitalWrite(id, HIGH);
    } else {
      digitalWrite(id, LOW);
    }
  } else {
    // Failed to get JSON data at defined database path, print out the error reason
    Serial.println(firebaseData.errorReason());
  }
}

void sendData(float data, String key) {
  String mainKey = getMainKey(key);
  if (Firebase.setFloat(firebaseData, mainKey, data)) {
    Serial.println("send " + key + " successful");
  } else {
    Serial.println("send " + key + " failure");
  }
}

void sendData(bool data, String key) {
  String mainKey = getMainKey(key);
  if (Firebase.setBool(firebaseData, mainKey, data)) {
    Serial.println("send " + key + " successful");
  } else {
    Serial.println("send " + key + " failure");
  }
}

String getMainKey(String key) {
  String mainKey = MainKey + key;
  return mainKey;
}

void pinLed() {
  pinMode(LED_2, OUTPUT);
  pinMode(LED_RAIN, OUTPUT);
}

void pinMotionSensor() {
  pinMode(MOTION_SENSOR, INPUT);
}

void readMotionSensor() {
  int statusMotion = digitalRead(MOTION_SENSOR);
  if (statusMotion == HIGH) {
    Serial.println("Detector motion");
    // send motion
    sendData(true, KeyMotion);
  } else {
    Serial.println("No Detector motion");
    // send motion
    sendData(false, KeyMotion);
  }
}

void pinRainSensor() {
  pinMode(RAIN_SENSOR, INPUT);
}

void readRainSensor() {
  if (digitalRead(RAIN_SENSOR) == LOW) {
    sendData(true, KeyRain);
    Serial.println("Detector rain");
    digitalWrite(LED_RAIN, HIGH);
  } else {
    sendData(false, KeyRain);
    Serial.println("No detector rain");
    bool statusLedRain = false;
    Firebase.getBool(firebaseData, getMainKey(KeyLedRain), &statusLedRain);
    Serial.println("Status Led Rain: " + String(statusLedRain));
    if (statusLedRain == false) {
      digitalWrite(LED_RAIN, LOW);
    } else {
      digitalWrite(LED_RAIN, HIGH);
    }
  }
}
