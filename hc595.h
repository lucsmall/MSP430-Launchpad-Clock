#ifndef HC595_H_
#define HC595_H_

void hc595_init(void);
void hc595_write_byte(unsigned char byte);
void hc595_latch(void);
void hc595_oe(void);

#endif /*HC595_H_*/
