//Included Libraries
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//Defines
#define button_pin      2
#define right_servo_pin 5
#define left_servo_pin  6
#define right_led       7
#define left_led        8
#define left_qti        A0
#define middle_qti      A1
#define right_qti       A2
#define min_pulse       1300
#define max_pulse       1700
#define standstill      1500
#define qti_threshold   387

//Servo variables
Servo g_left_wheel;
Servo g_right_wheel;

//ButtonPress variable
int buttonPress = 0;

//Right turn variables
int rightT = 23, endJunction = 0;
boolean finish = false;

//Time variables
boolean bPress = false;
unsigned long time1 = 0;
unsigned long time2 = 0;

//Lengths of the squares on the LCD screen in pixels
int sqrLength1 = 7;  
int sqrLength2 = 7;
int sqrLength3 = 7;
int sqrLength4 = 7;
//Widths of the squares on the LCD screen in pixels
int sqrWidth1 = 2; 
int sqrWidth2 = 4;
int sqrWidth3 = 2;
int sqrLength21difference;
//Reversals
int reversal = 0;
boolean sqrChange1 = false;

//LCD variables
byte point1[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte point2[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte point3[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte point4[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte point5[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};


//Private functions

//Funtion to set the speed of the wheels
void setWheels(int delay_left = 1500, int delay_right = 1500) { 
  g_left_wheel.writeMicroseconds(delay_left);
  g_right_wheel.writeMicroseconds(delay_right);
  delay(20);
}

//Functions for directions
void turnRight(){
  setWheels(1550, 1550);
}
void uTurn(){
  setWheels(1550, 1550);
  delay(2000);
}
void turnLeft(){
  setWheels(1450, 1450);
}
void goStraight(){
  setWheels(1550, 1450);
}
void stop(){
  setWheels(1500, 1500);
}
void reverse(){
  setWheels(1400, 1600);
}

// Function to read current position on map
byte readQti (byte qti) {                               
  digitalWrite(qti, HIGH);                            
  delayMicroseconds(500);                              
  digitalWrite(qti, LOW);                              
  return ( analogRead(qti) > qti_threshold ? 1 : 0);     
}

//Function to read the button press
void ButtonRead(){
  int reading = digitalRead(button_pin); 
  if (reading == HIGH){
     lcd.clear();
     buttonPress++;
     goStraight();
     delay(200);
    }
}

//Timer function
void timer(){
  time2 = millis();
  time2 -= time1;
}

//LED lights function
void setLed(byte value_left = LOW, byte value_right = LOW) {
  digitalWrite(right_led, value_right);
  digitalWrite(left_led, value_left);
}

void setup() {
  //Start serial monitor
  Serial.begin(9600);
  //Attaches servos to digital pins
  g_left_wheel.attach(left_servo_pin);
  g_right_wheel.attach(right_servo_pin);
  //Sets wheels to standstill
  setWheels();
  // Start LCD and display static info
  lcd.begin(16, 2);
  lcd.clear();
  lcd.createChar(5, point5);
  
  lcd.setCursor(0,0);
  lcd.write('W');
  lcd.write('A');
  lcd.write('I');
  lcd.write('T');
  lcd.write('I');
  lcd.write('N');
  lcd.write('G');
  
  lcd.write('.');
  lcd.write('.');
  lcd.write('.');
}

void loop(){
  ButtonRead();
  //Checks for the button press
  if(buttonPress & 1){ 
    //Saves the time when the button was pressed
    if(bPress == false){
      time1 = millis();
      bPress = true;
    }
    //Starts to take the time from 0 seconds
    timer();
    //All QTI sensors report back the color black. Checks if it is a junction or the finish line(drives straight for a bit to check)
    if(readQti(left_qti) && readQti(middle_qti) && readQti(right_qti)){
      goStraight();
      delay(250);
      //All black again = robot has reached the finish line
      if(readQti(left_qti) && readQti(middle_qti) && readQti(right_qti)){
        if(rightT <= 0){
          buttonPress = 2;
          setLed(HIGH, HIGH);
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.write(1);
          lcd.setCursor(1, 1);
          lcd.write(2);
          lcd.setCursor(1, 0);
          lcd.write(3);
          lcd.setCursor(0, 0);
          lcd.write(4);
        }
        //Remembers that the robot reached the finish line at least once
        else{
          uTurn();
          finish = true;
        }
      }
      else if(!readQti(left_qti) && readQti(middle_qti) && !readQti(right_qti)){
        if(finish == true && rightT <=0){
          if(endJunction < 2){
            goStraight();
            delay(100);
            endJunction++;
          }
          else{
            goStraight();
            delay(100);
            turnLeft();
            delay(1100);
          }
        }
        //If the left and right QTI sensors report white and the middle reports black then the robot is in a junction
        //Turns right
        else{
          rightT--;
          goStraight();
          delay(100);
          turnRight();
          delay(1100);
        }
      }
    }
    //If all QTI sensors report white then the robot turns around
    else if(!readQti(left_qti) && !readQti(middle_qti) && !readQti(right_qti)){
      reversal++;
      uTurn();
      reverse();
      delay(100);
    }
    //If the left reports white and the right reports black, turns right
    else if(!readQti(left_qti) && readQti(right_qti)){
      turnRight();
    }
    //If the left reports black and the right reports white, turns left
    else if(readQti(left_qti) && !readQti(right_qti)){
      turnLeft();
    }
    //If the left reports white and the right reports white, goes straight
    else if(!readQti(left_qti) && !readQti(right_qti)){
      goStraight();
    }
    if(time2 > 550){
        switch (rightT){
          case 23:
            time1 = millis();
            point1[sqrLength1] += 4;
            sqrLength1--;
            lcd.createChar(1, point1);
            break;
          case 22:
            time1 = millis();
            sqrWidth1--;
            point1[sqrLength1] += pow(2, sqrWidth1);
            lcd.createChar(1, point1);
            lcd.setCursor(0, 1);
            lcd.write(5);
            lcd.write(5);
            lcd.write(5);
            lcd.write(5);
            break;
          case 21:
            time1 = millis();
            if(sqrChange1
           == false){
               sqrLength2 = sqrLength1;
               sqrChange1
               = true;
            }
            point2[sqrLength2] += 16;
            sqrLength2++;
            lcd.createChar(2, point2);
            lcd.setCursor(4, 1);
            lcd.write(5);
            break;
          case 20:
            time1 = millis();
            if(reversal == 0){
              sqrLength21difference = sqrLength2 - sqrLength1;
              point1[sqrLength21difference] += 1;
              lcd.createChar(1, point1);
            }
            lcd.setCursor(5, 1);
            lcd.write(5);
            lcd.write(5);
            break;
          case 19:
            time1 = millis();
            if(reversal == 1){
              point2[sqrLength2] += 16;
              sqrLength2++;
              lcd.createChar(2, point2);
            }
            else{
              sqrLength2--;
            }
            lcd.setCursor(7, 1);
            lcd.write(5);
            break;
          case 18:
            time1 = millis();
            sqrWidth2--;
            point2[sqrLength2] += pow(2, sqrWidth2);
            lcd.createChar(2, point2);
            lcd.setCursor(8, 1);
            lcd.write(5);
            lcd.write(5);
            break;
          case 17:
            time1 = millis();
            if(reversal == 2){
              point2[sqrLength2] += pow(2, sqrWidth2);
              sqrLength2++;
              lcd.createChar(2, point2);
            }
            else{
              sqrLength2--;
            }
            lcd.setCursor(10, 1);
            lcd.write(5);
            lcd.write(5);
            break;
          case 16:
            time1 = millis();
            if(reversal == 3){
              sqrWidth2--;
              point2[sqrLength2] += pow(2, sqrWidth2);
              lcd.createChar(2, point2);
            }
            else{
              sqrWidth2++;
            }
            lcd.setCursor(12, 1);
            lcd.write(5);
            lcd.write(5);
            break;
          case 15:
            time1 = millis();
            if(reversal == 4){
              point2[sqrLength2] += pow(2, sqrWidth2);
              lcd.createChar(2, point2);
            }
            else{
              sqrLength2++;
            }
            lcd.setCursor(14, 1);
            lcd.write(5);
            break;
          case 14:
            time1 = millis();
            sqrWidth2++;
            lcd.setCursor(15, 1);
            lcd.write(5);
            break;
          case 13:
            time1= millis();
            sqrLength2--;
            break;
          case 12:
            time1 = millis();
            point2[sqrLength2] += pow(2, sqrWidth2);
            lcd.createChar(2, point2);
            break;
          case 11:
            time1 = millis();
            if(reversal == 5){
              point2[sqrLength2] += pow(2, sqrWidth2);
              sqrLength2++;
              lcd.createChar(2, point2);
            }
            else{
              sqrLength2--;
            }
            lcd.setCursor(0, 0);
            lcd.write(5);
            lcd.write(5);
            break;
          case 10:
            time1 = millis();
            if(reversal == 6){
              point2[sqrLength2] += pow(2, sqrWidth2);
              sqrWidth2--;
              lcd.createChar(2, point2);
            }
            else{
              sqrWidth2++;
            }
            lcd.setCursor(2, 0);
            lcd.write(5);
            break;
          case 9:
            time1 = millis();
            if(sqrLength2 >= 0){
              point2[sqrLength2] += pow(2, sqrWidth2);
              sqrLength2--;
            }
            else if(sqrLength2 <= -1 && finish == false){
              point3[sqrLength3] += pow(2, sqrWidth2);
              sqrLength3--;
              lcd.createChar(3, point3);
              lcd.setCursor(1, 1);
            }
            else if(finish == true && sqrLength3 <= 7){
              sqrLength3++;
            }
            else if(sqrLength3 >= 7){
              sqrLength2++;
            }
            lcd.setCursor(3, 0);
            lcd.write(5);
            break;
          case 8:
            time1 = millis();
            sqrWidth2++;
            lcd.setCursor(4, 0);
            lcd.write(5);
            lcd.write(5);
            lcd.write(5);
            lcd.write(5);
            break;
          case 7:
            time1 = millis();
            if(reversal == 7){
              sqrLength2--;
              point2[sqrLength2] += pow(2, sqrWidth2);
              lcd.createChar(2, point2);
            }
            else{
              sqrLength2++;
            }
            break;
          case 6:
            time1 = millis();
            sqrWidth1++;
            break;
          case 5:
            time1 = millis();
            if(sqrLength1 >= 0){
              sqrLength1--;
              point1[sqrLength1] += pow(2, sqrWidth1);
              lcd.createChar(1, point1);
            }
            else{
              lcd.setCursor(0, 0);
              point4[sqrLength4] += pow(2, sqrWidth1);
              sqrLength4--;
              lcd.createChar(4, point4);
            }
            lcd.setCursor(8, 0);
            lcd.write(5);
            break;
          case 4:
            time1 = millis();
            if(reversal == 8){
              sqrWidth3--;
              point4[sqrLength4] += pow(2, sqrWidth3);
              lcd.createChar(4, point4);
            }
            else{
              sqrWidth3++;
            }
            lcd.setCursor(9, 0);
            lcd.write(5);
            lcd.write(5);
            lcd.write(5);
            break;
          case 3:
            time1 = millis();
            if(reversal == 9){
              sqrLength4--;
              point4[sqrLength4] += pow(2, sqrWidth3);
              lcd.createChar(4, point4);
            }
            else{
              sqrLength4++;
            }
            lcd.setCursor(12, 0);
            lcd.write(5);
            break;
          case 2:
            time1 = millis();
            if(reversal == 10){
              sqrWidth3++;
              point4[sqrLength4] += pow(2, sqrWidth3);
              lcd.createChar(4, point4);
            }
            else{
              sqrWidth3--;
            }
            lcd.setCursor(13, 0);
            lcd.write(5);
            break;
          case 1:
            time1 = millis();
            if(sqrLength4 >= 0){
              sqrLength4++;
            }
            else{
              sqrLength1++;
            }
            lcd.setCursor(14, 0);
            lcd.write(5);
            break;
          case 0:
            time1 = millis();
            if(reversal == 11){
              lcd.setCursor(0, 1);
              sqrWidth1++;
              point1[sqrLength1] += pow(2, sqrWidth1);
              lcd.createChar(1, point1);
            }
            lcd.setCursor(15, 0);
            lcd.write(5);
            break;
        }
      }
  }
  else{
    stop();
  }
}
