#include "stm8s.h"
#include "dot-led-indicator.h"

void printDigit(char digit, char position, char* dotIndicatorBuffer) {
	char i;
  const char digitPatterns[10][4] = {{0xFE,0x82,0x82,0xFE}, {0x00,0x00,0x00,0xFE}, {0x9E,0x92,0x92,0xF2}, {0x92,0x92,0x92,0xFE}, {0xF0,0x10,0x10,0xFE},   //0-1-2-3-4
                                     {0xF2,0x92,0x92,0x9E}, {0xFE,0x92,0x92,0x9E}, {0x80,0x80,0x80,0xFE}, {0xFE,0x92,0x92,0xFE}, {0xF2,0x92,0x92,0xFE}};  //5-6-7-8-9
  for (i = position * 4; i < ((position * 4) + 4); i++) {
		*(dotIndicatorBuffer + i) = digitPatterns[digit][i - (position * 4)];
	}	
}

void printTime(char hours, char minutes, char* dotIndicatorBuffer) {
	printDigit(hours/10, 0, dotIndicatorBuffer);
	printDigit(hours%10, 1, dotIndicatorBuffer);
	printDigit(minutes/10, 2, dotIndicatorBuffer);
	printDigit(minutes%10, 3, dotIndicatorBuffer);
}

void initDotLeds(void){
	PORT_HORIZONTAL->CR1 = 0x00; 
	PORT_HORIZONTAL->CR2 = 0xFF;
	PORT_HORIZONTAL->DDR = 0xFF;
	PORT_HORIZONTAL->ODR = 0x00;
	
	PORT_VERTICAL_12->ODR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->ODR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR1     &= ~VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR1 &= ~VERTICAL_MASK_345678;
	PORT_VERTICAL_12->DDR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->DDR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR2     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR2 |= VERTICAL_MASK_345678;
}

void refreshIndicator(char updCol, char* dotIndicatorBuffer) {
	char dl=0;
	if (updCol & 1) { //1-3-5-7-9-11-13-15 col
	
		PORT_VERTICAL_12     -> ODR |= VERTICAL_MASK_12;
		PORT_VERTICAL_345678 -> ODR |= VERTICAL_MASK_345678;
	  dl = ~(*(dotIndicatorBuffer + updCol));
		PORT_HORIZONTAL->CR1 = 0x00; 
		PORT_HORIZONTAL->ODR = ~(*(dotIndicatorBuffer + updCol));
		switch(updCol) {
		  case 1:{ PORT_VERTICAL_12     -> CR1 |= BIT_V1; break;}
		  case 3:{ PORT_VERTICAL_12     -> CR1 |= BIT_V2; break;}
			case 5:{ PORT_VERTICAL_345678 -> CR1 |= BIT_V3; break;}
			case 7:{ PORT_VERTICAL_345678 -> CR1 |= BIT_V4; break;}
			case 9:{ PORT_VERTICAL_345678 -> CR1 |= BIT_V5; break;}
			case 11:{PORT_VERTICAL_345678 -> CR1 |= BIT_V6; break;}
			case 13:{PORT_VERTICAL_345678 -> CR1 |= BIT_V7; break;}
			case 15:{PORT_VERTICAL_345678 -> CR1 |= BIT_V8; break;}
			
		}
	} else {          //0-2-4-6-8-10-12-14 col
	
		PORT_VERTICAL_12     -> CR1 &= ~VERTICAL_MASK_12;
		PORT_VERTICAL_345678 -> CR1 &= ~VERTICAL_MASK_345678;
		
	  PORT_HORIZONTAL->CR1 = *(dotIndicatorBuffer + updCol);
	  PORT_HORIZONTAL->ODR = 0xFF;
		
		switch(updCol) {
			case 0:{ PORT_VERTICAL_12     -> ODR &= ~BIT_V1;	break;}
		  case 2:{ PORT_VERTICAL_12     -> ODR &= ~BIT_V2;	break;}
			case 4:{ PORT_VERTICAL_345678 -> ODR &= ~BIT_V3;	break;}
			case 6:{ PORT_VERTICAL_345678 -> ODR &= ~BIT_V4;	break;}
			case 8:{ PORT_VERTICAL_345678 -> ODR &= ~BIT_V5;	break;}
			case 10:{PORT_VERTICAL_345678 -> ODR &= ~BIT_V6;	break;}
			case 12:{PORT_VERTICAL_345678 -> ODR &= ~BIT_V7;	break;}
		  case 14:{PORT_VERTICAL_345678 -> ODR &= ~BIT_V8;	break;}
			
		}
	}
	
}
/*
long int i = 0;
	int k = 5000;
	PORT_HORIZONTAL->CR1 = 0xFF;
	PORT_HORIZONTAL->CR2 = 0xFF;
	PORT_HORIZONTAL->DDR = 0xFF;
	PORT_HORIZONTAL->ODR = 0xFF;
	
	PORT_VERTICAL_12->ODR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->ODR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR1     &= ~VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR1 &= ~VERTICAL_MASK_345678;
	PORT_VERTICAL_12->DDR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->DDR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR2     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR2 |= VERTICAL_MASK_345678;
	
while (1){
	
	PORT_HORIZONTAL->CR1 = 0xFF;
	PORT_HORIZONTAL->CR2 = 0xFF;
	PORT_HORIZONTAL->DDR = 0xFF;
	PORT_HORIZONTAL->ODR = 0xFF;
	
	PORT_VERTICAL_12->ODR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->ODR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR1     &= ~VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR1 &= ~VERTICAL_MASK_345678;
	PORT_VERTICAL_12->DDR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->DDR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR2     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR2 |= VERTICAL_MASK_345678;
	
	
	nop();
  PORT_VERTICAL_12->ODR     &= ~BIT_V1;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_12->ODR     |= BIT_V1;
	
  PORT_VERTICAL_12->ODR     &= ~BIT_V2;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_12->ODR     |= BIT_V2;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V3;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V3;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V4;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V4;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V5;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V5;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V6;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V6;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V7;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V7;
	
  PORT_VERTICAL_345678->ODR     &= ~BIT_V8;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->ODR     |= BIT_V8;
	
	//////////////////
	
	PORT_HORIZONTAL->CR1 = 0x00; //-
	PORT_HORIZONTAL->CR2 = 0xFF;
	PORT_HORIZONTAL->DDR = 0xFF;
	PORT_HORIZONTAL->ODR = 0x00;
	
	PORT_VERTICAL_12->ODR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->ODR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR1     &= ~VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR1 &= ~VERTICAL_MASK_345678;
	PORT_VERTICAL_12->DDR     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->DDR |= VERTICAL_MASK_345678;
	PORT_VERTICAL_12->CR2     |= VERTICAL_MASK_12;
	PORT_VERTICAL_345678->CR2 |= VERTICAL_MASK_345678;

	
	nop();
  PORT_VERTICAL_12->CR1     |= BIT_V1;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_12->CR1     &= ~BIT_V1;
	
  PORT_VERTICAL_12->CR1     |= BIT_V2;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_12->CR1     &= ~BIT_V2;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V3;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V3;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V4;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V4;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V5;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V5;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V6;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V6;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V7;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V7;
	
  PORT_VERTICAL_345678->CR1     |= BIT_V8;
	for(i=1;i<k;i++) {
	  nop();
  }
	PORT_VERTICAL_345678->CR1     &= ~BIT_V8;

}
}*/