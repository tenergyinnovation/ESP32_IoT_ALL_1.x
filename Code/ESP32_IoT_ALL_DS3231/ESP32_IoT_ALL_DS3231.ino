/***********************************************************************
 * Project      :     ESP32_IoT_ALL_DS3231
 * Description  :     DS3231 test            
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     6 Dec 2018
 * Revision     :     1.0
 * Rev1.0       :     Original
   website      :     http://www.tenergyinnovation.co.th
   Facebook     :     https://www.facebook.com/tenergy.innovation
   Email        :     uten.boonliam@innovation.co.th
   TEL          :     089-140-7205
***********************************************************************/

/**************************************/
/*         include library            */
/**************************************/ 
#include <DS3231.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
#include "images.h"
#include <String.h>


/**************************************/
/*           GPIO define              */
/**************************************/
#define BUTTON_SW1  33
#define BUTTON_SW2  14
#define LED         12
#define BUZZER      13
#define SLID_SW     15
#define INPUT_1     39
#define INPUT_2     34
#define INPUT_3     35
#define INPUT_4     32
#define OUTPUT_1    19
#define OUTPUT_2    18
#define OUTPUT_3    5
#define OUTPUT_4    2




/**************************************/
/*        object define               */
/**************************************/
SSD1306Wire  display(0x3c, 21, 22);  //21 = SDA, 22 = SCL
#define DEMO_DURATION 3000
#define tenergy_logo    0
#define progressive     1
DS3231 Clock;



/**************************************/
/*        define function define      */
/**************************************/
typedef void (*Demo)(void);
void drawImageDemo();
void drawProgressBarDemo();
void ds3231_getdata(void);
void ds3231_setting(void);
void buzzer_beep(int times);

/**************************************/
/*        global variable             */
/**************************************/
int demoMode = 0;
int counter = 1;
Demo demos[] = {drawImageDemo, drawProgressBarDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));

bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
char date_string[20],time_string[20],temp_string[20];


 /***********************************************************************
 * FUNCTION:    drawProgressBarDemo
 * DESCRIPTION: drawProgressBarDemo
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void drawProgressBarDemo() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 32, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 15, String(progress) + "%");
}

 /***********************************************************************
 * FUNCTION:    drawImageDemo
 * DESCRIPTION: drawImageDemo
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void drawImageDemo() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}


 /***********************************************************************
 * FUNCTION:    ds3231_getdata
 * DESCRIPTION: Get data from DS3231
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void ds3231_getdata(void){
 
      // send what's going on to the serial monitor.
      // Start with the year
      Serial.print("2");
      if (Century) {      // Won't need this for 89 years.
        Serial.print("1");
      } else {
        Serial.print("0");
      }
      Serial.print(Clock.getYear(), DEC);
      Serial.print(' ');
      // then the month
      Serial.print(Clock.getMonth(Century), DEC);
      Serial.print(' ');
      // then the date
      Serial.print(Clock.getDate(), DEC);
      Serial.print(' ');
      // and the day of the week
      Serial.print(Clock.getDoW(), DEC);
      Serial.print(' ');
      // Finally the hour, minute, and second
      Serial.print(Clock.getHour(h12, PM), DEC);
      Serial.print(' ');
      Serial.print(Clock.getMinute(), DEC);
      Serial.print(' ');
      Serial.print(Clock.getSecond(), DEC);
      // Add AM/PM indicator
      if (h12) {
        if (PM) {
          Serial.print(" PM ");
        } else {
          Serial.print(" AM ");
        }
      } else {
        Serial.print(" 24h ");
      }
      // Display the temperature
      Serial.print("T=");
      Serial.print(Clock.getTemperature(), 2);
      // Tell whether the time is (likely to be) valid
      if (Clock.oscillatorCheck()) {
        Serial.print(" O+");
      } else {
        Serial.print(" O-");
      }
      // Indicate whether an alarm went off
      if (Clock.checkIfAlarm(1)) {
        Serial.print(" A1!");
      }
      if (Clock.checkIfAlarm(2)) {
        Serial.print(" A2!");
      }
      // New line on display
      Serial.print('\n');
      // Display Alarm 1 information
      Serial.print("Alarm 1: ");
      Clock.getA1Time(ADay, AHour, AMinute, ASecond, ABits, ADy, A12h, Apm);
      Serial.print(ADay, DEC);
      if (ADy) {
        Serial.print(" DoW");
      } else {
        Serial.print(" Date");
      }
      Serial.print(' ');
      Serial.print(AHour, DEC);
      Serial.print(' ');
      Serial.print(AMinute, DEC);
      Serial.print(' ');
      Serial.print(ASecond, DEC);
      Serial.print(' ');
      if (A12h) {
        if (Apm) {
          Serial.print('pm ');
        } else {
          Serial.print('am ');
        }
      }
      if (Clock.checkAlarmEnabled(1)) {
        Serial.print("enabled");
      }
      Serial.print('\n');
      // Display Alarm 2 information
      Serial.print("Alarm 2: ");
      Clock.getA2Time(ADay, AHour, AMinute, ABits, ADy, A12h, Apm);
      Serial.print(ADay, DEC);
      if (ADy) {
        Serial.print(" DoW");
      } else {
        Serial.print(" Date");
      }
      Serial.print(' ');
      Serial.print(AHour, DEC);
      Serial.print(' ');
      Serial.print(AMinute, DEC);
      Serial.print(' ');
      if (A12h) {
        if (Apm) {
          Serial.print('pm');
        } else {
          Serial.print('am');
        }
      }
      if (Clock.checkAlarmEnabled(2)) {
        Serial.print("enabled");
      }
      // display alarm bits
      Serial.print('\nAlarm bits: ');
      Serial.print(ABits, BIN);
    
      Serial.print('\n');
      Serial.print('\n');
      delay(1000); 
  
  }

  
/***********************************************************************
 * FUNCTION:    buzzer_beep
 * DESCRIPTION: buzzer sound beep
 * PARAMETERS:  int times
 * RETURNED:    nothing
 ***********************************************************************/
