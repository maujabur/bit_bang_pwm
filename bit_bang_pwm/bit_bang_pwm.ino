/////////////////////////////////////////////////////////
//
// bit bang pwm
//
// using interrupt on TIMER2
//
// Mau at MauMaker dot com
//
// attach leds to pins 2-11, 
// in series with 1K ohm resistors to ground
//
/////////////////////////////////////////////////////////

#define MAX_BRIGHT 10 // 1-255
#define QT_PINS 10

volatile byte leds[QT_PINS] = {};
// led intensities 0-MAX_BRIGHT

int bright = 0;


void setup()
{
  // pins are hardcoded on the interrupt routine
  pinMode (2, OUTPUT);  // led 0
  pinMode (3, OUTPUT);  // led 1
  pinMode (4, OUTPUT);  // led 2
  pinMode (5, OUTPUT);  // led 3
  pinMode (6, OUTPUT);  // led 4
  pinMode (7, OUTPUT);  // led 5
  pinMode (8, OUTPUT);  // led 6
  pinMode (9, OUTPUT);  // led 7
  pinMode (10, OUTPUT); // led 8
  pinMode (11, OUTPUT); // led 9

  init_timer2();
}

void loop()
{
  for (int which_led = 0; which_led < QT_PINS; which_led = which_led + 1)
  {

    leds[which_led] = bright; // sets the brightness of the led

    bright = bright + 1;
    if (bright > MAX_BRIGHT) bright = 0;
  }
  delay(100);

}

void init_timer2()
{
  noInterrupts();

  // TIMER2 configuration
  TCCR2A = 0; // configure timer2 for normal operation
  TCCR2B = 0; // clear register

  TCCR2B |= (1 << WGM22) | (0 << CS22) | (1 << CS21) | (1 << CS20);
  // CTC mode, prescaler 32: 16MHz/32 = 500KHz (updates counter 500.000 times a second)

  OCR2A = 35; // when the counter reaches this number, call ISR function
  // bigger = slower PWM

  TIMSK2 |= (1 << OCIE2A); // enables compare counter interrupt

  interrupts();
}

/////////////////////////////////////////////////////////
// bit bang PWM
byte compare_to = 0;

ISR(TIMER2_COMPA_vect) {
  TCNT2  = 0; // reset timer counter

  uint16_t to_out;
  for (int i = 0; i < QT_PINS; i++) {
    if (leds[i] > compare_to)
    {
      bitSet (to_out, i);
    }
    else
    {
      bitClear (to_out, i);
    }
  }

  // outputs
  PORTD = (PORTD & 0b00000011 ) | (0b11111100 & (to_out << 2));
  PORTB = (PORTB & 0b11110000 ) | (0b00001111 & (to_out >> 6));

  compare_to++;
  if (compare_to > MAX_BRIGHT) compare_to = 0;  
}
