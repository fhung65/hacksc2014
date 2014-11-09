#include "HBridge.h"
#include "Arduino.h"

typedef struct
{
    uint8_t IN1;
    uint8_t IN2;
    uint8_t IN3;
    uint8_t IN4;
    uint8_t ENA;
    uint8_t ENB;
} motor_data_t;

static motor_data_t m_motorData;

void initMotors(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t ENA, uint8_t ENB)
{
  #ifdef TEENSY
    analogWriteRes(12);
    #endif

    m_motorData.IN1 = IN1;
    m_motorData.IN2 = IN2;
    m_motorData.IN3 = IN3;
    m_motorData.IN4 = IN4;
    m_motorData.ENA = ENA;
    m_motorData.ENB = ENB;

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(ENA, LOW);
    digitalWrite(ENB, LOW);
}

void setMotor(uint8_t motor, float speed)
{
    if(speed > 1)
    {
        speed = 1;
    }
    if(speed < -1)
    {
        speed = -1;
    }

    speed *= MOTOR_FULL_FORWARD;
    setMotor(motor, (int16_t)speed);
}

void setMotor(uint8_t motor, int16_t speed)
{
    if(speed < 0)
    {
        if(speed < MOTOR_FULL_REVERSE)
        {
            speed = MOTOR_FULL_REVERSE;
        }

        if(motor & MOTOR_A)
        {
#ifdef MOTOR_A_INVERT
            pinMode(m_motorData.IN2, OUTPUT);
            digitalWrite(m_motorData.IN2, LOW);
            analogWrite(m_motorData.IN1, -speed);
#endif
#ifndef MOTOR_A_INVERT
            pinMode(m_motorData.IN1, OUTPUT);
            digitalWrite(m_motorData.IN1, LOW);
            analogWrite(m_motorData.IN2, -speed);
#endif // MOTOR_A_INVERT
        }

        if(motor & MOTOR_B)
        {
#ifdef MOTOR_B_INVERT
            pinMode(m_motorData.IN4, OUTPUT);
            digitalWrite(m_motorData.IN4, LOW);
            analogWrite(m_motorData.IN3, -speed);
#endif
#ifndef MOTOR_B_INVERT
            pinMode(m_motorData.IN3, OUTPUT);
            digitalWrite(m_motorData.IN3, LOW);
            analogWrite(m_motorData.IN4, -speed);
#endif // MOTOR_B_INVERT
        }
    }
    else if(speed > 0)
    {
        if(speed > MOTOR_FULL_FORWARD)
        {
            speed = MOTOR_FULL_FORWARD;
        }

        if(motor & MOTOR_A)
        {

#ifdef MOTOR_A_INVERT
            pinMode(m_motorData.IN1, OUTPUT);
            digitalWrite(m_motorData.IN1, LOW);
            analogWrite(m_motorData.IN2, speed);
#endif
#ifndef MOTOR_A_INVERT
            pinMode(m_motorData.IN2, OUTPUT);
            digitalWrite(m_motorData.IN2, LOW);
            analogWrite(m_motorData.IN1, speed);
#endif // MOTOR_A_INVERT
        }

        if(motor & MOTOR_B)
        {

#ifdef MOTOR_B_INVERT
            pinMode(m_motorData.IN3, OUTPUT);
            digitalWrite(m_motorData.IN3, LOW);
            analogWrite(m_motorData.IN4, speed);
#endif
#ifndef MOTOR_B_INVERT
            pinMode(m_motorData.IN4, OUTPUT);
            digitalWrite(m_motorData.IN4, LOW);
            analogWrite(m_motorData.IN3, speed);
#endif // MOTOR_B_INVERT
        }
    }
    else
    {
        stopMotor(motor);
    }
}

void stopMotor(uint8_t motor)
{
    if(motor & MOTOR_A)
    {
        pinMode(m_motorData.IN1, OUTPUT);
        pinMode(m_motorData.IN2, OUTPUT);
        digitalWrite(m_motorData.IN1, LOW);
        digitalWrite(m_motorData.IN2, LOW);
    }

    if(motor & MOTOR_B)
    {
        pinMode(m_motorData.IN3, OUTPUT);
        pinMode(m_motorData.IN4, OUTPUT);
        digitalWrite(m_motorData.IN3, LOW);
        digitalWrite(m_motorData.IN4, LOW);
    }
}

void enableMotor(uint8_t motor)
{
    if(motor & MOTOR_A)
    {
        pinMode(m_motorData.ENA, OUTPUT);
        digitalWrite(m_motorData.ENA, HIGH);
    }

    if(motor & MOTOR_B)
    {
        pinMode(m_motorData.ENB, OUTPUT);
        digitalWrite(m_motorData.ENB, HIGH);
    }
}

void disableMotor(uint8_t motor)
{
    if(motor & MOTOR_A)
    {
        pinMode(m_motorData.ENA, OUTPUT);
        digitalWrite(m_motorData.ENA, LOW);
    }

    if(motor & MOTOR_B)
    {
        pinMode(m_motorData.ENB, OUTPUT);
        digitalWrite(m_motorData.ENB, LOW);
    }
}
