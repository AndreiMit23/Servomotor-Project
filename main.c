#include "p33Fxxxx.h"
 
// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching and Configure
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF);
 
double getPulseWidthValue(double buff)
{
    // calculez latimea pulsului in ms normalizata in intervalul 1-2ms.
    double value = (buff / 4095 + 1);
    // calculez valoarea ce trebuie introdusa in registrul P1DC1.
    // (value / 25) * 10^6 / 64(prescaler)
    unsigned int registerValue = (int)(value / 25 * 2000000) / 64;
#ifdef DEBUG
    printf("Value: %f | \t registerValue: %d (%b)\n", value, registerValue, registerValue);
#endif
    return registerValue;
}
 
void initPLL(void)
{
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    _PLLDIV = 41; // M = 43 FRC
    _PLLPRE = 0;  // N1 = 2
    _PLLPOST = 0; // N2 = 2
 
    // Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
    __builtin_write_OSCCONH(0x01); // FRC
    //__builtin_write_OSCCONH(0x03); // XT
    __builtin_write_OSCCONL(0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b001)
        ; // FRC
    // while (OSCCONbits.COSC != 0b011); // XT
    // Wait for PLL to lock
    while (OSCCONbits.LOCK != 1)
    {
    };
}
 
void initPWM(void)
{
    // 20/25 * 10^6 = 800.000 / 64 = 12.500
    P1TPER = 12500; // Perioada generator PWM1
    P2TPER = 12500; // Perioada generator PWM2
    
    // 1/25 * 10^6 * 2 = 80.000 / 64 = 1250
	P1DC2 = 1250;
 
    PTCONbits.PTCKPS = 0b11; // Prescale de 1:64
    PWM1CON1bits.PMOD1 = 1;  // Pinii Low si High independenti
 
    // Pin PWM1H setat ca iesire
    PWM1CON1bits.PEN1H = 1; // RB14
	PWM1CON1bits.PEN2H = 1; // RB12
 
    // Resetam counter-ul
    PTMR = 0;
    PTCONbits.PTEN = 1; // Start timer
}
 
void initAdc1(void)
{
    AD1CON1bits.AD12B = 1; // conversie AD pe 12 biti
    AD1CON1bits.SSRC = 2;  // timerul 3 startează conversia
    AD1CON1bits.ASAM = 1;  // începe esantionarea după terminarea unei conversii
    AD1CON2bits.CSCNA = 1; // scanează intrările pe CH0+
 
    AD1CON3bits.ADRC = 0;  // folose?te ceasul de magistrală
    AD1CON3bits.ADCS = 63; // Tad=Tcy*(adcs+1)=25ns*64=1.6us
 
    AD1CSSLbits.CSS4 = 1;   // este scanată intrarea analogică AN4(RB2)
    AD1PCFGL = 0xFFFF;      // setează pinii portului ADC1 ca fiind digitali
    AD1PCFGLbits.PCFG4 = 0; // setează pinul AN4(RB2) ca intrare analogică
 
    _AD1IF = 0; // reseteaza flag-ul întreruperii convertorului AD
    _AD1IE = 1; // permite întreruperea convertorului AD
 
    AD1CON1bits.ADON = 1; // porne?te convertorul AD
}
 
void initTmr3()
{
	T3CONbits.TCKPS = 0b11; // setare prescale 256 pentru 4.000.000 ns
	TMR3 = 0;
 	PR3 = 15500; //valoare pentru timer3 perioada de 0.1 s 
 	T3CONbits.TON = 1; // reprezinta timer3 activ 
}
 
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
// procesare cod digital obtinut în registrul ADC1BUF0 în urma conversiei
 
	P1DC1 = getPulseWidthValue(ADC1BUF0);
	_AD1IF = 0; // Achita intreruperea convertorului AD
}
 
int main(void)
{	
	initPLL();
	initAdc1();
	initTmr3();	
	initPWM();
 
	_TRISB12 = 0; // ca iesire 
	_TRISB14 = 0;
 
 
	while(1){
 
	}
}