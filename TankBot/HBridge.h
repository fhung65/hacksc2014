#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <stdint.h>

#define MOTOR_FULL_REVERSE -4095
#define MOTOR_FULL_FORWARD 4095
#define MOTOR_STOP 0

#define MOTOR_A 1
#define MOTOR_B 2

#define MOTOR_A_INVERT
//#define MOTOR_B_INVERT

void initMotors(uint8_t IN1, uint8_t IN2, uint8_t IN3, uint8_t IN4, uint8_t ENA, uint8_t ENB);
void setMotor(uint8_t motor, int16_t speed);
void setMotor(uint8_t motor, float speed);
void stopMotor(uint8_t motor);
void enableMotor(uint8_t motor);
void disableMotor(uint8_t motor);

#endif // HBRIDGE_H
