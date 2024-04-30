/*
 * LWM_MSSY.c
 *
 * Created: 6.4.2017 15:42:46
 * Author : Krajsa
 */ 
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "libs/libprintfuart.h"
#include "uart/uart.h"
#include "makra.h"
#include "timer/timer.h"
#include "i2c/i2c.h"
#include "ADC/ADC.h"

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "hal.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysTimer.h"
#include "halBoard.h"
#include "halUart.h"
#include "main.h"

/*- Definitions ------------------------------------------------------------*/
#ifdef NWK_ENABLE_SECURITY
#define APP_BUFFER_SIZE     (NWK_MAX_PAYLOAD_SIZE - NWK_SECURITY_MIC_SIZE)
#else
#define APP_BUFFER_SIZE     NWK_MAX_PAYLOAD_SIZE
#endif

#define INTERRUPT_PIN   PORTE5 //interrupt pin pro ultrazvuk

/*- Types ------------------------------------------------------------------*/
typedef enum AppState_t
{
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
} AppState_t;

/*- Prototypes -------------------------------------------------------------*/
static void appSendData(void);
void measureDistance();
void timer1_init();
void board_init();
void initialize_interrupt();

/*- Variables --------------------------------------------------------------*/
static AppState_t appState = APP_STATE_INITIAL;
static SYS_Timer_t appTimer;
static NWK_DataReq_t appDataReq;
static bool appDataReqBusy = false;
static uint8_t appDataReqBuffer[APP_BUFFER_SIZE];
static uint8_t appUartBuffer[APP_BUFFER_SIZE];
static uint8_t appUartBufferPtr = 0;
static char message;

int obsazeno = 0;

volatile uint16_t pulse_width = 0;
volatile uint8_t echo_received = 0;
volatile uint8_t done = 0;

volatile uint8_t pinState;
volatile uint16_t echo_time;
volatile uint16_t lenght;


/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static void appDataConf(NWK_DataReq_t *req)
{
appDataReqBusy = false;
(void)req;
}

/*************************************************************************//**
*****************************************************************************/
static void appSendData(void)
{
if (appDataReqBusy || 0 == appUartBufferPtr)
return;

memcpy(appDataReqBuffer, appUartBuffer, appUartBufferPtr);

appDataReq.dstAddr = 1-APP_ADDR;
appDataReq.dstEndpoint = APP_ENDPOINT;
appDataReq.srcEndpoint = APP_ENDPOINT;
appDataReq.options = NWK_OPT_ENABLE_SECURITY;
appDataReq.data = appDataReqBuffer;
appDataReq.size = appUartBufferPtr;
appDataReq.confirm = appDataConf;
NWK_DataReq(&appDataReq);

appUartBufferPtr = 0;
appDataReqBusy = true;
}


static void SendOcupancy(char *data){

if (appDataReqBusy)
return;
/*
if ((ADC_get(3)>300) && (obsazeno==0))
{
	appDataReq.data = "Obsazeno \r\n";
	obsazeno=1;
}
if ((ADC_get(3)<300) && (obsazeno==1))
{
	appDataReq.data = "Neobsazeno \r\n";
	obsazeno=0;
}
*/

appDataReq.dstAddr = 1-APP_ADDR;
appDataReq.dstEndpoint = APP_ENDPOINT;
appDataReq.srcEndpoint = APP_ENDPOINT;
appDataReq.options = NWK_OPT_ENABLE_SECURITY;

appDataReq.data = data;
appDataReq.size = strlen(data);
appDataReq.confirm = appDataConf;
NWK_DataReq(&appDataReq);
appDataReqBusy = true;
}

/*************************************************************************//**
*****************************************************************************/
void HAL_UartBytesReceived(uint16_t bytes)
{
for (uint16_t i = 0; i < bytes; i++)
{
uint8_t byte = HAL_UartReadByte();

if (appUartBufferPtr == sizeof(appUartBuffer))
appSendData();

if (appUartBufferPtr < sizeof(appUartBuffer))
appUartBuffer[appUartBufferPtr++] = byte;
}

SYS_TimerStop(&appTimer);
SYS_TimerStart(&appTimer);
}

/*************************************************************************//**
*****************************************************************************/
static void appTimerHandler(SYS_Timer_t *timer)
{
appSendData();
(void)timer;
}

/*************************************************************************//**
*****************************************************************************/
static bool appDataInd(NWK_DataInd_t *ind)
{
for (uint8_t i = 0; i < ind->size; i++)
HAL_UartWriteByte(ind->data[i]);
return true;
}

