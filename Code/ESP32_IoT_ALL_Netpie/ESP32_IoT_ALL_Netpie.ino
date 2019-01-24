/***********************************************************************
   Project      :     ESP32_IoT_ALL_Netpie
   Description  :     Example code to interface with Netpie platform
   Author       :     Tenergy Innovation Co., Ltd.
   Date         :     14 Nov 2018
   Revision     :     1.0
   Rev1.0       :     Original
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
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson
#include <FS.h>  
#include "SPIFFS.h"
#include <Ticker.h> //for LED status
#include <MicroGear.h>
#include "EEPROM.h"

/**************************************/
/*           GPIO define              */
/**************************************/
#define BUTTON_SW1  14
#define BUTTON_SW2  33
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
#define ONE_WIRE_BUS  23

/**************************************/
/*        object define               */
/**************************************/
SSD1306Wire  display(0x3c, 21, 22);  //21 = SDA, 22 = SCL
DS3231 Clock;
Ticker ticker;
Ticker Logo_ticker;
WiFiClient client;
MicroGear microgear(client);

/**************************************/
/*          define constant           */
/**************************************/
//Oled Image
#define ON    LOW
#define OFF   HIGH
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
#define DEMO_DURATION 3000

//eeprom define
#define EEPROM_SIZE         64
#define output1_eeprom      0x00
#define output2_eeprom      0x01
#define output3_eeprom      0x02
#define output4_eeprom      0x03
#define led_r_eeprom        0x04
#define led_g_eeprom        0x05
#define led_b_eeprom        0x06


/**************************************/
/*        pre-define function         */
/**************************************/
//Oled Image
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
void oled_tick();

//ds3231
void ds3231_getdata(void);
void ds3231_setting(void);

//buzzer
void buzzer_beep(int times);

//led blink
void tick();

//wifi
void saveConfigCallback ();

//netpie
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen);
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen);
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen);
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen);
void netpie_refresh(void);


/**************************************/
/*        global variable             */
/**************************************/

//oled
int demoMode = 0;
int counter = 1;
Demo demos[] = {tenergyImage, drawProgressBarDemo, WiFiImage, WiFiOnImage, WiFiOffImage, HotspotImage, WiFiCoffeeImage, FacebookImage, BugImage, FixImage,
                CameraImage, ClockImage, MotorImage, EepromImage, HomeImage, LedImage, MotionImage, RelayImage, Rs485Image, SteppingImage, TemperatureImage, Tmp1638Image,
                UltrasonicImage, UtenImage, PotentiometerImage, LdrImage, PayuthImage, Dip3swImage, MjuImage, InputImage,SwitchImage,NetpieImage,AmpereImage,BlynkImage
               };
int demoLength = (sizeof(demos) / sizeof(Demo));
int logo_blink;

//ds3231
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
char date_string[20],time_string[20],temp_string[20];

//led rgb
uint8_t ledArray[3] = {1, 2, 3}; // three led channels
const boolean invert = true; // set true if common anode, false if common cathode
uint8_t color = 0;          // a value from 0 to 255 representing the hue
uint32_t R, G, B;           // the Red Green and Blue color components
uint8_t brightness = 255;  // 255 is maximum brightness, but can be changed.  Might need 256 for common anode to fully turn off.

//netpie
char netpie_appid[20];
char netpie_key[20];
char netpie_secret[30];
char netpie_alias[20];
char netpie_feedid[20];
bool shouldSaveConfig = false;

