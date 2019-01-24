/***********************************************************************
 * Project      :     ESP32_IoT_ALL_LED_RGB
 * Description  :     LED RGB Test            
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
#define ledR        25
#define ledG        26
#define ledB        27




/**************************************/
/*        object define               */
/**************************************/
SSD1306Wire  display(0x3c, 21, 22);  //21 = SDA, 22 = SCL
#define DEMO_DURATION 3000
#define tenergy_logo        0
#define progressive         1
#define wifi_logo           2
#define wifi_on_logo        3
#define wifi_off_logo       4
#define hotspot_logo        5
#define wificoffee_logo     6
#define facebook_logo       7
#define bug_logo            8
#define maintenance_logo    9
#define camera_logo         10
#define clock_logo          11
#define motor_logo          12
#define eeprom_logo         13
#define home_logo           14
#define led_logo            15
#define motion_logo         16
#define relay_logo          17
#define rs485_logo          18
#define stepping_logo        19
#define temperature_logo    20
#define tmp1638_logo        21
#define ultrasonic_logo     22
#define uten_logo           23
#define potentiometer_logo  24
#define ldr_logo            25
#define payuth_logo         26
#define dip3sw_logo         27
#define mju_logo            28
#define input_logo          29
#define switch_logo         30
#define netpie_logo         31
#define ampere_logo         32
#define blynk_logo          33
DS3231 Clock;




/**************************************/
/*        define function define      */
/**************************************/
//oled
typedef void (*Demo)(void);
void tenergyImage();
void WiFiImage();
void WiFiOnImage();
void WiFiOffImage();
void HotspotImage();
void WiFiCoffeeImage();
void FacebookImage();
void BugImage();
void FixImage();
void drawProgressBarDemo();
void CameraImage();
void ClockImage();
void MotorImage();
void EepromImage();
void HomeImage();
void LedImage();
void MotionImage();
void RelayImage();
void Rs485Image();
void SteppingImage();
void TemperatureImage();
void Tmp1638Image();
void UltrasonicImage();
void UtenImage();
void PotentiometerImage();
void LdrImage();
void PayuthImage();
void Dip3swImage();
void MjuImage();
void InputImage();
void SwitchImage();
void NetpieImage();
void AmpereImage();
void BlynkImage();

//ds3231
void ds3231_getdata(void);
void ds3231_setting(void);
//buzzer
void buzzer_beep(int times);
//led rgb
void hueToRGB(uint8_t hue, uint8_t brightness);

/**************************************/
/*        global variable             */
/**************************************/
int demoMode = 0;
int counter = 1;
Demo demos[] = {tenergyImage, drawProgressBarDemo, WiFiImage, WiFiOnImage, WiFiOffImage, HotspotImage, WiFiCoffeeImage, FacebookImage, BugImage, FixImage,
                CameraImage, ClockImage, MotorImage, EepromImage, HomeImage, LedImage, MotionImage, RelayImage, Rs485Image, SteppingImage, TemperatureImage, Tmp1638Image,
                UltrasonicImage, UtenImage, PotentiometerImage, LdrImage, PayuthImage, Dip3swImage, MjuImage, InputImage,SwitchImage,NetpieImage,AmpereImage,BlynkImage
               };
int demoLength = (sizeof(demos) / sizeof(Demo));
int logo_blink;
//
uint8_t ledArray[3] = {1, 2, 3}; // three led channels
const boolean invert = true; // set true if common anode, false if common cathode
uint8_t color = 0;          // a value from 0 to 255 representing the hue
uint32_t R, G, B;           // the Red Green and Blue color components
uint8_t brightness = 255;  // 255 is maximum brightness, but can be changed.  Might need 256 for common anode to fully turn off.
//
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
char date_string[20],time_string[20],temp_string[20];

/**************************************/
/*        word define               */
/**************************************/
#define ON    LOW
#define OFF   HIGH



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
 * FUNCTION:    hueToRGB
 * DESCRIPTION: hueToRGB
 * PARAMETERS:  hue,brightness
 * RETURNED:    nothing
 ***********************************************************************/
// Courtesy http://www.instructables.com/id/How-to-Use-an-RGB-LED/?ALLSTEPS
// function to convert a color to its Red, Green, and Blue components.

