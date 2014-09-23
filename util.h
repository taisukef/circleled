// util.h
// license: PUBLIC DOMAIN

#define boolean unsigned int
#define true 1
#define false 0

// systick
volatile int systick;
void InitSysTick(int hz) {
	SYST_RVR = SYSCLK / hz - 1;
	SYST_CSR = 0x07;
}
void SysTick_Handler(void) {
	systick++;
}
void wait(int n) {
	int endt = systick + n;
	for (;;) {
		if (systick > endt)
			break;
	}
}

// uart

void uart_init() {
	uart0_init();
	xdev_out(uart0_putc);
	xdev_in(uart0_getc);
}

// string

boolean startsWith(char* s, char* key) {
	for (int i = 0;; i++) {
		char c1 = s[i];
		char c2 = key[i];
		if (c2 == '\0')
			return true;
		if (c1 == '\0')
			return false;
		if (c1 != c2)
			return false;
	}
}

int parseInt(char* s) {
	int res = 0;
	boolean minus = false;
	if (*s == '-') {
		s++;
		minus = true;
	}
	for (;;) {
		char c = *s++;
		if (c >= '0' && c <= '9') {
			res = res * 10 + (c - '0');
		} else
			break;
	}
	return minus ? -res : res;
}
