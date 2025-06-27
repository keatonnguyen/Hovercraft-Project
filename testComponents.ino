
/*
  Test file to integrate: 
  - MPU6050 IMU (yaw, pitch, roll)
  - Servo motor
  - Ultrasonic sensor
  - IR sensor
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


// UART SETUP FUNCTIONS
void uart_init() 
{
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char c) 
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char* str) 
{
    while (*str) uart_transmit(*str++);
}

void uart_print_int(int val) 
{
    char buffer[10];
    itoa(val, buffer, 10);
    uart_print(buffer);
}

void uart_print_float(float val) 
{
    char buffer[10];
    dtostrf(val, 6, 2, buffer);
    uart_print(buffer);
}


// SETUP FUNCTIONS
void setup_mpu6050() 
{
    uart_print("Initializing MPU6050...\r\n");
}

void setup_ultrasonic() 
{
    uart_print("Initializing Ultrasonic Sensor...\r\n");
    DDRB |= (1 << PB3);     // Trigger pin
    DDRD &= ~(1 << PD2);    // Echo pin
}

void setup_ir_sensor() 
{
    uart_print("Initializing IR Sensor...\r\n");
    ADMUX = 0x00;
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}


// READ FUNCTIONS
int read_ir_value() 
{
    ADMUX = (ADMUX & 0xF0); // ADC0
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int read_ultrasonic_distance() 
{
    PORTB &= ~(1 << PB3); _delay_us(2);
    PORTB |= (1 << PB3); _delay_us(10);
    PORTB &= ~(1 << PB3);

    while (!(PIND & (1 << PD2)));
    uint16_t start = TCNT1;
    while (PIND & (1 << PD2));
    uint16_t end = TCNT1;

    if (end < start) end += 65536;
    uint16_t pulse = end - start;
    return (pulse * 0.0343) / 2;
}


//MAIN PART
void setup() 
{
    uart_init();
    setup_mpu6050();
    setup_ultrasonic();
    setup_ir_sensor();
    uart_print("Setup complete.\r\n");
}

void loop() 
{
    int ir = read_ir_value();
    int dist = read_ultrasonic_distance();

    uart_print("IR ADC: ");
    uart_print_int(ir);
    uart_print(" | US Dist: ");
    uart_print_int(dist);
    uart_print(" cm\r\n");
  
    _delay_ms(1000);
}
