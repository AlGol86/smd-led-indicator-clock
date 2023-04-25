#include "stm8s.h"
#include "rtc.h"
#include "main.h"
#include "encoder.h"
#include "iic_embedded_tx.h"
#include "oled.h"
#include "dot-led-indicator.h"



char dotIndicatorBuffer[INDICATOR_HORIZONTAL_SIZE];
char currentCol;
char colonPWD;

encoder_t              encoder;

AlignmentTimeCache     alignmentTimeCache;
TransferBody           transferBody; 
TimeTransferBodyCache  timeTransferBodyCache;
SecondsRtcUtcCache     secondsRtcUtcCache;
DateFromEpochDaysCache dateFromEpochDaysCache;

TimeAlignment          timeAlignment;
eeprom_data_t          eeprom_data @0x4100;         //store setting variables (in EEPROM)()

OledDigitBuffer oledBuffer;

unsigned int resetCondition;
char strokeDuration;
char strokeStartHour;
char strokeStopHour;

main() { 
  char h, iic_error;
	char i;
	char ii;
  long l;
	
  GPIOD->DDR |=1;
  GPIOD->ODR |=1;

  init_iic_emb_tx();
  
  init_rtc();
	
	init_ssd1306();
	if(resetCondition == 10000) iic_error = 1;
	else iic_error = 0;
	if(!iic_error) init_encoder(&encoder);
	initDotLeds();
	init_tim2();
  populate_timeAlignment_from_eeprom();
	strokeDuration = eeprom_data.strokeDuration;
	strokeStartHour = eeprom_data.strokeStartHour;
	strokeStopHour = eeprom_data.strokeStopHour;
  encoder.but_data_lim = 1;
  encoder_setter(0, 10, 5);
  while(1) { 
	


		printTime(transferBody.hr, transferBody.min, dotIndicatorBuffer);
		
	
		

	  //set brightness
		if(encoder.transient_counter & 0x1000 && !iic_error) {
		//	i = get_ADC(6);       //adopted
			i = encoder.enc_data*5; //manual
		  set_brightness_ssd1306(i < 25 ? 0 : 1);//(i >= 50 ? 255 : i*5);
	  }
		
		refreshTimeTransferBody();
		i2c_wr_reg(RX_ADDR, 0x00, &transferBody.year, TRANSFERED_SIZE);
		if(!iic_error) print_time();	
		
	/*	
		//stroke an hour
		if ((transferBody.min == 59 && transferBody.sec > 55) || (transferBody.min == 0 && transferBody.sec < 10)) {
		 // stroke_com(ON);
	  }	else if (transferBody.min == 0){
		  //stroke_com(OFF);
		}
		if (h != transferBody.hr && transferBody.hr >= strokeStartHour && transferBody.hr <= strokeStopHour) {
		  h = transferBody.hr;
			//stroke(strokeDuration);
	  }
		*/
		
    //fall into adjusting parameters
    if (encoder.but_data != 0) {   
      //stroke_com(OFF);		
      menu_selector();
			oled_Clear_Screen();
		//	encoder_setter(0, 10, i/5);
    }
  }
	
}


