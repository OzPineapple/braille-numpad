#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

char alphabet[] = {
	#include "es.mx"
};
unsigned char beep_str[25], * buffer = NULL;

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
	free( buffer );
	printf("\n");
	exit(0);
}

void beep( unsigned short note, unsigned short len ){
	sprintf( beep_str, "beep -f %hu -l %hu", ++note * 37, len );
	system( beep_str );
}

int main(){
	setup_term();
	signal(SIGINT, restore_term);
	unsigned int buffer_size = 0;
	unsigned char next, braille;
	unsigned char encoding[] = { 0x00, 0x80, 0x04, 0x20, 0x40, 0x02, 0x10, 0x00, 0x01, 0x08 };
	unsigned char alpha;
	while( next = getchar() ) switch( next ){
		case '\n':
			printf("\n");
			for( unsigned int i = 0; i < buffer_size; i++ ){
				beep( buffer[i], 100 );
				alpha = alphabet[ buffer[i] ];
				printf("%c", alpha );
			}
			if( buffer_size ) printf("\n");
			buffer_size = 0;
		break;
		case '+':
			system("beep -f 1073 -l 100");
			braille = 0;
		break;
		case '-':
			system("beep -f 962 -l 50 -r 2 -n -f 851 -l 100");
			restore_term();
		break;
		case '.':
			printf("\e[D\xe2\xa0\x80\e[D");
			system("beep -f 962 -l 50 -r 2");
			braille = 0;
			buffer_size = buffer_size ? buffer_size-- : 0;
		break;
		case '0':
			printf("%c%c%c", 0xe2, 0xa0 | braille >> 6, 0x80 | braille & 0x3f );
			buffer = realloc( buffer, ++ buffer_size );
			buffer[ buffer_size - 1 ] = braille;
			braille = 0x00;
		break;
		default:
			beep( encoding[ next & 0x0f ] + 2, 25 );
			braille |= encoding[ next & 0x0f ];
	}
	restore_term();
}
