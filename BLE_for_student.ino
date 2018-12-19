#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
//#include <ESP32Servo.h>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;
int servoPin = 26;

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50110319006" //請在E50之後輸入自己的學號
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50110319006"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50110319006"

//Servo mydoor;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      Serial.println("I'm ready!");
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      Serial.println("bye !");
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        if (rxValue.find(/*你的字元*/) != -1) { 
          Serial.print("Turning ON!");
        }
        Serial.println();
      }
    }
};

void setup() {
  Serial.begin(115200);

  //Initial Servo
  mydoor.setPeriodHertz(100);    // standard 50 hz servo
  mydoor.attach(servoPin, 1000, 2000);
  mydoor.write(0);
  // Create the BLE Device
    BLEDevice::init("ESP32 Door bell"); // 裝置名字在這裡喔!

  // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}
void loop() {
    if (deviceConnected) {
    //signal
    //action
        Serial.println("Ring!");
        pCharacteristic->setValue("Ring!"); //設定內容
        pCharacteristic->notify();          //推送出去
        delay(20);
  }
}
