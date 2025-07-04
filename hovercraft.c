#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


// MPU6050 definitions
#define MPU6050_ADDR 0x68
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_GYRO_XOUT_H 0x43


// Ultrasonic pins definition
#define TRIG_LEFT PB3
#define ECHO_LEFT PD2
#define TRIG_RIGHT PB5
#define ECHO_RIGHT PD3


//Values to use for the code
volatile uint8_t turning = 0;
volatile uint8_t turn_complete = 0;
volatile uint8_t yaw_active = 0;
volatile float current_yaw = 0.0;
volatile float yaw_start = 0.0;
volatile uint16_t left_distance = 0, right_distance = 0;
volatile uint16_t timer_count = 0;
volatile uint8_t echo_flag = 0;
volatile uint32_t turn_timer_ms = 0;


// UART functions
void UART_init() 
{
    UBRR0H = 0;
    UBRR0L = 103;
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}
void UART_sendChar(char c) 
{
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
}
void UART_sendString(const char* s) 
{
    while (*s) UART_sendChar(*s++);
}
void UART_sendInt(int val) 
{
    char buffer[16];
    itoa(val, buffer, 10);
    UART_sendString(buffer);
}


// I2C functions (TWI)
void TWI_init() 
{
    TWSR = 0x00;
    TWBR = 72;
    TWCR = (1<<TWEN);
}
void TWI_start() 
{
    TWCR = (1<<TWSTA)|(1<<TWEN)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
}
void TWI_stop() 
{
    TWCR = (1<<TWSTO)|(1<<TWINT)|(1<<TWEN);
}
void TWI_write(uint8_t data) 
{
    TWDR = data;
    TWCR = (1<<TWEN)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
}
uint8_t TWI_read_ACK() 
{
    TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWEA);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}
uint8_t TWI_read_NACK() 
{
    TWCR = (1<<TWEN)|(1<<TWINT);
    while (!(TWCR & (1<<TWINT)));
    return TWDR;
}


// MPU6050 functions
void MPU6050_init() 
{
    TWI_start();
    TWI_write((MPU6050_ADDR << 1));
    TWI_write(MPU6050_PWR_MGMT_1);
    TWI_write(0x00);
    TWI_stop();
}
int16_t MPU6050_read_gyro_z() 
{
    int16_t gz;
    TWI_start();
    TWI_write((MPU6050_ADDR << 1));
    TWI_write(MPU6050_GYRO_XOUT_H + 4);
    TWI_start();
    TWI_write((MPU6050_ADDR << 1) | 0x01);
    uint8_t high = TWI_read_ACK();
    uint8_t low = TWI_read_NACK();
    TWI_stop();
    gz = ((int16_t)high << 8) | low;
    return gz;
}


// IR sensor functions
void ADC_init() 
{
    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);
}
uint16_t read_IR() 
{
    ADMUX = (ADMUX & 0xF0);
    ADCSRA |= (1<<ADSC);
    while (ADCSRA & (1<<ADSC));
    return ADC;
}
uint16_t read_IR_average() 
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 100; i++) 
    {
        sum += read_IR();
        _delay_ms(7);
    }
    return sum / 100;
}


// Ultrasonic sensor functions
void US_init() 
{
    DDRB |= (1<<TRIG_LEFT)|(1<<TRIG_RIGHT);
    DDRD &= ~((1<<ECHO_LEFT)|(1<<ECHO_RIGHT));
    EICRA |= (1<<ISC00)|(1<<ISC10);
    EIMSK |= (1<<INT0)|(1<<INT1);
    TCCR2B |= (1<<CS21);
    TIMSK2 |= (1<<TOIE2);
}
void trigger_US(uint8_t trig_pin) 
{
    PORTB &= ~(1<<trig_pin);
    _delay_us(2);
    PORTB |= (1<<trig_pin);
    _delay_us(10);
    PORTB &= ~(1<<trig_pin);
}
ISR(INT0_vect) 
{
    if (PIND & (1<<ECHO_LEFT)) { TCNT2=0; timer_count=0; echo_flag=1; }
    else { left_distance = (timer_count * 256 + TCNT2) * 0.027; echo_flag=0; }
}
ISR(INT1_vect) 
{
    if (PIND & (1<<ECHO_RIGHT)) { TCNT2=0; timer_count=0; echo_flag=2; }
    else { right_distance = (timer_count * 256 + TCNT2) * 0.027; echo_flag=0; }
}
ISR(TIMER2_OVF_vect) 
{ 
    timer_count++;
    if (turning) turn_timer_ms += 33;
 }


