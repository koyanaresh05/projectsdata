


 /*
https://microcontrollerslab.com/gsm-module-interfacing-arduino-send-receive-sms/
*/
#include <SoftwareSerial.h>

SoftwareSerial mySerial(16,17);//ESP(Rx,Tx) |||  ESP(Rx,Tx)<==>GSM(Tx,Rx)

void gsm_setup(int baud_rate)

{
  mySerial.begin(baud_rate);   // Setting the baud rate of GSM Module 
  //Serial.begin(19200);    // Setting the baud rate of Serial Monitor (Arduino)
  delay(100);
}

 void SendMessage(String mobile_no,String OTP)
{
/*There will be one while loop which will keep tracking if there are OTP to be sent and it will run till num_OTP b/m zero
 * Next Each time findOne will run in MongoDB and fetch OTP and Mob no
 * Send Message will be called in same loop after getting mob and OTP

*/
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode

  delay(1000);  // Delay of 1000 milli seconds or 1 second

  //mySerial.println("AT+CMGS=\"+916261613772\"\r"); // Replace x with mobile number
  //mySerial.println("AT+CMGS=\""+mobile_no+"\"\r");
  mySerial.println("AT+CMGS=\"+91"+mobile_no+"\"\r");
  delay(1000);

  mySerial.println("Your One Time Password(OTP) for current Travel is:" + OTP);// The SMS text you want to send
  Serial.println("Message sent from GSM Module");
  delay(100);

  mySerial.println((char)26);// ASCII code of CTRL+Z

  delay(100);

}
