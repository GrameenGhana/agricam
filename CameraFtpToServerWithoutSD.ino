#include <Sleep_n0m1.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
////////////////////////////////////
#if defined(__arm__)
#include <itoa.h>
#endif
////////////////////////////////////
#define FONA_RX  14 //connect to FONA RX
#define FONA_TX  15 //connect to FONA TX
#define FONA_RST 16 //Only connected for testing and debuging purposes
#define FONA_KEY 17 //connection to FONA KEY - Not connected
#define FONA_PS 18 //connect to FONA PS
////////////////////////////////////


Sleep sleep;
unsigned long sleepTime = 1200000; //Sleep for 20 mins

const int CS1 = 4;
const int CS2 = 5;
const int CS3 = 6;
const int CS4 = 7;
bool cam = true;
uint32_t jpglen;
uint8_t vid, pid;
uint8_t temp,temp_last;
int keyTime = 2000;

String _month, _day = "";

int Years, Months, Days, Hours, Mins, Secs = 0;

unsigned long ATtimeOut = 10000;

//String image_name;

 
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA( FONA_RST); //This declaration resets the GSM everytime the arduino isturned of
//Date and time needs to be recalibrated after every reset

SoftwareSerial *fonaSerial = &fonaSS;
ArduCAM myCAM1(OV5642, CS1);
ArduCAM myCAM2(OV5642, CS2);
ArduCAM myCAM3(OV5642, CS3);
ArduCAM myCAM4(OV5642, CS4);

ArduCAM uniCAM; //Placeholder to save the reference to the last used CAM
uint8_t captureUploadImage(ArduCAM myCAM); //Declaration of CAM capture and Upload function


void setup() { 
  
  
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif

///////////////////////////////////////
  Serial.begin(115200); 
  Serial.println(F("AgriCAM Start!" ));
  //turnOnFona();
  delay(2000);

// set the SPI_CS as an output:
  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);
  pinMode(CS3, OUTPUT);
  pinMode(CS4, OUTPUT);

  //GSM Power Switch key
  pinMode(FONA_KEY,OUTPUT); 
///////////////////////////////////


// initialize SPI:
  SPI.begin();
  
  //Change to JPEG capture mode and initialize the OV2640 module
 // myCAM1.set_format(JPEG);
 // myCAM1.InitCAM();
  //myCAM1.OV5642_set_JPEG_size(OV5642_2592x1944);
  ////////////////////////////////////

  myCAM1.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM2.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM3.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  myCAM4.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  
  myCAM1.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  myCAM2.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  myCAM3.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  myCAM4.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power

//////////////////////////////////////////////////////////////////////

  //Check if the ArduCAM SPI bus is OK for all 4 cams
  //This had to be done one after the other because of battery constraints
  myCAM1.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
  
  myCAM1.set_format(JPEG);
  myCAM1.InitCAM();
  myCAM1.OV5642_set_JPEG_size(OV5642_1280x720);
  
  myCAM1.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM1.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    cam = false;
    while(1);
  }myCAM1.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  


  myCAM2.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
  myCAM2.set_format(JPEG);
  myCAM2.InitCAM();
  myCAM2.OV5642_set_JPEG_size(OV5642_1280x720);
  myCAM2.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM2.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    cam = false;
    while(1);
  }myCAM2.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power


  myCAM3.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
  myCAM3.set_format(JPEG);
  myCAM3.InitCAM();
  myCAM3.OV5642_set_JPEG_size(OV5642_1280x720);
  myCAM3.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM3.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    cam = false;
    while(1);
  }myCAM3.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power

  myCAM4.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
  myCAM4.set_format(JPEG);
  myCAM4.InitCAM();
  myCAM4.OV5642_set_JPEG_size(OV5642_1280x720);
  myCAM4.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM4.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println("SPI4 interface Error!");
    cam = false;
    while(1);
  }myCAM4.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  