//gpio
int output1_val=0;
int output2_val=0;
int output3_val=0;
int output4_val=0;
int led_r_val=0;
int led_g_val=0;
int led_b_val=0;



 /***********************************************************************
 * FUNCTION:    setup
 * DESCRIPTION: setup process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void setup() {
  Serial.begin(115200);
  Serial.println("************************************************");
  Serial.println("Project      :  ESP32_IoT_ALL_Netpie");                 
  Serial.println("Author       :  Tenergy Innovation Co., Ltd.");
  Serial.println("Date         :  26 Dec 2018");
  Serial.println("website      :  http://www.tenergyinnovation.co.th");
  Serial.println("Email        :  uten.boonliam@innovation.co.th");
  Serial.println("TEL          :  089-140-7205");
  Serial.println("************************************************");
  ticker.attach(0.2, tick); //led status blink with 0.2/sec

   /* gpio initial */ 
   Serial.print("Info: Intial gpio...");
   pinMode(BUTTON_SW1,INPUT);
   pinMode(BUTTON_SW2,INPUT);
   pinMode(SLID_SW,INPUT);
   pinMode(BUZZER,OUTPUT);
   digitalWrite(BUZZER,LOW);
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
   pinMode(INPUT_1,INPUT);
   pinMode(INPUT_2,INPUT);
   pinMode(INPUT_3,INPUT);
   pinMode(INPUT_4,INPUT);
   pinMode(LED,OUTPUT);
   Serial.println("done"); 
   buzzer_beep(1);
  
  /* ds3231 RTC initial */
  Serial.print("Info: Intial DS3231...");
  Wire.begin();
  Serial.println("done");

  Serial.println("Info: *** Data from DS3231 ***");
  ds3231_getdata(); //**Need to call before display.init() **


  /* oled initial */
  Serial.print("Info: Intial OLED 128x64...");
  display.init();
  Serial.println("done");
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  /* oled show tenergy logo*/
  display.clear();
  Serial.print("Info: Oled show Tenergy logo...");
  demos[tenergy_logo]();
  display.display();
  delay(1000);
  Serial.println("done");
  display.clear();

  /* oled show progrssaive bar 0-100% */
  Serial.print("Info: Oled show prograssive...");
  counter =0;
  do{
    display.clear();
    demos[progressive]();
    display.display();
    delay(5);
  }while(counter++ <= 498);
  Serial.println("done");

  /* eeprom initial */
  logo_blink = eeprom_logo;
  Logo_ticker.attach(0.3, oled_tick);
  Serial.println("Info: eeprom initial ....");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    buzzer_beep(3);
    Serial.println("Error: failed to initialise EEPROM");
  }
  else{
    /*format eeprom*/
//    for(int i=0; i<EEPROM_SIZE; i++) EEPROM.write(i,0xff); EEPROM.commit();

    /*output#1 eeprom*/
    if(EEPROM.read(output1_eeprom) == 0xff){
        output1_val=0;
        EEPROM.write(output1_eeprom,output1_val);
        EEPROM.commit();
      }
    else{
        output1_val = EEPROM.read(output1_eeprom);
      }

    /*output#2 eeprom*/
    if(EEPROM.read(output2_eeprom) == 0xff){
        output2_val=0;
        EEPROM.write(output2_eeprom,output2_val);
        EEPROM.commit();
      }
    else{
        output2_val = EEPROM.read(output2_eeprom);
      }

    /*output#3 eeprom*/
    if(EEPROM.read(output3_eeprom) == 0xff){
        output3_val=0;
        EEPROM.write(output3_eeprom,output3_val);
        EEPROM.commit();
      }
    else{
        output3_val = EEPROM.read(output3_eeprom);
      }

    /*output#4 eeprom*/
    if(EEPROM.read(output4_eeprom) == 0xff){
        output4_val=0;
        EEPROM.write(output4_eeprom,output4_val);
        EEPROM.commit();
      }
    else{
        output4_val = EEPROM.read(output4_eeprom);
      }    

    /*led_r eeprom*/
    if(EEPROM.read(led_r_eeprom) == 0xff){
        led_r_val=0;
        EEPROM.write(led_r_eeprom,led_r_val);
        EEPROM.commit();
      }
    else{
        led_r_val = EEPROM.read(led_r_eeprom);
      }  
              
    /*led_g eeprom*/
    if(EEPROM.read(led_g_eeprom) == 0xff){
        led_g_val=0;
        EEPROM.write(led_g_eeprom,led_g_val);
        EEPROM.commit();
      }
    else{
        led_g_val = EEPROM.read(led_g_eeprom);
      } 

    /*led_b eeprom*/
    if(EEPROM.read(led_b_eeprom) == 0xff){
        led_b_val=0;
        EEPROM.write(led_b_eeprom,led_b_val);
        EEPROM.commit();
      }
    else{
        led_b_val = EEPROM.read(led_b_eeprom);
      }     

     Serial.printf("Info: output1_val = %d\r\n",output1_val);  
     Serial.printf("Info: output2_val = %d\r\n",output2_val); 
     Serial.printf("Info: output3_val = %d\r\n",output3_val); 
     Serial.printf("Info: output4_val = %d\r\n",output4_val);
     digitalWrite(OUTPUT_1,!output1_val);
     digitalWrite(OUTPUT_2,!output2_val);
     digitalWrite(OUTPUT_3,!output3_val);
     digitalWrite(OUTPUT_4,!output4_val);
     Serial.printf("Info: led_r_val = %d\r\n",led_r_val);  
     Serial.printf("Info: led_g_val = %d\r\n",led_g_val);  
     Serial.printf("Info: led_b_val = %d\r\n",led_b_val);  
     delay(1000);
    }
  Serial.println("...done");

  /* led rgb initial */
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
  ledcWrite(1, led_r_val);
  ledcWrite(2, led_g_val);
  ledcWrite(3, led_b_val);
  Serial.println("done");
  

 /* file system initial */ 
