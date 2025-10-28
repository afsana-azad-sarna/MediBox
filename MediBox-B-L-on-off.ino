#include <ESP8266WiFi.h>
#include <Servo.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define FIREBASE_HOST "medibox-45997-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "jvl3BVlcV7frNx9wmv2yqOodTUlTnoVhaJj1mL4t"
#define WIFI_SSID "Rafiqul Jakir" //provide ssid (wifi name)
#define WIFI_PASSWORD "11223344" //wifi password



Servo servo1, servo2, servo3, servo4, servo5, servo6;
int LED_pin = 15; // LED pin D8
int buzzerPin = 16; // D0

int switchPin = 5; // D1
int drawers[] = {4, 0, 2, 14, 12, 13}; // Drawer pins(D2, D3,D4,D5,D6,D7)

int drawer_Open_Duration_Time = 300000; // 5 minutes in milliseconds
String sokale_khawar_age, sokale_khawar_pore, dupure_khawar_age, dupure_khawar_pore, rate_khawar_age, rate_khawar_pore;
String p_sokale_khawar_age, p_sokale_khawar_pore, p_dupure_khawar_age, p_dupure_khawar_pore, p_rate_khawar_age, p_rate_khawar_pore;
String buzzer_status, led_status;


FirebaseData firebaseData;
FirebaseJson json;
const long utcOffsetInSeconds = 21600; // UTC offset for your timezone (in seconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


unsigned long startTime , elapsedTime, Alarm_auto_off_time; // Variable to store the start time 
unsigned long lastAlarmOffTime = 0;



void setup() {
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  pinMode(switchPin, INPUT_PULLUP);
  for(int i = 0; i < 6; i++) {
    pinMode(drawers[i], OUTPUT);
  }

  // Attach servo to pins
  servo1.attach(drawers[0],500,2500); 
  servo2.attach(drawers[1],500,2500);
  servo3.attach(drawers[2],500,2500);
  servo4.attach(drawers[3],500,2500);
  servo5.attach(drawers[4],500,2500);
  servo6.attach(drawers[5],500,2500);

  // ALl Drawer Close
  close_Drawer(servo1,179, 180);
  close_Drawer(servo2,179, 180);
  close_Drawer(servo3,179, 180);
  close_Drawer(servo4,179, 180);
  close_Drawer(servo5,179, 180);
  close_Drawer(servo6,179, 180);

  Serial.println("All Drawe close \n");
  
  connectToWiFi();
  configTime(0, 0, "pool.ntp.org", "5");

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);


  timeClient.begin();
  timeClient.update();


  


}

void loop() {
  startTime = millis();
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  String currentTime = convertTo12HourFormat(currentHour,currentMinute);

  String path;
  String date = today_date();

  if(date != NULL){
    path = "/1694256957188366/" + date;
  }
  else{
    return;
  }


  p_sokale_khawar_age = path + "/Morning Before";
  p_sokale_khawar_pore = path + "/Morning After";
  p_dupure_khawar_age = path + "/Afternoon Before";
  p_dupure_khawar_pore = path + "/Afternoon After";
  p_rate_khawar_age = path + "/Night Before";
  p_rate_khawar_pore = path + "/Night After";

  //Drawer opening Time Scheduling (set From Android APp)
  sokale_khawar_age = get_target_time("/Patient-Time-Scheduling/1694256957188366/Morning Before");
  sokale_khawar_pore = get_target_time("/Patient-Time-Scheduling/1694256957188366/Morning After");
  dupure_khawar_age = get_target_time("/Patient-Time-Scheduling/1694256957188366/Afternoon Before");
  dupure_khawar_pore = get_target_time("/Patient-Time-Scheduling/1694256957188366/Afternoon After");
  rate_khawar_age = get_target_time("/Patient-Time-Scheduling/1694256957188366/Night Before");
  rate_khawar_pore = get_target_time("/Patient-Time-Scheduling/1694256957188366/Night After");





  Serial.print(currentTime + " = " + sokale_khawar_age + "\n");
  Serial.print(currentTime + " = " + sokale_khawar_pore + "\n");
  Serial.print(currentTime + " = " + dupure_khawar_age + "\n");
  Serial.print(currentTime + " = " + dupure_khawar_pore + "\n");
  Serial.print(currentTime + " = " + rate_khawar_age + "\n");
  Serial.print(currentTime + " = " + rate_khawar_pore + "\n");
  Serial.print("Buzzer = " + buzzer_status + "\n");
  Serial.print("LED = " + led_status + "\n");

  if(currentTime.equals(sokale_khawar_age )) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000 ) {
        return;
      }


      open_Drawer(servo1,90,180,"servo1");

  } else if(currentTime.equals(sokale_khawar_pore) ) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000) {
        return;
      }


      open_Drawer(servo2,80,180,"servo2");

  }  else if(currentTime.equals(dupure_khawar_age) ) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000) {
        return;
      }

      open_Drawer(servo3,60,180,"servo3");

  } else if(currentTime.equals(dupure_khawar_pore)) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000) {
        return;
      }

      open_Drawer(servo4,40,180,"servo4");

  } else if(currentTime.equals(rate_khawar_age) ) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000) {
        return;
      }

      open_Drawer(servo5,60,180,"servo5");

  } else if(currentTime.equals(rate_khawar_pore) ) {

      if (millis() - lastAlarmOffTime < 30000 || millis() - Alarm_auto_off_time < 30000) {
        return;
      }

      open_Drawer(servo6,65,180,"servo6");
  }else{
    Serial.println("Test success\n");
  }
  
}


