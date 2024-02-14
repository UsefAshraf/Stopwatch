#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
unsigned char tick=0;
unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;
void TIMER1_Init(void )
{
	//FOC1A = 1 when non PWM is chosen
	//WGM12 = 1 to operate at compare mode
	//CS10,11 set prescaler to 64
	/* Configure the timer mode
	 * 1. Non PWM mode FOC1A = 1 & FOC1B = 1
	 * 2. CTC Mode WGM10 = 0 & WGM11 = 0 & WGM12 = 1 & WGM13 = 0
	 * 3. clock = CPU clock/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 */
	TCCR1A = (1<<FOC1A);
	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS11);
	TCNT1 = 0;
	OCR1A = 15625;
		/* Configure timer interrupt
	 * 1. Enable Timer1 Compare match interrupt
	 * 2. Enable Global Interrupt
	 */
	SREG|=(1<<7); //enable interrupt when compare

	TIMSK|=(1<<OCIE1A); // Timer1 Compare match interrupt
}
ISR(TIMER1_COMPA_vect)
{
	second ++;
	if (second == 60){
		second = 0;
		minute ++;
	}
	if (minute == 60){
		second = 0;
		minute = 0;
		hour ++;
	}
	if (hour == 12){
		second = 0;
		minute = 0;
		hour = 0 ;
	}

}
void INT0_Init_Reset(void)
{
	DDRD &=~(1<<PD2);
	PORTD|=(1<<PD2);
    // internal pull up

	/* Configure interrupt 0
	 * 1. Enable interrupt 0 from GICR
	 * 2. Enable Falling edge
	 * 3. Enable Global Interrupt
	 */
	MCUCR|=(1<<ISC01);	// Falling edge
	GICR |=(1<<INT0);
	SREG |=(1<<7);
}
ISR(INT0_vect)
{
	second = 0;
	minute = 0;
	hour = 0 ;
}
void INT1_Init_Pause(void)
{
	DDRD &= ~(1<<PD3);
	/* Configure interrupt 1
	 * 1. Enable interrupt 1 from GICR
	 * 2. Enable Global Interrupt
	 */
	MCUCR|=(1<<ISC10)|(1<<ISC11);	// Raising edge
	GICR |=(1<<INT1);   //interrupt 1 enable
	SREG|=(1<<7);
}
ISR(INT1_vect)
{
	/* this ISR is to stop the clock by switch timer1 ctc mode interrupt OFF */
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void INT2_Init_Resume(void)
{
	DDRB &=~(1<<PB2);
	PORTB |= (1<<PB2);   // internal pull up

	/* Configure interrupt 2
	 * 1. Enable interrupt 2 from GICR
	 * 2. Enable Falling edge mode >> ISC2 = 0
	 * 3. Enable Global Interrupt*/
	MCUCR&=~(1<<ISC2);	// falling edge
	GICR |=(1<<INT2);
	SREG|=(1<<7);
}
ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS11);
}
int main()
{
	DDRA = 0xff;	//configure the 7seg data pins to input
	PORTA = 0xff;
	DDRC = 0x0f;	//configure the 7seg data pins to output
	PORTC = 0x00;  // at beginning all LEDS off


	INT0_Init_Reset();
	INT1_Init_Pause();
	INT2_Init_Resume();
	TIMER1_Init( );
	while(1)
	{
		//s = 13
		PORTA = (1<<5);
		PORTC = second % 10;
		_delay_ms(5);
		PORTA = (1<<4);
		PORTC = second / 10;
		_delay_ms(5);
		PORTA = (1<<3);
		PORTC = minute % 10;
		_delay_ms(5);
		PORTA = (1<<2);
		PORTC = minute / 10;
		_delay_ms(5);
		PORTA = (1<<1);
		PORTC = hour % 10;
		_delay_ms(5);
		PORTA = (1<<0);
		PORTC = hour / 10;
		_delay_ms(5);
	}

}