// Serial.println("Please wait 30 secs for SPIFFS to be formatted");
// SPIFFS.format();

 Serial.print("Info: File System Initializing ...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(netpie_appid, json["netpie_appid"]);
          strcpy(netpie_key, json["netpie_key"]);
          strcpy(netpie_secret, json["netpie_secret"]);
          strcpy(netpie_alias, json["netpie_alias"]);
          strcpy(netpie_feedid, json["netpie_feedid"]);

        } else {
          Serial.println("failed to load json config");
          display.clear(); demos[maintenance_logo](); display.display();delay(2000);buzzer_beep(4);
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
    display.clear(); demos[maintenance_logo](); display.display();delay(2000);buzzer_beep(4); //if fail will show maintenanace logo on oled
  }
  //end read    
    buzzer_beep(1);
    WiFiManagerParameter custom_netpie_appid("appid", "netpie appid", netpie_appid, 20);
    WiFiManagerParameter custom_netpie_key("key", "netpie key", netpie_key, 20);
    WiFiManagerParameter custom_netpie_secret("secret", "netpie secret", netpie_secret, 30);    
    WiFiManagerParameter custom_netpie_alias("alias", "netpie alias", netpie_alias, 20);
    WiFiManagerParameter custom_netpie_feedid("feedid", "netpie feedid", netpie_feedid, 20); 
 
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
   //set static ip
//    wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    
    /*blink logo*/
    logo_blink = hotspot_logo;
    Logo_ticker.attach(0.3, oled_tick);
    wifiManager.addParameter(&custom_netpie_appid);
    wifiManager.addParameter(&custom_netpie_key);
    wifiManager.addParameter(&custom_netpie_secret);
    wifiManager.addParameter(&custom_netpie_alias);
    wifiManager.addParameter(&custom_netpie_feedid);
  
