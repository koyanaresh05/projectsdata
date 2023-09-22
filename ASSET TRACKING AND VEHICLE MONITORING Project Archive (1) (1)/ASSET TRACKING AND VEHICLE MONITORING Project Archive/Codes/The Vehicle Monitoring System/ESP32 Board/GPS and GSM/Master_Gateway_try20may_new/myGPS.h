#ifndef _MY_GPS_H
#define _MY_GPS_H
#include <TinyGPS++.h>
//#include <SoftwareSerial.h>
/* Create object named bt of the class SoftwareSerial */
//SoftwareSerial GPS_SoftSerial(4, 3);/* UNO(Rx, Tx) <==>gps(Tx,Rx)*/
/* Create an object named gps of the class TinyGPSPlus */
TinyGPSPlus gps;      

volatile float minutes, seconds;
volatile int degree, secs, mins;
unsigned long start;
double lat_val, lng_val, alt_m_val;
uint8_t hr_val, min_val, sec_val;
bool loc_valid, alt_valid, time_valid;

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial1.available())  /* Encode data read from GPS while data is available on serial port */
      gps.encode(Serial1.read());
/* Encode basically is used to parse the string received by the GPS and to store it in a buffer so that information can be extracted from it */
  } while (millis() - start < ms);
}

void DegMinSec( double tot_val)   /* Convert data in decimal degrees into degrees minutes seconds form */
{  
  degree = (int)tot_val;
  minutes = tot_val - degree;
  seconds = 60 * minutes;
  minutes = (int)seconds;
  mins = (int)minutes;
  seconds = seconds - minutes;
  seconds = 60 * seconds;
  secs = (int)seconds;
}

String mylatstr(long int latf)
{
  String lat_f_str;
    if(latf<10)
  lat_f_str = "00000" + String(latf);
  else if(latf<100)
  lat_f_str = "0000" + String(latf);
  else if(latf<1000)
  lat_f_str = "000" + String(latf);
  else if(latf<10000)
  lat_f_str = "00" + String(latf);
  else if(latf<100000)
  lat_f_str = "0" + String(latf);
  else
  lat_f_str = "" + String(latf);

  return lat_f_str;
}

String prepare_gmap_string()
{
  /**************************************************************************************
  float to string conversion
  ***************************************************************************************/
  // dtostrf( [doubleVar] , [sizeBeforePoint] , [sizeAfterPoint] , [WhereToStoreIt] )
  double lat_val2, lng_val2;
 /* lat_val2=lat_val;
  lng_val2=lng_val;
  char* lat_str;
  dtostrf(lat_val2,3,6,lat_str);
  char* lng_str;
  dtostrf(lng_val2,3,6,lng_str);*/
  lat_val2 = lat_val *1000000;    lng_val2 = lng_val *1000000;
  long int lat_int,lng_int,lat_d,lat_f,lng_d,lng_f;
  lat_int = lat_val2;
  lng_int = lng_val2;
  lat_d =lat_val;
  lng_d = lng_val;
  lat_f = lat_int % 1000000;
  lng_f = lng_int % 1000000;
  String lng_f_str,lat_f_str;
  lat_f_str = mylatstr(lat_f);
  lng_f_str = mylatstr(lng_f);


  
  String lat_str = String(lat_d) + "." + lat_f_str;
  String lng_str = String(lng_d) + "." + lng_f_str;
  //Serial.print("lat_str:"+lat_str);
  String gmap_str = "https://www.google.com/maps/@" + lat_str + "," + lng_str + ",14z";

   // https://www.google.com/maps/@8.2630696,77.3022699,14z
   return gmap_str;
}


String get_val_str(double val){
  double val2;
  val2 = val *1000000;    
  long int l_int,l_d,l_f;
  l_int = val2; 
  l_d =val;
  l_f = l_int % 1000000;
  String l_f_str,val_str;
  l_f_str = mylatstr(l_f);
  val_str = String(l_d) + "." + l_f_str;
 return val_str;
}


void gps_init(int baud_rate)
{
//  GPS_SoftSerial.begin(baud_rate); /* Define baud rate for software serial communication */
    Serial1.begin(baud_rate,SERIAL_8N1, 4,2);//Pin SD2=9,SD3=10    //GPS Init Baud, SERIAL_8N1, RXD2, TXD2
  
}

void get_gps_data(){
  
        lat_val = gps.location.lat(); /* Get latitude data */
        loc_valid = gps.location.isValid(); /* Check if valid location data is available */
        lng_val = gps.location.lng(); /* Get longtitude data */
        alt_m_val = gps.altitude.meters();  /* Get altitude data in meters */
        alt_valid = gps.altitude.isValid(); /* Check if valid altitude data is available */
        hr_val = gps.time.hour(); /* Get hour */
        min_val = gps.time.minute();  /* Get minutes */
        sec_val = gps.time.second();  /* Get seconds */
        time_valid = gps.time.isValid();  /* Check if valid time data is available */
}

void print_gps_data(){
          if (!loc_valid)
        {          
          Serial.print("Latitude : ");
          Serial.println("*****");
          Serial.print("Longitude : ");
          Serial.println("*****");
        }
        else
        {
          DegMinSec(lat_val);
          Serial.print("Latitude in Decimal Degrees : ");
          Serial.println(lat_val, 6);
          Serial.print("Latitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
          DegMinSec(lng_val); /* Convert the decimal degree value into degrees minutes seconds form */
          Serial.print("Longitude in Decimal Degrees : ");
          Serial.println(lng_val, 6);
          Serial.print("Longitude in Degrees Minutes Seconds : ");
          Serial.print(degree);
          Serial.print("\t");
          Serial.print(mins);
          Serial.print("\t");
          Serial.println(secs);
        }
        if (!alt_valid)
        {
          Serial.print("Altitude : ");
          Serial.println("*****");
        }
        else
        {
          Serial.print("Altitude : ");
          Serial.println(alt_m_val, 6); // Unit: meters   
        }
        if (!time_valid)
        {
          Serial.print("Time : ");
          Serial.println("*****");
        }
        else
        {
          char time_string[32];
          sprintf(time_string, "Time : %02d/%02d/%02d \n", hr_val, min_val, sec_val);
          Serial.print(time_string);    
        }

}




#endif
