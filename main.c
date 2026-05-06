#include "p33Fxxxx.h"

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching and Configure
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF);

void initPLL(void)
{
// Configure PLL prescaler, PLL postscaler, PLL divisor
 _PLLDIV = 41; // M = 43 FRC
 _PLLPRE = 0; // N1 = 2
 _PLLPOST = 0; // N2 = 2

// Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
 __builtin_write_OSCCONH(0x01); // FRC
 //__builtin_write_OSCCONH(0x03); // XT
 __builtin_write_OSCCONL(0x01);
// Wait for Clock switch to occur
 while (OSCCONbits.COSC != 0b001); // FRC
 //while (OSCCONbits.COSC != 0b011); // XT
// Wait for PLL to lock
 while(OSCCONbits.LOCK!=1) {};
}

void initPWM(void)
{	
	
	// nu mai am nevoie timere sa le setez, modulul PWM are deja timer intern
 	P1TPER = 12500;

	//latimea pulsului
	P1DC1 = 625; // 5% factor umplere 

	PTCONbits.PTCKPS = 0b10;
	PWM1CON1bits.PMOD1 = 1; 
	
	//Pin PWM2H setat pe iesire - e legat de RB12 daca te uiti pe schema 
	PWM1CON1bits.PEN2H = 1;
	//counter - counting down 
	PTMR = 0;  
	PTCONbits.PTEN = 1;  //Start timer 

	// DACA TE UITI PE OSCILOSCOP O SA AIBA O VALOARE CONSTATNA (POSIBIL SI O PERTURBATIE) MI A ZIS PROFU CA E CORECT CA EU L AM SETAT SA STEA CONSTATN DECI CERINTA 3 DONE 
}

void initAdc1(void)
{
 AD1CON1bits.AD12B = 1; // conversie AD pe 12 bi?i
 AD1CON1bits.SSRC = 2; // timerul 3 starteazã conversia
 AD1CON1bits.ASAM = 1; // începe e?antionarea dupã terminarea unei conversii
 AD1CON2bits.CSCNA = 1; // scaneazã intrãrile pe CH0+

 AD1CON3bits.ADRC = 0; // folose?te ceasul de magistralã
 AD1CON3bits.ADCS = 63; // Tad=Tcy*(adcs+1)=25ns*64=1.6us

 AD1CSSLbits.CSS4 = 1; // este scanatã intrarea analogicã AN4(RB2)
 AD1PCFGL=0xFFFF; // seteazã pinii portului ADC1 ca fiind digitali
 AD1PCFGLbits.PCFG4 = 0; // seteazã pinul AN4(RB2) ca intrare analogicã

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
// procesare cod digital ob?inut în registrul ADC1BUF0 în urma conversiei
 // ………………
	_RB14 = ~_RB14;
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