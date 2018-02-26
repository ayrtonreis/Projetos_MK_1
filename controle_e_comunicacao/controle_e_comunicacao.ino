#include "Arduino.h"
#include <Servo.h>
#include <OneWire.h>    //usado no sensor de T
#include <DallasTemperature.h> 
#include "pitches.h"

#define RELAY1  3  
const int relayPin = 2;
const int pumpPin = 4;
Servo servo_1, servo_2;

// Temeparture sensor parameters
const int temp_sensor_pin = 6;
OneWire oneWire(temp_sensor_pin);
DallasTemperature temp_sensors(&oneWire);
DeviceAddress temp_sensor;

// Distance sensors parameters
const int trigPin_1 = 9, echoPin_1 = 10, trigPin_2 = 11, echoPin_2 = 12;
double duration_1, distance_1, duration_2, distance_2, sound_speed = 0.034029;

// Is the machine currently making coffee?
boolean active;

void setup() {
  active = false;
  
  // Servos
  servo_1.attach(7);
  servo_2.attach(8);
  
  // Temperature sensor
  temp_sensors.begin();
  if (!temp_sensors.getAddress(temp_sensor, 0))
    Serial.println("Sensor de temperatura nao encontrado.");

  // Distance sensors 
  pinMode(trigPin_1, OUTPUT);
  pinMode(echoPin_1, INPUT);
  pinMode(trigPin_2,OUTPUT);
  pinMode(echoPin_2, INPUT); 

  pinMode(pumpPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  Serial.begin(9600);
}

//Funcao para ligar a bomba
void turn_pump(int in){
  if (in == 'b') {
    digitalWrite(pumpPin, HIGH);
  }
  if (in == 'a') {
    digitalWrite(pumpPin, LOW);
  }
 
}

// Funcao para girar um servo.
void turn_servo(int servo_number)
{
  if(servo_number == 1)
 {
    if(servo_1.read() == 0)
      servo_1.write(180);
    else
      servo_1.write(0);
  }
  else if(servo_number == 2)
  {
    if(servo_2.read() == 0)
      servo_2.write(180);
    else
      servo_2.write(0);
  }
}

void relay(int on){
  if(on == 1){
    digitalWrite(relayPin, LOW);  //desativa o pino
  }
  else{
    digitalWrite(relayPin, HIGH); //aciona o pino
  }
}

// Funcao que retorna a temperatura do sensor.
float temperature()
{
  temp_sensors.requestTemperatures();
  return temp_sensors.getTempC(temp_sensor);
}

// Funcao que retorna a distancia em cm do sensor indicado.
double distance_cm(int sensor_number)
{
  if(sensor_number == 1)
  {
    digitalWrite(trigPin_1, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin_1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_1, LOW);

    duration_1 = pulseIn(echoPin_1, HIGH);
    distance_1 = duration_1*sound_speed/2;
    
    return distance_1;
  }
  else if(sensor_number == 2)
  {
    digitalWrite(trigPin_2, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin_2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_2, LOW);

    duration_2 = pulseIn(echoPin_2, HIGH);
    distance_2 = duration_2*sound_speed/2;
  
    return distance_2;
  }
}

void print_sensors(){
  Serial.print("Distancia sensor_1: ");
  Serial.print(distance_cm(1));
  Serial.println("cm");
  
  Serial.print("Distancia sensor_2: ");
  Serial.print(distance_cm(2));
  Serial.println("cm");
  
  Serial.print("Temperatura: ");
  Serial.print(temperature());
  Serial.println(" C");
  
  Serial.println();
  
  delay(500);
}


void loop() {

//  Serial.flush();
//  while(!Serial.available()){  //wait for character from raspi
//  }
  
  char commandChar = Serial.read();
  
  //delay(2); //this delay needs to be less than the one in the python code
  
  switch (commandChar) {
    case 's': //send sensors information
      send_sensors_data();
      break;
     case 'i': //start!
       music_start(24); //buzzer is in pin 24
       active = true;
       send_ok();
       break;
      case 'f': //finish!
        music_end(24); //buzzer is in pin 24
        active = false;
        send_ok("arduino finalized");
        break;
      case 'r': //turn on relay
        digitalWrite(RELAY1,0);
        send_ok("relay on");
        break;
      case 'y': //turn off relay
        relay(0);
        send_ok("relay off");
        break;
      case 'b': //turn on pump
        turn_pump('b');
        send_ok("relay pump on");
        break;
      case 'a': //turn off pump
        turn_pump('a');
        send_ok("relay pump off");
        break;
      case '1': //turn servo 1 by 180 deg
        turn_servo(1);
        send_ok("servo_1 turned 180 deg");
        break;
      case '2': //turn servo 2 by 180deg
        turn_servo(2);
        send_ok("servo_2 turned 180 deg");
        break;
      
     //default:
  } 
  commandChar = 0;
}

void send_sensors_data(){ //d_sugar, d_coffee, d_milk, temperature, state
  Serial.println("0;" +String(distance_cm(1))+ ";" +String(distance_cm(2))+ ";" +String(temperature()));
}


//send_ok is an overloaded function
void send_ok(){
   Serial.println("ok!");
}
void send_ok(String msg){
   Serial.println("ok! " + msg);
}
