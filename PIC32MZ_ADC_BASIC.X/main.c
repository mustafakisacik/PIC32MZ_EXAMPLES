#include "main.h"
#include "common.h"
#include "uart.h"

/* refer to DS60001344E example 22-1 & DS60001320F 28.1 active sequence */
uint32_t adcVal[2];

void initADC(void);

void main(void) {
    sysInit();
    UART_Init();
    initADC();

    printf("%s\r\n", "### PIC32MZ ADC Basic Demo ###");

    while (1) {
        /* trigger a conversion */
        ADCCON3bits.GSWTRG = 1;

        /* wait the conversions */
        while (ADCDSTAT1bits.ARDY0 == 0);
        adcVal[0] = ADCDATA0;
        while (ADCDSTAT1bits.ARDY1 == 0);
        adcVal[1] = ADCDATA1;

        printf("ADC[0]=%04lu, ADC[1]=%04lu\r\n", adcVal[0], adcVal[1]);
        __delay_ms(1000);
    };
}

void initADC(void) {

    /* enable analog input */
    ANSELBbits.ANSB0 = 1; //RB0(AN0) analog input
    ANSELBbits.ANSB1 = 1; //RB1(AN1) analog input

    /* ADC calibration registers */
    ADC0CFG = DEVADC0;
    ADC1CFG = DEVADC1;
    ADC2CFG = DEVADC2;
    ADC3CFG = DEVADC3;
    ADC4CFG = DEVADC4;    
    ADC7CFG = DEVADC7;

    /* ADCCON1 control register */
    ADCCON1bits.ON = 0; //disable ADC
    ADCCON1bits.FSSCLKEN = 1; //fast synchronous system clock    
    ADCCON1bits.AICPMPEN = 0; //when vdd >= 2.5V, clear   
    CFGCONbits.IOANCPEN = 0; //when vdd >= 2.5V, clear   

    /* ADCCON2: control register */
    ADCCON2 = 0; //class 1 inputs, no setting is required

    /* ADCANCON: warm-up control register */
    ADCANCON = 0;
    ADCANCONbits.WKUPCLKCNT = 10; // 2^10 clocks

    /* ADCCON3: control register */
    ADCCON3 = 0;
    ADCCON3bits.ADCSEL = 1; //system clock, 200MHz = 0.005us
    ADCCON3bits.CONCLKDIV = 1; //divider2, Tq = 100MHz = 0.01us
    ADCCON3bits.VREFSEL = 0; // select AVdd and AVss as reference source

    /* ADCxTIME: sampling time */
    ADC0TIMEbits.ADCDIV = 1; //divider2, Tad = 50MHz = 0.02us
    ADC0TIMEbits.SAMC = 50; //ADC0 sampling time = 50 * Tad = 1us
    ADC0TIMEbits.SELRES = 3; //ADC0 resolution is 12 bits
    ADC1TIMEbits.ADCDIV = 1; //divider2, Tad = 50MHz = 0.02us
    ADC1TIMEbits.SAMC = 50; //ADC1 sampling time = 50 * Tad = 1us
    ADC1TIMEbits.SELRES = 3; // ADC1 resolution is 12 bits

    /* ADCTRGMODE */
    ADCTRGMODEbits.SH0ALT = 0; //AN0(0), AN45(1) 
    ADCTRGMODEbits.SH1ALT = 0; //AN1(0), AN46(1)

    /* ADCIMCON1 */
    ADCIMCON1bits.SIGN0 = 0; //unsigned format
    ADCIMCON1bits.DIFF0 = 0; //single ended
    ADCIMCON1bits.SIGN1 = 0; //unsigned format
    ADCIMCON1bits.DIFF1 = 0; //single ended

    /* ADCGIRQENx */
    ADCGIRQEN1 = 0; // no interrupts are used
    ADCGIRQEN2 = 0;

    /* ADCCSSx */
    ADCCSS1 = 0; // No scanning is used
    ADCCSS2 = 0;

    /* ADCTRGSNS, ADCTRG1 */
    ADCTRGSNSbits.LVL0 = 0; // Edge trigger
    ADCTRGSNSbits.LVL1 = 0; // Edge trigger
    ADCTRG1bits.TRGSRC0 = 1; //set AN0 to trigger from software.
    ADCTRG1bits.TRGSRC1 = 1; //set AN1 to trigger from software.    

    /* early interrupt */
    ADCEIEN1 = 0; // No early interrupt
    ADCEIEN2 = 0;

    /* Turn the ADC on */
    ADCCON1bits.ON = 1;

    /* wait for voltage reference to be stable */
    while (!ADCCON2bits.BGVRRDY); //wait Vref ready
    while (ADCCON2bits.REFFLT); //wait Vref fault

    /* enable clock to analog circuit */
    ADCANCONbits.ANEN0 = 1; //enable the clock to analog bias
    ADCANCONbits.ANEN1 = 1; //enable the clock to analog bias

    /* wait for ADC to be ready */
    while (!ADCANCONbits.WKRDY0); //wait until ADC0 is ready
    while (!ADCANCONbits.WKRDY1); //wait until ADC1 is ready

    /* enable the ADC module */
    ADCCON3bits.DIGEN0 = 1; //enable ADC0
    ADCCON3bits.DIGEN1 = 1; //enable ADC1    
}
