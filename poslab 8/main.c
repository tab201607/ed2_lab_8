#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"

/**
 * main.c
 */

char timerLED = 0b0;
char redLight = 0b0;
char greenLight = 0b0;
char blueLight = 0b0;

int Timer0IntHandler(void) {
    timerLED = !timerLED;
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, timerLED << 2); //prende luz
    TimerLoadSet(TIMER0_BASE, TIMER_A, 20000000); //colocar el valor en el timer de nuevo
    TimerIntClear(TIMER0_BASE, TIMER_A); //rehabilitar timer interrupt
    return 0;
}

int UARTIntHandler(void) {
int32_t entry = UARTCharGet(UART0_BASE); //recibir señal del uart

switch (entry) { //invertir valor del luz dependiendo de que se recibio
case 'r':
    redLight = ~redLight;
        //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, redLight << 1);
        break;
case 'g':
    greenLight = ~greenLight;
        //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, greenLight);
        break;
case 'b':
    blueLight = ~blueLight;
       //GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, blueLight);
       break;
default: break;
}

//encender luces correspondientes
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, (redLight << 1));
GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, (greenLight << 3) | (blueLight << 2));

UARTIntClear(UART0_BASE, UART_INT_RX);
UARTIntClear(UART0_BASE, UART_INT_RT);
return 0;


}

int main(void)
    {

        SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); //configurar clock a 40 MHz
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //clock para puerto F, A, UART y timer
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

        IntMasterEnable();

        //config timer
        TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); //configure al timer 0 como una periodica
        TimerLoadSet(TIMER0_BASE, TIMER_A, 20000000);
        IntEnable(INT_TIMER0A); //activar interrupt para timer
        TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

        TimerEnable(TIMER0_BASE, TIMER_A); //activa el timer 0

        GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //configurar luces en la tiva
        GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2); //configurar luz aparte

        //configurar pins para UART
        GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
        GPIOPinConfigure(GPIO_PA0_U0RX);
        GPIOPinConfigure(GPIO_PA1_U0TX);

        UARTConfigSetExpClk(UART0_BASE, 40000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)); //Configurar UART

        //configurar interrupt para UART
        IntEnable(INT_UART0);
        UARTIntEnable(UART0_BASE, UART_INT_RX  | UART_INT_RT);
        UARTEnable(UART0_BASE);


        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0b10);


        while(1);
	return 0;
}


