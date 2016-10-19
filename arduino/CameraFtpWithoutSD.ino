#include <Sleep_n0m1.h>
#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"
#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include <CheapStepper.h>


////////////////////////////////////
#if defined(__arm__)
#include <itoa.h>
#endif
////////////////////////////////////
#define FONA_RX  14 //connect to FONA RX
#define FONA_TX  15 //connect to FONA TX
#define FONA_RST 16 //Only connected for testing and debuging purposes

#define FONA_KEY 17 //connection to FONA KEY - Not connected until after testing and debugging
#define FONA_PS 18 //connect to FONA PS - - Not connected used to power off/on GSM
////////////////////////////////////


CheapStepper stepper = CheapStepper(2,3,4,5);
// here we declare our stepper using default pins:
// arduino pin <--> pins on ULN2003 board:
// 8 <--> IN1
// 9 <--> IN2
// 10 <--> IN3
// 11 <--> IN4
/////////////////////////


Sleep sleep;
unsigned long sleepTime = 60000; //Sleep for 2 mins

const int CS1 = 10;


bool cam = true;
boolean moveClockwise = true;
uint32_t jpglen;
uint8_t vid, pid;
uint8_t temp,temp_last;
int keyTime = 2000;

String _date, _time;
String image_name;


unsigned long ATtimeOut = 10000;

//String image_name;

 
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA fona = Adafruit_FONA( FONA_RST); //This declaration resets the GSM everytime the arduino isturned off
//Date and time needs to be recalibrated after every reset

SoftwareSerial *fonaSerial = &fonaSS;

ArduCAM myCAM1(OV5642, CS1);

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

  Serial.println(F("Setting up motor!" ));
  stepper.setRpm(6);
  stepper.setTotalSteps(4096);

  //turnOnFona();
  delay(2000);

  Serial.print(F("stepper RPM: ")); Serial.print(stepper.getRpm());
  Serial.println();

  //stepper.moveDegrees (moveClockwise, 180);

// set the SPI_CS as an output:
  pinMode(CS1, OUTPUT);
 

  //GSM Power Switch key
  pinMode(FONA_KEY,OUTPUT); 
///////////////////////////////////


// initialize SPI:
  SPI.begin();
  ////////////////////////////////////

  myCAM1.set_bit(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  
  myCAM1.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
//////////////////////////////////////////////////////////////////////

  //Check if the ArduCAM SPI bus is OK for all 4 cams
  //This had to be done one after the other because of battery constraints
  myCAM1.clear_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);
  
 
  
  myCAM1.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM1.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    cam = false;
    while(1);
  }
  
    Serial.println(F("OV5642 detected."));
  
  myCAM1.set_format(JPEG);
  Serial.println(F("Fomat set."));
  myCAM1.InitCAM();
  Serial.println(F("CAM1 Initialized."));
  myCAM1.OV5642_set_JPEG_size(OV5642_640x480);
 // Resolutions
//OV5642_320x240, OV5642_640x480, OV5642_1280x720, OV5642_1920x1080, 
//2048x1563, 2592x1944


  
  Serial.println(F("CAM1 size set."));
  
  myCAM1.set_bit(ARDUCHIP_GPIO,GPIO_PWDN_MASK);//enable low power
  
/////////////////////////////////////////////////////////////////////////
  
  
  myCAM1.clear_fifo_flag();
  myCAM1.write_reg(ARDUCHIP_FRAMES, 0x00);
  
  ////////////////////////////////////////////////////


 

  Serial.println(F("Setting GSM!" ));
  //Reset GSM, Connect to network, Check network status
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
    }

    Serial.println(F("Getting Network" ));
    while(!(fona.getNetworkStatus() == 1));
  delay(2000); 

  

  
}


 
void loop()
{ 
  //turnOnFona();
  
  delay(2000);
  setupGPRS();
  
  getSyncedTime();

  for(int i = 0; i < 4; i++){
  moveStepperMotor90(1024);
  getImageName();
  
  connectToServer();
  openPutSession();
  
  //captureUploadImage(myCAM1);
  delay(5000);

 }
 
  disconnectGPRS();

  //turnOffFona();

    Serial.println(F("\nSleeping ")); 
    delay(100); //delay to allow serial to fully print before sleep  
    sleep.pwrDownMode(); //set sleep mode - sets arduino in a low power state to conserve energy
    sleep.sleepDelay(sleepTime); //sleep for: sleepTime declared above
  moveClockwise = !moveClockwise;
  
}




