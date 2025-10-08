#include <avr/io.h>
#include <util/delay.h>

// Definindo os pinos
#define IN1_PORT PORTD
#define IN1_DDR  DDRD
#define IN1_PIN  PD2  // Pino 4

#define IN2_PORT PORTD
#define IN2_DDR  DDRD
#define IN2_PIN  PD3  // Pino 5

#define ENA_PORT PORTD
#define ENA_DDR  DDRD
#define ENA_PIN  PD5  // Pino 11 (PWM)

// Pino do potenciômetro (ADC0 - PC0 - pino 23)
#define POT_ADC_CHANNEL 0

// Parâmetros ajustáveis
#define MIN_PWM 30       // Valor mínimo de PWM onde o motor começa a girar
#define MAX_PWM 255      // Valor máximo de PWM
#define DEADZONE 20      // Zona morta no início do potenciômetro (0-1023)

void setup() {
    // Configura os pinos de controle como saída
    IN1_DDR |= (1 << IN1_PIN);
    IN2_DDR |= (1 << IN2_PIN);
    ENA_DDR |= (1 << ENA_PIN);
    
    // Configura sentido único (horário)
    IN1_PORT |= (1 << IN1_PIN);
    IN2_PORT &= ~(1 << IN2_PIN);
    
    // Configura PWM
    TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B |= (1 << CS01);
    
    // Configura ADC
    ADMUX |= (1 << REFS0); // Referência AVcc
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(uint8_t channel) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

// Função de mapeamento proporcional com deadzone
uint8_t mapProportional(uint16_t adcValue) {
    // Aplica deadzone
    if(adcValue < DEADZONE) return 0;
    
    // Remove a deadzone do range
    adcValue -= DEADZONE;
    uint16_t effectiveRange = 1023 - DEADZONE;
    
    // Mapeia proporcionalmente para o range PWM
    uint32_t mapped = (uint32_t)adcValue * (MAX_PWM - MIN_PWM) / effectiveRange + MIN_PWM;
    
    // Limita ao máximo
    return (mapped > MAX_PWM) ? MAX_PWM : mapped;
}

int main(void) {
    setup();
    
    while(1) {
        uint16_t potValue = readADC(POT_ADC_CHANNEL);
        uint8_t motorSpeed = mapProportional(potValue);
        
        OCR0B = motorSpeed; // Aplica PWM
        
        _delay_ms(20); // Taxa de atualização de ~50Hz
    }
    
    return 0;
}