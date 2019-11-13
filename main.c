#include "16f877a.h"
#use delay(clock=20Mhz)
#use rs232(UART, baud=19200, enable=PIN_C5,stream=SLAVES)
#use rs232(xmit=PIN_B1, rcv=PIN_B0, baud=19200, stream=RASPBERRY)
#fuses HS

#define	MAXBUFFER	64
#define	STX 		0x02
#define	ETX			0x03

int buffer[MAXBUFFER], c;
boolean flag=false;
int next=0;

typedef struct{
	int 	_stx;
	int 	_address;
	int 	_func;
	long 	_payload;
	long 	_crc;
	int 	_etx;
}Packet;

Packet frame;

void clearBuffer(void){
	memset(buffer, 0, sizeof(buffer));
	next=0;
}

void main(void){
enable_interrupts(int_ext_h2l);
//enable_interrupts(int_rda);
enable_interrupts(global);
output_low(PIN_C5);
delay_ms(20);
	while(true){
		if(flag){
			disable_interrupts(global);
			frame=buffer;
			fprintf(SLAVES,"%c%c%c%c%c%c%c%c",frame._stx,frame._address,frame._func,frame._payload,frame._payload>>8,frame._crc,frame._crc>>8,frame._etx);
			flag=false;
			output_low(PIN_C5);
			clearBuffer();
			enable_interrupts(global);
		}
	}
}

#int_rda
void isrRda(void){
	output_low(PIN_C5);
}

#int_ext
void isrExt(void){
c=getc(RASPBERRY);
	if(c==ETX&&next==7){
		buffer[next]=c;
		next++;
		flag=true;
	}
	else if(next<8) {
		buffer[next]=c;
		next++;
	}
	else if(next>=8)
		clearBuffer();	
}