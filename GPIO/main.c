#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// ====== Configuráveis ======
#define USE_PINB_TOGGLE 0   // 0 = PORTB ^= (1<<PB5) (RMW); 1 = PINB = (1<<PB5) (toggle HW)

// ====== UART básica ======
static void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

static void print_decimal32(uint32_t value) {
	char buf[11]; // max 4294967295 + '\0'
	int i = 10;
	buf[i--] = '\0';
	if (value == 0) { uart_tx('0'); return; }
	while (value > 0) {
		buf[i--] = (value % 10U) + '0';
		value /= 10U;
	}
	for (i = i + 1; buf[i] != '\0'; i++) uart_tx(buf[i]);
}

// ====== Medição ======
int main(void) {
	// UART 9600 bps @16MHz
	UBRR0L = 103;
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// PB5 como saída
	DDRB |= (1 << PB5);

	// ---- Parametrização do teste ----
	// 16 bits: suficiente até 65535; 
	uint16_t toggle_count = 10000; // ajuste aqui: 1, 10, 100, 10000...

	// Estimativa conservadora de ciclos por toggle em -O0:
	// - Com PORTB^=: ~50–150 ciclos/toggle (depende do build)
	// - Com PINB: ~1–5 ciclos/toggle
	
	const uint16_t cycles_per_toggle_est =
	#if USE_PINB_TOGGLE
	8;     // muito conservador para PINB
	#else
	200;   // conservador para PORTB^= em -O0
	#endif

	// Total estimado de ciclos (conservador)
	uint32_t est_cycles = (uint32_t)toggle_count * (uint32_t)cycles_per_toggle_est;

	// Escolha adaptativa do prescaler para evitar overflow do Timer1 (16 bits)
	// Capacidade sem overflow = 65535 contagens * prescaler
	uint16_t presc_bits = (1 << CS10); // default /1
	uint16_t presc_val  = 1;

	if (est_cycles > 65535UL) {               
		if (est_cycles <= 65535UL * 8UL) {
			presc_bits = (1 << CS11);         // /8
			presc_val  = 8;
			} else if (est_cycles <= 65535UL * 64UL) {
			presc_bits = (1 << CS11) | (1 << CS10); // /64
			presc_val  = 64;
			} else if (est_cycles <= 65535UL * 256UL) {
			presc_bits = (1 << CS12);         // /256
			presc_val  = 256;
			} else {
			presc_bits = (1 << CS12) | (1 << CS10); // /1024
			presc_val  = 1024;
		}
	}

	// ---- Medição ----
	// Timer1 em modo normal
	TCCR1A = 0;
	TCNT1  = 0;

	// Start timer com prescaler escolhido
	TCCR1B = presc_bits;

	
	uint16_t n = toggle_count;
	while (n--) {
		#if USE_PINB_TOGGLE
		// Toggle por hardware: escrever 1 em PINx alterna o bit correspondente
		PINB = (1 << PB5);
		#else
		// Toggle via read-modify-write (mais caro)
		PORTB ^= (1 << PB5);
		#endif
	}

	// Stop timer antes de qualquer I/O de UART
	TCCR1B = 0;

	// Leitura do contador (sem overflow porque dimensionamos prescaler)
	uint16_t ticks = TCNT1;

	// Converte contagens do timer para ciclos de CPU
	// (Timer conta F_CPU/prescaler, então ciclos = ticks * prescaler)
	uint32_t cpu_cycles = (uint32_t)ticks * (uint32_t)presc_val;

	// ---- Saída ----
	uart_tx('\n');
	uart_tx('C'); uart_tx('i'); uart_tx('c'); uart_tx('l'); uart_tx('o'); uart_tx('s'); uart_tx(':'); uart_tx(' ');
	print_decimal32(cpu_cycles);
	uart_tx('\n');



	while (1) { }
}
