#include "wifi_setup.h"
#include <Servo.h>

#define WIFI_SSID "your_ssid"
#define WIFI_PW "your_pw"

//example received data
//234-34;191-48;157-65;148-71;140-77;131-82;122-88;117-97;

#define PI 3.14159265359
#define Rad2Deg 180/PI
#define Deg2Rad PI/180

//scara parallel arm robot parameters
#define L1 64
#define L2 85
#define L3 13
#define VARTHETA 120*Deg2Rad
//#define L2_ 92.19
#define L2_ sqrt(sq(L2)+sq(L3)-2*L2*L3*cos(VARTHETA))
//#define VARPHI 7.014522784
#define VARPHI acos((-sq(L3)+sq(L2_)+sq(L2))/(2*L2*L2_))
#define D 36
#define X_OFFSET -30
#define Y_OFFSET 105
#define A_OFFSET -4//-4
#define B_OFFSET +6//+3
#define SCALEING 92/90

#define move_delay 50
 
//servo & led
#define LED 2
#define PWM_A D0
#define PWM_B D1
#define PWM_Z D2
#define PEN_UP 50
#define PEN_DOWN 120
Servo servo_a;
Servo servo_b;
Servo servo_z;


void setup(){
  Serial.begin(9600);
  start("KabelBox-E84E","67534404306756752453");
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  servo_a.attach(PWM_A);
  servo_b.attach(PWM_B);
  servo_z.attach(PWM_Z);
  servo_z.write(PEN_UP); //pen up
}

void loop(){
  //init: move to 0
  float x = 0; float y = 0;
  move2(x,y);
  
  while(true){
    //get request
    wait4req = true;
    do{check4req();} while(wait4req == true);
  
    send2client("ack");
  
    //prepare pen
    servo_z.write(PEN_UP); //pen up
  
    //process request
    String data;
    bool process = false; //first find data entry point, than process
    while(client.available()>0){
      digitalWrite(LED,LOW); //led on
      if (process){
        //get one coordinate
        data = client.readStringUntil(';');
        int addr_start = data.indexOf('-');
  
        float next_x = data.substring(0,addr_start).toInt() /10;
        float next_y = data.substring(addr_start + 1).toInt() /10;
        
        Serial.print('x');
        Serial.print(String(next_x));
        Serial.print('-');
        Serial.print('y');
        Serial.print(String(next_y));
        Serial.println(';');
        
        trace_line(next_x,next_y,x,y);
        x = next_x;
        y = next_y;
      }
      else{
        data = client.readStringUntil('\r');
        //Serial.println(data);
        //if (data.indexOf("Connection: close")>0){
        if (data == "\nConnection: close"){
        Serial.println("data: ");
        process = true;
        }
      }
    }
    servo_z.write(PEN_UP); //pen up
    digitalWrite(LED,HIGH); //led off
  }
}

void trace_line(float next_x, float next_y, float x, float y){
  //and execute function move2() on every step
  //the algorithm steps along the long side and correct the shorter side if the acculuator "over" reaches a certain value
 
  //go to initial position and move pen down (before tracing):
  move2(x,y);
  delay(move_delay);
  servo_z.write(PEN_DOWN); //pen down
  
  long i = 0; //steps in long direction
  long over; //accumulator
  //distances in x & y
  long dx = next_x - x;
  long dy = next_y - y;
  //directions:
  int dirx = dx>0 ? 1:-1;
  int diry = dy>0 ? 1:-1;
  dx = abs(dx);
  dy = abs(dy);

  //trace
  if(dx>dy){
    over = dx/2;
    for(i=0; i<dx; i++){
      over += dy;
      x += dirx; //take steps in x direction
      if(over>=dx){
        over -= dx;
        y += diry; //correct in y direction
      }
      move2(x,y);
      delay(move_delay);
    }
  }
  else{
    over = dy/2;
    for(i=0; i<dy; i++){
      over += dx;
      y += diry; //take steps in y direction
      if(over>=dy){
        over -= dy;
        x += dirx; //correct in x direction
      }
      move2(x,y);
      delay(move_delay);
    }
  }
  servo_z.write(PEN_DOWN); //pen down
}


void move2(float x_, float y_){
  
  Serial.print(" x_ ");
  Serial.print(String(x_));
  Serial.print(" y_: ");
  Serial.println(String(y_));
  
  x_ += X_OFFSET;
  y_ += Y_OFFSET;
  
  //calculate
  float delta_ = atan2(y_,x_);
  float c_ = sqrt(sq(x_)+sq(y_));
  float gamma_ = acos((-sq(L2_)+sq(c_)+sq(L1))/(2*L1*c_));
  float alpha = delta_ + gamma_;
  float phi = acos((-sq(c_)+sq(L1)+sq(L2_))/(2*L1*L2_)) - VARPHI;
  float theta = alpha + phi - VARTHETA;
  float x = x_+L3*sin(theta-PI/2);
  float y = y_-L3*cos(theta-PI/2);
  float e = sqrt(sq(D-x)+sq(y));
  float psi = atan2(y,(D-x));
  float epsillon = acos((-sq(L2)+sq(e)+sq(L1))/(2*L1*e));
  float beta = (PI - psi - epsillon)*Rad2Deg;
  alpha = alpha*Rad2Deg;
  /*
  Serial.print(" delta_ ");
  Serial.print(String(delta_));
  Serial.print(" c_ ");
  Serial.print(String(c_));
  Serial.print(" gamma_ ");
  Serial.print(String(gamma_));
  Serial.print(" alpha_ ");
  Serial.print(String(alpha));
  Serial.print(" phi ");
  Serial.print(String(phi));
  Serial.print(" theta ");
  Serial.print(String(theta));
  Serial.print(" x ");
  Serial.print(String(x));
  Serial.print(" y ");
  Serial.print(String(y));
  Serial.print(" e ");
  Serial.print(String(e));
  Serial.print(" psi ");
  Serial.print(String(psi));
  Serial.print(" epsilon ");
  Serial.print(String(epsillon));
  Serial.print(" beta ");
  Serial.println(String(beta));
  */
  alpha = alpha +A_OFFSET;
  beta = beta +B_OFFSET;
  
  //move
  servo_a.write((uint8_t)alpha*SCALEING);
  servo_b.write((uint8_t)beta*SCALEING);
}
