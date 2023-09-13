#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "inc/tm4c123gh6pm.h"

int N = 20;                         /* controls timer accuracy ('1000/N' ms) */
char blink = 0, glow;               /* global variables to blink LED */
char interstat = 0;                 /* GPIO interrupt will work if it is 0 */
int adc_val = 0;                    /* to store ADC input value */
int adc_max = 4095;                 /* maximum output of ADC */
int adc_count = 0;                  /* acceleration value from ADC */
int pwm_init = 320000;              /* to set PWM clock frequency */
int servo_count = 0;                /* counter for servo motor */
int motor_count = 0;                /* counter for DC motor */
char door = 0;                      /* variable to control door open or close */
char engine = 0;                    /* variable to control engine ON or OFF */
char safety_belt = 0;               /* variable to control safety belt ON or OFF */
int acceleration;                   /* acceleration (in %) */
char warnstat = 0;                  /* to represent a warning */
char warndisp = 0;                  /* to display warning */
char obstacle = 0;                  /* to detect obstacle */
float obj_distance = 0;             /* variable for distance of obstacle */
float obj_th = 2;                   /* threshold distance below which warning will arise */
char segment = 0;                   /* 7-segment number (left to right) */
char buzz = 0;                      /* to enable the buzzer */
char embrake = 0;                   /* to enable emergency break */
char reverse = 0;                   /* for reverse gear */
char human = 0;                     /* to detect living (human) obstacle */
int value[10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};    // 7-segment digits
int warncount = 0;                  /* counts down to 0 for temporary warnings, disabled when it is 0 */

#define min(X,Y) ((X) < (Y)) ? (X) : (Y)
#define max(X,Y) ((X) < (Y)) ? (Y) : (X)

// List of functions
void init_setup(void);
int process_UART(char* data, char idx);
void UART0_Int_Handler(void);
void SysTick_Handler(void);
void GPIOPortF_Handler(void);
void GPIOPortD_Handler(void);
void GPIOPortC_Handler(void);
void measure_dist(void);
void read_adc(void);
void lcd_cmd(unsigned char command);
void lcd_data(unsigned char disp_data);
void lcd_print(unsigned char a[16]);
void seg_print(char i);
void delayUs(int n);
void DisableInterrupts(void);
void EnableInterrupts(void);
void WaitForInterrupt(void);

//===============================================================================================

// Main Function
int main(void){
    init_setup();
    EnableInterrupts();                 /* Enable global Interrupt flag (I) */

    float servo_temp;
    char* warning[] = {"CAR DOOR OPEN!",
                       "SAFETY-BELT OFF!",
                       "OBSTACLE BEHIND!",
                       "EMERGENCY BRAKE!",
                       "HUMAN DETECTED!!",
                       "REVERSE GEAR ON!",
                       "RELEASE ACCLRTOR"};

    lcd_print("EN-  SB-  A-   %");
    while(1) {
        // read adc for acceleration if engine is ON
        read_adc();

        // evaluating PWM signals for servo and DC motors
        servo_temp = (19- 0.12*acceleration);
        servo_count = pwm_init*servo_temp/120;              /* servo motor duty cycle */
        motor_count = pwm_init*(100-acceleration)/100;      /* DC motor duty cycle */

        if(engine){
            PWM0_ENABLE_R = 0xC0;                           /* enable PWM0 ch6,7 */
            PWM0_3_CMPA_R = servo_count;                    /* for servo motor */
            PWM0_3_CMPB_R = motor_count;                    /* for DC motor */
        }
        else
            PWM0_ENABLE_R = 0x00;                           /* disable PWM */

        if(reverse){
            GPIO_PORTE_DATA_R |= 0X10;
            GPIO_PORTE_DATA_R &= ~(0X20);                   /* for DC motor reverse */
        }
        else{
            GPIO_PORTE_DATA_R &= ~(0X10);                   /* for DC motor forward */
            GPIO_PORTE_DATA_R |= 0X20;
        }

        // display in LCD
        lcd_cmd(0x83);
        lcd_data(engine+'0');
        lcd_cmd(0x88);
        lcd_data(safety_belt+'0');
        lcd_cmd(0x8C);
        lcd_data((acceleration/100)+'0');
        lcd_data(((acceleration/10)%10)+'0');
        lcd_data((acceleration%10)+'0');

        lcd_cmd(0xC0);
        if(warnstat != 0)
            if(warndisp)
                lcd_print(warning[warnstat-1]);         /* display warning in LCD */
            else
                for(int i=0; i<16; i++)
                    lcd_print(" ");
        else
            for(int i=0; i<16; i++)
                lcd_print(" ");

        // blinking of warning, LED & buzzer
        GPIO_PORTF_DATA_R &= ~(0x0E);                   /* LEDs OFF */
        GPIO_PORTE_DATA_R &= ~(0x02);                   /* Buzzer OFF */

        if(engine){
            if(glow){
                if(blink == 1){
                    GPIO_PORTF_DATA_R &= ~(0x0E);
                    GPIO_PORTF_DATA_R |= glow;              /* blink LED */
                    if(buzz) GPIO_PORTE_DATA_R |= 0x02;     /* blink buzzer */
                }
            }
            else GPIO_PORTF_DATA_R |= 8;    /* glow green LED indicating normal functioning */
        }
    }
}