// Setting servo's PWM on Timer1 OCR1A (PB1 = P9)
void PWM_servo_init() 
{
    DDRB |= (1<<PB1);
    TCCR1A = (1<<COM1A1)|(1<<WGM11);
    TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
    ICR1 = 39999;
    OCR1A = 2400;
}
void servo_center() { OCR1A = 2600; }    // angle to center servo 
void servo_left()   { OCR1A = 1200; }    // angle for a left turn
void servo_right()  { OCR1A = 4300; }    // angle for a right turn


// Setting thrust fan PWM on Timer0 OCR0A (PD6 = P4)
void PWM_thrust_fan_init() 
{
    DDRD |= (1<<PD6);
    TCCR0A = (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B = (1<<CS01);
    OCR0A = 130;  // power of the fan
}


// Lift fan control on P17 (PD4)
void lift_fan_on() 
{
    DDRD |= (1<<PD4);
    PORTD |= (1<<PD4);
}
void lift_fan_off() 
{
    DDRD |= (1<<PD4);
    PORTD &= ~(1<<PD4);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


int main() 
{
    //Initialize all definitions
    cli();
    UART_init();
    ADC_init();
    US_init();
    TWI_init();
    MPU6050_init();
    PWM_servo_init();
    _delay_ms(2500);
    PWM_thrust_fan_init();
    lift_fan_on();
    sei();

    //Center the thrust fan & stabilize IR readings at the beginning
    servo_center();
    UART_sendString("Waiting for IR sensor to stabilize...\r\n");
    UART_sendString("IR sensor ready.\r\n");

    while (1) 
    {
        trigger_US(TRIG_LEFT);
        _delay_ms(60);
        trigger_US(TRIG_RIGHT);
        _delay_ms(60);
        
        // Reads distance with IR while not turning
        if (!turning && !turn_complete) 
        {
            uint16_t ir = read_IR_average();
            UART_sendString("IR value: "); UART_sendInt(ir); UART_sendString("\r\n");

            //Exit override condition (50cm)
            if (left_distance < 50 && right_distance >= 50) 
            {
                UART_sendString("Turning RIGHT!\r\n");
                servo_right();
                yaw_start = current_yaw;
                turning = 1;
                yaw_active = 0;
                turn_timer_ms = 0;
            }
            else if (right_distance < 50 && left_distance >= 50) 
            {
                UART_sendString("Turning LEFT!\r\n");
                servo_left();
                yaw_start = current_yaw;
                turning = 1;
                yaw_active = 0;
                turn_timer_ms = 0;
            }

            //When detecting obstacle, compare the side US sensors & determine which turn to make
            if (ir > 120) 
            {  
                //trigger_US(TRIG_LEFT);                                           Only enable if US sensor has a glitch
                //_delay_ms(60);
                //trigger_US(TRIG_RIGHT);
                //_delay_ms(60);
                UART_sendString("Obstacle detected!\r\n");
                UART_sendString("Left US: "); UART_sendInt(left_distance);
                UART_sendString(" cm | Right US: "); UART_sendInt(right_distance);
                UART_sendString(" cm\r\n");

                if (left_distance > right_distance) 
                {
                    UART_sendString("Turning LEFT!\r\n");
                    servo_left();
                } 
                else 
                {
                    UART_sendString("Turning RIGHT!\r\n");
                    servo_right();
                }
                yaw_start = current_yaw;
                turning = 1;
                yaw_active = 0;
                turn_timer_ms = 0;
                }
        }

        //Perform the turn until IMU detects a full 90 degree turn
        if (turning) 
        {
            int16_t gz = MPU6050_read_gyro_z();
            float gyro_z_dps = gz / 131.0;

            //Turn on yaw readings when turn is sharp enough to omit slight tilts on hovercraft during the run
            if (!yaw_active && fabs(gyro_z_dps) > 5.0) 
            {
                UART_sendString("Yaw integration activated!\r\n");
                yaw_active = 1;
            }

            if (yaw_active) 
            {
                if (fabs(gyro_z_dps) > 2.0) 
                {
                    current_yaw += gyro_z_dps * 0.01;
                    if (current_yaw >= 360) current_yaw -= 360;
                    if (current_yaw < 0) current_yaw += 360;
                }

                float delta_yaw = fabs(current_yaw - yaw_start);
                if (delta_yaw >= 90) 
                {
                    turning = 0;
                    turn_complete = 1;
                }
            }

            //Reset the hovercraft when a certain amount of time has gone by in case it gets stuck turning on the wrong side of a corner
            if (turn_timer_ms >= 1600000) 
            {
                UART_sendString("Turn timeout! Resetting...\r\n");
                servo_center();
                turning = 0;
                turn_complete = 0;
            }
        }
        
        //After a turn is performed, reset hovercraft
        if (turn_complete) 
        {
            UART_sendString("Turn complete. Servo centered.\r\n");
            servo_center();
            turn_complete = 0;
        }
        _delay_ms(10);
    }
}
