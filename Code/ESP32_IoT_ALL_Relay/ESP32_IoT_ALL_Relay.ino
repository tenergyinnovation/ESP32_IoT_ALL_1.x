/***********************************************************************
 * Project      :     ESP32_IoT_ALL_Relay
 * Description  :     Relay test          
 * Author       :     Tenergy Innovation Co., Ltd.
 * Date         :     6 Dec 2018
 * Revision     :     1.0
 * Rev1.0       :     Original
 * website      :     http://www.tenergyinnovation.co.th
 * Facebook     :     https://www.facebook.com/tenergy.innovation
 * Email        :     uten.boonliam@innovation.co.th
 * TEL          :     089-140-7205
 ***********************************************************************/

/**************************************/
/*         include library            */
/**************************************/ 
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

/**************************************/
/*        word define               */
/**************************************/
#define ON    LOW
#define OFF   HIGH



/**************************************/
/*        define function define      */
/**************************************/
typedef void (*Demo)(void);
void drawImageDemo();
void drawProgressBarDemo();
void ds3231_getdata(void);
void ds3231_setting(void);
void buzzer_beep(int times);
void display_show(void);

/**************************************/
/*        global variable             */
/**************************************/
int demoMode = 0;
int counter = 1;
Demo demos[] = {drawImageDemo, drawProgressBarDemo};
int demoLength = (sizeof(demos) / sizeof(Demo));
boolean output_var_1,output_var_2,output_var_3,output_var_4;




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
 * FUNCTION:    buzzer_beep
 * DESCRIPTION: buzzer sound beep
 * PARAMETERS:  int times
 * RETURNED:    nothing
 ***********************************************************************/
void display_show(void){
  display.clear();  
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);

 //check input#1
   if( output_var_1 == OFF) 
      display.drawString(15, 10, "Output 1 => off");
   else
      display.drawString(15, 10, "Output 1 => on");

 //check input#2
   if( output_var_2 == OFF) 
      display.drawString(15, 20, "Output 2 => off");
   else
      display.drawString(15, 20, "Output 2 => on");

 //check input#3
   if( output_var_3 == OFF) 
      display.drawString(15, 30, "Output 3 => off");
   else
      display.drawString(15, 30, "Output 3 => on");

 //check input#4
   if( output_var_4 == OFF) 
      display.drawString(15, 40, "Output 4 => off");
   else
      display.drawString(15, 40, "Output 4 => on");           
  
  display.display();
  
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
  Serial.println("Project      :  ESP32_IoT_ALL_Relay");                 
  Serial.println("Author       :  Tenergy Innovation Co., Ltd.");
  Serial.println("Date         :  6 Dec 2018");
  Serial.println("website      :  http://www.tenergyinnovation.co.th");
  Serial.println("Email        :  uten.boonliam@innovation.co.th");
  Serial.println("TEL          :  089-140-7205");
  Serial.println("************************************************\r\n");

  Serial.print("Info: Intial gpio...");
   pinMode(OUTPUT_1,OUTPUT);
   pinMode(OUTPUT_2,OUTPUT);
   pinMode(OUTPUT_3,OUTPUT);
   pinMode(OUTPUT_4,OUTPUT);
   pinMode(BUZZER,OUTPUT);
   digitalWrite(BUZZER,LOW);
   digitalWrite(OUTPUT_1,OFF);
   digitalWrite(OUTPUT_2,OFF);
   digitalWrite(OUTPUT_3,OFF);
   digitalWrite(OUTPUT_4,OFF);
   Serial.println("done"); 
   buzzer_beep(1);
  
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

    digitalWrite(OUTPUT_1,ON);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = ON; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = OFF;
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = OFF;    
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,ON);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = ON;
    output_var_3 = OFF;
    output_var_4 = OFF;    
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = OFF;    
    display_show();
    delay(1000);    

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,ON);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = ON;
    output_var_4 = OFF;    
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = OFF;   
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,ON);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = ON;   
    display_show();
    delay(1000);

    digitalWrite(OUTPUT_1,OFF);
    digitalWrite(OUTPUT_2,OFF);
    digitalWrite(OUTPUT_3,OFF);
    digitalWrite(OUTPUT_4,OFF);
    output_var_1 = OFF; 
    output_var_2 = OFF;
    output_var_3 = OFF;
    output_var_4 = OFF;    
    display_show();
    delay(1000);       
}



