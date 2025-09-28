#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

struct termios old_tio, new_tio;

void setup_term(){
	tcgetattr(0, &old_tio );
	new_tio = old_tio;
	new_tio.c_lflag &= ~ICANON;
	new_tio.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &new_tio);
}

void restore_term(){
	tcsetattr(0, TCSANOW, &old_tio);
	printf("\n");
	exit(0);
}

int main(){
	setup_term();
	signal(SIGINT, restore_term);
	unsigned char next, braille;
	unsigned char encoding[] = { 0x00, 0x80, 0x04, 0x20, 0x40, 0x02, 0x10, 0x00, 0x01, 0x08 };
	while( next = getchar() ){
		if( next == '-' ) break;
		if( next == 0x30 ){
			printf(" ");
			continue;
		}
		if( next == 0x0a ){
			printf("%c%c%c", 0xe2, 0xa0 | braille >> 6, 0x80 | braille & 0x3f );
			braille = 0x00;
		} else braille |= encoding[ next & 0x0f ];
	}
	restore_term();
}
