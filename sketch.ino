//**************************************************************************************************//
//  CI116 Assembly language assignment - Alan Thomas 2014                                           //
//  Reference - Arduino assembly language instruction set - http://www.atmel.com/images/doc0856.pdf //
//**************************************************************************************************//

// shared global variables, visible to both C and assembly
word millisecs;      // C variable read by assembly code contains blink delay 
//
// setup function sets portB (digital pins 8-13) as outputs, and initialises serial port
//
void setup() 
{
  // set portB (digital pins 8-13) as outputs
  // achieved by writing to port B Data Direction Register (DDRB) at I/O address 4
  // if a bit is set, the corresponding pin is an output, otherwise it's an input
  // Bit designations for Data Direction Register B (DDRB) and Port B (PORTB)
  // bit5 = Pin13
  // bit4 = Pin12
  // bit3 = Pin11
  // bit2 = Pin10
  // bit1 = Pin9
  // bit0 = Pin8
  asm volatile(
    "          ldi r16,0x3F    ; r16 = 00111111\n"
    "          out 4,r16       ; set pins 8-13 as outputs in DDRB\n"
    ::: "r16");
  millisecs = 1000;            // 1s blink delay
  Serial.begin(9600);
}
//
// assembly language implementation of blink
//
void loop() 
{
  long starttime = millis();        // make a note of the start time

    asm volatile(
    // jump to "blink" - ie jump around the delay_ms subroutine
    "            rjmp  blink%=        ; relative jump to 'blink' \n"

    //-----------------------------------------------------------------------
    // Task 1 - complete delay_ms subroutine below until displayed error < 1%
    //-----------------------------------------------------------------------
    
    /* Input variable - millisecond count in register pair r30:r31
  
    registers used:
     r31 - millisecond count (lo byte)
     r30 - millisecond count (hi byte)
     r17 - 100 microsecond count
     r16 - 1 microsecond count
    
     Overall delay (ms) = r30:r31 * r17 * r16
    ---------------------------------------------------------------------*/
    "delay_ms%=:    r17=10                 ; replace nop with your code  \n"
    "delay_100us%=: r17=10                 ; replace nop with your code  \n"
    "delay_1us%=:   r16=100                 ; replace nop with your code  \n"
    "               sbiw r30,1          ; decrement ms count (r31:r30)\n"
    "               brne delay_ms%=     ; loop to delay_ms while > 0  \n"
    "               ret                 ; return from subroutine      \n"

    //-----------------------------------------------------------------------
    // Task 2 - modify blink code below to implement four phase traffic light
    //-----------------------------------------------------------------------
  
    " blink%=:                               ; start of blink code    \n"
    //
    // turn onboard LED on
    //   
    "               ldi  r18,0x20            ; bit 5 (pin 13) = high  \n"
    "               out  5,r18               ; output to port B       \n"
    //
    // delay by value in millisecs variable
    //
    "               lds  r30,millisecs      ; r30 = hi byte           \n"
    "               lds  r31,millisecs + 1  ; r31 = lo byte           \n"
    "               call delay_ms%=         ; call millisec delay sub \n"
    //
    // turn onboard LED off
    // 
    "               ldi  r18,0x00           ; value for all LEDs off  \n"
    "               out  5,r18              ; output to port B        \n"
    //
    // delay by value in millisecs variable
    //
    "               lds  r30,millisecs      ; r30 = hi byte           \n"
    "               lds  r31,millisecs + 1  ; r31 = lo byte           \n"
    "               call delay_ms%=         ; call millisec delay sub \n"

    ::: "r16", "r17", "r18", "r30", "r31");    // clobbered registers
  
  //--------------------------------------------------------------------------------
  // calculate the execution time of the blink routine, and print details 
  long endtime = millis();                  // make a note of the end time
  float ms = endtime-starttime;             // calculate the interval
  float expected = 2 * millisecs;           // expected delay is millisecs * 2 (2 delays in blink)
  float overheads = 17;                     // overheads due to the timing
  expected = expected + overheads;   
  float error_percent = 100.0*(ms-expected)/expected;
  Serial.print("delay="); Serial.print(ms); Serial.print("ms  "); 
  Serial.print("error: ");
  if(error_percent>0)
    Serial.print("+");
  Serial.print(error_percent);Serial.println("%");
}
