
.include "m328pdef.inc"

.equ TOGGLE_INIT   = 10000     ; altere para 1, 10, 100, 10000...
.equ T1_PRESC_BITS = (1<<CS11)|(1<<CS10) ; /64
.equ MASK_PB5      = (1<<PB5)

.cseg
.org 0x0000
    rjmp RESET

;--------------------------------------------------------
; RESET
;--------------------------------------------------------
RESET:
    ; Stack
    ldi     r16, high(RAMEND)
    out     SPH, r16
    ldi     r16, low(RAMEND)
    out     SPL, r16
    clr     r1                      ; ABI: r1 = 0

    ; PB5 como saída
    in      r16, DDRB
    ori     r16, (1<<DDB5)
    out     DDRB, r16

    ; UART 9600 @16MHz
    ldi     r16, 0
    sts     UBRR0H, r16
    ldi     r16, 103               ; 16MHz -> 9600 (UBRR=103)
    sts     UBRR0L, r16
    ldi     r16, (1<<TXEN0)        ; TX on
    sts     UCSR0B, r16
    ldi     r16, (1<<UCSZ01)|(1<<UCSZ00) ; 8N1
    sts     UCSR0C, r16

    ; ---- Timer1 ----
    ldi     r16, 0
    sts     TCCR1A, r16            ; modo normal
    sts     TCCR1B, r16            ; parado
    sts     TCNT1H, r16            ; zera (H antes de L)
    sts     TCNT1L, r16

    ; ---- Carrega contador 16-bit (igual ao uint16_t) ----
    ldi     r25, high(TOGGLE_INIT) ; r25:r24 = TOGGLE_INIT
    ldi     r24, low(TOGGLE_INIT)

    ; ---- Inicia Timer1 (/64) ----
    ldi     r16, T1_PRESC_BITS
    sts     TCCR1B, r16

    ; ---- Loop de toggles (PORTB ^= (1<<PB5)) ----
    ldi     r17, MASK_PB5
LOOP:
    in      r16, PORTB
    eor     r16, r17
    out     PORTB, r16

    sbiw    r24, 1                 ; r25:r24--
    brne    LOOP

    ; ---- Para Timer1 ----
    ldi     r16, 0
    sts     TCCR1B, r16

    ; ---- Lê TCNT1 (L depois H, latch em H) ----
    lds     r22, TCNT1L            ; ticks low
    lds     r23, TCNT1H            ; ticks high

    ; ---- Converte para ciclos: cycles = ticks << 6 ( *64 )
    ;    Usa registradores 32-bit: r21:r20:r23:r22 (high..low)
    clr     r20
    clr     r21
    lsl     r22            ; <<1
    rol     r23
    rol     r20
    rol     r21
    lsl     r22            ; <<2
    rol     r23
    rol     r20
    rol     r21
    lsl     r22            ; <<3
    rol     r23
    rol     r20
    rol     r21
    lsl     r22            ; <<4
    rol     r23
    rol     r20
    rol     r21
    lsl     r22            ; <<5
    rol     r23
    rol     r20
    rol     r21
    lsl     r22            ; <<6
    rol     r23
    rol     r20
    rol     r21
    ; Agora cycles = r21:r20:r23:r22

    ; ---- Impressão: "Ciclos: 0x" + 8 hex + "\r\n"
    ldi     r16, 'C'       ; "Ciclos: "
    rcall   UART_TX
    ldi     r16, 'i'
    rcall   UART_TX
    ldi     r16, 'c'
    rcall   UART_TX
    ldi     r16, 'l'
    rcall   UART_TX
    ldi     r16, 'o'
    rcall   UART_TX
    ldi     r16, 's'
    rcall   UART_TX
    ldi     r16, ':'
    rcall   UART_TX
    ldi     r16, ' '
    rcall   UART_TX
    ldi     r16, '0'
    rcall   UART_TX
    ldi     r16, 'x'
    rcall   UART_TX

    ; imprime 32-bit em hex (r21:r20:r23:r22)
    mov     r18, r21
    rcall   SEND_HEX_BYTE
    mov     r18, r20
    rcall   SEND_HEX_BYTE
    mov     r18, r23
    rcall   SEND_HEX_BYTE
    mov     r18, r22
    rcall   SEND_HEX_BYTE

    ldi     r16, '\r'
    rcall   UART_TX
    ldi     r16, '\n'
    rcall   UART_TX

END:
    rjmp    END

;--------------------------------------------------------
; Sub-rotinas UART/HEX
;--------------------------------------------------------

; UART_TX: envia caractere em r16
UART_TX:
WAIT_UDRE:
    lds     r17, UCSR0A
    sbrs    r17, UDRE0
    rjmp    WAIT_UDRE
    sts     UDR0, r16
    ret

; SEND_HEX_BYTE: entrada r18 = byte, imprime 2 hex
SEND_HEX_BYTE:
    mov     r16, r18
    swap    r16
    rcall   SEND_HEX_NIBBLE
    mov     r16, r18
    rcall   SEND_HEX_NIBBLE
    ret

; SEND_HEX_NIBBLE: r16 low-nibble -> ASCII e envia
SEND_HEX_NIBBLE:
    andi    r16, 0x0F
    cpi     r16, 10
    brlo    HEX_DIG
    subi    r16, 10
    ldi     r19, 'A'
    add     r16, r19
    rcall   UART_TX
    ret
HEX_DIG:
    ldi     r19, '0'
    add     r16, r19
    rcall   UART_TX
    ret
