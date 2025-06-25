#define F_CPU 16000000UL // 16 MHz clock
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define TRIG_PIN PB3  // Trig pin (PB3)
#define ECHO_PIN PD2  // Echo pin (PD2)
#define D1 11  // Distance for 100% brightness (in cm)
#define BRIGHT1 255 // Max Brightness
#define D2 41  // Distance for 10% brightness (in cm)
#define BRIGHT2 26  // Min Brightness
#define LED_PIN PB3  // LED on PB3
#define FLASH_LED PB5  // Led on PB5


// Function to send trigger pulse
void sendTriggerPulse() 
{
    PORTB &= ~(1 << TRIG_PIN); // Make sure trigger pin is LOW
    _delay_us(2);              // Small delay to ensure a clean pulse
    PORTB |= (1 << TRIG_PIN);  // Send trigger pulse HIGH
    _delay_us(10);             // Pulse width
    PORTB &= ~(1 << TRIG_PIN); // Ensure trigger pin is LOW again
}

// Function to measure pulse width
uint32_t measurePulse() 
{
    uint32_t time_start, time_end;

    while (!(PIND & (1 << ECHO_PIN)));
    {
    time_start = TCNT1;
    }
    while (PIND & (1 << ECHO_PIN));
    {
    time_end = TCNT1;
    }
    if (time_end < time_start) 
    {
        time_end += 65536;
    }
    
    uint32_t pulse = time_end - time_start;
    pulse = (pulse * 8) / (F_CPU / 1000000);
    return pulse;
}


// Function to calculate the distance based on multiple measurements
uint32_t getDistance() 
{
    uint32_t totalDistance = 0;
    uint32_t measurements = 5;
    
    for (uint32_t i = 0; i < measurements; i++) 
    {
        sendTriggerPulse();
        uint32_t duration = measurePulse();
        uint32_t distance = (duration * 0.0343) / 2;
        totalDistance += distance;
        _delay_ms(100);  // Small delay between measurements (CHANGE IF NEEDED)
    }
    
    uint32_t avgDistance = totalDistance / measurements;
    return avgDistance;
}


// Function to transmit data via UART
void uartTransmit(char c) 
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}


void uartPrint(const char* str) 
{
    while (*str) 
    {
        uartTransmit(*str++);
    }
}


void uartPrintInt(uint32_t num) 
{
    char buffer[10];
    itoa(num, buffer, 10);
    uartPrint(buffer);
}


void printDistance(uint32_t distance) 
{
    uartPrint("Distance: ");
    uartPrintInt(distance);
    uartPrint(" cm\r\n");
}


void controlLED(uint32_t distance)   //Control LED brightness
{
    
    if (distance <= D1) 
    {
        OCR2A = 255 - BRIGHT1;
    } 
    else if (distance >= D2) 
    {
       OCR2A = 255 - BRIGHT2;
    } 
    else 
    {
        OCR2A = 255 - (BRIGHT1 - ((distance - D1) * (BRIGHT1 - BRIGHT2)) / (D2 - D1));
    }

    // Flash PB5 when the object is outside [D1, D2]
    if (distance < D1 || distance > D2) 
    {
        PORTB ^= (1 << FLASH_LED);
        _delay_ms(250);
    } 
    else 
    {
        PORTB &= ~(1 << FLASH_LED);
    }
}


void setup() 
{
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud
    UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable TX and RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Configure trigPin as OUTPUT
    DDRB |= (1 << TRIG_PIN);
    // Configure echoPin as INPUT
    DDRD &= ~(1 << ECHO_PIN);
    // Configure PB5 (LED) as OUTPUT
    DDRB |= (1 << FLASH_LED);
    PORTB &= ~(1 << FLASH_LED);

    // Configure Timer1
    TCCR1A = 0;
    TCCR1B |= (1 << CS11); // Prescaler 8
    TCNT1 = 0;

    // Set LED_PIN (PB3) as output
    DDRB |= (1 << LED_PIN);
}


int main() 
{
    setup();

    while (1) 
    {
        uint32_t distance = getDistance();
        printDistance(distance);  // Output the average distance to UART
       controlLED(distance);     // Control LED based on distance
        _delay_ms(500);  // Wait a bit before measuring again
    }
}