//    wifiManager.resetSettings(); //reset settings - for testing
//    if (!wifiManager.autoConnect("ESP32_IoT_ALL", "password")) { //if need password for AP
    if (!wifiManager.autoConnect("ESP32_IoT_ALL")) {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.restart();
        delay(5000);
    }    
    buzzer_beep(1);
    Logo_ticker.detach();
    /*blink logo*/


    
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  
  /* show logo wifi coffee */
  logo_blink = wificoffee_logo;
  Logo_ticker.attach(0.3, oled_tick);
  delay(3000);

  //read updated parameters
  strcpy(netpie_appid, custom_netpie_appid.getValue());
  strcpy(netpie_key, custom_netpie_key.getValue());
  strcpy(netpie_secret, custom_netpie_secret.getValue());
  strcpy(netpie_alias, custom_netpie_alias.getValue());
  strcpy(netpie_feedid, custom_netpie_feedid.getValue());
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["netpie_appid"] = netpie_appid;
    json["netpie_key"] = netpie_key;
    json["netpie_secret"] = netpie_secret;
    json["netpie_alias"] = netpie_alias;
    json["netpie_feedid"] = netpie_feedid;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }
    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }
  
    Logo_ticker.detach();

    /* print netpie parameter */
    Serial.printf("Info: netpie_appid = %s\r\n",netpie_appid);
    Serial.printf("Info: netpie_key = %s\r\n",netpie_key);
    Serial.printf("Info: netpie_secret = %s\r\n",netpie_secret);
    Serial.printf("Info: netpie_alias = %s\r\n",netpie_alias);
    Serial.printf("Info: netpie_feedid = %s\r\n",netpie_feedid);
    Serial.print("Info: local ip = ");
    Serial.println(WiFi.localIP());
    Serial.print("Info: SSID = ");
    Serial.println(WiFi.SSID());


    /* now netpie parameter to oled screen */
    display.clear();
    char _string[40];
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Netpie Parameter");
    display.drawString(0, 8, "------------------");
    sprintf(_string,"Appid: %s",netpie_appid);
    display.drawString(0, 16, _string);
    
    sprintf(_string,"Key: %s",netpie_key);
    display.drawString(0, 24, _string);
    
    sprintf(_string,"Appid: %s",netpie_secret);
    display.drawString(0, 32, _string);
    
    sprintf(_string,"Alias: %s",netpie_alias);
    display.drawString(0, 40, _string);
    
    sprintf(_string,"Feedid: %s",netpie_feedid);
    display.drawString(0, 48, _string);
    
    display.display();
    delay(3000);
    buzzer_beep(1);


  


    //wifi config switch if turn-on board will go to AP mode
    if(digitalRead(SLID_SW) == ON){
      logo_blink = maintenance_logo;
      Logo_ticker.attach(0.3, oled_tick);
      Serial.println("Info: WiFi configulation"); 
      wifiManager.resetSettings();
      delay(3000);
      ESP.restart();
    }

    /* show netpie logo*/
    logo_blink = netpie_logo;
    Logo_ticker.attach(0.3, oled_tick);

    /* netpie initial */
    Serial.println("\n>>Info: Initial Microgear callback funcition....");
    /* Call onMsghandler() when new message arraives */
    microgear.on(MESSAGE,onMsghandler);
        /* Call onConnected() when NETPIE connection is established */
    microgear.on(CONNECTED,onConnected);

    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(netpie_key,netpie_secret,netpie_alias);

    /* connect to NETPIE to a specific APPID */
    microgear.connect(netpie_appid);
    Serial.println("done.");
    Logo_ticker.detach();
    delay(1000);
    netpie_refresh(); //update status board to netpie
    delay(1000);
        


  buzzer_beep(2);  
  display.clear(); demos[ampere_logo]();display.display(); 
  ticker.attach(2.0, tick);  //led status blink with 2/sec
}


 /***********************************************************************
 * FUNCTION:    loop
 * DESCRIPTION: loop process
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void loop() {
//    display.clear(); demos[ampere_logo]();display.display();
    static int _timer = 0;
    static int _lost_cnt = 0; //ตัวแปรนับจำนวนการ lost netpie connection
    char _temp[10];
    float _temp_float;
    String _msg="";
    
    /* wifi config switch if turn-on board will go to AP mode */
    if(digitalRead(SLID_SW) == ON){
        ticker.attach(0.2, tick); 
        WiFiManager wifiManager;  
        logo_blink = maintenance_logo;
        Logo_ticker.attach(0.3, oled_tick);
        Serial.println("Info: WiFi configulation"); 
        wifiManager.resetSettings();
        delay(3000);
        ESP.restart();
    }
    else{
      
       if (microgear.connected()) {
//          Serial.println("connected");
            microgear.loop();
            Logo_ticker.detach();
            _lost_cnt=0;
        }
        else {
            Serial.println("connection lost, reconnect...");
            microgear.connect(netpie_appid);
            logo_blink = maintenance_logo;
            Logo_ticker.attach(0.3, oled_tick);
            ticker.attach(0.2, tick);  //led status blink with 2/sec
            
            /*ถ้า lost มากกว่าหรือเท่ากับ 10 ครั้ง ให้ทำการ restart mcu */
            if(_lost_cnt >= 10){ 
              ESP.restart();
            }
            _lost_cnt++;
              
            
              
        }     
      }


    /* get tempeature value and send to netpie every 2 second */
    if(_timer >= 20){ 
            Wire.begin();
            _temp_float = Clock.getTemperature();
            if( (_temp_float < 100) && (_temp_float > 0) ){ //ตรวจสอบว่าค่าอุณหภูมิอยู่ใน range

                /*netpie freeboard*/
                sprintf(_temp,"%.2f",_temp_float);
                Serial.printf("Info: Temp=%.2f\r\n",_temp);
                microgear.publish("/esp32",_temp);
    
                /*netpie feed*/
                _msg ="temp:";
                _msg += String(_temp_float);
//                Serial.println(_msg);
                microgear.writeFeed(netpie_feedid,_msg); /*Ampere001Feed need to change by config file */            

                /*oled screen*/
                display.clear(); demos[temperature_logo](); display.display();
                display.setFont(ArialMT_Plain_16);
                sprintf(_temp,"T = %.2f",_temp_float);
                display.drawString(25, 47, _temp);
                display.display();   
            }
            _timer =0;
          }         
    _timer++;
    delay(100);

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
 * FUNCTION:    ds3231_setting
 * DESCRIPTION: ds3231 RTC setting
 * PARAMETERS:  nothing
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
   FUNCTION:    oled_tick
   DESCRIPTION: logo on oled blink
   PARAMETERS:  nothing
   RETURNED:    nothing
 ***********************************************************************/