//--------------------------------------------------------------
void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.println("Connecting...");

  }
  Serial.println("Connected to WiFi");

}



//--------------------------------------------------------------
void open_Drawer(Servo servo, int Min_Angle, int Max_Angle, String servo_name){

  for (int angle = Max_Angle; angle >= Min_Angle; angle -= 1) {
    servo.write(angle);
    delay(15);               
  } 

  Serial.println(servo_name + " Activated Successfully\n");

  while(true) {
    Serial.println("Inside Loop \n");
    buzzer_status = buzzer_led_status("/Patient-Time-Scheduling/1694256957188366/buzzer");
    led_status = buzzer_led_status("/Patient-Time-Scheduling/1694256957188366/led");
    if(buzzer_status == "on"){
      tone(buzzerPin, 1000); // Turn on buzzer
      delay(1000);
      noTone(buzzerPin); // Turn off buzzer
    }
    if(led_status == "on"){
      digitalWrite(LED_pin, HIGH);
      delay(1000);// wait for a second
      digitalWrite(LED_pin, LOW);// turn the LED off by making the voltage LOW
    }


    if(digitalRead(switchPin) == LOW) { // switch pin LOW means Switch is ON

      // Patient pressed the switch indicating medicine taken
      lastAlarmOffTime = millis();

      if(servo_name == "servo1"){

          Firebase.setString(firebaseData, p_sokale_khawar_age, "true");

      }else if(servo_name.equals("servo2")){

          Firebase.setString(firebaseData, p_sokale_khawar_pore, "true");

      }else if(servo_name.equals("servo3")){

          Firebase.setString(firebaseData, p_dupure_khawar_age, "true");

      }else if(servo_name.equals("servo4")){

          Firebase.setString(firebaseData, p_dupure_khawar_pore, "true");

      }else if(servo_name.equals("servo5")){

          Firebase.setString(firebaseData, p_rate_khawar_age, "true");

      }else if(servo_name.equals("servo6")){

          Firebase.setString(firebaseData, p_rate_khawar_pore, "true");

      }

      close_Drawer(servo,Min_Angle, Max_Angle);

      break; // Exit the loop if switch is pressed

    }else{
      if(servo_name == "servo1"){

        Firebase.setString(firebaseData, p_sokale_khawar_age, "false");
          
      }else if(servo_name.equals("servo2")){

        Firebase.setString(firebaseData, p_sokale_khawar_pore, "false");

      }else if(servo_name.equals("servo3")){

        Firebase.setString(firebaseData, p_dupure_khawar_age, "false");

      }else if(servo_name.equals("servo4")){

        Firebase.setString(firebaseData, p_dupure_khawar_pore, "false");

      }else if(servo_name.equals("servo5")){

        Firebase.setString(firebaseData, p_rate_khawar_age, "false");

      }else if(servo_name.equals("servo6")){

        Firebase.setString(firebaseData, p_rate_khawar_pore, "false");
      }

    }


    unsigned long currentTime = millis(); // Get the current time
    elapsedTime = (currentTime - startTime); // Calculate elapsed time in milisecond
    if (elapsedTime >= 30000) {
      Serial.println("\n loop Break Successfully(Time COunter)\n");
      close_Drawer(servo,Min_Angle, Max_Angle);
      Alarm_auto_off_time = millis();
      break; // Break Loop
    }
    else{
      Serial.println("elapsedTime = " + String(elapsedTime/1000));
      Serial.println("\n loop Not break Break(Time COunter)\n");
    }
    
  }  
  
  Serial.println("\n Drawer Close Successfully(outside loops)\n");
}



//-----------------------------------------------------------------------
void close_Drawer(Servo servo, int Min_Angle, int Max_Angle){

  for (int angle = Min_Angle; angle <= Max_Angle; angle += 1) {
    servo.write(angle);    
    delay(15);
  }

}



//------------------------------------------------------------------------
String get_target_time(String path){

  if (Firebase.getString(firebaseData, path)) { // "/user/kawser/Time"
    if (firebaseData.dataType() == "string") {
      String timeString = firebaseData.stringData();

      // Extract hours and minutes from the time string
      int hours, minutes;
      char amPm[3];
      sscanf(timeString.c_str(), "%d:%d %2s", &hours, &minutes, amPm);

      String target_time = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + " " + amPm;
      return  target_time;
    }
  } else {
    
    Serial.println("Error getting value");
    Serial.println(firebaseData.errorReason());

  }
}
//----------------------------------------------------------------
String buzzer_led_status(String path){
    if (Firebase.getString(firebaseData, path)) {
    if (firebaseData.dataType() == "string") {
      String status = firebaseData.stringData();
      return status;
    }
   }
}


//-----------------------------------------------------------------
String convertTo12HourFormat(int hour, int minute) {
    String period = (hour >= 12) ? "PM" : "AM";

    if (hour > 12)
        hour -= 12;
    else if (hour == 0)
        hour = 12;

    return String(hour) + ":" + (minute < 10 ? "0" : "") + String(minute) + " " + period;
}


//------------------------------------------------------------------
String today_date(){
  time_t now = time(nullptr);
  struct tm * timeinfo = localtime(&now);

  int year = timeinfo->tm_year + 1900;
  int month = timeinfo->tm_mon + 1;
  int day = timeinfo->tm_mday;
  String date;
 
  if(year > 1970){
    date = (day < 10 ? "0" : "") + String(day) + "-" + (month < 10 ? "0" : "") + String(month) + "-" + String(year);
    return date;
  }

}
//-------------------------------------------------------------------
