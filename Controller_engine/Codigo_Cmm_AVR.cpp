#include <avr/io.h>
#include <util/delay.h>

class MotorController {
private:
    // Definição das portas e pinos
    volatile uint8_t* const IN1_PORT = &PORTD;
    volatile uint8_t* const IN1_DDR  = &DDRD;
    const uint8_t IN1_PIN  = PD2;  // Pino 4

    volatile uint8_t* const IN2_PORT = &PORTD;
    volatile uint8_t* const IN2_DDR  = &DDRD;
    const uint8_t IN2_PIN  = PD3;  // Pino 5

    volatile uint8_t* const ENA_PORT = &PORTD;
    volatile uint8_t* const ENA_DDR  = &DDRD;
    const uint8_t ENA_PIN  = PD5;  // Pino 11 (PWM)

    // Pino do potenciômetro (ADC0 - PC0 - pino 23)
    const uint8_t POT_ADC_CHANNEL = 0;

    // Parâmetros ajustáveis
    const uint8_t MIN_PWM = 30;      // Valor mínimo de PWM efetivo
    const uint8_t MAX_PWM = 255;     // Valor máximo de PWM
    const uint16_t DEADZONE = 20;    // Zona morta no potenciômetro

public:
    MotorController() {
        initialize();
    }

    void initialize() {
        // Configura pinos como saída
        *IN1_DDR |= (1 << IN1_PIN);
        *IN2_DDR |= (1 << IN2_PIN);
        *ENA_DDR |= (1 << ENA_PIN);
        
        // Configura sentido único (horário)
        *IN1_PORT |= (1 << IN1_PIN);
        *IN2_PORT &= ~(1 << IN2_PIN);
        
        // Configura PWM no Timer0
        TCCR0A |= (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
        TCCR0B |= (1 << CS01);
        
        // Configura ADC
        ADMUX |= (1 << REFS0); // Referência AVcc
        ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    }

    uint16_t readPotentiometer() {
        ADMUX = (ADMUX & 0xF8) | (POT_ADC_CHANNEL & 0x07);
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        return ADC;
    }

    uint8_t mapProportional(uint16_t adcValue) {
        // Aplica deadzone
        if(adcValue < DEADZONE) return 0;
        
        // Remove a deadzone do range
        adcValue -= DEADZONE;
        uint16_t effectiveRange = 1023 - DEADZONE;
        
        // Mapeia proporcionalmente
        uint32_t mapped = static_cast<uint32_t>(adcValue) * (MAX_PWM - MIN_PWM) / effectiveRange + MIN_PWM;
        
        // Limita ao máximo
        return (mapped > MAX_PWM) ? MAX_PWM : static_cast<uint8_t>(mapped);
    }

    void setSpeed(uint8_t speed) {
        OCR0B = speed;
    }

    void update() {
        uint16_t potValue = readPotentiometer();
        uint8_t motorSpeed = mapProportional(potValue);
        setSpeed(motorSpeed);
        _delay_ms(20);
    }
};

// Instância global do controlador
MotorController motor;

int main() {
    while(true) {
        motor.update();
    }
    return 0;
}