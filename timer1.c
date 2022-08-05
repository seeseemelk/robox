#include "timer1.h"
#include "button.h"

#include <avr/interrupt.h>

// timer 1 overflow ISR
#define ENABLE_TIMER1_OV  SET_BIT(TIMSK, TOIE1)
#define DISABLE_TIMER1_OV CLEAR_BIT(TIMSK, TOIE1)
#define ONGOING_TIMER1_OV TEST_BIT_SET(TIMSK, TOIE1)

// timer 1 A
#define ENABLE_TIMER1_A  SET_BIT(TIMSK, OCIE1A)
#define DISABLE_TIMER1_A CLEAR_BIT(TIMSK, OCIE1A)
#define ONGOING_TIMER1_A TEST_BIT_SET(TIMSK, OCIE1A)

// timer 1 B
#define ENABLE_TIMER1_B  SET_BIT(TIMSK, OCIE1B)
#define DISABLE_TIMER1_B CLEAR_BIT(TIMSK, OCIE1B)
#define ONGOING_TIMER1_B TEST_BIT_SET(TIMSK, OCIE1B)

// timer 1 D
// #define ENABLE_TIMER1_D  SET_BIT(TIMSK, OCIE1D)
// #define DISABLE_TIMER1_D CLEAR_BIT(TIMSK, OCIE1D)
// #define ONGOING_TIMER1_D TEST_BIT_SET(TIMSK, OCIE1D)


// #define PURPOSE_TIMER_MENU 0
// #define PURPOSE_TIMER_DELAY_25MS 1
// #define PURPOSE_TIMER_NIGHTLIGHT 2
// #define PURPOSE_TIMER_BEAT 3

// static u8 timer_purpose = PURPOSE_TIMER_MENU;
volatile u16 counter_25ms = 0;
static volatile bool test_timer_1 = false;

// bool timer1_in_use()
// {
//     bool rtn = false;
//     if (ONGOING_TIMER1_OV || ONGOING_TIMER1_A || ONGOING_TIMER1_B || ONGOING_TIMER1_D)
//         rtn = true;

//     return rtn;
// }

void setup_button_menu()
{
    // cli();
    // disable_timer1();
    // SET_BIT(DDRB, PB0);
    
    OCR1C = 116;    // Timer 1 top value 62.5Hz
    ENABLE_TIMER1_OV;
    TCCR1B = MASK(CS13) | MASK(CS12) | MASK(CS10);    // Timer 1 clock prescaler

    // sei();
}

void setup_beat_detection_counter()
{
    // cli();
    // disable_timer1();
    
    // Configure the output compare register
    OCR1A = 80;
    ENABLE_TIMER1_A;
    TCCR1B = MASK(CS13) | MASK(CS12) | MASK(CS11) | MASK(CS10);

    // sei();
}

void setup_25ms_interrupt()
{
    // cli();
    // disable_timer1();
    
    OCR1B = 98;    // Timer 1 top value (25ms)
    counter_25ms = 0;
    ENABLE_TIMER1_B;
    TCCR1B = MASK(CS13) | MASK(CS12); // | MASK(CS10);    // Timer 1 clock prescaler

    // sei();
}

void disable_timer1()
{
    TCCR1B = 0;
    OCR1C = 0xFF;
    TCNT1 = 0;
    DISABLE_TIMER1_OV;
    DISABLE_TIMER1_A;
    DISABLE_TIMER1_B;
    // DISABLE_TIMER1_D;
}

ISR(TIMER1_COMPB_vect)
{
	TCNT1 = 0;
    counter_25ms++;

    if (global_modus == mapper_night_light)
        night_light_counter++;

    // test_timer_1 = !test_timer_1;
    // if (test_timer_1)
    //     SET_BIT(PORTB, PB0);
    // else
    //     CLEAR_BIT(PORTB, PB0);
}