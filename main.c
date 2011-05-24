#include <msp430x20x3.h>
#include "hc595.h"

#define SWITCH_IN  P1IN
#define SWITCH_OUT P1OUT
#define SWITCH_REN P1REN
#define SWITCH_0   BIT6
#define SWITCH_1   BIT7

void main_init(void);

// Mapping between HC595 outputs and 7 segment display segments
unsigned char digits[] = {
	0x3f,
	0x06,
	0x5b,
	0x4f,
	0x66,
	0x6d,
	0x7d,
	0x07,
	0x7f,
	0x6f
};

// Timekeeping variables
unsigned char hh = 0, mm = 0, ss = 0;
volatile unsigned char switch0_state = 0;
volatile unsigned char switch1_state = 0;
volatile unsigned char time_set = 0;

void main(void)
{
 	unsigned char i, data;	
	
 	main_init();
 	hc595_init();
 	// Blank output
 	hc595_write_byte(0x00);
 	hc595_write_byte(0xff);
 	hc595_latch();
 	hc595_oe();
  
 	__enable_interrupt();
  

 	i = 1;
 	while(1) {
 		if (switch0_state) {
 			hh++;
 			if (hh > 23) hh = 0;
 			switch0_state = 0;	
 			time_set = 1;
 		}
 		if (switch1_state) {
 			mm++;
 			if (mm > 59) mm = 0;
 			ss = 0;
 			switch1_state = 0;
 			time_set = 1;	
 		} 		
  		// Continuously scan multiplexed display
  		// Assert the current digit's anode
  		hc595_write_byte(~i);
  		// Select correct data for current digit
  		switch(i) {
  			case 1:
  				data = digits[hh / 10];
  				break;
  			case 2:
  				data = digits[hh % 10];
  				// Toggle the middle decimal point as a seconds heartbeat
  				if (ss % 2) data |= 0x80;
  				break;
  			case 4:
  				data = digits[mm / 10];
  				break;
  			case 8:	
  				data = digits[mm % 10];
  				break;
  		}
  		// flash if time not set
  		if ((!time_set) && (ss%2)) {
  			data = 0;	
  		}
  		hc595_write_byte(data);
		hc595_latch();
		// move on to the next digit	
		i <<= 1;
		if (i>8) i = 1;
 	}
}

void main_init(void)
{
 	// Stop the watchdog timer
	WDTCTL = WDTPW + WDTHOLD;        
	
	// make it a pullup resistor
	SWITCH_OUT |= SWITCH_0 + SWITCH_1;
	// enable resistor
	SWITCH_REN |= SWITCH_0 + SWITCH_1;
	
	         
  	// Set load capacitance for 32.768kHz crystal
  	BCSCTL3 |= XCAP1 + XCAP0;
  	// Enable output for ACLK on P1.0 pin
  	P1SEL |= BIT0;
 	// Setup watchdog timer as an interval timer with one second interrupts
  	WDTCTL = WDTPW + WDTTMSEL + WDTCNTCL + WDTSSEL + WDTIS1 + WDTIS0; 
  	// Enable the watchdog timer interrupt
  	IE1 |= WDTIE;
}

inline void update_switches(void)
{
  	static unsigned char state[2] = {0xFF, 0xFF}; // Current debounce status

 	state[0] = (state[0] << 1) | ((SWITCH_IN & SWITCH_0) ? 1 : 0);
  	state[1] = (state[1] << 1) | ((SWITCH_IN & SWITCH_1) ? 1 : 0);

  	if (state[0] == 0x80) {
  		switch0_state = 1;
  	}
  	if (state[1] == 0x80) {
  		switch1_state = 1;
  	}
}

// Interrupt fired 32768/64 = 512 times a second
#pragma vector=WDT_VECTOR
__interrupt void wdt_interrupt(void)
{
	static unsigned int ticks = 0;
	if (ticks > 511) 
	{
		// Implement real time clock
		ss++;
		if (ss > 59) {
			ss = 0;
			mm ++;
			if (mm > 59) {
				mm = 0;	
				hh++;
				if (hh > 23) hh = 0;
			} 
		}
		
		ticks = 0;
	}
	ticks++;
	
	update_switches();
}