void ds3231_setting(void){
  
    Serial.println("Info: ds3231 setting");  
    buzzer_beep(2);
    
    int _hrs = Clock.getHour(h12, PM);
    int _mins = Clock.getMinute();
    int _year = Clock.getYear();
    int _mnt = Clock.getMonth(Century);
    int _day = Clock.getDate();
    
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(10, 10,"SET TIME");
    display.drawString(0, 20,"--------------------");
    display.display();
    while( !digitalRead(BUTTON_SW2) || !digitalRead(BUTTON_SW1) );


    //*** setting hour ***
    do{      
        if(!digitalRead(BUTTON_SW1)){
            buzzer_beep(1);
            _hrs++; 
            if(_hrs>23) _hrs = 0;
            while(!digitalRead(BUTTON_SW1));
         }

        sprintf(time_string,"%d:%02d",_hrs,_mins);
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(10, 10,"SET TIME");
        display.drawString(0, 25,"--------------------");
        display.setFont(ArialMT_Plain_16);
        display.drawString(40, 45, time_string);
        display.display();
        delay(100);
      
      }while( digitalRead(BUTTON_SW2) );
      while( !digitalRead(BUTTON_SW2) );
      Clock.setHour(_hrs);
      buzzer_beep(2);

    //*** setting minute ***
    do{
        if(!digitalRead(BUTTON_SW1)){
            buzzer_beep(1);
            _mins++; 
            if(_mins>59) _mins = 0;
            while(!digitalRead(BUTTON_SW1));
         }

        sprintf(time_string,"%d:%02d",_hrs,_mins);
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(10, 10,"SET TIME");
        display.drawString(0, 25,"--------------------");
        display.setFont(ArialMT_Plain_16);
        display.drawString(40, 45, time_string);
        display.display();
        delay(100);
      
      }while( digitalRead(BUTTON_SW2) );
      while( !digitalRead(BUTTON_SW2) );    
      Clock.setMinute(_mins);  
      buzzer_beep(2);


    //*** setting day ***
    do{
        if(!digitalRead(BUTTON_SW1)){
            buzzer_beep(1);
            _day++; 
            if(_day>31) _day = 1;
            while(!digitalRead(BUTTON_SW1));
         }

        sprintf(date_string,"%d / %02d / 20%02d",_day,_mnt,_year);
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(10, 10,"SET DATE");
        display.drawString(0, 25,"--------------------");
        display.setFont(ArialMT_Plain_16);
        display.drawString(20, 45, date_string);
        display.display();
        delay(100);
      
      }while( digitalRead(BUTTON_SW2) );
      while( !digitalRead(BUTTON_SW2) );    
      Clock.setDate(_day);  
      buzzer_beep(2);      

      
    //*** setting month ***
    do{
        if(!digitalRead(BUTTON_SW1)){
            buzzer_beep(1);
            _mnt++; 
            if(_mnt>12) _mnt = 1;
            while(!digitalRead(BUTTON_SW1));
         }

        sprintf(date_string,"%d / %02d / 20%02d",_day,_mnt,_year);
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(10, 10,"SET DATE");
        display.drawString(0, 25,"--------------------");
        display.setFont(ArialMT_Plain_16);
        display.drawString(20, 45, date_string);
        display.display();
        delay(100);
      
      }while( digitalRead(BUTTON_SW2) );
      while( !digitalRead(BUTTON_SW2) );    
      Clock.setMonth(_mnt);  
      buzzer_beep(2);  


    //*** setting year ***
    do{
        if(!digitalRead(BUTTON_SW1)){
            buzzer_beep(1);
            _year++; 
            if(_year>99) _year = 1;
            while(!digitalRead(BUTTON_SW1));
         }

        sprintf(date_string,"%d / %02d / 20%02d",_day,_mnt,_year);
        display.clear();
        display.setFont(ArialMT_Plain_24);
        display.drawString(10, 10,"SET DATE");
        display.drawString(0, 25,"--------------------");
        display.setFont(ArialMT_Plain_16);
        display.drawString(20, 45, date_string);
        display.display();
        delay(100);
      
      }while( digitalRead(BUTTON_SW2) );
      while( !digitalRead(BUTTON_SW2) );    
      Clock.setYear(_year);  
      buzzer_beep(2);        
   
      display.clear();
      display.setFont(ArialMT_Plain_24);
      display.drawString(10, 10,"SET DATE");
      display.drawString(0, 25,"--------------------");
      display.setFont(ArialMT_Plain_16);
      display.drawString(30, 45, "Finished");
      display.display();
      delay(500);  

    

    
  
}

 /***********************************************************************
 * FUNCTION:    buzzer_beep
 * DESCRIPTION: buzzer sound beep
 * PARAMETERS:  int times
 * RETURNED:    nothing
 ***********************************************************************/