void hueToRGB(uint8_t hue, uint8_t brightness)
{
    uint16_t scaledHue = (hue * 6);
    uint8_t segment = scaledHue / 256; // segment 0 to 5 around the
                                            // color wheel
    uint16_t segmentOffset =
      scaledHue - (segment * 256); // position within the segment

    uint8_t complement = 0;
    uint16_t prev = (brightness * ( 255 -  segmentOffset)) / 256;
    uint16_t next = (brightness *  segmentOffset) / 256;

    if(invert)
    {
      brightness = 255 - brightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
    }

    switch(segment ) {
    case 0:      // red
        R = brightness;
        G = next;
        B = complement;
    break;
    case 1:     // yellow
        R = prev;
        G = brightness;
        B = complement;
    break;
    case 2:     // green
        R = complement;
        G = brightness;
        B = next;
    break;
    case 3:    // cyan
        R = complement;
        G = prev;
        B = brightness;
    break;
    case 4:    // blue
        R = next;
        G = complement;
        B = brightness;
    break;
   case 5:      // magenta
    default:
        R = brightness;
        G = complement;
        B = prev;
    break;
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
  Serial.println("Project      :  ESP32_IoT_ALL_LED_RGB");                 
  Serial.println("Author       :  Tenergy Innovation Co., Ltd.");
  Serial.println("Date         :  6 Dec 2018");
  Serial.println("Www          :  https://www.tenergyinnovation.co.th");
  Serial.println("Facebook     :  https://www.facebook.com/tenergy.innovation");
  Serial.println("Email        :  uten.boonliam@tenergyinnovation.co.th");
  Serial.println("TEL          :  +6689-140-7205");
  Serial.println("************************************************");

  Serial.print("Info: Intial gpio...");
   pinMode(BUTTON_SW1,INPUT);
   pinMode(BUTTON_SW2,INPUT);
   pinMode(INPUT_1,INPUT);
   pinMode(INPUT_2,INPUT);
   pinMode(INPUT_3,INPUT);
   pinMode(INPUT_4,INPUT);
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

  Serial.print("Info: Initial LED RGB...");
  ledcAttachPin(ledR, 1); // assign RGB led pins to channels
  ledcAttachPin(ledG, 2);
  ledcAttachPin(ledB, 3);
  // Initialize channels 
  // channels 0-15, resolution 1-16 bits, freq limits depend on resolution
  // ledcSetup(uint8_t channel, uint32_t freq, uint8_t resolution_bits);
  ledcSetup(1, 12000, 8); // 12 kHz PWM, 8-bit resolution
  ledcSetup(2, 12000, 8);
  ledcSetup(3, 12000, 8);
  Serial.println("done");
  
  
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

  display.clear();
  demos[led_logo]();
  display.display();
}

 /***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop() {

 Serial.println("Send all LEDs a 255 and wait 2 seconds.");
  // If your RGB LED turns off instead of on here you should check if the LED is common anode or cathode.
  // If it doesn't fully turn off and is common anode try using 256.
  ledcWrite(1, 255);
  ledcWrite(2, 255);
  ledcWrite(3, 255);
  delay(2000);
  Serial.println("Send all LEDs a 0 and wait 2 seconds.");
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
  delay(2000);
 
  Serial.println("Starting color fade loop.");
  
 for (color = 0; color < 255; color++) { // Slew through the color spectrum

  hueToRGB(color, brightness);  // call function to convert hue to RGB

  // write the RGB values to the pins
  ledcWrite(1, R); // write red component to channel 1, etc.
  ledcWrite(2, G);   
  ledcWrite(3, B); 
 
  delay(100); // full cycle of rgb over 256 colors takes 26 seconds
 } 

//  ledcWrite(1, 255);
//  ledcWrite(2, 40);
//  ledcWrite(3, 200);
//  while(1);

  
}

/***********************************************************************
   FUNCTION:    drawProgressBarDemo
   DESCRIPTION: drawProgressBarDemo
   PARAMETERS:  nothing
   RETURNED:    nothing
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
  FUNCTION:    tenergyImage
  DESCRIPTION: tenergyImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void tenergyImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, Tenergy_Logo_width, Tenergy_Logo_height, Tenergy_Logo_bits);
}

/***********************************************************************
  FUNCTION:    WiFiImage
  DESCRIPTION: WiFiImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void WiFiImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

/***********************************************************************
  FUNCTION:    hotspotImage
  DESCRIPTION: hotspotImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void WiFiOnImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(34, 14, WiFi_on_Logo_width, WiFi_on_Logo_height, WiFi_on_bits);
}


/***********************************************************************
  FUNCTION:    hotspotoffImage
  DESCRIPTION: hotspotoffImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void WiFiOffImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(47, 16, WiFi_off_Logo_width, WiFi_off_Logo_height, WiFi_off_bits);
}

/***********************************************************************
  FUNCTION:    HotspotImage
  DESCRIPTION: HotspotImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void HotspotImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 15, hotspot_Logo_width, hotspot_Logo_height, hotspot_bits);
}


/***********************************************************************
  FUNCTION:    WiFiCoffeeImage
  DESCRIPTION: WiFiCoffeeImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void WiFiCoffeeImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 14, wificoffee_Logo_width, wificoffee_Logo_height, wificoffee_bits);
}


/***********************************************************************
  FUNCTION:    FacebookImage
  DESCRIPTION: FacebookImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void FacebookImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(45, 19, facebook_Logo_width, facebook_Logo_height, facebook_bits);
}

/***********************************************************************
  FUNCTION:    BugImage
  DESCRIPTION: BugImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void BugImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(45, 19, bug_Logo_width, bug_Logo_height, bug_bits);
}

/***********************************************************************
  FUNCTION:    FixImage
  DESCRIPTION: FixImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void FixImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(45, 15, maintenance_Logo_width, maintenance_Logo_height, maintenance_bits);
}


/***********************************************************************
  FUNCTION:    CameraImage
  DESCRIPTION: CameraImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void CameraImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(30, 10, camera_Logo_width, camera_Logo_height, camera_bits);
}

/***********************************************************************
  FUNCTION:    ClockImage
  DESCRIPTION: ClockImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void ClockImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(35, 10, clock_Logo_width, clock_Logo_height, clock_bits);
}

/***********************************************************************
  FUNCTION:    MotorImage
  DESCRIPTION: MotorImage
  PARAMETERS:  nothing
  RETURNED:    nothing
***********************************************************************/
void MotorImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(35, 10, motor_dc_Logo_width, motor_dc_Logo_height, motor_dc_bits);
}

