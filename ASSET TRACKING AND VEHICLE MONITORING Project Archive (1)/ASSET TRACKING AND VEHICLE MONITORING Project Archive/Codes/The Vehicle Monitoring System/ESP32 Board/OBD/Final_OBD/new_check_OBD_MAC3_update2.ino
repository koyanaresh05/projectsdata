/**************************************************************************************************************************************************************
 * Name : SHUBHAM KUMAR SAURAV
 * DATE : 22/06/2022(LAST MODIFIED) 
 * 
 *************************************************************************************************************************************************************/

// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//NOTE: Don't use delay while ELM object or SerialBT is working, as delay is a blocking statement which creates problem for the Non-Blocking functions of ELM Objects
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

/**************************************************************************************************************************************************************
 * This project contains the functionality for reading OBD parameters from CAR and storing them on MongoDB cloud
 * We need to connect OBD Scanner(BT) at the port provided below the steering wheel of CAR
 * The working condition of the scanner can be checked by installing any car obd scanner app from playstore
 * We need to get the MAC Address of the Scanner by pairing our mobile app and then after successful attempt, viewing the properties
 * The Mac Address must be then updated in this program in the variable address1 or address2 (whichever used)
 * 
 * The WIFI credentials must be modified in the header file named "secrets" 
 * The program can then be uploaded in ESP32 board and checked in a real CAR
 * Engine must be turned ON before running the program
 **************************************************************************************************************************************************************/

/**************************************************************************************************************************************************************
 * Required Header Files 
 *************************************************************************************************************************************************************/
#include "BluetoothSerial.h"
#include "myELMduino.h"
#include "postdata.h"


BluetoothSerial SerialBT;    
 
//MACROS
#define ELM_PORT   SerialBT
#define DEBUG_PORT Serial

/**************************************************************************************************************************************************************
 * GLOBAL VARIABLES
 *************************************************************************************************************************************************************/

//OBD PARAMETERS
/* The data type of different parameters may be int, float or bool. 
 * The data type of variable is decided with reference to the return type of functions for each parameter
 */
float engineLoad_var, engineCoolantTemp_var, fuelPressure_var, rpm_var, intakeAirTemp_var, mafRate_var, throttle_var, fuelLevel_var, absLoad_var, relativeThrottle_var, ambientAirTemp_var, absThrottlePosB_var, absThrottlePosC_var, absThrottlePosD_var, absThrottlePosE_var, absThrottlePosF_var, maxMafRate_var, relativePedalPos_var, oilTemp_var;
int32_t kph_var;
uint16_t runTime_var, distTravelWithMIL_var, distSinceCodesCleared_var, timeRunWithMIL_var, timeSinceCodesCleared_var;
uint8_t manifoldPressure_var, absBaroPressure_var;

//Variables to perform check operations
int param_num=0;
bool post_check=false;
int loop_num=0;

ELM327 myELM327;    //ELM object

//MAC ADDRESS OF OBD SCANNER
//WE HAD TWO SCANNERS, THEREFORE TWO ADDRESSES ARE WRITTEN HERE 
uint8_t address1[6]  = {0x66, 0x1E, 0x11, 0xD0, 0x5A, 0x6C};  //enter the ELM327 MAC address after the 0x
uint8_t address2[6]  = {0xAA, 0xBB, 0xCC, 0x11, 0x22, 0x33};  //enter the ELM327 MAC address after the 0x

uint32_t rpm = 0;// no use

/**************************************************************************************************************************************************************
 * When we make a request for a particular OBD parameter, we get response as :
 * 1. response header  => This should be correct for corresponding parameter, otherwise it means any previous request's output is received
 * 2. response content => Decides the value of parameter
 * 
 * Therefore we perform a check, before storing the received value from OBD response
 * Following are the expected response header for correct response check for corresponding OBD parameter:
 *************************************************************************************************************************************************************/