void buzzer_beep(int times){
  for(int _i=0; _i < times; _i++){
    digitalWrite(BUZZER,HIGH);
    delay(70);
    digitalWrite(BUZZER,LOW);
    delay(70);
  }
}  
 
 /***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println("************************************************");
  Serial.println("Project      :  ESP32_IoT_ALL_DS3231");                 
  Serial.println("Author       :  Tenergy Innovation Co., Ltd.");
  Serial.println("Date         :  6 Dec 2018");
  Serial.println("website      :  http://www.tenergyinnovation.co.th");
  Serial.println("Email        :  uten.boonliam@innovation.co.th");
  Serial.println("TEL          :  089-140-7205");
  Serial.println("************************************************");

   Serial.print("Info: Intial gpio...");
   pinMode(BUTTON_SW1,INPUT);
   pinMode(BUTTON_SW2,INPUT);
   pinMode(BUZZER,OUTPUT);
   digitalWrite(BUZZER,LOW);
   Serial.println("done"); 
   buzzer_beep(1);
  
  //
  Serial.print("Info: Intial DS3231...");
  Wire.begin();
  Serial.println("done");

  Serial.println("Info: *** Data from DS3231 ***");
  ds3231_getdata(); //**Need to call before display.init() **

  
  //**** setting time ***/
//  Serial.print("Info: Setting DS3231...");
//  Clock.setYear(18);
//  Clock.setMonth(12);
//  Clock.setDate(6);
//  Clock.setDoW(5);
//  Clock.setHour(2);
//  Clock.setMinute(9);
//  Clock.setSecond(30);
//  Serial.println("done");
  
  
  Serial.print("Info: Intial OLED 128x64...");
  display.init();
  Serial.println("done");
  
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  Serial.print("Info: Oled show logo...");
  demos[tenergy_logo]();
  display.display();
  delay(1000);
  Serial.println("done");
  
  display.clear();

  Serial.print("Info: Oled show prograssive...");
  counter =0;
  do{
    display.clear();
    demos[progressive]();
    display.display();
    delay(10);
  }while(counter++ <= 498);
  Serial.println("done");
  buzzer_beep(2);
}

 /***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop() {

  
  //copy data from ds3231 to char variable
  sprintf(date_string,"%d / %02d / 20%02d",Clock.getDate(),Clock.getMonth(Century),Clock.getYear());
  sprintf(time_string,"%d:%02d:%02d",Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
  sprintf(temp_string,"T=%.2f C\r\n",Clock.getTemperature());

  //show serial terminal
  Serial.printf("DATE: %s\r\n",date_string);
  Serial.printf("TIME: %s\r\n",time_string);
  Serial.printf("TEMP: %s\r\n",temp_string);

  //show oled display
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_24);
  
  if(Clock.getHour(h12, PM) >9) //Need to show center of display
    display.drawString(15, 10, time_string);
  else
    display.drawString(22, 10, time_string);

  display.setFont(ArialMT_Plain_10);
  display.drawString(35, 35, date_string);
  display.drawString(40, 52, temp_string);
  display.display();
  delay(1000);

  //check button switch for set date-time
  if( !digitalRead(BUTTON_SW2) || !digitalRead(BUTTON_SW1) ){
      ds3231_setting();
    }
  
}



