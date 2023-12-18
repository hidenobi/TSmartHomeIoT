#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <Servo.h>  // servo library
Servo servo;
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
#define MainKey "MainKey/"
#define KeyDoor "KeyDoor"
#define KeyFlame "KeyFlame"

bool lastStatus;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lastStatus = false;
  Serial.println("Begin connect wifi");
  connectWifi();
  setUpFirebase();
  servo.attach(0);
  servo.write(0);
}

void loop() {
  bool statusDoor = false;
  Firebase.getBool(firebaseData, getMainKey(KeyDoor) + "/state", &statusDoor);
  Serial.println("Status door: "+statusDoor);
  if(statusDoor==true&&lastStatus!=statusDoor){
    Serial.println("Open door");
    servo.write(180);
    lastStatus = statusDoor;
  }
  else if(statusDoor==false&&lastStatus!=statusDoor){
    Serial.println("Close door");
    servo.write(0);
    lastStatus = statusDoor;
  }
  delay(1000);
}

void connectWifi() {
  // Connect or reconnect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to WiFi");
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
String getMainKey(String key) {
  String mainKey = MainKey + key;
  return mainKey;
}

void sendData(bool data, String key) {
  String mainKey = getMainKey(key);
  if (Firebase.setBool(firebaseData, mainKey, data)) {
    Serial.println("send " + key + " successful");
  } else {
    Serial.println("send " + key + " failure");
  }
}
