#define INDICATOR_HORIZONTAL_SIZE      16

#define PORT_HORIZONTAL                GPIOC

#define PIN_H1       1
#define BIT_H1                      (1<<PIN_H1 )
#define PIN_H2       2
#define BIT_H2                      (1<<PIN_H2 )
#define PIN_H3       3
#define BIT_H3                      (1<<PIN_H3 )
#define PIN_H4       4
#define BIT_H4                      (1<<PIN_H4 )
#define PIN_H5       5
#define BIT_H5                      (1<<PIN_H5 )
#define PIN_H6       6
#define BIT_H6                      (1<<PIN_H6 )
#define PIN_H7       7
#define BIT_H7                      (1<<PIN_H7 )


#define PORT_VERTICAL_12                GPIOA

#define PIN_V1       1
#define BIT_V1                      (1<<PIN_V1 )
#define PIN_V2       2
#define BIT_V2                      (1<<PIN_V2 )

#define PORT_VERTICAL_345678            GPIOD

#define PIN_V3       7
#define BIT_V3                      (1<<PIN_V3 )
#define PIN_V4       6
#define BIT_V4                      (1<<PIN_V4 )
#define PIN_V5       5
#define BIT_V5                      (1<<PIN_V5 )
#define PIN_V6       4
#define BIT_V6                      (1<<PIN_V6 )
#define PIN_V7       3
#define BIT_V7                      (1<<PIN_V7 )
#define PIN_V8       2
#define BIT_V8                      (1<<PIN_V8 )

#define VERTICAL_MASK_12            (BIT_V1 | BIT_V2) 
#define VERTICAL_MASK_345678        (BIT_V3 | BIT_V4 | BIT_V5 | BIT_V6 | BIT_V7 | BIT_V8)

void printDigit(char digit, char position, char* dotIndicatorBuffer);
void printTime(char hours, char minutes, char* dotIndicatorBuffer);
void initDotLeds(void);
void refreshIndicator(char updCol, char* dotIndicatorBuffer);