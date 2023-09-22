/**************************************************************************************************************************************************************
 * This header file contains the objects for the http requests to MongoDB cloud
 **************************************************************************************************************************************************************/

/**************************************************************************************************************************************************************
 * Required Header Files 
 *************************************************************************************************************************************************************/
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"


/**************************************************************************************************************************************************************
 * Task: Connects to the WIFI SSID mentioned in header file named "secrets"
 * Return: True if connected successfully, otherwise False
 *************************************************************************************************************************************************************/
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

/**************************************************************************************************************************************************************
 * Task: Makes a post request to the MongoDB endpoint URL ending with endpoint_str passed as an argument. The payload is sent as body in the request.
 * Ex: 
 *  For making a request to endpoint:  "https://us-east-1.aws.data.mongodb-api.com/app/10minexample-fzdpe/endpoint/post" and sending text "hello", 
 *  we have to call function as :
 *          String str1 = "hello", str2 = "post";
 *          POST_Data(str1,str2);
 *************************************************************************************************************************************************************/

void POST_Data(String payload, String endp_str)
{
      Serial.println("Posting data for " + endp_str + "...." );
      
      if(WiFi.status()== WL_CONNECTED){
            HTTPClient http;
            String url = endpoint_url + endp_str ;                    // Complete URL is concatenation of common URL to app and specific endpoint name
            Serial.print("URL: "); Serial.println(url);
            http.begin(url);
            http.addHeader("Content-Type", "application/json");
           
            int httpResponseCode = http.POST(payload);              // Response Code = 200 for Success
                                                                    // Response Code = -1 => Invalid attempt!!,  check internet_connection/ Endpoint_URL
                                                                    
            Serial.print("HTTP request performed. Response code for  " + endp_str +" is :");
            Serial.println(httpResponseCode);   
             http.end();
      }
      else
            Serial.println("XXXXXXXXXXX  WIFI not connected  XXXXXXXXXX");       
}
