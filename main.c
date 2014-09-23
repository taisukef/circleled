//  Circle LED (12LED for LPC1114FN28 DIP)
//    License: CC BY http://fukuno.jig.jp/
//    Hardware & Description http://fukuno.jig.jp/790

#include "LPC1100.h"
#include "uart.h"
#include "xprintf.h"
#include "util.h"

/*
12LED
	http://www.aitendo.com/product/6524

PIO0_8	x
PIO0_9	x
PIO0_10	x
PIO0_11	D1
PIO0_5	x
PIO0_6	D3
VDDA
VSSA
PIO1_0	D12
PIO1_1	D10
PIO1_2	D8
PIO1_3	D6
PIO1_4	D4
PIO1_5	D2

PIO0_7	x
PIO0_4	x
PIO0_3	D9
PIO0_2	D11
ISP
RESET
VSS
VDD
XTALIN
XTALOUT
PIO1_9	D7
PIO1_8	D5
TXD
RXD
*/

void init() {
	IOCON_PIO0_3 = 0xd0;
	GPIO0DIR |= 1 << 3;
	IOCON_PIO0_2 = 0xd0;
	GPIO0DIR |= 1 << 2;
	IOCON_PIO1_9 = 0xd0;
	GPIO1DIR |= 1 << 9;
	IOCON_PIO1_8 = 0xd0;
	GPIO1DIR |= 1 << 8;
	
	IOCON_R_PIO0_11 = 0xd1;
	GPIO0DIR |= 1 << 11;
	IOCON_PIO0_6 = 0xd0;
	GPIO0DIR |= 1 << 6;
	IOCON_R_PIO1_0 = 0xd1;
	GPIO1DIR |= 1 << 0;
	IOCON_R_PIO1_1 = 0xd1;
	GPIO1DIR |= 1 << 1;
	IOCON_R_PIO1_2 = 0xd1;
	GPIO1DIR |= 1 << 2;
	IOCON_SWDIO_PIO1_3 = 0xd1;
	GPIO1DIR |= 1 << 3;
	IOCON_PIO1_4 = 0xd0; // 使わないほうがよかったかも
	GPIO1DIR |= 1 << 4;
	IOCON_PIO1_5 = 0xd0;
	GPIO1DIR |= 1 << 5;
}
void circleled_on(int n) {
	/*
PIO0_11	D1
PIO1_5	D2
PIO0_6	D3
PIO1_4	D4
PIO1_8	D5
PIO1_3	D6
PIO1_9	D7
PIO1_2	D8
PIO0_3	D9
PIO1_1	D10
PIO0_2	D11
PIO1_0	D12
	*/
	int pos[] = { 11, 105, 6, 104, 108, 103, 109, 102, 3, 101, 2, 100 };
	
	int set0 = 0;
	int set1 = 0;
	
	for (int i = 0; i < 12; i++) {
		if (n & (1 << i)) {
			int m = pos[i];
			if (m < 100) {
				set0 |= 1 << m;
			} else {
				set1 |= 1 << (m - 100);
			}
		}
	}
/*
PIO0_2	D11
PIO0_3	D9
PIO0_6	D3
PIO0_11	D1
	
PIO1_0	D12
PIO1_1	D10
PIO1_2	D8
PIO1_3	D6
PIO1_4	D4
PIO1_5	D2
PIO1_8	D5
PIO1_9	D7
*/
	GPIO0MASKED[0b100001001100] = set0;
	GPIO1MASKED[0b001100111111] = set1;
}

// uart

void uart() {
#define SIZE_BUF 128
	
	char buf[SIZE_BUF];
	int nbuf = 0;
	
	for (int i = 0;; i++) {
		while (uart0_test()) {
			int c = uart0_getc();
			if (c == '\n') {
				buf[nbuf] = '\0';
				if (startsWith(buf, "'CLED ")) {
					int n = parseInt(buf + 9);
					circleled_on(n);
				}
				nbuf = 0;
				continue;
			} else if (c == '\r') {
			} else {
				if (nbuf < SIZE_BUF - 1)
					buf[nbuf++] = c;
			}
		}
	}
}

void test() {
	for (int i = 0;; i++) {
		for (int j = 0; j < 12; j++) {
			circleled_on(1 << j);
			wait(10);
			if (uart0_test()) {
				return;
			}
		}
	}
}

int main() {
	InitSysTick(200);
	init();
	
	circleled_on(0);
	
	uart_init();
	
	test();
	uart();
	return 0;
}
