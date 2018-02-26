// Programa para controlar 2 servos, 1 sensor de temperatura e 2 sensores de distancia.
// Conectar:
// Servo 1: pin 7
// Servo 2: pin 8
// Sensor de distancia 1: trig em pin 9, echo em pin 10
// Sensor de distancia 2: trig em pin 11, echo em pin 12
// Sensor de temperatura: pin 6

#include "Arduino.h"
#include <Servo.h>
#include <OneWire.h>    //usado no sensor de T
#include <DallasTemperature.h>  


const int RelePin = 2;
const int pumpPin = 4;
Servo servo_1, servo_2;

// Para o sensor de temperatura.
const int temp_sensor_pin = 6;
OneWire oneWire(temp_sensor_pin);
DallasTemperature temp_sensors(&oneWire);
DeviceAddress temp_sensor;

// Para os sensores de distancia.
const int trigPin_1 = 9, echoPin_1 = 10, trigPin_2 = 11, echoPin_2 = 12;
double duration_1, distance_1, duration_2, distance_2, sound_speed = 0.034029;

// Configuracoes iniciais.
void setup() 
{
  Serial.begin(9600);
  
  // Para os servos.
  servo_1.attach(7);
  servo_2.attach(8);
  
  // Para o sensor de temperatura.
  temp_sensors.begin();
  if (!temp_sensors.getAddress(temp_sensor, 0))
    Serial.println("Sensor de temperatura nao encontrado.");

  // Para os sensores de distancia.
  pinMode(trigPin_1, OUTPUT);
  pinMode(echoPin_1, INPUT);
  pinMode(trigPin_2,OUTPUT);
  pinMode(echoPin_2, INPUT); 

  pinMode(pumpPin, OUTPUT);
  pinMode(RelePin, OUTPUT);
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
  if(servo_number == '1')
 {
    if(servo_1.read() == 0)
      servo_1.write(180);
    else
      servo_1.write(0);
    Serial.println("Servo_1 acionado.");
  }
  else if(servo_number == '2')
  {
    if(servo_2.read() == 0)
      servo_2.write(180);
    else
      servo_2.write(0);
    Serial.println("Servo_2 acionado.");
  }
}

void relay(int on){
  if(on == 1){
    digitalWrite(RelePin, LOW);  //desativa o pino
  }
  else{
    digitalWrite(RelePin, HIGH); //aciona o pino
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

// Funcao principal.
void loop()
{
  float temp = temperature();
  char command = Serial.read();

  if(command == 'r'){
    Serial.print("Liga rele");
    relay(1);
  }
  else if(command == 'y'){
    relay(0);
  }
  else if(command == 'b' || command == 'a'){
    turn_pump(command);
  }
  else{
    turn_servo(command);
  }

  if(temp > 50){
    relay(0);
  }

  print_sensors();

}
