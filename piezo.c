#include <msp430.h>
#include <stdbool.h>

int count = 0;
int piezo_count = 0; // for timing the changing pitches
int debounce_count = 0; // for timing the debounce period
bool tone = true; // if true, the higher pitch will play
bool tuned_tone = true; // if true, the higher pitch will be controlled by the pot
int period0; // lower pitch
int period1; // higher pitch

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT

  P1DIR |= BIT0 + BIT6; // P1.0 buzzer output, P1.6 selected tone indicator
  P1OUT |= BIT3;
  P1REN |= BIT3;
  P1IES |= BIT3;
  P1IFG &= ~BIT3;
  P1IE |= BIT3;


  CCTL0 = CCIE; // CCR0 interrupt enabled
  CCR0 = 0.0005*32768-1; // clock period of about 0.5 ms
  TACTL = TASSEL_1 + MC_1; // ACLK, upmode
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
  ADC10CTL1 = INCH_1; // input A1
  ADC10AE0 |= 0x02; // PA.1 ADC option select
  
  ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
  
  // calculating base periods for the 2 pitches
  period1 = 2+2000/ADC10MEM;
  period0 = 2*period1;
  
  __bis_SR_register(LPM3_bits + GIE); // Enter LPM3 w/ interrupt
 }

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    // debouncing the switch for selecting the pitch to tune
    if(P1IFG & BIT3)
    {
      debounce_count++;
      // 40 * 0.5 ms = 20 ms
      if(debounce_count >= 40)
      {
        if(~(P1IN & BIT3))
        {
          tuned_tone = !tuned_tone;
          P1OUT ^= BIT6; // toggling P1.6 to indicate which tone is being tuned
        }
        P1IFG &= ~BIT3;
        debounce_count = 0;
      }
    }

    
    ADC10CTL0 |= ENC + ADC10SC; // Sampling and conversion start
    // switching which pitch is tuned
    if(tuned_tone)
    {
      period1 = 2+2000/ADC10MEM;
    }
      else
     {
      period0 = 4+2*2000/ADC10MEM;
    }
     
    if(count >= 2000)
    {
      tone = !tone;
      count = 0;
      piezo_count = 0;
    }
    
    count++;
    piezo_count++;
    
    // switching the pitches
    if(tone)
    {
      if(piezo_count >= period1)
      {
        P1OUT ^= BIT0; // Toggle P1.0
        piezo_count = 0;
      }
    }
    else
    {
      if(piezo_count >= period0)
      {
        P1OUT ^= BIT0; // Toggle P1.0
        piezo_count = 0;
      }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
{

}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

}