//================================================================================================

void init_setup(){
    // basic GPIO setup
    SYSCTL_RCGC2_R |= 0x0000003F;           /* enable clock to GPIO at clock gating control register */
    GPIO_PORTA_DIR_R = 0xF0;                /* enable the GPIO pins of Port-A as output */
    GPIO_PORTA_DEN_R = 0xF3;                /* enable the GPIO pins for digital function */
    GPIO_PORTB_DIR_R = 0xFF;                /* enable the GPIO pins of Port-B as output */
    GPIO_PORTB_DEN_R = 0xFF;                /* enable the GPIO pins for digital function */
    GPIO_PORTC_DEN_R = 0x70;                /* enable the GPIO pins of Port-C for digital function */
    GPIO_PORTD_DIR_R = 0x43;                /* enable the GPIO pins of Port-D as output */
    GPIO_PORTD_DEN_R = 0x4F;                /* enable the GPIO pins for digital function */
    GPIO_PORTE_DIR_R = 0x37;                /* enable the GPIO pins of Port-E as output */
    GPIO_PORTE_DEN_R = 0x37;                /* enable the GPIO pins for digital function */
    GPIO_PORTE_ODR_R = 0X01;                /* make PE0 open drain for board switches */
    GPIO_PORTE_DATA_R = 0x04;               /* set pin 2 of Port-E as high */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;         /* unlock commit register */
    GPIO_PORTF_CR_R = 0x01;                 /* make PORTF0 configurable */
    GPIO_PORTF_DIR_R = 0x0E;                /* enable the GPIO pins for the LED (PF3,2,1) as output */
    GPIO_PORTF_DEN_R = 0x1F;                /* enable the GPIO pins for digital function */

    // GPIO interrupt setup
    GPIO_PORTF_PUR_R = 0x11;                /* enable pull up for PF4, PF0 */
    GPIO_PORTF_IS_R &= ~0x11;               /* PF4 & PF0 is edge-sensitive */
    GPIO_PORTF_IBE_R &= ~0x11;              /* PF4 & PF0 is not both edges */
    GPIO_PORTF_IEV_R &= ~0x11;              /* PF4 & PF0 falling edge event */
    GPIO_PORTF_ICR_R = 0x11;                /* Clear flag4 & flag0 */
    GPIO_PORTF_IM_R |= 0x11;                /* arm interrupt on PF4 & PF0 */
    GPIO_PORTC_PUR_R = 0x70;                /* enable pull up for PC4 & PC5 */
    GPIO_PORTC_IS_R &= ~0x70;               /* PC4 & PC5 is edge-sensitive */
    GPIO_PORTC_IBE_R &= ~0x70;              /* PC4 & PC5 is not both edges */
    GPIO_PORTC_IEV_R &= ~0x70;              /* PC4 & PC5 falling edge event */
    GPIO_PORTC_ICR_R = 0x70;                /* Clear flag4 & flag 5*/
    GPIO_PORTC_IM_R |= 0x70;                /* arm interrupt on PC4 & PC5 */
    GPIO_PORTD_PUR_R = 0x04;                /* enable pull up for PD2 */
    GPIO_PORTD_PDR_R = 0x08;                /* enable pull down for PD3 */
    GPIO_PORTD_IS_R &= ~0x0C;               /* PD2 & PD3 is edge-sensitive */
    GPIO_PORTD_IBE_R &= ~0x0C;              /* PD2 & PD3 is not both edges */
    GPIO_PORTD_IEV_R &= ~0x04;              /* PD2 falling edge event */
    GPIO_PORTD_IEV_R |= 0x08;               /* PD3 rising edge event */
    GPIO_PORTD_ICR_R = 0x0C;                /* Clear flag2 & flag3 */
    GPIO_PORTD_IM_R |= 0x0C;                /* arm interrupt on PD2 & PD3 */
    NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF1FFFFF) | 0x00A00000;      /* priority 5 */
    NVIC_PRI0_R = (NVIC_PRI0_R & 0x1F1FFFFF) | 0xA0A00000;      /* priority 5 */
    NVIC_EN0_R = 0x4000000C;            /* Enable interrupts 2, 30 in NVIC */

    // SysTick interrupt setup
    NVIC_ST_RELOAD_R = 16000000/N-1;        /* reload with number of clocks per 200 ms */
    NVIC_ST_CTRL_R = 7;                     /* enable SysTick interrupt, use system clock */
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x1FFFFFFF) | 0x60000000;    /*  priority 3 */

    // UART interrupt setup
    SYSCTL_RCGCUART_R = 1;                  /* provide clock to UART0 */
    delayUs(1000);
    UART0_CTL_R = 0;                        /* disable UART0 */
    UART0_IBRD_R = 104;                     /* integer portion of baud rate */
    UART0_FBRD_R = 11;                      /* fractional portion of baud rate */
    UART0_LCRH_R = 0x60;                    /* configure line control for UART0 */
    UART0_CTL_R = 0x0301;                   /* Enable UART0 along with Tx and Rx */
    GPIO_PORTA_AFSEL_R  |= 0x03;            /* Set the pins to alternate function */
    UART0_IM_R  = 0x10;                     /* Enable interrupt on rx */
    NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFF1FFF) | 0x0000A000;  /* Priority level 5 */
    NVIC_EN0_R  = NVIC_EN0_R | 0x00000020;

    // ADC setup
    SYSCTL_RCGCADC_R |= 1;                  /* enable clock to ADC0 */
    GPIO_PORTE_AFSEL_R |= 8;                /* enable alternate function */
    GPIO_PORTE_DEN_R &= ~8;                 /* disable digital function */
    GPIO_PORTE_AMSEL_R |= 8;                /* enable analog function */
    ADC0_ACTSS_R &= ~8;                     /* disable SS3 during configuration */
    ADC0_EMUX_R &= ~0xF000;                 /* software trigger conversion */
    ADC0_SSMUX3_R = 0;                      /* get input from channel 0 */
    ADC0_SSCTL3_R |= 6;                     /* take one sample at a time, set flag at 1st sample */
    ADC0_ACTSS_R |= 8;                      /* enable ADC0 sequencer 3 */

    // PWM setup
    SYSCTL_RCGCPWM_R |= 1;                  /* enable clock to PWM0 */
    SYSCTL_RCC_R &= ~0x00100000;            /* no pre-divide for PWM clock */
    GPIO_PORTD_AFSEL_R = 0x03;              /* PD0,1 uses alternate function */
    GPIO_PORTD_PCTL_R &= ~0x000000FF;       /* make PD0,1 PWM output pin */
    GPIO_PORTD_PCTL_R |= 0x00000044;        /* pin digital */
    PWM0_3_CTL_R = 0;                       /* stop counter */
    PWM0_3_GENA_R = 0x0000008C;             /* M0PWM6 output set when reload, clear when match PWMCMPA */
    PWM0_3_GENB_R = 0x0000008C;             /* M0PWM7 output set when reload, clear when match PWMCMPB */
    PWM0_3_LOAD_R = pwm_init;               /* set load value for 50Hz */
    PWM0_3_CTL_R = 1;                       /* start timer */

    // LCD setup
    lcd_cmd(0x38);                          /* for using 2 lines and 5X7 matrix of LCD */
    lcd_cmd(0x0C);                          /* turn display ON, cursor OFF */
    lcd_cmd(0x01);                          /* clear LCD screen */
    lcd_cmd(0x80);                          /* bring cursor to position 1 of line 1 */
}

