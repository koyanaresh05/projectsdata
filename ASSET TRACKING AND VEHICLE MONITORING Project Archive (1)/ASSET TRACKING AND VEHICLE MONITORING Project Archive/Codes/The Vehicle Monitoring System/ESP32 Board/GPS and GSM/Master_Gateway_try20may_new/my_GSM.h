


 /*
https://microcontrollerslab.com/gsm-module-interfacing-arduino-send-receive-sms/
*/
/*Function will fetch all the pending OTPs, send one by one and remove the document related to that OTP from database*/
void Send_loc_SMS(String Mobile_number, String loc_String)
{
    
    
           
                // SendMessage(Mobile_number,new_OTP);
                  Serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
                  delay(1000);  // Delay of 1000 milli seconds or 1 second                
                  Serial2.println("AT+CMGS=\"+91"+Mobile_number+"\"\r");
                  delay(1000);
                  Serial2.println("Your Assets last connected location for current Travel is:" + loc_String);// The SMS text you want to send
                  Serial.println("Message sent from GSM Module");
                  delay(100);
                  Serial2.println((char)26);// ASCII code of CTRL+Z
                  delay(100);
    
    Serial.println("\n Perform location SMS task Done");
    delay(500);
}

void gsm_setup(int baud_rate)

{
   Serial2.begin(baud_rate, SERIAL_8N1, 16, 17);//9600, SERIAL_8N1, RXD2, TXD2 for GSM
  //Serial.begin(19200);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
}

 void SendMessage(String mobile_no,String OTP)
{
/*There will be one while loop which will keep tracking if there are OTP to be sent and it will run till num_OTP b/m zero
 * Next Each time findOne will run in MongoDB and fetch OTP and Mob no
 * Send Message will be called in same loop after getting mob and OTP

*/
  Serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second

  //Serial2.println("AT+CMGS=\"+916261613772\"\r"); // Replace x with mobile number
  //Serial2.println("AT+CMGS=\""+mobile_no+"\"\r");
  Serial2.println("AT+CMGS=\"+91"+mobile_no+"\"\r");
  delay(1000);
  Serial2.println("Your One Time Password(OTP) for current Travel is:" + OTP);// The SMS text you want to send
  Serial.println("Message sent from GSM Module");
  delay(100);
  Serial2.println((char)26);// ASCII code of CTRL+Z
  delay(100);

}
