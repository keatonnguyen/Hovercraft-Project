#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// Pin Definitions
#define ADC_CHANNEL 0  // PC0 = ADC0
#define LED_PIN PB3     // PWM brightness (D11)
#define FLASH_LED PB5   // Yellow onboard LED (D13)

// Brightness thresholds (based on distance approx from datasheet)
#define ADC_D1 704  // ADC value ~11 cm (adjust as needed)
#define ADC_D2 215  // ADC value ~41 cm (adjust as needed)
#define BRIGHT1 26  // 10% brightness
#define BRIGHT2 255 // 100% brightness

void uart_init() {
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_print(const char* str) {
    while (*str) uart_transmit(*str++);
}

void uart_print_int(uint16_t val) {
    char buffer[10];
    itoa(val, buffer, 10);
    uart_print(buffer);
}

void adc_init() {
    ADMUX = 0x00;  // Use AREF pin as Vref
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);  // Prescaler 64
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Select channel
    ADCSRA |= (1 << ADSC);                     // Start conversion
    while (ADCSRA & (1 << ADSC));              // Wait for conversion
    return ADC;
}

void pwm_init() {
    DDRB |= (1 << LED_PIN);
    TCCR2A = (1 << COM2A1) | (1 << WGM20);  // Fast PWM, non-inverting
    TCCR2B = (1 << CS21);  // Prescaler 8
}

void setup() {
    DDRB |= (1 << FLASH_LED);
    PORTB &= ~(1 << FLASH_LED);

    uart_init();
    adc_init();
    pwm_init();
}

void control_led(uint16_t adc_val) {
    if (adc_val >= ADC_D1) {
        OCR2A = 255 - BRIGHT1;
    } else if (adc_val <= ADC_D2) {
        OCR2A = 255 - BRIGHT2;
    } else {
        uint8_t brightness = BRIGHT1 + ((ADC_D1 - adc_val) * (BRIGHT2 - BRIGHT1)) / (ADC_D1 - ADC_D2);
        OCR2A = 255 - brightness;
    }

    // Flash yellow LED if out of range
    if (adc_val > ADC_D1 || adc_val < ADC_D2) {
        PORTB ^= (1 << FLASH_LED);
        _delay_ms(250);
    } else {
        PORTB &= ~(1 << FLASH_LED);
    }
}

int main() {
    setup();
    while (1) {
        uint16_t adc_val = adc_read(ADC_CHANNEL);

        uart_print("ADC: ");
        uart_print_int(adc_val);
        uart_print("\r\n");

        control_led(adc_val);
        _delay_ms(500);
    }
}