void getSyncedTime(){
 
   if(fona.enableNTPTimeSync(true, F("pool.ntp.org")))
   Serial.println(F("Time Synced" ));
   
   
  }



void setupGPRS()
{
  
  fonaSS.println(F("AT+CSQ")); // Signal quality check
  delay(100); 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
 
  fonaSS.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));//setting the SAPBR, the connection type is using gprs
  delay(1000);
  ShowSerialData();
 
  fonaSS.println(F("AT+SAPBR=3,1,\"APN\",\"internet\""));//setting the APN, Access point name string
  delay(3000); 
  ShowSerialData();
 

if (!fona.enableGPRS(false))
Serial.println(F("Failed to turn off"));
delay(1000);

if(!fona.enableGPRS(true))

  while (!fona.enableGPRS(true)){
    for(int i = 0; i<6; i++){
    if(i == 5) loop();
    
    }
    };
          Serial.println(F("GPRS enabled"));

          
          
  fonaSS.println(F("AT+SAPBR =2,1")); //Querry GPRS for IP
  delay(2000);
  ShowSerialData();

////////////////////////////////////////////
}


void getImageName(){
  
  char buffer[23];
        fona.getTime(buffer, 23);  // make sure replybuffer is at least 23 bytes!
        Serial.print(F("Date/Time is ")); Serial.println(buffer);
    _date = "";
    _time ="";
    
    for(int i = 1; i<9; i++)
  _date = _date += buffer[i];

  for(int j = 10; j<15; j++)
  _time = _time += buffer[j];

  _date.replace("/", "_");
   
        
        Serial.print(F("Date is ")); Serial.print(_date); 
        Serial.print(F(" and time is ")); Serial.print(_time); 
        Serial.println(); 
        
        image_name =  _date + "_" + _time;
        
        }


void connectToServer(){



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
  }



void openPutSession(){
  
  fonaSS.println("AT+FTPPUTNAME=\"CAM2-20" + image_name + ".jpg\" ");
  delay(2000);
  ShowSerialData();

  fonaSS.println(F("AT+FTPPUTPATH=\"/agricam/\""));
  delay(2000);
  ShowSerialData();

  //fonaSS.println(F("AT+FTPPUT=1"));
  //delay(5000);
  //ShowSerialData();

if (sendATcommand("AT+FTPPUT=1", "+FTPPUT:1,1,", 6000) != 1){
  
  captureUploadImage(myCAM1);
  
  }
  }

 
uint8_t captureUploadImage(ArduCAM myCAM)
{ //uniCAM = myCAM;
  int reply;
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
    Serial.println(F("CAM Capture Done!"));
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

        do{
          reply = sendATcommand("AT+FTPPUT=2,1000","+FTPPUT:2,1000",7500);
          Serial.println(reply);
          if (reply != 1){
          for(int i = 0; i < 1000; i++){
            temp_last = temp;
            temp = myCAM.read_fifo();
            fonaSS.write(temp);
          }
          jpglen -= 1000; 
          
          }else reply = 1;
          
          
      }while (reply == 1);
      
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
        sendATcommand(CcomA,CcomB,5000);
        for(int i = 0; i < jpglen; i++){ //Upload remaining bytes to ftp
          temp_last = temp;
            temp = myCAM.read_fifo();
            fonaSS.write(temp);
        }

  //Transfer complete   
    //Close FTP connection
    sendATcommand("AT+FTPPUT=2,0", "OK", 10000);

  
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


void disconnectGPRS(){
  
 if (!fona.enableGPRS(false))
          Serial.println(F("Failed to turn off"));
          else 
          Serial.println(F("GSM off"));
  
  }


void moveStepperMotor90(int _step ){
  for (int s = 0; s < _step; s++){
    stepper.step(moveClockwise);
    int nStep = stepper.getStep();

  
   // if (nStep % _step == 0){ 
     // Serial.print("Moved 90deg at step position: "); Serial.print(nStep);
      //Serial.println();
   // }
  }
  delay(1000);
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