char menu_selector() {
	LocalTime presetTime;
	LocalDate presetDate;
	TimeAlignment presetCorrection;
	int i;
	
	//set time	
	presetTime.hr = transferBody.hr;
	presetTime.min = transferBody.min;
	oled_print_XXnumber(0, 96, 1);
	
	encoder.but_data = 0;
	encoder_setter(0, 23, presetTime.hr);
	presetTime.hr = scan_value_at_pos(4);
	oled_print_XXnumber(presetTime.hr, 4, 1);
	
	encoder.but_data = 0;
	encoder_setter(0, 59, presetTime.min);
	presetTime.min = scan_value_at_pos(50);	
	
	//set date
	presetDate.dayOfMonth = transferBody.dayOfMonth;
	presetDate.month = transferBody.month;
  presetDate.year = transferBody.year;	
	oled_print_giga_char('-', 81);
	oled_print_giga_char('-', 35);
	oled_print_XXnumber(presetDate.month, 50, 0); 
	oled_print_XXnumber(presetDate.year, 96, 0);  
	
	encoder.but_data = 0;
	encoder_setter(1, 31, presetDate.dayOfMonth);
	presetDate.dayOfMonth = scan_value_at_pos(4);
	oled_print_XXnumber(presetDate.dayOfMonth, 4, 1);
	
	encoder.but_data = 0;
	encoder_setter(1, 12, presetDate.month);
	presetDate.month = scan_value_at_pos(50);
	oled_print_XXnumber(presetDate.month, 50, 1);
	
	encoder.but_data = 0;
	encoder_setter(0, 99, presetDate.year);
	presetDate.year = scan_value_at_pos(96);
  
	//save date and time if changed
	if(presetDate.dayOfMonth != transferBody.dayOfMonth || presetDate.month != transferBody.month || presetDate.year != transferBody.year || presetTime.hr != transferBody.hr || presetTime.min != transferBody.min) {
		print_save(); 
		rtc_set_time_date(presetTime, presetDate);
    secondsRtcUtcCache.cacheEneble = 0;
		timeAlignment.epochSecFirstPoint = receiveEpochSecondsRtcMoscow();
		saveFirstPointTimeToEeprom();
		
	}
	
	//set correction
	oled_Clear_Screen();
	presetCorrection.timeCorrSec = timeAlignment.timeCorrSec;
	presetCorrection.timeCorrDecaMs = timeAlignment.timeCorrDecaMs;
	presetCorrection.positiveCorr = timeAlignment.positiveCorr;
	oled_print_giga_char('.',48); 
	oled_print_XXnumber(presetCorrection.timeCorrDecaMs, 60, 0); 
		
	encoder.but_data = 0;
	encoder_setter(-99, 99, presetCorrection.positiveCorr ? presetCorrection.timeCorrSec : -presetCorrection.timeCorrSec);
	i = scan_value_at_pos(0);
	presetCorrection.timeCorrSec = (i<0) ? -i : i;
	presetCorrection.positiveCorr = (i<0) ? 0 : 1;
	oled_print_XXnumber(i, 0, 1);
	
	encoder.but_data = 0;
	encoder_setter(0, 99, presetCorrection.timeCorrDecaMs);
	presetCorrection.timeCorrDecaMs = scan_value_at_pos(60);
	
  //save correction if changed
	if(presetCorrection.timeCorrSec != timeAlignment.timeCorrSec || presetCorrection.timeCorrDecaMs != timeAlignment.timeCorrDecaMs || presetCorrection.positiveCorr != timeAlignment.positiveCorr) {
		print_save(); 
		timeAlignment.timeCorrSec = presetCorrection.timeCorrSec;
		timeAlignment.timeCorrDecaMs = presetCorrection.timeCorrDecaMs;
		timeAlignment.positiveCorr = presetCorrection.positiveCorr;
		saveTimeCorrectionToEeprom();		
		alignmentTimeCache.cacheEneble = 0;
	}
	
	//display days from first point	
	oled_Clear_Screen();
	encoder.but_data = 0;
	while(encoder.but_data == 0) {
		i = ((receiveEpochSecondsRtcMoscow() - timeAlignment.epochSecFirstPoint) / 24 / 3600) % 10000;
		oled_print_XXnumber(i % 100, 82, 1);
		i /= 100;
		oled_print_XXnumber(i, 50, 1);
	}
	
	//set strokeDuration
	//stroke_com(ON);
	encoder.but_data = 0;
	oled_Clear_Screen();
	encoder_setter(1, 199, strokeDuration);
	while(encoder.but_data == 0) {
		strokeDuration = encoder.enc_data;
		oled_print_XXnumber(strokeDuration - 100, 50, 0);
	if((encoder.transient_counter >> 14) & 1) {
//	  if(i) stroke(strokeDuration);
		i=0;
  } else {
    i=1;
  }
	
	}
	//stroke_com(OFF);
	
	//set strokeStart-Stop
	encoder.but_data = 0;
	oled_Clear_Screen();
	oled_print_giga_char('-', 56);
	encoder_setter(0, 12, strokeStartHour);
	strokeStartHour = scan_value_at_pos(0);
	
	encoder.but_data = 0;
	encoder_setter(12, 23, strokeStopHour);
	strokeStopHour = scan_value_at_pos(80);
	
	//save strokeDuration
	if(strokeDuration != eeprom_data.strokeDuration || strokeStartHour != eeprom_data.strokeStartHour || strokeStopHour != eeprom_data.strokeStopHour) {
		print_save(); 
    saveStrokeDataToEeprom();
	}
	
	encoder.but_data = 0;
}

