#include "HBridge.h"
#include "Wire.h"
//#include "L3G.h"
#include "ITG3200.h"

ITG3200 gyro = ITG3200();

long lastMillis, curMillis;

float rates[3] = {
  0};
float angle[3] = {
  0};

float linearVelocity, angularVelocity;

void setup()
{
  Serial.begin(9600);

  Serial.println("Startup!");

  Wire.begin();

  //Serial.print("Initializing gyro... ");
  //if(!gyro.init()) Serial.println("Failed to init gyro!");

  //gyro.enableDefault();

  gyro.init(ITG3200_ADDR_AD0_HIGH);

  delay(1000);

  Serial.println("Calibrating gyro!");
  gyro.zeroCalibrate(500, 2);

  Serial.println("Init motors!");
  initMotors(3,9, 6,10, 12,12);
  enableMotor(MOTOR_A | MOTOR_B);

  lastMillis = 0;

  linearVelocity = 0.00003;
  angularVelocity = 0;

//  setMotor(MOTOR_A, MOTOR_FULL_FORWARD);
//  setMotor(MOTOR_B, MOTOR_FULL_REVERSE);
//  while(1)
//    delay(100);
}

void loop()
{
  curMillis = millis();

  float dt = ((float)(curMillis - lastMillis))/1000;

  if(gyro.isRawDataReady())
  {
    gyro.readGyro(rates);

    //gyro.read();
    //Serial.print(rates[2]);

    //  rates[0] = gyro.g.x;
    //  rates[1] = gyro.g.y;
    //  rates[2] = gyro.g.z;

    for(int i = 0; i < 3; i++)
    {
      angle[i] += (rates[i]/100000 + angularVelocity) * dt; 
    }
  }

  //  setMotor(MOTOR_A, MOTOR_FULL_FORWARD);
  //  setMotor(MOTOR_B, MOTOR_FULL_FORWARD);
  //
  //  delay(10000);
  //  
  //  setMotor(MOTOR_A, MOTOR_FULL_FORWARD);
  //  setMotor(MOTOR_B, MOTOR_FULL_REVERSE);
  //  
  //  delay(1000);

  setMotor(MOTOR_A, constrain((-angle[2] + linearVelocity) * MOTOR_FULL_FORWARD, MOTOR_FULL_REVERSE, MOTOR_FULL_FORWARD));
  setMotor(MOTOR_B, constrain((angle[2] + linearVelocity) * MOTOR_FULL_FORWARD, MOTOR_FULL_REVERSE, MOTOR_FULL_FORWARD));

  lastMillis = curMillis;

  delay(5);
}






