#include <avr/io.h>
#include <stdint.h>

// ====== Configuração ======
#define USE_PINB_TOGGLE 0   // 0 = PORTB ^= (1<<PB5);  1 = PINB = (1<<PB5)

// ====== UART (C++ estático simples) ======
struct UART {
	static void init_9600_16MHz() {
		UBRR0L = 103;                    // 9600 bps @ 16 MHz
		UCSR0B = (1 << TXEN0);
		UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	}
	static void tx(char c) {
		while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = c;
	}
	static void printDec32(uint32_t v) {
		char buf[11]; // 0..4294967295 + '\0'
		int i = 10;
		buf[i--] = '\0';
		if (v == 0) { tx('0'); return; }
		while (v > 0) {
			buf[i--] = char('0' + (v % 10U));
			v /= 10U;
		}
		for (i = i + 1; buf[i] != '\0'; ++i) tx(buf[i]);
	}
	static void printStr(const char* s) {
		while (*s) tx(*s++);
	}
};

// ====== Timer1 utilitário ======
struct Timer1 {
	// Escolhe prescaler para comportar "est_cycles" sem overflow no contador 16-bit
	static void choose_prescaler(uint32_t est_cycles, uint16_t& presc_val, uint16_t& presc_bits) {
		presc_val  = 1;
		presc_bits = (1 << CS10); // /1
		if (est_cycles > 65535UL) {
			if (est_cycles <= 65535UL * 8UL) {
				presc_val  = 8;
				presc_bits = (1 << CS11); // /8
				} else if (est_cycles <= 65535UL * 64UL) {
				presc_val  = 64;
				presc_bits = (1 << CS11) | (1 << CS10); // /64
				} else if (est_cycles <= 65535UL * 256UL) {
				presc_val  = 256;
				presc_bits = (1 << CS12); // /256
				} else {
				presc_val  = 1024;
				presc_bits = (1 << CS12) | (1 << CS10); // /1024
			}
		}
	}
	static void start(uint16_t presc_bits) {
		TCCR1A = 0;    // modo normal
		TCNT1  = 0;    // zera
		TCCR1B = presc_bits;
	}
	static uint16_t stop_and_read() {
		TCCR1B = 0;          // para o timer
		return TCNT1;        // lê contador
	}
};

// ====== GPIO PB5 ======
struct GpioPB5 {
	static void as_output() {
		DDRB |= (1 << PB5);
	}
	static void toggle() {
		#if USE_PINB_TOGGLE
		// Toggle por hardware: escrever 1 em PINx alterna o bit correspondente
		PINB = (1 << PB5);
		#else
		// Toggle via read-modify-write (mais caro em ciclos)
		PORTB ^= (1 << PB5);
		#endif
	}
};

int main() {
	// UART
	UART::init_9600_16MHz();

	// PB5 saída
	GpioPB5::as_output();

	// ----- Parâmetros do teste -----
	// Seus cenários típicos (1, 10, 100, 10000) cabem em 16 bits
	uint16_t toggle_count = 10000;  // ajuste aqui

	// Estimativa conservadora de ciclos/toggle em -O0
	const uint16_t cycles_per_toggle_est =
	#if USE_PINB_TOGGLE
	8;     // conservador para PINB
	#else
	200;   // conservador para PORTB^= em -O0
	#endif

	uint32_t est_cycles = (uint32_t)toggle_count * (uint32_t)cycles_per_toggle_est;

	// Escolha adaptativa de prescaler para não dar overflow
	uint16_t presc_val = 1, presc_bits = (1 << CS10);
	Timer1::choose_prescaler(est_cycles, presc_val, presc_bits);

	// ----- Medição -----
	Timer1::start(presc_bits);

	// Laço mais barato em -O0: while (n--)
	uint16_t n = toggle_count;
	while (n--) {
		GpioPB5::toggle();
	}

	uint16_t ticks = Timer1::stop_and_read();

	// Converte contagens do timer para ciclos de CPU:
	// Timer1 conta F_CPU/prescaler => ciclos = ticks * prescaler
	uint32_t cpu_cycles = (uint32_t)ticks * (uint32_t)presc_val;

	// ----- Saída -----
	UART::printStr("\nCiclos: ");
	UART::printDec32(cpu_cycles);
	UART::printStr("\n");

	while (true) { }
}