char IAT_res_header[5]     ={'4','1','0','F','\0'};
char MAF_res_header[5]     ={'4','1','0','C','\0'};
char Fuel_res_header[5]    ={'4','1','0','5','\0'};
char Throttle_res_header[5]={'4','1','0','C','\0'};
char RPM_res_header[5]     ={'4','1','0','C','\0'};
char Coolant_res_header[5] ={'4','1','0','5','\0'};
char OBD_param_str[][15] = {"IAT","MAF","Fuel_Level","Throttle","RPM","Coolant_Temp"};

/***********************************************************************************************************************************************************
 * This variable is used to store the values of following variables:
 * data_val_float[] = {IAT, MAF, Fuel, Throttle, RPM, Coolant_Temp}
 ************************************************************************************************************************************************************/
float data_val_float[6]; 


/**************************************************************************************************************************************************************
 *        SETUP 
 *************************************************************************************************************************************************************/
void setup()
{
#if LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  DEBUG_PORT.begin(115200);
  SerialBT.setPin("1234");    // Password for EML327 OBD Scanner
 while(!wifi_connect());
  ELM_PORT.begin("ArduHUD", true);
  
  Serial.println("new_check_OBD_MAC3");
  if (!ELM_PORT.connect(address2))
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");  //SERIAL BT Not Connected
    while(1);
  }

  if (!myELM327.begin(ELM_PORT, true, 2000))
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");    //CAN BUS Not Connected, for this connection, Engine must be turned ON
    while (1);
  }

  Serial.println("Connected to ELM327");
}

/**************************************************************************************************************************************************************
 * void print_OBD_val(float temp_var, String OBD_param)
 * temp_var contains the value received from OBD, and OBD_param contains the parameter name for which the value is recieved
 * This function prints the value received 
 * tmp_check contains the validity check information of current response by reading the resonse header 
 * loop_num contains the number of successful attempts on reading the parameter after last http post request
 * On every 10 successful attempts of response check, the values are posted as a document in database and loop_num is made 0
 *************************************************************************************************************************************************************/
void print_OBD_val(float temp_var, String OBD_param)
{

// Check request status
  if (myELM327.nb_rx_state == ELM_SUCCESS){
          // uint32_t temp = temp_var;
          String temp = "";
          temp.concat(temp_var);
          Serial.print(OBD_param);
          Serial.print(":  ");
          Serial.println(temp);
          bool tmp_check = check_OBD_val(OBD_param);
          
          // Perform Response Header check
          if(tmp_check==true){ 
                    loop_num++;
                    if((loop_num % 10) == 0)
                    post_check=true; //becomes true on every 10th loop
                    else
                    post_check=false;
                    //      char OBD_param_str[][15] = {"IAT","MAF","Fuel_Level","Throttle","RPM","Coolant_Temp"};
                    //      float data_val_float[6];
                    
                    //Update data value
                    if(OBD_param=="IAT")  data_val_float[0] = temp_var;
                    else if(OBD_param=="MAF")  data_val_float[1] = temp_var;
                    else if(OBD_param=="Fuel_Level")  data_val_float[2] = temp_var;
                    else if(OBD_param=="Throttle")  data_val_float[3] = temp_var;
                    else if(OBD_param=="RPM")  data_val_float[4] = temp_var;
                    else if(OBD_param=="Coolant_Temp")  data_val_float[5] = temp_var;
                    
                    // Perform HTTPS Post request
                    if(post_check==true){
                                String payload = set_OBD_string();
                                String endp_str = "new_post_bus";
                                if(WiFi.status() == WL_CONNECTED){
                                
                                // It is necessary to disconnect and end BT port in order to perform blocking actions like https post or delay, 
                                //otherwise non-blocking statements of BT interrupts the output and working of ESP32         
                                ELM_PORT.disconnect();
                                ELM_PORT.end();
                                Serial.println("Disconnected to ELM327");
                                delay(20);
                                POST_Data(payload, endp_str);
                                
                                //After post operation, reconnecting the Scanner
                                ELM_PORT.begin("ArduHUD", true);
                                if (!ELM_PORT.connect(address2))
                                {
                                  DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
                                  while(1);
                                }
                                
                                if (!myELM327.begin(ELM_PORT, true, 2000))
                                {
                                  Serial.println("Couldn't connect to OBD scanner - Phase 2");
                                  while (1);
                                }
                                
                                Serial.println("Reconnected to ELM327");
                                 
                          }
                        else
                                {
                                Serial.println("\n\n WIFI Disconnected!!!! ");
                                //while(!wifi_connect());
                                //POST_Data(payload, endp_str);
                                }
                      
                        }
          //   set_OBD_string();
            }

  }
  else if (myELM327.nb_rx_state != ELM_GETTING_MSG){
      myELM327.printError(); delay(100);
  }
}


