# GPIO Pin usage

| Pin			| Configuration		| Usage 
|---			|---				|---	
| PA1,0		| UART 			| UART Rx, Tx 
| PA7:4		| Output			| LCD, 7 segment address output 
| PB7:0		| Output			| LCD, 7 segment data output 
| PC6:4		| Input			| Keypad buttons 
| PD1,0		| PWM				| Motor speed control, servo 
| PD2			| Input			| Ultrasound sensor echo 
| PD3			| Input			| PIR input signal 
| PD6			| Output			| Ultrasound sensor trigger 
| PE0   		| Open drain out 		| Keypad row select 
| PE1			| Output			| Buzzer 
| PE2			| Output			| 7 segment display enable 
| PE3			| ADC				| Potentiometer for accelerometer 
| PE5,4		| Output			| Motor direction control 
| PF3:1		| Output			| RGB LEDs 
| PF4,0		| Input			| sw1 (door) and sw2 (engine) switches 
| PF5			| Input			| Switch (engine ON) 

---

# Interrupt handlers

There are 5 interrupts. The priorities are as follows (lower number has stronger priority):

|Name 		| Priority		|
|---		|---			|
| SysTick	| 3				|
| GPIOC		| 5				|
| GPIOD		| 5				|
| GPIOF		| 5				|
| UART0		| 5				|

### SysTick_Handler

- Decrements the status counts (```inter_stat``` and ```warncount```) to 0
- If ```warncount``` is 0, the variables indicating warning states for temporary warnings (```reverse_warn```, ```engine_acc_warn```) will be cleared
- Checks the safety conditions and sets the ```warnstat```, ```glow```, ```buzz``` and ```n``` variables accordingly	

| Condition						| warnstat	| glow	| buzz		| n (LED half-period) 	
|---							|---		|---		|---			|---					
| Human present					| 5		| 2		| 1			| N / 10 |
| Obstacle present				| 3		| 2		| if dist < 1	| n * dist if dist < 1 |
| Emergency brake					| 4	 	| 0		| no change		| N / 2 |
| Reverse with accelerator			| 7	 	| 8		| 1			| N / 2 |
| No seat belt with acceleration| 2		| 4		| 1					| N / 2 |
| Reverse						| 6		| 10		| no change		| N / 2 |
| Engine on with door open			| 1		| 0		| no change		| N / 2 |
| Engine on with accelerator			| 8		| 8		| 1			| N / 2 |
| Default						| 0		| 0		| no change		| N / 2 |

- Flips the ```blink``` variable every ```n``` ticks
- Flips the ```warndisp``` variable and sends a pulse to the ultrasound sensor every ```N/2``` ticks

### GPIOPortF_Handler

- Checks PF4 (sw1) and PF0 (sw2) inputs. 
- If sw1 goes low, the ```door``` variable is flipped and if sw2 goes low, the ```engine``` variable is flipped if accelerator is off.
- If accelerator is nonzero, set ```warncount``` to 3N (3 second warning) and ```reverse_warn``` to 1
- The ```interstat``` variable is used to implement debouncing.
  - It is set to N/2 when either switch goes low. 
  - It counts down to 0 in the systick handler. 
  - Unless ```interstat``` is 0, the check for sw1 and sw2 is not performed, for debouncing the switch inputs.

### GPIOPortD_Handler

- Checks PD3 for PIR input and PD6 for ultrasound echo.
- If the engine is ON, the checks are performed. Else, ```human``` and ```distance``` are 0.
- If the PIR input is high, set the ```human``` variable to high. If the vehicle has non-zero acceleration and is not in reverse mode, ```embrake``` is high (emergency stop)
- If PD6 is low, call the ```measure_distance()``` function

### GPIOPortC_Handler

- Checks PD4 (safety belt), PD5 (emergency brake), PD6 (reverse)
- ```interstat``` variable is set to N for debouncing. Following checks are done only when it is 0.
- If PD4 is low, flip the ```safety belt``` variable
- If PD5 is low, flip the ```embrake``` variable
- If PD6 is low and there is no acceleration applied, flip the ```reverse``` variable
- If PD6 is low and there is acceleratoion, set ```warncount``` to 3N (3 second warning) and ```reverse_warn``` to 1

### UART0_Int_Handler
	
