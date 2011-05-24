#include <msp430x20x3.h>
#include "hc595.h"
#include "hc595-config.h"


void hc595_init(void)
{
	// set as outputs
	HC595_DIR |= HC595_DATA;
	HC595_DIR |= HC595_CLOCK;
	HC595_DIR |= HC595_LATCH;	
	HC595_DIR |= HC595_OE;
	HC595_OUT |= HC595_OE; // active low
}

void hc595_write_byte(unsigned char byte)
{
	unsigned char i;

	for(i=0;i<8;i++) {
		HC595_OUT &= ~HC595_DATA;
		if ((byte << i) & 0x80)
			HC595_OUT |= HC595_DATA;
		
		HC595_OUT |=  HC595_CLOCK;		
		HC595_OUT &= ~HC595_CLOCK;	
	}	
}

void hc595_latch(void)
{
	HC595_OUT |=  HC595_LATCH;		
	HC595_OUT &= ~HC595_LATCH;	
}

void hc595_oe(void)
{
	HC595_OUT &= ~HC595_OE;	
}
