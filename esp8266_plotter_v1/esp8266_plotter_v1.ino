#include "wifi_setup.h"
#include <Servo.h>

#define WIFI_SSID "your_ssid"
#define WIFI_PW "your_pw"

//example received data
//234-34;191-48;157-65;148-71;140-77;131-82;122-88;117-97;

//scara parallel arm robot parameters
#define L1 65
#define L2 65
#define D 22
#define X_OFFSET -24
#define Y_OFFSET 38
#define A_OFFSET 57-90
#define B_OFFSET 119-90

#define move_delay 50

//servo & led
#define LED 2
#define PWM_A D0
#define PWM_B D1
#define PWM_Z D2
#define PEN_UP 120
#define PEN_DOWN 50
Servo servo_a;
Servo servo_b;
Servo servo_z;

#define PI 3.14159265359
#define Rad2Deg 180/PI

void setup(){
  Serial.begin(9600);
  start("KabelBox-E84E","67534404306756752453");
  pinMode(LED,OUTPUT);
  digitalWrite(LED,HIGH);
  servo_a.attach(PWM_A);
  servo_b.attach(PWM_B);
  servo_z.attach(PWM_Z);
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
  //interpolate move2 (bresensham line algorithm)
  //step along the long side and correct the shorter side if 
  long i = 0; //steps in long direction
  long over;
  long dx = next_x - x;
  long dy = next_y - y;
  int dirx = dx>0 ? 1:-1;
  int diry = dy>0 ? 1:-1;
  dx = abs(dx);
  dy = abs(dy);

  if(dx>dy){
    over = dx/2;
    for(i=0; i<dx; i++){
      over += dy;
      x += dirx;
      if(over>=dx){
        over -= dx;
        y += diry;
      }
      move2(x,y);
      delay(move_delay);
    }
  }
  else{
    over = dy/2;
    for(i=0; i<dy; i++){
      over += dx;
      y += diry;
      if(over>=dy){
        over -= dy;
        x += dirx;
      }
      move2(x,y);
      delay(move_delay);
    }
  }
  servo_z.write(PEN_DOWN); //pen down
}


void move2(float x, float y){
  
  Serial.print(String(x));
  Serial.print(' ');
  Serial.print(String(y));
  Serial.print(' ');
  
  x += X_OFFSET;
  y += Y_OFFSET;
  
  //calculate
  float c = sqrt(sq(x)+sq(y));
  float e = sqrt(sq(D-x)+sq(y));
  float delta = atan2(y,x);
  float psi = atan2(y,(D-x));
  float temp = sq(L1)-sq(L2);
  float epsillon = acos((sq(e)+temp)/(2*L1*e));
  float gamma = acos((sq(c)+temp)/(2*L1*c));
  float alpha = (delta + gamma)*Rad2Deg;
  float beta = (PI - psi - epsillon)*Rad2Deg;
  
  Serial.print(String(alpha));
  Serial.print(' ');
  Serial.println(String(beta));
  
  alpha = alpha +A_OFFSET;
  beta = beta +B_OFFSET;
  
  //move
  servo_a.write((uint8_t)alpha);
  servo_b.write((uint8_t)beta);
}
