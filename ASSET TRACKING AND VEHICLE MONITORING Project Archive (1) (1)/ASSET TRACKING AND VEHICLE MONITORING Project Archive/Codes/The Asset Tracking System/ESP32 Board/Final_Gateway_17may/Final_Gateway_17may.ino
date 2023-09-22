/**
 *Final work for Gateway
 *Updated on 17/05/22
 *Shubham saurav
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
//#include "my_GSM.h"
#include "BLEDevice.h"
//#include "BLEScan.h"
#include <ArduinoJson.h>
//#include <SoftwareSerial.h>


// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

int lock_status=1;

//SoftwareSerial mySerial(16,17);//ESP(Rx,Tx) |||  ESP(Rx,Tx)<==>GSM(Tx,Rx)
//SoftwareSerial mySerial(3,1);//ESP(Rx,Tx) |||  ESP(Rx,Tx)<==>GSM(Tx,Rx)

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks


void setup() {
  Serial.begin(19200);
  //mySerial.begin(19200);
  Serial2.begin(19200, SERIAL_8N1, 16, 17);//9600, SERIAL_8N1, RXD2, TXD2
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);  // 1349/0.625 = 2158 ms
  pBLEScan->setWindow(449);     // 449/0.625 = 720ms
  pBLEScan->setActiveScan(true); 
  pBLEScan->start(60, false);   // 60 sec duration

  //  *window*-*interval*-*window*-*interval*-*window*-*interval*
  //  *----------------duration--------------------------------*

while( !wifi_connect());
/*
  Serial.println("Connecting to ");
  Serial.print(ssid);
  Serial.print(" with password ");
  Serial.println(password);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
*/
} // End of setup.


void POSTData()
{
        Serial.println("Posting Status...");

 
      if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
   
      
   if(connected == true)
  //  if(myconnect==true)
    http.begin(endpoint_url + "update_safe");
   // servername = endpoint_url + "update_safe";
    else
     http.begin(endpoint_url + "update_unsafe");
   // servername = endpoint_url + "update_unsafe";
    
      //http.begin(servername);
      http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("bid_qr_007");
 
      Serial.println(httpResponseCode);
      }
            else
     while( !wifi_connect());

       Serial.println("\nDone.");
}




/*Lock function*/
void get_lock_Data()
{
        Serial.println("Lock Status being received...");

 
      if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
   
      
    http.begin(endpoint_url + "get_Lock_Data");
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST("bid_qr_007");
    Serial.print("Your lock http response code =" + String(httpResponseCode));
    
      Serial.println("Lock Status:");
      
      String payload = http.getString();
      Serial.print(payload);
      
      if(payload == "\"1\"")
      lock_status = 1;
      else if(payload == "\"0\"")
      lock_status = 0;
      else
      lock_status = 2;

      Serial.println("Lock Status2:");
      Serial.print(lock_status);
      }

       Serial.println("\nDone.");
}


/*Function will fetch all the pending OTPs, send one by one and remove the document related to that OTP from database*/
void perform_OTP_task()
{
    
    if(WiFi.status()== WL_CONNECTED){
      Serial.println("Pending OTP data being received...");
    
            HTTPClient http;
            http.begin(endpoint_url + "get_OTP");
            http.addHeader("Content-Type", "application/json");
            int httpResponseCode = http.POST("Dummy_payload");
            int check = 3;
            while((--check)>0 && httpResponseCode!=200)
              httpResponseCode = http.POST("Dummy_payload");
            Serial.print("Your http response code =" + String(httpResponseCode));
            String payload = http.getString();
            Serial.print(payload);
            
            Serial.println("Parsing start: ");
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);
            JsonObject obj = doc.as<JsonObject>();
            
            if(obj["Mob"] != "Not_found")
            {
                  String Mobile_number = obj["Mob"];
                  String new_OTP = obj["OTP"];
                  Serial.println("Your MOB is :" + Mobile_number);
                  Serial.println("Your OTP is :" + new_OTP);
                  
                  //call send message(Mobile_number,new_OTP)
                // SendMessage(Mobile_number,new_OTP);
                  Serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
                  delay(1000);  // Delay of 1000 milli seconds or 1 second                
                  Serial2.println("AT+CMGS=\"+91"+Mobile_number+"\"\r");
                  delay(1000);
                  Serial2.println("Your One Time Password(OTP) for current Travel is:" + new_OTP);// The SMS text you want to send
                  Serial.println("Message sent from GSM Module");
                  delay(100);
                  Serial2.println((char)26);// ASCII code of CTRL+Z
                  delay(100);

                  //call delete otp_doc fun(Mobile_number,new_OTP)
                  Serial.println("OTP Sent. Removing doc from Server");
                  http.begin(endpoint_url + "delete_OTP");
                  httpResponseCode = http.POST(Mobile_number);
                 check=5;
                 while(httpResponseCode!=200 && --check>0)
                    httpResponseCode = http.POST(Mobile_number);
            }
            else
                  Serial.println("No pending OTP found");
            
      }

      
    
    
    
    Serial.println("\n Get OTP Done.");
    delay(500);
}
bool wifi_connect()
{
  Serial.println("Connecting to ");
  Serial.print(ssid);
  Serial.print(" with password ");
  Serial.println(password);

  WiFi.begin(ssid, password);
  int check = 100;
  while(WiFi.status() != WL_CONNECTED && (check--)>0) {
    delay(500);
    Serial.print(".");
  }
  if(WiFi.status() == WL_CONNECTED){
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  return true;
  }
  else
  return false;
  
}




// This is the Arduino main loop function.
void loop() {


 POSTData();
 get_lock_Data();
// perform_OTP_task();
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = String(lock_status);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
            //this start 0 must be modified  > 0sec duration
  }

  
  delay(500); // Delay a second between loops.
} // End of loop