/////////////////////////////////////////////////////////////////////////
  
  
  myCAM1.clear_fifo_flag();
  myCAM2.clear_fifo_flag();
  myCAM3.clear_fifo_flag();
  myCAM4.clear_fifo_flag();
  
  myCAM1.write_reg(ARDUCHIP_FRAMES, 0x00);
  myCAM2.write_reg(ARDUCHIP_FRAMES, 0x00);
  myCAM3.write_reg(ARDUCHIP_FRAMES, 0x00);
  myCAM4.write_reg(ARDUCHIP_FRAMES, 0x00);
  ////////////////////////////////////////////////////


 

  Serial.println(F("Setting GSM!" ));
  //Reset GSM, Connect to network
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
    }

    Serial.println(F("Getting Network" ));
    while(!(fona.getNetworkStatus() == 1));
  delay(2000); 


 fonaSS.println(F("AT+CCLK=\"16/09/06,10:41:45+22\""));
 delay(2000);
   //turnOffFona();
}


 
void loop()
{
  
  //turnOnFona();
  
  delay(5000);
  setupGPRS();
/////////////////////////
 syncDateTime();

 //CAM1 - connect to server, capture and upload image
  connectToServer();
  captureUploadImage(myCAM1);
  timer(5);

//CAM2 - connect to server, capture and upload image
  connectToServer();
  captureUploadImage(myCAM2);
  timer(5);

//CAM3 - connect to server, capture and upload image
  connectToServer();
  captureUploadImage(myCAM3);
  timer(5);

//CAM4 - connect to server, capture and upload image
  connectToServer();
  captureUploadImage(myCAM4);
  timer(5);

  
//////////////////////
  disconnectGPRS();

  //turnOffFona();

    Serial.println("\nSleeping "); 
    delay(100); //delay to allow serial to fully print before sleep
    
    sleep.pwrDownMode(); //set sleep mode - sets arduino in a low power state to conserve energy
    sleep.sleepDelay(sleepTime); //sleep for: sleepTime declared above
  
}





void turnOnFona()
{ 
  if(! digitalRead(FONA_PS)) { //Check if it's On already. LOW is off, HIGH is ON.
        Serial.print(F("FONA was OFF, Powering ON: "));
        digitalWrite(FONA_KEY,LOW); //pull down power set pin
        unsigned long KeyPress = millis(); 
        while(KeyPress + keyTime >= millis()) {} //wait seconds
        digitalWrite(FONA_KEY,HIGH); //pull it back up again
        delay(1000);
        fonaSerial->begin(4800);
        
  }     
  else {
        Serial.println(F("FONA Already On, Did Nothing"));
    }
}

void turnOffFona()
{//does the opposite of turning the FONA ON (ie. OFF)
    if(digitalRead(FONA_PS)) { //check if FONA is OFF
        Serial.print(F("FONA was ON, Powering OFF: ")); 
        digitalWrite(FONA_KEY,LOW);
        unsigned long KeyPress = millis();
        while(KeyPress + keyTime >= millis()) {}
        digitalWrite(FONA_KEY,HIGH);
        Serial.println(F("FONA is Powered Down"));
    } else {
        Serial.println(F("FONA is already off, did nothing."));
    }
}



void setupGPRS()
{
  fonaSS.println(F("AT+CSQ")); // Signal quality check
  delay(100); 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
  
  fonaSS.println(F("AT+CGATT?")); //Attach or Detach from GPRS Support
  delay(100); 
  ShowSerialData();
  
  fonaSS.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));//setting the SAPBR, the connection type is using gprs
  delay(1000);
  ShowSerialData();
 
  fonaSS.println(F("AT+SAPBR=3,1,\"APN\",\"internet\""));//setting the APN, Access point name string
  delay(3000); 
  ShowSerialData();
 
  fonaSS.println(F("AT+SAPBR=1,1"));//setting the SAPBR
  delay(2000);
  ShowSerialData();

  fonaSS.println(F("AT+SAPBR =2,1")); //Querry GPRS for IP
  delay(2000);
  ShowSerialData();
////////////////////////////////////////////
}




void syncDateTime(){
  
   
   //Setting the time
   fonaSS.println("AT+CCLK?");
   
    while (fonaSS.available() != 0){
      if (!(fonaSS.available() != 0)) break;     
      }
           
     Years = fonaSS.parseInt();
     Months = fonaSS.parseInt();
     Days = fonaSS.parseInt();
     //Hours = fonaSS.parseInt();
     //Mins = fonaSS.parseInt();
     //Secs = fonaSS.parseInt();
     
      if (Months <= 9 ){
      _month = "0" + String(Months);
      
        }else _month = String(Months);

       if (Days <= 9 ){
      _day = "0" + String(Days);
      
        }else _day = String(Days);       
}