/***********************************************************************
   FUNCTION:    EepromImage
   DESCRIPTION: EepromImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void EepromImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(30, 6, eeprom_Logo_width, eeprom_Logo_height, eeprom_bits);
}

/***********************************************************************
   FUNCTION:    HomeImage
   DESCRIPTION: HomeImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void HomeImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 15, home_Logo_width, home_Logo_height, home_bits);
}

/***********************************************************************
   FUNCTION:    LedImage
   DESCRIPTION: LedImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void LedImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 15, led_Logo_width, led_Logo_height, led_bits);
}

/***********************************************************************
   FUNCTION:    MotionImage
   DESCRIPTION: MotionImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void MotionImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 15, motion_Logo_width, motion_Logo_height, motion_bits);
}

/***********************************************************************
   FUNCTION:    RelayImage
   DESCRIPTION: RelayImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void RelayImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(45, 10, relay_Logo_width, relay_Logo_height, relay_bits);
}

/***********************************************************************
   FUNCTION:    Rs485Image
   DESCRIPTION: Rs485Image
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void Rs485Image() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(38, 10, rs485_Logo_width, rs485_Logo_height, rs485_bits);
}

/***********************************************************************
   FUNCTION:    SteppingImage
   DESCRIPTION: SteppingImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void SteppingImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(45, 15, stepping_Logo_width, stepping_Logo_height, stepping_bits);
}

/***********************************************************************
   FUNCTION:    TemperatureImage
   DESCRIPTION: TemperatureImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void TemperatureImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
//  display.drawXbm(45, 15, temperature_Logo_width, temperature_Logo_height, temperature_bits); //center
  display.drawXbm(45, 3, temperature_Logo_width, temperature_Logo_height, temperature_bits); //top
}

/***********************************************************************
   FUNCTION:    Tmp1638Image
   DESCRIPTION: Tmp1638Image
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void Tmp1638Image() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(12, 5, tmp1638_Logo_width, tmp1638_Logo_height, tmp1638_bits);
}

/***********************************************************************
   FUNCTION:    UltrasonicImage
   DESCRIPTION: UltrasonicImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void UltrasonicImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(35, 8, ultrasonic_Logo_width, ultrasonic_Logo_height, ultrasonic_bits);
}


/***********************************************************************
   FUNCTION:    UtenImage
   DESCRIPTION: UtenImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void UtenImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, uten_Logo_width, uten_Logo_height, uten_bits);
}

/***********************************************************************
   FUNCTION:    UtenImage
   DESCRIPTION: UtenImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void PotentiometerImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 5, potentiometer_Logo_width, potentiometer_Logo_height, potentiometer_bits);
}

/***********************************************************************
   FUNCTION:    LdrImage
   DESCRIPTION: LdrImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void LdrImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(40, 5, ldr_Logo_width, ldr_Logo_height, ldr_bits);
}

/***********************************************************************
   FUNCTION:    PayuthImage
   DESCRIPTION: PayuthImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void PayuthImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, payuth_Logo_width, payuth_Logo_height, payuth_bits);
}

/***********************************************************************
   FUNCTION:    Dip3swImage
   DESCRIPTION: Dip3swImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void Dip3swImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(20, 3, dip3sw_Logo_width, dip3sw_Logo_height, dip3sw_bits);
}


/***********************************************************************
   FUNCTION:    MjuImage
   DESCRIPTION: MjuImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void MjuImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(15, 3, mju_Logo_width, mju_Logo_height, mju_bits);
}


/***********************************************************************
   FUNCTION:    InputImage
   DESCRIPTION: InputImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void InputImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, input_Logo_width, input_Logo_height, input_bits);
}

/***********************************************************************
   FUNCTION:    SwitchImage
   DESCRIPTION: SwitchImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void SwitchImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, switch_Logo_width, switch_Logo_height, switch_bits);
}

/***********************************************************************
   FUNCTION:    NetpieImage
   DESCRIPTION: NetpieImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void NetpieImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, netpie_Logo_width, netpie_Logo_height, netpie_bits);
}


/***********************************************************************
   FUNCTION:    AmpereImage
   DESCRIPTION: AmpereImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void AmpereImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, ampere_Logo_width, ampere_Logo_height, ampere_bits);
}

/***********************************************************************
   FUNCTION:    BlynkImage
   DESCRIPTION: BlynkImage
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void BlynkImage() {
  // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
  // on how to create xbm files
  display.drawXbm(0, 0, blynk_Logo_width, blynk_Logo_height, blynk_bits);
}



