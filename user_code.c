//PIC32duino code
// - using PIC32MX1xx/2xx
// - free running core timer for ticks() (or tmr2, selected by the user)
// - free running tmr2 for pwm / output compare / input capture
//
// - version history
// - v2.0, 5/13/2021: original port
// - v2.1, 5/26/2021: implemented SystemCoreClockUpdate() that updates SystemCoreClock during initialization
//                    implemented systicks() from TMR2 ISR - optional (in addition to core timer)
// - v2.2, 5/27/2021: implemented i2c/rtcc
// - v2.3, 6/19/2021: added support for output compare (dah!)
// - v2.4, 5/29/2021: output compare routines no longer advance compare registers
// - v2.5, 5/24/2022: added support for C32 compiler
//
//
//hardware configuration
//
//               PIC32MX1/2xx
//              |=====================|
//    Vcc       |                     |
//     |        |                AVdd |>---+--[1k]-->Vcc
//     |        |                     |  [.1u]
//     +-[10K]-<| MCLR           AVss |>---+-------->GND
//              |                     |
//              |                     |
//     +------->| OSCI           Vcap |>--[.1u]-+->GND
//  [Xtal]      |                     |         |
//     +-------<| OSCO           Vss  |>--------+
//              |                     |
//     +------->| SOSCI          RPB0 |>---------->Uart2TX
// [32,768Hz]   |                     |
//     +-------<| SOSCO           RB7 |>---------->LED
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |                     |
//              |=====================|
//
//
//

#include "pic32duino.h"					//we use pic32duino

//hardware configuration
#define LED			PB7					//led pin
#define LED1_DLY		(F_CPU / 2)			//duration of delay
#define LED2_DLY		(F_CPU / 3)			//duration of delay
#define U2_DLY			(F_CPU / 2)			//duration of delay
#define U3_DLY			(F_CPU / 3)			//duration of delay
//end hardware configuration

//global defines

//global variables

//thread 1
//blinking led at LED1_DLY interval
//no need to use while (1) loop
static int led1(void) {
	static uint32_t tick0=0;

	TS_RUN_WHILE(ticks() - tick0 > LED1_DLY) {	//hold execution until enough time has passed
		tick0+= LED1_DLY;						//advance the tick
		pinFlip(LED);							//task1=flip the led
	}
	return tick0;
}

//thread 2
//blinking the same led at LED2_DLY interval
//no need to use while (1) loop
static int led2(void) {
	static uint32_t tick0=0;

	TS_RUN_WHILE(ticks() - tick0 > LED2_DLY) {	//hold execution until enough time has passed
		tick0+= LED2_DLY;						//advance the tick
		pinFlip(LED);							//task2=flip the led
	}
	return tick0;
}

//thread 3
//print to uart
//OK to use while(1) loop
static int u2Msg(void) {
	static uint32_t tick0=0;

	TS_RUN_WHILE(ticks() - tick0 > U2_DLY) {//hold execution until enough time has passed
		tick0+= U2_DLY;							//advance the tick
		//u2Print("F_CPU=                    ", F_CPU);	//print msg
		uart2Puts("U2Msg Print... -> ");
		u2Print("tick0=                    ", tick0);	//print msg
		u2Println();									//print a line return
	}
	return tick0;
}

//thread 4
//print to uart
//no need to use while (1) loop
static int u3Msg(void) {
	static uint32_t tick0=0;

	TS_RUN_WHILE(ticks() - tick0 > U3_DLY) {//hold execution until enough time has passed
		tick0+= U3_DLY;							//advance the tick
		//u2Print("F_CPU=                    ", F_CPU);	//print msg
		uart2Puts("U3Msg Print... -> ");
		u2Print("tick0=                    ", tick0);	//print msg
		u2Println();									//print a line return
	}
	return tick0;
}

//user defined set up code
void setup(void) {
    pinMode(LED, OUTPUT);				//led as output pin

    //initialize the uart
    //uart1Init(UART_BR9600);				//initialize uart1
    uart2Init(UART_BR9600);				//initialize uart2

	ei();
}

//user defined main loop
void loop(void) {
	//running through all threads
	led1();						//pt1
	led2();						//pt2
	u2Msg();					//pt3
	u3Msg();					//pt4
}