- Handles incoming data on UART0. 
- Manages a 20 character buffer with backspace support and loopback.
- On receiving a next line input, the buffer is passed to the ```process_uart(uart_data, uart_idx)``` function to process the data received
- It then calls the control_sw function to execute the desired operation.

---

# Main program

### One-time initialization
- Initialize using ```init_all()``` function
- Enable interrupts using ```EnableInterrupts()``` function
- Print ```EN-  SB-  A-   %``` on the first line of the LCD

### Infinite loop
- Read ADC value by calling ```read_adc()```
- Find and send the servo and motor duty cycles
- Reverse the motor if required

- Display ```engine```, ```safety_belt``` and ```acceleration``` variables
- Check for warnings and display on the 2nd line of the LCD
  - String to be displayed is indexed by ```warnstat``` 
  - ```warndisp``` is used to blink the warning string. It flips every N / 2 number of ticks (half a second) in the systick handler

| warnstat 	| Message 	|
|---|---			|
| 1 | CAR DOOR OPEN!	|
| 2 | SAFETY-BELT OFF!	|
| 3 | OBSTACLE BEHIND!	|
| 4 | EMERGENCY BRAKE!	|
| 5 | HUMAN DETECTED!!	|
| 6 | REVERSE GEAR ON!	|
| 7 | STOP TO REVERSE!	|
| 8 | RELEASE ACCEL!	|

- Glow the RGB LED depending on the ```glow``` signal value and blink it and the buzzer depending on the ```blink``` signal value (and ```buzz``` for buzzer)

---

# Helper functions

### init_setup()

- Initializes GPIO ports, clock gating, peripherals and iterrupts

### process_uart(char* data, char idx)

- Used to process inputs from UART
- Detects one among the following commands from the ```data``` string with ```idx``` length and modify global variables depending on the detected command.

| idx | Command 				| Action |
|---|---					|---|
| 0 | door open				|door = 1
| 1 | door close				|door = 0
| 2 | engine on				|engine = 1
| 3 | engine off				|engine = 0
| 4 | safety belt on			|safety_belt = 1
| 5 | safety belt off			|safety_belt = 0
| 6 | emergency brake on		|embrake = 1
| 7 | emergency brake off		|embrake = 0
| 8 | reverse gear on			|reverse = 1
| 9 | reverse gear off			|reverse = 0


### measure_dist()

- Calculates object distance in meters from the current systick value from the moment it was reset
- Stores the distance calculated in the ```obj_distance``` variable. 
- If object distance is less than 1, ```obstacle``` variable is set. Else it is cleared.

### read_adc()

- Reads the ADC value into ```adc_val``` variable
- Scales it to 0-100 range and stores in the ```adc_count``` variable
- ```adc_count``` is only updated if the change is > 1% to stabilize the value
- Sets the ```acceleration``` variable according to the following rules
  - If the emergency brake is applied (```embrake```), it is 0
  - If in reverse gear (```reverse```), use ```adc_count``` saturated at 25%
  - Else use current ```adc_count```

### lcd_cmd(unsigned char command)

- Sends a command to the LCD
- ```0x80``` in ```PORTA```
- If ```obstacle``` variable is high, the distance is displayed on the 7-segment display using ```seg_print```

### lcd_data(unsigned char data)

- Sends data to be displayed to the LCD
- ```0xC0``` in ```PORTA```
- If ```obstacle``` variable is high, the distance is displayed on the 7-segment display using ```seg_print```

### lcd_print(unsigned char a[16])

- Sends all characters in the string a (till encountering null character) using ```lcd_data```

### seg_print(char n)

- Displays the nth digit of the distance on the 7-segment display

### delayUs(int n)

- Delays n number of microseconds by looping in a for loop for ```3 x n``` times

### DisableInterrupts()

- Disables all interrupts

### EnableInterrupts()

- Enables interrupts

### WaitForInterrupt()

- Waits till some interrupt is triggered

-------------------------------------------

# Interlocks:

-> Car cannot move without engine being ON.
-> Car cannot move with emergency gear being ON.
-> Engine cannot be started pressing accelerator.
-> Gear cannot be reversed while car is moving
-> Sensors won't work without engine being ON.
-> Warnings won't buzz (except reverse gear) if car isn't moving.