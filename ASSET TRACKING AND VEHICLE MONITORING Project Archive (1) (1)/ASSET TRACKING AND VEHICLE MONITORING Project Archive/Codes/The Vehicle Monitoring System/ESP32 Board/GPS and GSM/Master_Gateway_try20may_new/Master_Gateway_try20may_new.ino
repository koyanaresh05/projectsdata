/**
 *Final work for Gateway
 *Updated on 17/05/22
 *Shubham saurav
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
#include "my_GSM.h"
#include "myGPS.h"
#include <ArduinoJson.h>
//#include <SoftwareSerial.h>

//SoftwareSerial mySerial(16,17);//ESP(Rx,Tx) |||  ESP(Rx,Tx)<==>GSM(Tx,Rx)
//SoftwareSerial mySerial(3,1);//ESP(Rx,Tx) |||  ESP(Rx,Tx)<==>GSM(Tx,Rx)
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
void setup() {
  Serial.begin(19200);
  //mySerial.begin(19200);
  Serial2.begin(19200, SERIAL_8N1, 16, 17);//9600, SERIAL_8N1, RXD2, TXD2 for GSM
  Serial.println("Starting Arduino BLE Client application...");
  Serial1.begin(9600,SERIAL_8N1, 4,2);//Pin SD2=9,SD3=10    //GPS Init Baud, SERIAL_8N1, RXD2, TXD2
  
  while(!wifi_connect());
} // End of setup.



void update_last_loc(String loc_str)
{
        Serial.println("Posting Location String...");

 
      if(WiFi.status()== WL_CONNECTED){
          HTTPClient http;
          http.begin(endpoint_url + "update_last_loc");
          http.addHeader("Content-Type", "application/json");

/*************************************************
 * Latest Modification
 */
              String latstr = get_val_str(lat_val);
              String lngstr = get_val_str(lng_val);
              String gmap_str= prepare_gmap_string();
           //   String payload =  "{\"lattitude\":\""+latstr+"\",\"longitude"\":\""+lngstr+"\",\"id\":\""+String(loc_num) +"\"}";
    
              
    String payload = "{";
         
            payload.concat("\"");
            payload.concat("lattitude");
            payload.concat("\":\"");
            payload.concat(latstr);
            payload.concat("\",");
            
            payload.concat("\"");
            payload.concat("longitude");
            payload.concat("\":\"");
            payload.concat(lngstr);
            payload.concat("\",");
    
            payload.concat("\"");
            payload.concat("gmap_str");
            payload.concat("\":\"");
            payload.concat(gmap_str);
    
            
            payload.concat("\"}");
            int httpResponseCode = http.POST(payload);

            /*******************************************************************************/


          
          //int httpResponseCode = http.POST(loc_str);
          
          //Retry in case of failure in http request. "200" refers to "Successful"
          int check = 3;
          while((check--) >0 && httpResponseCode != 200){
            int httpResponseCode = http.POST(payload);
            //httpResponseCode = http.POST(loc_str);
          }
          if(httpResponseCode != 200)
            Serial.println("Location update Failed with Response code: "+String(httpResponseCode));
      }
      else{
        while(!wifi_connect());
        //update_last_loc(loc_str)
        Serial.println("\nDone."); 
      }
       
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
                  Serial2.println("OTP/SMS:" + new_OTP);// The SMS text you want to send
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
            
      }else{
        while(!wifi_connect());
        Serial.println("\nDone."); 
      }    
    Serial.println("\n Get OTP Done.");
    delay(500);
}

// This is the Arduino main loop function.
void loop() {
    
    
    smartDelay(1000); /* Generate precise delay of 1ms */
    get_gps_data();
    print_gps_data();
    
    String gmap_str = prepare_gmap_string();
    //Send_loc_SMS("6261613772",gmap_str);
    //Send_loc_SMS(String Mobile_number, String loc_String); added in my_GSM.h
    if (loc_valid)
    update_last_loc(gmap_str);

    
    perform_OTP_task();
    delay(500); // Delay 500 millisecond between loops.
} // End of loop
