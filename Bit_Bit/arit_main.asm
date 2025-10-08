; =============================================================================
; Benchmark SRAM – ATmega328P @ 16 MHz
; - Array de 256 bytes
; - 100 iterações: AND (0x0F) -> OR (0xF0) -> NOT (COM)
; - Timer1 prescaler = 64 (1 tick = 4 us) – evita overflow (~262 ms)
; =============================================================================

.include "m328pdef.inc"

; -------------------- SRAM --------------------
.dseg
.org 0x0100                 ; início seguro da SRAM
ARRAY: .byte 256

; -------------------- FLASH -------------------
.cseg
.org 0x0000
    rjmp MAIN

STR_TIME:   .db "Tempo (us): 0x", 0
STR_CRLF:   .db 13,10,0

; -------------------- MAIN --------------------
MAIN:
    ; Stack
    ldi r16, high(RAMEND)
    out SPH, r16
    ldi r16, low(RAMEND)
    out SPL, r16
    clr r1                      ; convenção ABI

    rcall USART_Init
    rcall FILL_ARRAY            ; 0..255

    ; Timer1 parado, modo normal, zera contador e flag OVF
    ldi r16, 0
    sts TCCR1A, r16
    sts TCCR1B, r16
    sts TCNT1H, r16
    sts TCNT1L, r16
    ldi r16, (1<<TOV1)
    sts TIFR1, r16

    ; Inicia Timer1: prescaler = 64 (1 tick = 4 us)
    ldi r16, (1<<CS11)|(1<<CS10)
    sts TCCR1B, r16

    ; -------- Benchmark --------
    rcall RUN_BENCHMARK

    ; Para Timer1
    ldi r16, 0
    sts TCCR1B, r16

    ; Lê TCNT1 (LOW depois HIGH para latch correto)
    lds r24, TCNT1L
    lds r25, TCNT1H              ; r25:r24 = ticks (16-bit)

    ; Converte para microssegundos: us = ticks * 4  (2x LSL/ROL)
    lsl r24
    rol r25
    lsl r24
    rol r25                      ; r25:r24 = micros (16-bit)

    ; ---- Imprime apenas o tempo (micros) em HEX ----
    ldi r30, low(STR_TIME<<1)
    ldi r31, high(STR_TIME<<1)
    rcall UART_PRINT_PSTR

    rcall PRINT_HEX16            ; r25:r24 já contém os micros

    ldi r30, low(STR_CRLF<<1)
    ldi r31, high(STR_CRLF<<1)
    rcall UART_PRINT_PSTR

END:
    rjmp END

; -------------------- Rotinas --------------------

; Preenche ARRAY com 0..255
FILL_ARRAY:
    ldi r31, high(ARRAY)
    ldi r30, low(ARRAY)
    clr r16            ; valor
    clr r17            ; contador (0 -> 256 voltas via dec)
FL_LOOP:
    st  Z+, r16
    inc r16
    dec r17
    brne FL_LOOP
    ret

; 100 iterações: AND (0x0F) -> OR (0xF0) -> NOT
RUN_BENCHMARK:
    ldi r21, 100
RB_LP:
    ; AND pass
    ldi r31, high(ARRAY)
    ldi r30, low(ARRAY)
    clr r20
RB_AND:
    ld  r16, Z
    andi r16, 0x0F
    st  Z+, r16
    dec r20
    brne RB_AND

    ; OR pass
    ldi r31, high(ARRAY)
    ldi r30, low(ARRAY)
    clr r20
RB_OR:
    ld  r16, Z
    ori r16, 0xF0
    st  Z+, r16
    dec r20
    brne RB_OR

    ; NOT pass
    ldi r31, high(ARRAY)
    ldi r30, low(ARRAY)
    clr r20
RB_NOT:
    ld  r16, Z
    com r16
    st  Z+, r16
    dec r20
    brne RB_NOT

    dec r21
    brne RB_LP
    ret

; UART 9600 8N1
USART_Init:
    ldi r16, 0
    sts UBRR0H, r16
    ldi r16, 103              ; 16MHz/16/9600 - 1
    sts UBRR0L, r16
    ldi r16, (1<<TXEN0)
    sts UCSR0B, r16
    ldi r16, (1<<UCSZ01)|(1<<UCSZ00)
    sts UCSR0C, r16
    ret

; TX em r16
UART_TX:
    push r17
U_TXW:
    lds r17, UCSR0A
    sbrs r17, UDRE0
    rjmp U_TXW
    sts UDR0, r16
    pop r17
    ret

; Imprime string em Flash (Z aponta)
UART_PRINT_PSTR:
    push r16
    push r30
    push r31
UPL:
    lpm  r16, Z+
    tst  r16
    breq UPL_END
    rcall UART_TX
    rjmp UPL
UPL_END:
    pop r31
    pop r30
    pop r16
    ret

; ---------- HEX helpers ----------
; r25:r24 -> "HHHH"
PRINT_HEX16:
    mov  r16, r25
    rcall PRINT_HEX8
    mov  r16, r24
    rcall PRINT_HEX8
    ret

; r16 -> "HH"
PRINT_HEX8:
    push r17
    mov  r17, r16           ; guarda byte original

    ; nibble alto
    mov  r16, r17
    swap r16
    andi r16, 0x0F
    rcall NIBBLE_TO_HEX
    rcall UART_TX

    ; nibble baixo
    mov  r16, r17
    andi r16, 0x0F
    rcall NIBBLE_TO_HEX
    rcall UART_TX

    pop r17
    ret

; nibble em r16 -> ASCII
NIBBLE_TO_HEX:
    cpi  r16, 10
    brlo NTH_DIG
    subi r16, 10
    ldi  r17, 'A'
    add  r16, r17           ; 10->'A', 11->'B'...
    ret
NTH_DIG:
    ldi  r17, '0'
    add  r16, r17
    ret
