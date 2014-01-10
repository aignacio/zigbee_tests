#include <bsp.h>
#include <stdbool.h>

#define ECHO            BIT3                    // PORT 4 - P8           
#define TRIGGER         BIT4                    // PORT4  - P9     
#define VCC_ULTRA       BIT4                    //P7
#define NUM_CAP 1


extern void start_usens(void);
extern int getEcho(void);
extern void trigger(void);
extern __interrupt void Timer_B(void);


