#include "stm8s.h"

typedef enum {time} menu_t;
typedef enum {set_time, set_date, set_offset, info, set_bell} first_selector_t;

typedef struct {
	      char epochSecFirstPoint[4];
        char timeCorrSec;
        char timeCorrDecaMs;
        char positiveCorr;
				char strokeDuration;
				char strokeStartHour;
				char strokeStopHour;
} eeprom_data_t;

void init_tim2 (void);

void menu_partition(menu_t menu);
void populate_timeAlignment_from_eeprom(void);
void saveTimeCorrectionToEeprom(void);
void saveFirstPointTimeToEeprom(void); 
void saveStrokeDataToEeprom(void); 
char menu_selector(void);
int scan_value_at_pos(char pos);

