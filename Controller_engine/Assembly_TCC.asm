; Controle Proporcional do Motor com Potenciômetro
; ATmega328P - Conexões idênticas aos códigos C/C++
; Comportamento CORRIGIDO - Versão Final

.include "m328Pdef.inc"

; Constantes
.equ MIN_PWM = 30       ; Valor mínimo para o motor girar
.equ MAX_PWM = 255      ; Velocidade máxima
.equ DEADZONE = 20      ; Ignora valores abaixo de 20

; Pinos
.equ IN1 = 2            ; PD2 (pino 4)
.equ IN2 = 3            ; PD3 (pino 5)
.equ ENA = 5            ; PD5 (pino 11, PWM - OC0B)
.equ POT = 0            ; PC0 (ADC0, pino 23)

; Registradores
.def temp = r16
.def pwm_val = r17
.def adc_l = r24        ; LSB do ADC
.def adc_h = r25        ; MSB do ADC

.cseg
.org 0
    rjmp reset

reset:
    ; Inicialização
    ldi temp, high(RAMEND)
    out SPH, temp
    ldi temp, low(RAMEND)
    out SPL, temp

    ; Configura pinos
    sbi DDRD, IN1
    sbi DDRD, IN2
    sbi DDRD, ENA

    ; Sentido de rotação fixo
    sbi PORTD, IN1
    cbi PORTD, IN2

    ; Configura PWM
    ldi temp, (1<<COM0B1)|(1<<WGM01)|(1<<WGM00)
    out TCCR0A, temp
    ldi temp, (1<<CS01)  ; Prescaler 8
    out TCCR0B, temp

    ; Configura ADC
    ldi temp, (1<<REFS0)
    sts ADMUX, temp
    ldi temp, (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)
    sts ADCSRA, temp

main_loop:
    rcall read_adc       ; Lê potenciômetro (0-1023)

    ; --- LÓGICA CORRIGIDA ---
    ; 0% do potenciômetro (ADC ? 0) ? Motor PARADO (PWM = 0)
    ; 100% do potenciômetro (ADC ? 1023) ? Velocidade MÁXIMA (PWM = 255)

    ; 1. Aplica deadzone (valores < 20 = motor parado)
    cpi adc_l, low(DEADZONE)
    ldi temp, high(DEADZONE)
    cpc adc_h, temp
    brsh calc_pwm
    
    clr pwm_val          ; Motor parado
    rjmp apply_pwm

calc_pwm:
    ; 2. Remove deadzone
    subi adc_l, low(DEADZONE)
    sbci adc_h, high(DEADZONE)

    ; 3. Calcula PWM proporcional
    ; Fórmula: pwm_val = (adc_value * 255) / (1023 - DEADZONE)
    ; Simplificação: divide por 4 (>> 2) e ajusta
    lsr adc_h
    ror adc_l
    lsr adc_h
    ror adc_l
    mov pwm_val, adc_l

    ; 4. Ajusta faixa (MIN_PWM a MAX_PWM)
    subi pwm_val, -MIN_PWM

    ; 5. Limita máximo
    cpi pwm_val, MAX_PWM
    brlo apply_pwm
    ldi pwm_val, MAX_PWM

apply_pwm:
    out OCR0B, pwm_val   ; Aplica PWM

    ; Delay de 20ms
    ldi temp, 20
delay:
    ldi r18, 250
inner_delay:
    dec r18
    brne inner_delay
    dec temp
    brne delay

    rjmp main_loop

read_adc:
    lds temp, ADCSRA
    ori temp, (1<<ADSC)
    sts ADCSRA, temp
adc_wait:
    lds temp, ADCSRA
    sbrc temp, ADSC
    rjmp adc_wait
    lds adc_l, ADCL
    lds adc_h, ADCH
    ret