void connectToServer(){
  char str[8]; 
  static int k = 5;
  k = k + 1;
  itoa(k, str, 10);
  //strcat(str, ".jpg"); 
     

String image_name = String(Years) + "_" + _month + "_" + _day + "_" + str;
  
   
  fonaSS.println(F("AT+FTPCID=1"));
  delay(2000);
  ShowSerialData();

 fonaSS.println(F("AT+FTPSERV=\"188.166.30.140\""));
  delay(2000);
  ShowSerialData();

  fonaSS.println(F("AT+FTPUN=\"ftp\""));
  delay(2000);
  ShowSerialData();


  fonaSS.println(F("AT+FTPPW=\"ftp@ftp.com\""));
  delay(2000);
  ShowSerialData();

 
  fonaSS.println("AT+FTPPUTNAME=\"CAM1-20" + image_name + ".jpg\" ");
  delay(2000);
  ShowSerialData();

  fonaSS.println(F("AT+FTPPUTPATH=\"/agricam/\""));
  delay(2000);
  ShowSerialData();

  fonaSS.println(F("AT+FTPPUT=1"));
  delay(5000);
  ShowSerialData();
  }


 
uint8_t captureUploadImage(ArduCAM myCAM)
{ uniCAM = myCAM;

  myCAM.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK); //Power up Camera
  myCAM.flush_fifo();
  delay(1000);
  //Clear the capture done flag
  myCAM.clear_fifo_flag();
  //Start capture
  myCAM.start_capture();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK) && cam);
  myCAM.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  Serial.println(F("CAM Low power enabled"));
  if (cam == true)
  {
    Serial.println("CAM Capture Done!");
    }

  
////////////////////////////////////////////////////
  jpglen = myCAM.read_fifo_length(); //Read length/size of image
  Serial.print(F("Storing "));
  Serial.print(jpglen, DEC);
  Serial.print(F(" byte image.\n"));
  temp = 0;
  delay(1000);

     //While Imagesize is > 1000, tell ftp we want to upload per 1000 bytes.
      while(jpglen >= 1000){
          int answer = sendATcommand("AT+FTPPUT=2,1000","+FTPPUT:2,1000",3000);
          if (answer != 1){
          for(int i = 0; i < 1000; i++){
            temp_last = temp;
            temp = myCAM.read_fifo();
            fonaSS.write(temp);
          }
          jpglen -= 1000;              
      
      }
      else{
        captureUploadImage(uniCAM);
       }
} 
        String ScomA = "";
        String ScomB = "";
     
        ScomA.concat("AT+FTPPUT=2,");
        ScomA.concat(jpglen); 
        ScomA.concat("\"");

        ScomB.concat("+FTPPUT:2,");
        ScomB.concat(jpglen);
        ScomB.concat("\"");

        char CcomA[ScomA.length()], CcomB[ScomB.length()];

        ScomA.toCharArray(CcomA,ScomA.length());
        ScomB.toCharArray(CcomB,ScomB.length());

        //What's left of Imagesize is < 1000, tell FTP we want to upload remaining bytes
        sendATcommand(CcomA,CcomB,3000);
        for(int i = 0; i < jpglen; i++){ //Upload remaining bytes to ftp
          temp_last = temp;
            temp = myCAM.read_fifo();
            fonaSS.write(temp);
        }
    //Close FTP connection
    sendATcommand("AT+FTPPUT=2,0", "OK", 5000);

    //fonaSS.println(F("AT+FTPPUT=2,0"));
    //delay(2000);
    //ShowSerialData();

    
     
    //Clear the capture done flag
    myCAM.clear_fifo_flag();   
}



void ShowSerialData()
{
  while(fonaSS.available()!= 0)
    Serial.write(char (fonaSS.read()));
}



int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){
    
    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);
    fonaSS.println(ATcommand);    // Send the AT command 
       x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(fonaSS.available() != 0){    
            // if there are data in the UART input buffer, reads it and checks for the asnwer
            response[x] = fonaSS.read();
            Serial.print(response[x]);
            x++;
            
            // check if the desired answer  is in the response of the module
            if (strstr(response, expected_answer) != NULL)    
            {
                
                answer = 1;
                
            }
        }
    }
    // Waits for the asnwer with time out
    while((answer == 0) && ((millis() - previous) < timeout));    
        
        return answer;
}

void timer (int t){
    for (int i = t; i > 0; i--){
  delay(1000);
  } 
    }

void disconnectGPRS(){
  sendATcommand("AT+SAPBR=0,1", "OK", 3000);
  }