// Function to process data received by UART
int process_uart(char* data, char idx){
    const char* cmd[] = {"door open",               /* UART commands */
                         "door close",
                         "engine on",
                         "engine off",
                         "safety belt on",
                         "safety belt off",
                         "emergency brake on",
                         "emergency brake off",
                         "reverse gear on",
                         "reverse gear off"};

    for(int i=0; i<(sizeof(cmd)/4); i++){           /* check for all commands */
        for(int j=0; j<idx; j++){                   /* check for all characters in a command */
            if(cmd[i][j] != data[j])                /* check if any character is mismatched */
                break;
            else if(j == idx-1){                    /* if a complete string is matched */
                switch(i){
                case 0: door = 1; break;            // car door open
                case 1: door = 0; break;            // car door closed
                case 2: engine = 1; break;          // engine ON
                case 3: engine = 0; break;          // engine OFF
                case 4: safety_belt = 1; break;     // safety belt ON
                case 5: safety_belt = 0; break;     // safety belt OFF
                case 6: embrake = 1; break;         // emergency break ON
                case 7: embrake = 0; break;         // emergency break OFF
                case 8: reverse = 1; break;         // reverse gear ON
                case 9: reverse = 0; break;         // reverse gear OFF
                default: break;
                }
            }
        }
    }
    return 0;
}