//This function performs parameter's response header check
bool check_OBD_val(String OBD_param)
{
bool check = true;

    if(OBD_param=="IAT"){         
          for(int i=0; i<4; i++) 
          {
              if(IAT_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for IAT header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              }
          }
    }
    
    
    else if(OBD_param=="MAF"){         
          for(int i=0; i<4; i++) 
          {
              if(MAF_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for MAF header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              }
          }
    }
    
    else if(OBD_param=="Fuel_Level"){         
          for(int i=0; i<4; i++) 
          {
              if(Fuel_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for Fuel_level header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              };
          }
    }
    
    else if(OBD_param=="Throttle"){         
          for(int i=0; i<4; i++) 
          {
              if(Throttle_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for throttle header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              }
          }
    }
    
    else if(OBD_param=="RPM"){         
          for(int i=0; i<4; i++) 
          {
              if(RPM_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for RPM header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              }
          }
    }
    
    else if(OBD_param=="Coolant_Temp"){         
          for(int i=0; i<4; i++) 
          {
              if(Coolant_res_header[i] != myELM327.res_header[i]) 
              {
              Serial.println("Check for Coolant Temp header failed: "+ String(IAT_res_header[i]) + "!= " +String(myELM327.res_header[i]));
              check=false ;
              }
          }
    }
    
    else
    {
          Serial.println("OBD param don't match");
          check=false ;
    }

  
  return check;
}


/**********************************************************************************************************************************************************
 *                  LOOP
 *******************************************************************************************************************************************************/
void loop()
{

String OBD_param;
switch((param_num++)%6 + 1)
{    
case 1 : OBD_param = "IAT";
intakeAirTemp_var=myELM327.intakeAirTemp();  print_OBD_val(intakeAirTemp_var, OBD_param); break;

case 2: OBD_param = "RPM";
rpm_var = myELM327.rpm(); print_OBD_val(rpm_var, OBD_param);break;

case 3: OBD_param = "MAF";
mafRate_var=myELM327.mafRate();  print_OBD_val(mafRate_var, OBD_param);break;

case 4: OBD_param = "Throttle";
throttle_var=myELM327.throttle();  print_OBD_val(throttle_var, OBD_param);   break;


case 5: OBD_param = "Coolant_Temp";
engineCoolantTemp_var = myELM327.engineCoolantTemp(); print_OBD_val(engineCoolantTemp_var, OBD_param);break;

case 6: OBD_param = "Fuel_Level";
fuelLevel_var = myELM327.fuelLevel(); print_OBD_val(fuelLevel_var, OBD_param);break;

default: break;

}


    
}



/**************************************************************************************************************************************************************
 * This function creates the payload to be sent on the https request to MongoDB Cloud 
 * Ex: {"IAT":"27.15","MAF":"45.0","Fuel":"56"} This is a JSON document which will be stored in MongoDB CLuster and then used to plot real-time charts
 *************************************************************************************************************************************************************/

String set_OBD_string()
{
  String obd_data_values = "{";
      for(int i =0; i<6; i++)
      {
        obd_data_values.concat("\"");
        obd_data_values.concat(String(OBD_param_str[i]));
        obd_data_values.concat("\":\"");
        obd_data_values.concat(data_val_float[i]);
        if(i<5)
        obd_data_values.concat("\",");
        else
        obd_data_values.concat("\"}"); //in case of i==5 , no more characters , therefore end by '}'
      }

      Serial.println("\n\n");
      Serial.print("OBD String generation complete, current OBD payload is:  ");
      Serial.println("\n\n");
      Serial.println(obd_data_values);
      return obd_data_values;

}