/*************************************************************************//**
*****************************************************************************/
static void appInit(void)
{
NWK_SetAddr(APP_ADDR);
NWK_SetPanId(APP_PANID);
PHY_SetChannel(APP_CHANNEL);
#ifdef PHY_AT86RF212
PHY_SetBand(APP_BAND);
PHY_SetModulation(APP_MODULATION);
#endif
PHY_SetRxState(true);

NWK_OpenEndpoint(APP_ENDPOINT, appDataInd);

HAL_BoardInit();

appTimer.interval = APP_FLUSH_TIMER_INTERVAL;
appTimer.mode = SYS_TIMER_INTERVAL_MODE;
appTimer.handler = appTimerHandler;
}

/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
switch (appState)
{
case APP_STATE_INITIAL:
{
appInit();
appState = APP_STATE_IDLE;
} break;

case APP_STATE_IDLE:
break;

default:
break;
}
}


void initialize_interrupt() //inicializace interuptu pro ultrazvzvuk
{
	
	TIMSK1 |= (0 << TOIE1); //disable the overflow interrupt
	//trigger pin set as output
	DDRE |= (1 << PORTE2);
	
	// Configure interrupt pin as input
	DDRE &= ~(1 << INTERRUPT_PIN);
	
	// Enable external interrupt on INT5 (PE5)
	EIMSK |= (1 << INT5);
	
	// Trigger on falling edge
	EICRB |= (1 << ISC51);// | (1 << ISC50);
	_delay_ms(100);
	// Enable global interrupts
	sei();
}


void board_init(){
	
	cli(); //disable interrupts - defaultne je sice vyple, ale pro jistotu
	
	
	UART_init(38400); //baudrate 38400b/s
	
	TCCR1B |= (1 << CS12);  //Set up the clock source
	TIMSK1 |= (1 << TOIE1); //Enable the overflow interrupt
	
	
	
	i2cInit();
	ADC_Init(1,2);
	//PWM_Init();
	
	
	DDRE = 96;
	sei(); // enable interrupts
	
}

void getDistance()
{
	
	initialize_interrupt();
	
	
	done=0;
	PORTE |= (1<<PORTE2);
	_delay_us(10);
	PORTE &= ~(1<<PORTE2);
	TCCR4B |= 0x02;  //mozna odecist 10us
	while(!done){printf("Cekam \r\n");}
	lenght=(echo_time-2235)*0.017;
	//lenght=echo_time;
}

/*************************************************************************//**
*****************************************************************************/
int main(void)
{
	
board_init();

SYS_Init();
HAL_UartInit(38400);
HAL_UartWriteByte('a');

while (1)
{
SYS_TaskHandler();
HAL_UartTaskHandler();
APP_TaskHandler();
}
}

ISR(TIMER1_OVF_vect)
{
	if ((ADC_get(3)>300) && (obsazeno==0))
	{
		//printf("Obsazeno \r\n");
		getDistance();
		printf("Vzdalenost je %d",lenght);
		//message = ("Obsazeno, vzdalenost je %d \r\n", lenght);
		SendOcupancy("Obsazeno");
		obsazeno=1;
	}
	if ((ADC_get(3)<300) && (obsazeno==1))
	{
		SendOcupancy("Neobsazeno\r\n");
		obsazeno=0;
	}

	//printf("Aktualni intenzita svetla: %d \r\n" ,ADC_get(3));
}




ISR(INT5_vect) {
	
	/*
	
	// Read pin state
	pinState = (PINE5 & (1 << INTERRUPT_PIN)) ? 1 : 0;
	
	// Check pin state
	if (pinState) {
		// Pin is HIGH
		// Start timer
		TCNT1 = 0;
		} 
	else {
		// Pin is LOW
		echo_time = TCNT1; //mozná bude potreba predelat, jedna se o 16 bit cislo datasheet strana 302, 303 TCNT1H TCNT1L
		done = 1;
	}
	*/
// 	TCCR1B = 0;
// 	echo_time = TCNT1; //mozná bude potreba predelat, jedna se o 16 bit cislo datasheet strana 302, 303 TCNT1H TCNT1L
// 	TCNT1=0;
// 	done = 1;
	
	TCCR4B = 0;
	echo_time = TCNT4; //mozná bude potreba predelat, jedna se o 16 bit cislo datasheet strana 302, 303 TCNT1H TCNT1L
	TCNT4=0;
	done = 1;

}