void oled_tick()
{
  //toggle state
  static boolean _show_logo = 0;
  if (_show_logo) {
    display.clear(); demos[logo_blink](); display.display();
  }
  else {
    display.clear(); display.display();
  }
  _show_logo = !_show_logo;
  //    Serial.printf("_show_logo = %d\r\n",_show_logo);

}

/***********************************************************************
 * FUNCTION:    tick
 * DESCRIPTION: led toggle blink
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void tick()
{
  //toggle state
  int state = digitalRead(LED);  // get the current state of GPIO1 pin
  digitalWrite(LED, !state);     // set pin to the opposite state
}

/***********************************************************************
 * FUNCTION:    saveConfigCallback
 * DESCRIPTION: Call back function when auto connect fail, 
 * callback notifying us of the need to save config
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void saveConfigCallback () {
  Serial.println(">>Should save config");
  shouldSaveConfig = true;
} 

/***********************************************************************
 * FUNCTION:    onMsghandler
 * DESCRIPTION: Microgear callback function when get new message
 * PARAMETERS:  char *topic, uint8_t* msg, unsigned int msglen
 * RETURNED:    nothing
 ***********************************************************************/
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    char _msg[20];
    int _index=0; //ตัวแปรชี้ตำแหน่งข้อความเข้า
    int _cnt=0; //ตัวแปรนับตำแหน่งข้อความเข้า
    
    _index=0;
    _cnt=0;

   
    /*เก็บค่า wording แรก */    
    do{
        _msg[_cnt++] = (char)msg[_index++]; 
    }while((char)msg[_index] != ':');

    Serial.printf("Debug: _msg = %s\r\n",_msg);
    
    /* output#1 */
    if(String(_msg) == "output1"){
        buzzer_beep(1);
        display.clear(); demos[relay_logo](); display.display();
        
        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง
        if(msg[_index] == '0'){
            digitalWrite(OUTPUT_1,HIGH);
            Serial.println("Debug: output_1 = high");
            microgear.publish("/esp32/output1","0"); //update stat to netpie
            output1_val=0;
            EEPROM.write(output1_eeprom,output1_val); //update eeprom  
            EEPROM.commit();
          }
        else if( msg[_index] == '1'){
            digitalWrite(OUTPUT_1,LOW);
            Serial.println("Debug: output_1 = low");
            microgear.publish("/esp32/output1","1"); //update stat to netpie
            output1_val=1;
            EEPROM.write(output1_eeprom,output1_val); //update eeprom 
            EEPROM.commit(); 
          }
        else{
          Serial.println("Error: Not match any case"); 
          }  
        
      }
    /* output#2 */  
    else if(String(_msg) == "output2"){
        buzzer_beep(1);
        display.clear(); demos[relay_logo](); display.display();

        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง
        if(msg[_index] == '0'){
            digitalWrite(OUTPUT_2,HIGH);
            Serial.println("Debug: output_2 = high");
            microgear.publish("/esp32/output2","0"); //update stat to netpie
            output2_val=0;
            EEPROM.write(output2_eeprom,output2_val);//update eeprom 
            EEPROM.commit(); 
          }
        else if( msg[_index] == '1'){
            digitalWrite(OUTPUT_2,LOW);
            Serial.println("Debug: output_2 = low");
            microgear.publish("/esp32/output2","1"); //update stat to netpie
            output2_val=1;
            EEPROM.write(output2_eeprom,output2_val); //update eeprom  
            EEPROM.commit();           
          }
        else{
          Serial.println("Error: Not match any case"); 
          }        
      }   
    /* output#3 */    
    else if(String(_msg) == "output3"){
        buzzer_beep(1);
        display.clear(); demos[relay_logo](); display.display();

        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง
        if(msg[_index] == '0'){
            digitalWrite(OUTPUT_3,HIGH);
            Serial.println("Debug: output_3 = high");
            microgear.publish("/esp32/output3","0"); //update stat to netpie
            output3_val=0;
            EEPROM.write(output3_eeprom,output3_val); //update eeprom      
            EEPROM.commit();        
          }
        else if( msg[_index] == '1'){
            digitalWrite(OUTPUT_3,LOW);
            Serial.println("Debug: output_3 = low");
            microgear.publish("/esp32/output3","1"); //update stat to netpie
            output3_val=1;
            EEPROM.write(output3_eeprom,output3_val);//update eeprom      
            EEPROM.commit();      
          }
        else{
          Serial.println("Error: Not match any case"); 
          }        
      }  
    /* output#4 */    
    else if(String(_msg) == "output4"){
        buzzer_beep(1);
        display.clear(); demos[relay_logo](); display.display();
        
        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง
        if(msg[_index] == '0'){
            digitalWrite(OUTPUT_4,HIGH);
            Serial.println("Debug: output_4 = high");
            microgear.publish("/esp32/output4","0"); //update stat to netpie
            output4_val=0;
            EEPROM.write(output4_eeprom,output4_val); //update eeprom     
            EEPROM.commit();          
            
          }
        else if( msg[_index] == '1'){
            digitalWrite(OUTPUT_4,LOW);
            Serial.println("Debug: output_4 = low");
            microgear.publish("/esp32/output4","1"); //update stat to netpie
            output4_val=1;
            EEPROM.write(output4_eeprom,output4_val); //update eeprom     
            EEPROM.commit();         
          }
        else{
          Serial.println("Error: Not match any case"); 
          }        
      }  

    /* led R */    
    else if(String(_msg) == "ledR"){
        display.clear(); demos[led_logo](); display.display();
        buzzer_beep(1);

        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง

      /*เก็บค่า LED value แรก */  
      _cnt = 0;
      for(int i=0; i<sizeof(_msg); i++) _msg[i] = 0x00; //clear buffer

      /* correct value */
      do{
          _msg[_cnt++] = (char)msg[_index++]; 
      }while(_index < msglen);

      Serial.printf("Debug: _msg = %s\r\n",_msg);
      led_r_val = String(_msg).toInt();
      Serial.printf("led_r_val = %d[%d]\r\n",led_r_val,256*led_r_val/100);
      ledcWrite(1, 256*led_r_val/100);
      EEPROM.write(led_r_eeprom,led_r_val); //update eeprom     
      EEPROM.commit();  
      microgear.publish("/esp32/led_r",led_r_val); //update stat to netpie
      
      }  

    /* led G */    
    else if(String(_msg) == "ledG"){

        display.clear(); demos[led_logo](); display.display();
        buzzer_beep(1);

        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง

      /*เก็บค่า LED value แรก */  
      _cnt = 0;
      for(int i=0; i<sizeof(_msg); i++) _msg[i] = 0x00; //clear buffer

      /* correct value */
      do{
          _msg[_cnt++] = (char)msg[_index++]; 
      }while(_index < msglen);

      Serial.printf("Debug: _msg = %s\r\n",_msg);
      led_g_val = String(_msg).toInt();
      Serial.printf("led_g_val = %d[%d]\r\n",led_g_val,256*led_g_val/100);
      ledcWrite(2, 256*led_g_val/100);
      EEPROM.write(led_g_eeprom,led_g_val); //update eeprom     
      EEPROM.commit();  
      microgear.publish("/esp32/led_g",led_g_val); //update stat to netpie
      
      }  

    /* led B */    
    else if(String(_msg) == "ledB"){
        display.clear(); demos[led_logo](); display.display();
        buzzer_beep(1);

        _index++; //เลื่อน index ไปอีก 1 ตำแหน่ง

      /*เก็บค่า LED value แรก */  
      _cnt = 0;
      for(int i=0; i<sizeof(_msg); i++) _msg[i] = 0x00; //clear buffer

      /* correct value */
      do{
          _msg[_cnt++] = (char)msg[_index++]; 
      }while(_index < msglen);

      Serial.printf("Debug: _msg = %s\r\n",_msg);
      led_b_val = String(_msg).toInt();
      Serial.printf("led_b_val = %d[%d]\r\n",led_b_val,256*led_b_val/100);
      ledcWrite(3, 256*led_b_val/100);
      EEPROM.write(led_b_eeprom,led_b_val); //update eeprom     
      EEPROM.commit();  
      microgear.publish("/esp32/led_b",led_b_val); //update stat to netpie
      
      }  

                  
    else if(String(_msg) == "update"){

        netpie_refresh();
        
      } 
    else{
        Serial.println("Error: Not match any case"); 
        }  
        
}