int scan_value_at_pos(char pos) {
	char blinking;
	while(encoder.but_data == 0) {
		blinking = (encoder.transient_counter >> 10) & 1;
		oled_print_XXnumber(encoder.enc_data, pos, blinking);
	}
	encoder.but_data = 0;
	return encoder.enc_data;
}

void saveTimeCorrectionToEeprom() { 
  char i;
	sim();
  if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
  }
  rim();
  while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) ;
  
  eeprom_data.timeCorrSec = timeAlignment.timeCorrSec;
  eeprom_data.timeCorrDecaMs = timeAlignment.timeCorrDecaMs;
  eeprom_data.positiveCorr = timeAlignment.positiveCorr;
  
  FLASH->IAPSR &= ~(FLASH_IAPSR_DUL);  // lock EEPROM
}  

void saveFirstPointTimeToEeprom() { 
  char i;
	sim();
  if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
  }
  rim();
  while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) ;
  
  for(i =0; i<4; i++) {
    eeprom_data.epochSecFirstPoint[i] =  (timeAlignment.epochSecFirstPoint >> (i * 8)) & 0xFF;
  }
    
  FLASH->IAPSR &= ~(FLASH_IAPSR_DUL);  // lock EEPROM
}  

void saveStrokeDataToEeprom() { 
  char i;
	sim();
  if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
  }
  rim();
  while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) ;
  
  eeprom_data.strokeDuration = strokeDuration;
  eeprom_data.strokeStartHour = strokeStartHour;
  eeprom_data.strokeStopHour = strokeStopHour;
    
  FLASH->IAPSR &= ~(FLASH_IAPSR_DUL);  // lock EEPROM
}  

void populate_timeAlignment_from_eeprom() { 
  char i;
  for(i = 0; i<4; i++) {
     timeAlignment.epochSecFirstPoint |=  ((unsigned long) eeprom_data.epochSecFirstPoint[i]) << (i * 8);
  }
  timeAlignment.timeCorrSec = eeprom_data.timeCorrSec;
  timeAlignment.timeCorrDecaMs = eeprom_data.timeCorrDecaMs;
  timeAlignment.positiveCorr = eeprom_data.positiveCorr;
} 



void init_tim2 (void) {//encoder_poller
  TIM2->IER|=TIM2_IER_UIE;
	//TIM2->IER|=TIM1_IER_CC1IE;
	//261 sec 50*65536=3 276 800 --> 12555 cycles / sec when 16 MHz -- CLK->CKDIVR = 0x00; //~ 16 MHz   << 0x11 - 2MHz  0x10 - 4MHz  0x01 - 8MHz  0x00 - 16MHz >>
  TIM2->PSCR=0;
	TIM2->ARRH=5; //2
  TIM2->ARRL=0;//20
	TIM2->CNTRH=0;
	TIM2->CNTRL=0;
	rim();
	TIM2->CR1|=TIM2_CR1_CEN;
  }