// UART interrupt handler
void UART0_Int_Handler(void){
    static char uart_data[20];                      /* String received from UART */
    static short uart_idx = 0;                      /* index for UART data */

    if((uart_idx < 20) && (UART0_FR_R & 0x40)) {
        uart_data[uart_idx] = UART0_DR_R;           /* receive the data */

        // Wait for previous write to complete
        while(UART0_FR_R & 0x20);

        // process data and execute operation
        if((uart_data[uart_idx]) == '\r'){
            process_uart(uart_data, uart_idx);      /* process UART data */

            // reset the window
            while(uart_idx){
                UART0_DR_R = '\b';                  // backspace
                while(UART0_FR_R & 0x20);
                UART0_DR_R = ' ';                   // space
                while(UART0_FR_R & 0x20);
                UART0_DR_R = '\b';                  // backspace
                while(UART0_FR_R & 0x20);
                uart_idx--;                         // decrease index
            }
        }
        else if((uart_data[uart_idx] == '\b') && uart_idx){
            UART0_DR_R = '\b';                      // backspace
            while(UART0_FR_R & 0x20);
            UART0_DR_R = ' ';                       // space
            while(UART0_FR_R & 0x20);
            UART0_DR_R = '\b';                      // backspace
            while(UART0_FR_R & 0x20);
            uart_idx--;                             // decrease index
        }
        else{
            UART0_DR_R = uart_data[uart_idx];       /* echo back the data received */
            uart_idx++;             // increase index
        }
    }
}

