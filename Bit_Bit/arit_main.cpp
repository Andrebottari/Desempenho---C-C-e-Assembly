// Benchmark SRAM – ATmega328P @ 16 MHz (C++ com classes, equivalente ao ASM )
// - 256 bytes, 100 iterações, AND->OR->NOT
// - Timer1 prescaler = 64 (1 tick = 4 us) – evita overflow


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

class Uart {
	public:
	static void init() {
		UBRR0H = 0;
		UBRR0L = 103;                           // 16MHz / (16*9600) - 1
		UCSR0B = (1 << TXEN0);                  // habilita TX
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1
	}

	static void tx(uint8_t b) {
		while (!(UCSR0A & (1 << UDRE0))) { /* espera */ }
		UDR0 = b;
	}

	static void print_P(PGM_P s) {
		uint8_t c;
		while ((c = pgm_read_byte(s++)) != 0) { tx(c); }
	}

	static uint8_t hexDigit(uint8_t nibble) {
		nibble &= 0x0F;
		return (nibble < 10) ? uint8_t('0' + nibble) : uint8_t('A' + (nibble - 10));
	}

	static void printHex8(uint8_t v) {
		tx(hexDigit(v >> 4));
		tx(hexDigit(v));
	}

	static void printHex16(uint16_t v) {
		printHex8(uint8_t(v >> 8));
		printHex8(uint8_t(v & 0xFF));
	}
};

class Timer1 {
	public:
	static void startPrescaler64() {
		TCCR1A = 0;
		TCCR1B = 0;
		TCNT1  = 0;
		TIFR1  = (1 << TOV1);
		// CS12:0 = 0b011 (prescaler = 64)
		TCCR1B = (1 << CS11) | (1 << CS10);
	}

	static void stop() {
		TCCR1B = 0;
	}

	static uint16_t ticks() {
		// Timer parado -> leitura consistente
		return TCNT1;
	}

	static uint16_t ticksToMicros(uint16_t t) {
		// 1 tick = 4 us -> us = t * 4 (cabe em 16 bits até ~262 ms)
		return uint16_t(t << 2);
	}
};

class BitOpsBench {
	public:
	// Array em SRAM; 'volatile' garante LD/OP/ST reais
	static volatile uint8_t X[256];

	void fill_0_to_255() {
		for (uint16_t i = 0; i < 256; i++) {
			X[i] = static_cast<uint8_t>(i);
		}
	}

	// Executa o benchmark e retorna o tempo total em microssegundos
	uint16_t runAndMeasureMicros() {
		Timer1::startPrescaler64();

		// --- janela de medição (sem UART/ISR) ---
		for (uint8_t k = 0; k < 100; k++) {
			for (uint16_t i = 0; i < 256; i++) X[i] &= 0x0Fu;
			for (uint16_t i = 0; i < 256; i++) X[i] |= 0xF0u;
			for (uint16_t i = 0; i < 256; i++) X[i]  = static_cast<uint8_t>(~X[i]);
		}
		// --- fim da janela ---

		Timer1::stop();
		const uint16_t t = Timer1::ticks();
		return Timer1::ticksToMicros(t);
	}
};

// Definição do membro estático
volatile uint8_t BitOpsBench::X[256];

int main() {
	cli();                        // janela limpa (sem interrupções)
	Uart::init();

	BitOpsBench bench;
	bench.fill_0_to_255();

	const uint16_t micros = bench.runAndMeasureMicros();

	Uart::print_P(PSTR("Tempo (us): 0x"));
	Uart::printHex16(micros);
	Uart::print_P(PSTR("\r\n"));

	for (;;) { }
}
