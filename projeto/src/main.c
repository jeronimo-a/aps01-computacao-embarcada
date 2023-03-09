#include "asf.h"

// LED embutido
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão de voltar a música (SW1 da OLED1XPLAINED)
#define BACK_BUT_PIO				PIOD						// periferico que controla o SW 1 no modulo OLED
#define BACK_BUT_PIO_ID				ID_PIOD						// ID do periferico PIOD
#define BACK_BUT_IDX				28							// ID do pino conectado ao SW 1 do modulo OLED
#define BACK_BUT_IDX_MASK			(1 << BACK_BUT_IDX)			// mascara para controlarmos o SW 1 do modulo OLED

// Botão de pause-play (SW2 da OLED1XPLAINED)
#define PLAY_BUT_PIO				PIOC						// periferico que controla o SW 2 no modulo OLED
#define PLAY_BUT_PIO_ID				ID_PIOC						// ID do periferico PIOC
#define PLAY_BUT_IDX				31							// ID do pino conectado ao SW 2 do modulo OLED
#define PLAY_BUT_IDX_MASK			(1 << PLAY_BUT_IDX)			// mascara para controlarmos o SW 2 do modulo OLED

// Botão de avançar a música (SW3 da OLED1XPLAINED)
#define PASS_BUT_PIO				PIOA						// periferico que controla o SW 3 no modulo OLED
#define PASS_BUT_PIO_ID				ID_PIOA						// ID do periferico PIOA
#define PASS_BUT_IDX				19							// ID do pino conectado ao SW 3 do modulo OLED
#define PASS_BUT_IDX_MASK			(1 << PASS_BUT_IDX)			// mascara para controlarmos o SW 3 do modulo OLED

// Flags
volatile char BACK_BUT_DW;	// 1 para botao pressionado
volatile char PLAY_BUT_DW;	// 1 para botao pressionado
volatile char PASS_BUT_DW;	// 1 para botao pressionado

// Globais
int tempo_minimo = 4;	// subdivisao minima do tempo em BPS
int period_ms;

// Prototipos de funcao
void main(void);
void io_init(void);
void callback_back_but(void);
void callback_play_but(void);
void callback_pass_but(void);

//- --- --- ---  Callbacks	--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
void callback_back_but(void) {
	if (pio_get(BACK_BUT_PIO, PIO_INPUT, BACK_BUT_IDX_MASK)) { BACK_BUT_DW = 0;}
	else { BACK_BUT_DW = 1;}
}

void callback_play_but(void) {
	if (pio_get(PLAY_BUT_PIO, PIO_INPUT, PLAY_BUT_IDX_MASK)) { PLAY_BUT_DW = 0;}
	else { PLAY_BUT_DW = 1;}
}

void callback_pass_but(void) {
	if (pio_get(PASS_BUT_PIO, PIO_INPUT, PASS_BUT_IDX_MASK)) { PASS_BUT_DW = 0;}
	else { PASS_BUT_DW = 1;}
}
//- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

// Inicializa os PIOs usados e os interrupts dos botoes
void io_init(void) {

	// Inicializa clock dos PIOs
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);

	// Configura os PIOs para lidar com os pinos dos botoes como entradas com pull-up e debounce
	pio_configure(PIOA, PIO_INPUT, PASS_BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(PIOC, PIO_INPUT, PLAY_BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(PIOD, PIO_INPUT, BACK_BUT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	
	// Configura o PIOC para lidar com o pino do LED como saida
	pio_configure(PIOC, PIO_OUTPUT_1, LED_IDX_MASK, PIO_DEFAULT);

	// Define interrupcoes e associa a respectiva funcao de callback
	pio_handler_set(PIOA, ID_PIOA, PASS_BUT_IDX_MASK, PIO_IT_EDGE, callback_pass_but);
	pio_handler_set(PIOC, ID_PIOC, PLAY_BUT_IDX_MASK, PIO_IT_EDGE, callback_play_but);
	pio_handler_set(PIOD, ID_PIOD, BACK_BUT_IDX_MASK, PIO_IT_EDGE, callback_back_but);

	// Ativa as interrupcoes e limpa as primeiras IRQs geradas na ativacao
	pio_enable_interrupt(PIOA, PASS_BUT_IDX_MASK);
	pio_enable_interrupt(PIOC, PLAY_BUT_IDX_MASK);
	pio_enable_interrupt(PIOD, BACK_BUT_IDX_MASK);
	pio_get_interrupt_status(PIOA);
	pio_get_interrupt_status(PIOC);
	pio_get_interrupt_status(PIOD);
	
	// Configura NVIC para receber interrupcoes dos PIOs dos botoes com prioridade 4
	NVIC_EnableIRQ(ID_PIOA);
	NVIC_EnableIRQ(ID_PIOC);
	NVIC_EnableIRQ(ID_PIOD);
	NVIC_SetPriority(ID_PIOA, 4);
	NVIC_SetPriority(ID_PIOC, 4);
	NVIC_SetPriority(ID_PIOD, 4);
}

// Funcao principal chamada na inicalizacao do uC.
void main(void)
{

	sysclk_init();					// inicializa o clock
	WDT -> WDT_MR = WDT_MR_WDDIS;	// desativa o watchdog timer
	io_init();						// inicializa os PIOs
	
	while(1) {
		
		if (BACK_BUT_DW || PLAY_BUT_DW || PASS_BUT_DW) {
			pio_clear(PIOC, LED_IDX_MASK);
		}
		
		else {
			pio_set(PIOC, LED_IDX_MASK);
		}
		
		
	}
}