/***********************************************************************
 * FUNCTION:    onFoundgear
 * DESCRIPTION: Microgear callback function when found new gear
 * PARAMETERS:  char *attribute, uint8_t* msg, unsigned int msglen
 * RETURNED:    nothing
 ***********************************************************************/
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();  
}

/***********************************************************************
 * FUNCTION:    onLostgear
 * DESCRIPTION: Microgear callback function when lost gear
 * PARAMETERS:  char *attribute, uint8_t* msg, unsigned int msglen
 * RETURNED:    nothing
 ***********************************************************************/
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
        Serial.print((char)msg[i]);
    Serial.println();
}

/***********************************************************************
 * FUNCTION:    onConnected
 * DESCRIPTION: Microgear callback function when connected
 * PARAMETERS:  char *attribute, uint8_t* msg, unsigned int msglen
 * RETURNED:    nothing
 ***********************************************************************/
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    /* Set the alias of this microgear ALIAS */
    microgear.setAlias(netpie_alias);
}

/***********************************************************************
 * FUNCTION:    netpie_refresh
 * DESCRIPTION: send status board to netpie to update on freeboard
 * PARAMETERS:  nothing
 * RETURNED:    nothing
 ***********************************************************************/
void netpie_refresh(void) {
  
        /*** update netpie status ***/
        display.clear(); demos[payuth_logo](); display.display();       
        buzzer_beep(1);
        /*output1*/
        if(output1_val == 0){
             microgear.publish("/esp32/output1","0"); //update stat to netpie
          }
        else{
             microgear.publish("/esp32/output1","1"); //update stat to netpie
          }  

        /*output2*/
        if(output2_val == 0){
             microgear.publish("/esp32/output2","0"); //update stat to netpie
          }
        else{
             microgear.publish("/esp32/output2","1"); //update stat to netpie
          }  

        /*output3*/
        if(output3_val == 0){
             microgear.publish("/esp32/output3","0"); //update stat to netpie
          }
        else{
             microgear.publish("/esp32/output3","1"); //update stat to netpie
          }  

        /*output4*/
        if(output4_val == 0){
             microgear.publish("/esp32/output4","0"); //update stat to netpie
          }
        else{
             microgear.publish("/esp32/output4","1"); //update stat to netpie
          }  

        /*led R*/
        microgear.publish("/esp32/led_r",led_r_val); //update stat to netpie

        /*led G*/
        microgear.publish("/esp32/led_g",led_g_val); //update stat to netpie

        /*led B*/
        microgear.publish("/esp32/led_b",led_b_val); //update stat to netpie  
  
  }