// SysTick interrupt handler
void SysTick_Handler(void){
    static int i = 0;
    static int j = 0;
    int n = N/2;                                    /* default blink period of 1 second */
    buzz = 0;                                       /* buzzer off by default */

    if(interstat != 0)
        interstat--;                                /* SysTick counting down status to reset it */

    if(warncount != 0)
        warncount--;

    if(human){
        warnstat = 5; glow = 2;                     /* if human is detected */
        if(adc_count) buzz = 1;
        n = N/10;
    }
    else if(obstacle){
        warnstat = 3; glow = 2;                     /* if any obstacle is detected */
        if(obj_distance < 1){
            if(acceleration) buzz = 1;              /* buzzer ON */
            n = n*obj_distance;                     /* blinking proportional to object distance */
        }
    }
    else if(warncount) {                            /* reverse gear or engine start pressing accelerator */
        warnstat = 7;
    }
    else if(embrake){
        warnstat = 4; glow = 0;                     /* if emergency break is applied */
    }
    else if((acceleration > 0)&&(safety_belt == 0)){
        warnstat = 2; glow = 4; buzz = 1;           /* if car accelerate without safety belt ON */
    }
    else if(reverse){
        warnstat = 6; glow = 10; buzz = 1;          /* if reverse gear is applied */
    }
    else if((engine == 1)&&(door == 1)){
        warnstat = 1; glow = 0;                     /* if door is open */
    }
    else{
        warnstat = 0; glow = 0;                     /* default case */
    }

    if(i == 0){
        if(n) blink = 1-blink;                      /* to blink LED */
        else blink = 1;                             /* always buzz */
    }

    if(j == 0){
        if(warncount) warndisp = 1;
        else warndisp = 1-warndisp;                 /* to blink warning on LCD */

        /* send a pulse to trigger of HC-SR04 */
        GPIO_PORTD_DATA_R |= 0x40;
        delayUs(15);
        GPIO_PORTD_DATA_R &= ~(0x40);
    }

    i = (i+1)%(n);
    j = (j+1)%(N/2);
}

// GPIO Port-F interrupt handler
void GPIOPortF_Handler(void){
    GPIO_PORTF_ICR_R = 0x11;                        /* clear PF4 & PF0 int */
    char sw1 = GPIO_PORTF_DATA_R & 0x10;            /* sw1 corresponds to Bit 4 */
    char sw2 = GPIO_PORTF_DATA_R & 0x01;            /* sw2 corresponds to Bit 0 */

    if(interstat == 0){
        if(!sw1) door = 1-door;                     /* '1' - open, '0' - close */
        if(!sw2) {
            if((!adc_count)||(engine))
                engine = 1-engine;                  /* '1' - ON, '0' - OFF */
            else
                warncount = 2 * N;
        }
        interstat = N/2;                            /* 500 ms delay to prevent debouncing error */
    }
}

// GPIO Port-D interrupt handler
void GPIOPortD_Handler(void){
    GPIO_PORTD_ICR_R = 0x0C;                        /* clear PD2,3 int */
    char sw1 = GPIO_PORTD_DATA_R & 0x08;            /* sw1 corresponds to Bit 3 */
    char sw2 = GPIO_PORTD_DATA_R & 0x04;            /* sw2 corresponds to Bit 2 */

    if(engine){
        if(sw1) {
            human = 1;
            if(adc_count && (!reverse))
                embrake = 1;                        /* emergency break applied */
        }
        else
            human = 0;

        if(!sw2)  measure_dist();                   /* if any obstacle is detected */
    }
    else{
        human = 0; obstacle = 0;                    /* sensors won't work if engine is OFF */
    }
}

// GPIO Port-C interrupt handler
void GPIOPortC_Handler(void){
    GPIO_PORTC_ICR_R = 0x70;                        /* clear PC4-7 int */
    char sw1 = GPIO_PORTC_DATA_R & 0x10;            /* sw1 corresponds to Bit 4 */
    char sw2 = GPIO_PORTC_DATA_R & 0x20;            /* sw2 corresponds to Bit 5 */
    char sw3 = GPIO_PORTC_DATA_R & 0x40;            /* sw3 corresponds to Bit 6 */

    if(interstat == 0){
        if(!sw1) safety_belt = 1-safety_belt;       /* '1' - ON, '0' - OFF */
        if(!sw2) embrake = 1-embrake;               /* '1' - ON, '0' - OFF */
        if(!sw3) {
            if(!acceleration)                       /* can't be applied when car is accelerating */
                reverse = 1-reverse;
            else
                warncount = 2 * N;
        }
        interstat = N;                              /* 1 s delay to prevent debouncing error */
    }
}

