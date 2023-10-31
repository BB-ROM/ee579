#include <msp430.h>

int count = 0;

int main(void)
{
WDTCTL = WDTPW + WDTHOLD; // Stop WDT

P1DIR |= BIT0; // P1.0 output
P1OUT |= BIT3;
P1REN |= BIT3;
P1IES |= BIT3;
P1IFG &= ~BIT3;
P1IE |= BIT3;


CCTL0 = CCIE; // CCR0 interrupt enabled
CCR0 = 1.5*32768-1; // 1.5s clock period => 40 flashes a minute
TACTL = TASSEL_1 + MC_1; // ACLK, upmode
__bis_SR_register(LPM3_bits + GIE); // Enter LPM3 w/ interrupt
}
// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  if(P1IFG & BIT3)
  {
    count++;
    if(count == 20) // 20 clock periods = 30s
    {
      P1IFG &= ~BIT3;
      count = 0;
    }
  }
  else
  {
    P1OUT ^= BIT0; // Toggle P1.0
  }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1 (void)
{
  P1OUT &= ~BIT0;
}