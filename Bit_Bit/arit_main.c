// Benchmark SRAM – ATmega328P @ 16 MHz (C “equivalente” ao  ASM)
// - 256 bytes, 100 iterações, AND->OR->NOT
// - Timer1 prescaler = 64 (1 tick = 4 us) – evita overflow

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// ---------- UART (9600 8N1) ----------
static void uart_init(void) {
	// Baud = 9600 @ 16 MHz -> UBRR = 103
	UBRR0H = 0;
	UBRR0L = 103;
	UCSR0B = (1 << TXEN0);                // Habilita TX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
}

static void uart_tx(uint8_t b) {
	while (!(UCSR0A & (1 << UDRE0))) { /* espera buffer */ }
	UDR0 = b;
}

static void uart_print_str(const char *s) {
	while (*s) uart_tx((uint8_t)*s++);
}

static uint8_t hex_digit(uint8_t nibble) {
	nibble &= 0x0F;
	return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
}

static void uart_print_hex8(uint8_t v) {
	uart_tx(hex_digit(v >> 4));
	uart_tx(hex_digit(v));
}

static void uart_print_hex16(uint16_t v) {
	uart_print_hex8((uint8_t)(v >> 8));
	uart_print_hex8((uint8_t)(v & 0xFF));
}

// ---------- Dados ----------
static volatile uint8_t X[256];  // 'volatile' garante LD/ST reais na SRAM

static void fill_0_to_255(void) {
	for (uint16_t i = 0; i < 256; i++) {
		X[i] = (uint8_t)i;
	}
}

// ---------- Benchmark ----------
static uint16_t run_benchmark_and_measure_us(void) {
	// Timer1 em modo normal, parado
	TCCR1A = 0;
	TCCR1B = 0;
	// Zera contador e flag de overflow
	TCNT1  = 0;
	TIFR1  = (1 << TOV1);

	// Inicia: prescaler = 64 (1 tick = 4 us)
	// CS12:0 = 0b011 => CS11|CS10
	TCCR1B = (1 << CS11) | (1 << CS10);

	// --- janela de medição ---
	// (sem UART, sem interrupções)
	for (uint8_t k = 0; k < 100; k++) {
		// AND pass
		for (uint16_t i = 0; i < 256; i++) X[i] &= 0x0Fu;
		// OR pass
		for (uint16_t i = 0; i < 256; i++) X[i] |= 0xF0u;
		// NOT pass
		for (uint16_t i = 0; i < 256; i++) X[i]  = (uint8_t)~X[i];
	}
	// --- fim janela ---

	// Para Timer1
	TCCR1B = 0;

	// Lê ticks (TCNT1 é 16-bit; avr-gcc gera leitura L->H)
	uint16_t ticks = TCNT1;

	// Converte para microssegundos: us = ticks * 4
	// (cabe em 16 bits até ~262 ms; nosso ~38 ms cabe folgado)
	uint16_t micros = (uint16_t)(ticks << 2);
	return micros;
}

int main(void) {
	// Opcional: garantir janela “limpa” sem interrupções
	cli();

	uart_init();
	fill_0_to_255();

	// Medição
	uint16_t micros = run_benchmark_and_measure_us();

	// Saída (fora da janela medida)
	uart_print_str("Tempo (us): 0x");
	uart_print_hex16(micros);
	uart_print_str("\r\n");


	for (;;) { }
}