// Measure distance of the obstacle from the car
void measure_dist(){
    unsigned long count = NVIC_ST_RELOAD_R - NVIC_ST_CURRENT_R;
    unsigned long ttime = (count/16)-2200;          /* time in microseconds */
    obj_distance = 0.17*ttime/1000;                 /* object distance in metre */

    if(obj_distance < obj_th)
        obstacle = 1;                               /* obstacle detected */
    else
        obstacle = 0;
}

// ADC functionality
void read_adc(void){
    int prev_acc = acceleration;

    ADC0_PSSI_R |= 8;                               /* start a conversion sequence 3 */
    while((ADC0_RIS_R & 8) == 0){}                  /* wait for conversion complete */
    adc_val = ADC0_SSFIFO3_R;                       /* read conversion result */
    ADC0_ISC_R = 8;                                 /* clear completion flag */

    adc_count = 100*adc_val/adc_max;                /* calculate new value */
    if((adc_count)&&(abs(adc_count - prev_acc) <= 1))
        adc_count = prev_acc;                       /* to stabilize the output */

    if(embrake||(!engine))
        acceleration = 0;                           /* stop if emergency break or reverse gear applied */
    else if(reverse)
        acceleration = min(25,adc_count);           /* restrict to 25% in reverse gear */
    else
        acceleration = adc_count;                   /* accelerate if no restrictions applied */
}

// Function to send command instruction to LCD
void lcd_cmd(unsigned char command){
    GPIO_PORTB_DATA_R = command;                    /* command to be written */
    GPIO_PORTA_DATA_R = 0x80;
    delayUs(150);
    GPIO_PORTA_DATA_R = 0x00;
    delayUs(150);
    if(obstacle)
        seg_print((segment++)%4);                   /* print obstaclce distance */
}

// Function to send display data to LCD
void lcd_data(unsigned char disp_data){
    GPIO_PORTB_DATA_R = disp_data;                  /* data to be written */
    GPIO_PORTA_DATA_R = 0xC0;
    delayUs(150);
    GPIO_PORTA_DATA_R = 0X40;
    delayUs(150);
    if(obstacle)
        seg_print((segment++)%4);                   /* print obstacle distance */
}

// Function to print a string in LCD
void lcd_print(unsigned char a[16]){
    int i=0;
    while(a[i]!='\0'){
        lcd_data(a[i]);                             /* print each character */
        ++i;
        if(i == 16)
            break;
    }
}

// Display distance in 7-segments
void seg_print(char n){
    float dist = obj_distance;
    unsigned char dist_int = dist, exponent, k = 0;

    while(1){
        if(dist<10)                                 /* check if distance < 10 m */
            break;
        dist = dist/10;
        exponent++;                                 /* find 10's exponent of the distance */
    }
    for(int i=0;i<n;i++){
        dist = 10*(dist-dist_int);
        dist_int = dist;                            /* find the digit to be displayed */
    }
    if(n == exponent)
        k = 0x80;                                   /* for decimal point */

    GPIO_PORTE_DATA_R &= ~(0x04);                   /* enable 7-segments */
    GPIO_PORTB_DATA_R = value[dist_int] + k;        /* data to display digit */
    GPIO_PORTA_DATA_R = (0x10 << (3-n));            /* select an unit */
    delayUs(300);
    GPIO_PORTE_DATA_R |= 0x04;                      /* disable 7-segments */
}

// delay n microseconds (16 MHz CPU clock)
void delayUs(int n) {
    for(int i = 0 ; i < 3*n; i++){}
}

// Disables the interrupts
void DisableInterrupts(void){
    __asm ("    CPSID  I\n");
}
// Enables the interrupts
void EnableInterrupts(void){
    __asm  ("    CPSIE  I\n");
}
// Waits for the next interrupt
void WaitForInterrupt(void){
        __asm  ("    WFI\n");
}
