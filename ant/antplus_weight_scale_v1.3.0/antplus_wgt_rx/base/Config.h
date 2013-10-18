/*
   Copyright 2012 Dynastream Innovations, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef CONFIG_H
#define CONFIG_H





// APPLICATION TYPE
#define USE_WGTRX


//------------------------------------------------------------------------------
// DEVELOPMENT BOARD TYPE
//#define BOARD_EZ430_RF2500 1
#define BOARD_ANT_CPU 1
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SERIAL PORT TYPE
//#define USE_UART_DEBUG
#define SERIAL_UART_ASYNC
//#define SERIAL_BIT_SYNCHRONOUS
//#define SERIAL_BYTE_SYNCHRONOUS

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EXTERNAL CLOCK
//#define USE_EXTERNAL_32KHZ
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RESET TYPE
#if defined (SERIAL_UART_ASYNC)  //Asynchronous
//CHOOSE ONE OF THE TWO OPTIONS
#define PIN_RESET              //Recommended reset option for newer ANT modules
//#define SUSPEND_MODE_RESET       //Use this option only when a reset pin is unavailable
#else                            //Synchronous
//CHOOSE ONE OF THE TWO OPTIONS
#define PIN_RESET              //Recommended reset option for newer ANT modules
//#define SYNCH_RESET              //Use this option only when a reset pin is unavailable
#endif


//------------------------------------------------------------------------------

// Compiler Environment
#if defined (BOARD_EZ430_RF2500)
   #include <msp430x22x4.h>                                    // Target MCU
#elif defined (BOARD_ANT_CPU) 
   #include <msp430x22x4.h>                                    // Target MCU
#else
   #include "msp430x20x3.h"
#endif

// Bit Sync Serial Port Definitions
#if defined(BOARD_ANT_CPU)

// Clock out pin for driving AP1 32kHz clock
#define EXT_CLOCK_32KHZ_SEL                  P2SEL
#define EXT_CLOCK_32KHZ_DIR                  P2DIR
#define EXT_CLOCK_32KHZ_BIT                  BIT0           // P2.0
#define EXT_CLOCK_32KHZ_OUT                  P2OUT

// IOBOARD LED pin definitions
#if defined (SERIAL_BYTE_SYNCHRONOUS) || defined(SERIAL_BIT_SYNCHRONOUS)

#define IOBOARD_LED0_SEL                     P4SEL
#define IOBOARD_LED0_DIR                     P4DIR
#define IOBOARD_LED0_BIT                     BIT4           // P4.4
#define IOBOARD_LED0_OUT                     P4OUT

#define IOBOARD_LED1_SEL                     P4SEL
#define IOBOARD_LED1_DIR                     P4DIR
#define IOBOARD_LED1_BIT                     BIT5           // P4.5
#define IOBOARD_LED1_OUT                     P4OUT

#define IOBOARD_LED2_SEL                     P4SEL
#define IOBOARD_LED2_DIR                     P4DIR
#define IOBOARD_LED2_BIT                     BIT6           // P4.6
#define IOBOARD_LED2_OUT                     P4OUT

#define IOBOARD_LED3_SEL                     P4SEL
#define IOBOARD_LED3_DIR                     P4DIR
#define IOBOARD_LED3_BIT                     BIT7           // P4.7
#define IOBOARD_LED3_OUT                     P4OUT

#define IOBOARD_BUTTON_SEL                  P4SEL
#define IOBOARD_BUTTON_DIR                  P4DIR
#define IOBOARD_BUTTON_IN                   P4IN

#define IOBOARD_BUTTON0_SEL                  P4SEL
#define IOBOARD_BUTTON0_DIR                  P4DIR
#define IOBOARD_BUTTON0_BIT                  BIT0           // P4.0
#define IOBOARD_BUTTON0_IN                   P4IN

#define IOBOARD_BUTTON1_SEL                  P4SEL
#define IOBOARD_BUTTON1_DIR                  P4DIR
#define IOBOARD_BUTTON1_BIT                  BIT1           // P4.1
#define IOBOARD_BUTTON1_IN                   P4IN

#define IOBOARD_BUTTON2_SEL                  P4SEL
#define IOBOARD_BUTTON2_DIR                  P4DIR
#define IOBOARD_BUTTON2_BIT                  BIT2           // P4.2
#define IOBOARD_BUTTON2_IN                   P4IN

#define IOBOARD_BUTTON3_SEL                  P4SEL
#define IOBOARD_BUTTON3_DIR                  P4DIR
#define IOBOARD_BUTTON3_BIT                  BIT3           // P4.3
#define IOBOARD_BUTTON3_IN                   P4IN


#elif defined(SERIAL_UART_ASYNC)

#define IOBOARD_LED0_SEL                     P2SEL
#define IOBOARD_LED0_DIR                     P2DIR
#define IOBOARD_LED0_BIT                     BIT0           // P2.0
#define IOBOARD_LED0_OUT                     P2OUT

#define IOBOARD_LED1_SEL                     P4SEL
#define IOBOARD_LED1_DIR                     P4DIR
#define IOBOARD_LED1_BIT                     BIT5           // P4.5
#define IOBOARD_LED1_OUT                     P4OUT

#define IOBOARD_LED2_SEL                     P3SEL
#define IOBOARD_LED2_DIR                     P3DIR
#define IOBOARD_LED2_BIT                     BIT1           // P3.1
#define IOBOARD_LED2_OUT                     P3OUT
#define IOBOARD_LED2_REN                     P3REN

#define IOBOARD_LED3_SEL                     P4SEL
#define IOBOARD_LED3_DIR                     P4DIR
#define IOBOARD_LED3_BIT                     BIT7           // P4.7
#define IOBOARD_LED3_OUT                     P4OUT

#define IOBOARD_BUTTON_SEL                   P4SEL
#define IOBOARD_BUTTON_DIR                   P4DIR
#define IOBOARD_BUTTON_IN                    P4IN

#define IOBOARD_BUTTON0_SEL                  P2SEL
#define IOBOARD_BUTTON0_DIR                  P2DIR
#define IOBOARD_BUTTON0_BIT                  BIT5           // P2.5
#define IOBOARD_BUTTON0_IN                   P2IN

#define IOBOARD_BUTTON1_SEL                  P4SEL
#define IOBOARD_BUTTON1_DIR                  P4DIR
#define IOBOARD_BUTTON1_BIT                  BIT6           // P4.6
#define IOBOARD_BUTTON1_IN                   P4IN

#define IOBOARD_BUTTON2_SEL                  P3SEL
#define IOBOARD_BUTTON2_DIR                  P3DIR
#define IOBOARD_BUTTON2_BIT                  BIT2           // P3.2
#define IOBOARD_BUTTON2_IN                   P3IN

#define IOBOARD_BUTTON3_SEL                  P4SEL
#define IOBOARD_BUTTON3_DIR                  P4DIR
#define IOBOARD_BUTTON3_BIT                  BIT5           // P4.5
#define IOBOARD_BUTTON3_IN                   P4IN

#endif   // SERIAL_UART_ASYNC


// IOBOARD BUTTON definitions

// General GPIO Register
#define GPIO_P00_SEL                         P1SEL
#define GPIO_P00_DIR                         P1DIR
#define GPIO_P00_BIT                         BIT0           // P1.0
#define GPIO_P00_OUT                         P1OUT

// UCI Registers for byte synchronous. 
#define BYTE_SYNC_UCI_CTL0                   UCB0CTL0       // UCI Control Register 0 
#define BYTE_SYNC_UCI_CTL1                   UCB0CTL1       // UCI Control Register 1
#define BYTE_SYNC_UCI_BR0                    UCB0BR0        // UCI Baud Rate Register 0
#define BYTE_SYNC_UCI_BR1                    UCB0BR1        // UCI Baud Rate Register 1
#define BYTE_SYNC_UCI_STAT                   UCB0STAT       // UCI Status Register
#define BYTE_SYNC_UCI_RXBUF                  UCB0RXBUF      // UCI Recieve Buffer Register
#define BYTE_SYNC_UCI_TXBUF                  UCB0TXBUF      // UCI Transmit Buffer Register

#define BYTE_SYNC_UCI_IER                    IE2            // Interrupt enable register
#define BYTE_SYNC_UCI_IFR                    IFG2           // Interrupt flag register

#define BYTE_SYNC_RX_INT                     0x04           // Recieve interrupt bit
#define BYTE_SYNC_TX_INT                     0x08           // Transmit interrupt bit

// UCI Registers fro asynchronous.
#define UART_ASYNC_UCI_RXBUF                 UCA0RXBUF      // UCI Recieve Buffer Register
#define UART_ASYNC_UCI_TXBUF                 UCA0TXBUF      // UCI Transmit Buffer Register
#define UART_ASYNC_UCI_CTL0                  UCA0CTL0       // UCI Control Register 0 
#define UART_ASYNC_UCI_CTL1                  UCA0CTL1       // UCI Control Register 1
#define UART_ASYNC_UCI_BR0                   UCA0BR0        // UCI Baud Rate Register 0
#define UART_ASYNC_UCI_BR1                   UCA0BR1        // UCI Baud Rate Register 1
#define UART_ASYNC_UCI_MCTL                  UCA0MCTL       // UCI Modulation Control Register

#define UART_ASYNC_UCI_IER                   IE2            // Interrupt enable register
#define UART_ASYNC_UCI_IFR                   IFG2           // Interrupt flag register

#define UART_ASYNC_RX_INT                    0x01           // Recieve interrupt bit
#define UART_ASYNC_TX_INT                    0x02           // Transmit interrupt bit


// Serial Port Definitions (Common)

//------------------------------------------------------------------------------
// PORTSEL
#define SERIAL_PORTSEL_SEL                   P4SEL
#define SERIAL_PORTSEL_DIR                   P4DIR
#define SERIAL_PORTSEL_BIT                   BIT6           // P4.6
#define SERIAL_PORTSEL_OUT                   P4OUT

//------------------------------------------------------------------------------
// SFLOW
#define SERIAL_SFLOW_SEL                     P4SEL
#define SERIAL_SFLOW_DIR                     P4DIR
#define SERIAL_SFLOW_BIT                     BIT5           // P4.5
#define SERIAL_SFLOW_OUT                     P4OUT

//------------------------------------------------------------------------------
// IOSEL
#define SERIAL_IOSEL_SEL                     P2SEL          // Interrupt Flag
#define SERIAL_IOSEL_DIR                     P2DIR
#define SERIAL_IOSEL_OUT                     P2OUT
#define SERIAL_IOSEL_BIT                     BIT1            // P2.1 


// Asynchronous
#define ASYNC_RX_DIR                         P3DIR
#define ASYNC_RX_SEL                         P3SEL
#define ASYNC_RX_BIT                         BIT5           // P3.5 
#define ASYNC_RX_IN                          P3IN           // TX on ANT

#define ASYNC_TX_DIR                         P3DIR
#define ASYNC_TX_SEL                         P3SEL
#define ASYNC_TX_BIT                         BIT4           // P3.4
#define ASYNC_TX_OUT                         P3OUT          // RX on ANT

#define ASYNC_RTS_DIR                        P2DIR
#define ASYNC_RTS_SEL                        P2SEL
#define ASYNC_RTS_BIT                        BIT1           // P2.1
#define ASYNC_RTS_IN                         P2IN           // RTS on ANT

#define ASYNC_SUSPEND_DIR                    P4DIR
#define ASYNC_SUSPEND_SEL                    P4SEL
#define ASYNC_SUSPEND_BIT                    BIT0           // P4.0
#define ASYNC_SUSPEND_OUT                    P4OUT          // SUSPEND on ANT

#define ASYNC_SLEEP_DIR                      P4DIR
#define ASYNC_SLEEP_SEL                      P4SEL
#define ASYNC_SLEEP_BIT                      BIT1           // P4.1
#define ASYNC_SLEEP_OUT                      P4OUT          // SLEEP on ANT


// Bit Sync Serial Port Definitions

// SEN
#define SYNC_SEN_DIR                         P2DIR
#define SYNC_SEN_IN                          P2IN
#define SYNC_SEN_BIT                         BIT1           // P2.1 
#define SYNC_SEN_IFG                         P2IFG          // Interrupt Flag
#define SYNC_SEN_IES                         P2IES          // Interrupt Edge Select
#define SYNC_SEN_IE                          P2IE           // Interrupt Enable

//------------------------------------------------------------------------------
// SMSGRDY
#define SYNC_SMSGRDY_DIR                     P3DIR
#define SYNC_SMSGRDY_OUT                     P3OUT
#define SYNC_SMSGRDY_BIT                     BIT0           // P3.0

//------------------------------------------------------------------------------
// SIN
#define SYNC_SIN_SEL                         P3SEL
#define SYNC_SIN_DIR                         P3DIR
#define SYNC_SIN_BIT                         BIT1           // P3.1 (SOUT on ANT)
#define SYNC_SIN_IN                          P3IN

//------------------------------------------------------------------------------
// SOUT
#define SYNC_SOUT_SEL                        P3SEL
#define SYNC_SOUT_DIR                        P3DIR
#define SYNC_SOUT_BIT                        BIT2           // P3.2 (SIN on ANT)
#define SYNC_SOUT_OUT                        P3OUT

//------------------------------------------------------------------------------
// SCLK
#if defined(SERIAL_BYTE_SYNCHRONOUS)
#define SYNC_SCLK_SEL                        P3SEL
#define SYNC_SCLK_DIR                        P3DIR
#define SYNC_SCLK_BIT                        BIT3           // P3.3
#define SYNC_SCLK_IN                         P3IN

#elif defined(SERIAL_BIT_SYNCHRONOUS)
#define SYNC_SCLK_SEL                        P2SEL
#define SYNC_SCLK_DIR                        P2DIR
#define SYNC_SCLK_BIT                        BIT2           // P2.2
#define SYNC_SCLK_IN                         P2IN
#define SYNC_SCLK_IES                        P2IES
#define SYNC_SCLK_IE                         P2IE
#define SYNC_SCLK_IFG                        P2IFG
#endif

//------------------------------------------------------------------------------
// SRDY
#define SYNC_SRDY_SEL                        P2SEL
#define SYNC_SRDY_DIR                        P2DIR
#define SYNC_SRDY_OUT                        P2OUT
#define SYNC_SRDY_BIT                        BIT5           // P2.5

#elif defined(BOARD_EZ430_RF2500)

// SEN
#define SYNC_SEN_DIR                         P2DIR
#define SYNC_SEN_IN                          P2IN
#define SYNC_SEN_BIT                         BIT3           // P2.3 
#define SYNC_SEN_IFG                         P2IFG          // Interrupt Flag
#define SYNC_SEN_IES                         P2IES          // Interrupt Edge Select
#define SYNC_SEN_IE                          P2IE           // Interrupt Enable

//------------------------------------------------------------------------------
// SMSGRDY
#define SYNC_SMSGRDY_DIR                     P2DIR
#define SYNC_SMSGRDY_OUT                     P2OUT
#define SYNC_SMSGRDY_BIT                     BIT4           // P2.4

//------------------------------------------------------------------------------
// SIN
#define SYNC_SIN_DIR                         P2DIR
#define SYNC_SIN_BIT                         BIT0           // P2.0 (SOUT on ANT)
#define SYNC_SIN_IN                          P2IN

//------------------------------------------------------------------------------
// SOUT
#define SYNC_SOUT_SEL                        P2SEL
#define SYNC_SOUT_DIR                        P2DIR
#define SYNC_SOUT_BIT                        BIT1           // P2.1 (SIN on ANT)
#define SYNC_SOUT_OUT                        P2OUT

//------------------------------------------------------------------------------
// SCLK
#define SYNC_SCLK_SEL                        P2SEL
#define SYNC_SCLK_DIR                        P2DIR
#define SYNC_SCLK_BIT                        BIT2           // P2.2
#define SYNC_SCLK_IN                         P2IN
#define SYNC_SCLK_IES                        P2IES
#define SYNC_SCLK_IE                         P2IE
#define SYNC_SCLK_IFG                        P2IFG

//------------------------------------------------------------------------------
// SRDY
#define SYNC_SRDY_SEL                        P4SEL
#define SYNC_SRDY_DIR                        P4DIR
#define SYNC_SRDY_OUT                        P4OUT
#define SYNC_SRDY_BIT                        BIT3           // P4.3

#else
// SEN
#define SYNC_SEN_DIR                         P1DIR
#define SYNC_SEN_IN                          P1IN
#define SYNC_SEN_BIT                         BIT6           // P1.6 
#define SYNC_SEN_IFG                         P1IFG          // Interrupt Flag
#define SYNC_SEN_IES                         P1IES          // Interrupt Edge Select
#define SYNC_SEN_IE                          P1IE           // Interrupt Enable

//------------------------------------------------------------------------------
// SMSGRDY
#define SYNC_SMSGRDY_DIR                     P1DIR
#define SYNC_SMSGRDY_OUT                     P1OUT
#define SYNC_SMSGRDY_BIT                     BIT1           // P1.1

//------------------------------------------------------------------------------
// SIN
#define SYNC_SIN_DIR                         P1DIR
#define SYNC_SIN_BIT                         BIT2           // P1.2 (SOUT on ANT)
#define SYNC_SIN_IN                          P1IN

//------------------------------------------------------------------------------
// SOUT
#define SYNC_SOUT_SEL                        P1SEL
#define SYNC_SOUT_DIR                        P1DIR
#define SYNC_SOUT_BIT                        BIT3           // P1.3 (SIN on ANT)
#define SYNC_SOUT_OUT                        P1OUT

//------------------------------------------------------------------------------
// SCLK
#define SYNC_SCLK_SEL                        P1SEL
#define SYNC_SCLK_DIR                        P1DIR
#define SYNC_SCLK_BIT                        BIT4           // P1.4
#define SYNC_SCLK_IN                         P1IN
#define SYNC_SCLK_IES                        P1IES
#define SYNC_SCLK_IE                         P1IE
#define SYNC_SCLK_IFG                        P1IFG

//------------------------------------------------------------------------------
// SRDY
#define SYNC_SRDY_SEL                        P1SEL
#define SYNC_SRDY_DIR                        P1DIR
#define SYNC_SRDY_OUT                        P1OUT
#define SYNC_SRDY_BIT                        BIT5           // P1.5
#endif


//------------------------------------------------------------------------------
// ANT Reset
#define SYSTEM_RST_SEL                       P3SEL
#define SYSTEM_RST_DIR                       P3DIR
#define SYSTEM_RST_BIT                       BIT7           // P3.7
#define SYSTEM_RST_OUT                       P3OUT


//------------------------------------------------------------------------------
#define LED_SEL                              P1SEL
#define LED_DIR                              P1DIR
#define LED_OUT                              P1OUT
#define LED_BIT                              BIT0           // P1.0
#define LED0_BIT                             BIT0           // P1.0
#define LED1_BIT                             BIT1           // P1.1
//------------------------------------------------------------------------------
// Interrupt Servicing
#define SYNC_PORT1_ISR(func)                 PORT1_ISR(func)
#define SYNC_PORT2_ISR(func)                 PORT2_ISR(func)
//------------------------------------------------------------------------------
// MCU System definitions
// Watchdog
#define SYSTEM_WATCHDOG_TCTL                 WDTCTL         // Watchdog Timer Control
#define SYSTEM_WATCHDOG_PW                   WDTPW          // Watchdog Password
#define SYSTEM_WATCHDOG_HOLD                 WDTHOLD        // Watchdog Timer Hold (Disables Watchdog)
#define SYSTEM_WATCHDOG_WDTIE                WDTIE          // Watchdog Interrupt Enable
#define SYSTEM_WATCHDOG_WDTIFG               WDTIFG         // Watchdog Interrupt Flag
//------------------------------------------------------------------------------
// Clocks
#define SYSTEM_DCO_0                         DCO0           // DCO Select Bit 0
#define SYSTEM_DCO_1                         DCO1           // DCO Select Bit 1
#define SYSTEM_DCO_2                         DCO2           // DCO Select Bit 2 
#define SYSTEM_DCOCTL                        DCOCTL         // DCO Control
#define SYSTEM_BCSCTL1                       BCSCTL1        // Basic Clock System Control 1
#define SYSTEM_BCSCTL2                       BCSCTL2        // Basic Clock System Control 2
#define SYSTEM_BCSCTL3                       BCSCTL3        // Basic Clock System Control 3
#define SYSTEM_RSEL0                         RSEL0          // DCO Resistor Select 0
#define SYSTEM_RSEL1                         RSEL1          // DCO Resistor Select 1
#define SYSTEM_RSEL2                         RSEL2          // DCO Resistor Select 2
#define SYSTEM_RSEL3                         RSEL3          // DCO Resistor Select 3
#define SYSTEM_DCOR                          DCOR           // ROSC control
#define SYSTEM_XTS                           XTS            // LFXT1 control
#define SYSTEM_DIVA0                         DIVA0          // ACLK divide setting low bit
#define SYSTEM_DIVA1                         DIVA1          // ACLK divide setting high bit
#define SYSTEM_XT2OFF                        XT2OFF         // External clock 2 OFF/ON
#define SYSTEM_IE1                           IE1            // Interrupt Enable Register 1
#define SYSTEM_OFIE                          OFIE           // Oscillator Fault Interrupt Enable
#define SYSTEM_IFG1                          IFG1           // Interrupt Flag Register 1
#define SYSTEM_OFIFG                         OFIFG          // Oscillator Fault Interrup Flag
#define MAX_MCLK_FREQ                        (SYSTEM_DCO_0 | SYSTEM_DCO_1 | SYSTEM_DCO_2) // maximum DCO setting
//------------------------------------------------------------------------------
// System Ops
#define SYSTEM_LPM0                          LPM0           // LPM0 enable
#define SYSTEM_LPM0_EXIT                     LPM0_EXIT      // LPM0 disable
//------------------------------------------------------------------------------
// Timer A
#define TIMER_A_TACTL                        TACTL          // Timer A control register
#define TIMER_A_TACCTL0                      TACCTL0        // Timer A Capture/Compare Control register 0
#define TIMER_A_TACCTL1                      TACCTL1        // Timer A Capture/Compare Control register 1
#define TIMER_A_TACCTL2                      TACCTL2        // Timer A Capture/Compare Control register 2
#define TIMER_A_CCR0                         TACCR0         // Timer A Compare/Capture register 0
#define TIMER_A_CCR1                         TACCR1         // TImer A Compare/Capture register 1
#define TIMER_A_TAIV                         TAIV           // Timer A Interrupt Vector
#define TIMER_A_TAR                          TAR            // Timer A counter register
#define TIMER_A_MC                           MC_2           // Timer A mode control 
#define TIMER_A_TASSEL                       TASSEL_2       // Timer A clock select (SMCLK)
#define TIMER_A_ID                           ID_3           // Timer A clock divide select (8)
#define TIMER_A_TAIE                         TAIE           // Timer A Interrupt Enable

// Timer B
#define TIMER_B_TBCTL                        TBCTL          // Timer B control register
#define TIMER_B_TBCCTL0                      TBCCTL0        // Timer B Capture/Compare Control register 0
#define TIMER_B_TBCCTL1                      TBCCTL1        // Timer B Capture/Compare Control register 1
#define TIMER_B_TBCCTL2                      TBCCTL2        // Timer B Capture/Compare Control register 2
#define TIMER_B_CCR0                         TBCCR0         // Timer B Compare/Capture register 0
#define TIMER_B_CCR1                         TBCCR1         // TImer B Compare/Capture register 1
#define TIMER_B_TBIV                         TBIV           // Timer B Interrupt Vector
#define TIMER_B_TBR                          TBR            // Timer B counter register


#define TIMER_B_MC                           MC_2           // Timer B mode control 
#define TIMER_B_TBSSEL                       TBSSEL_1       // Timer B clock select (ACLK)
#define TIMER_B_ID                           ID_0           // Timer B clock divide select (1)
#define TIMER_B_TBIE                         TBIE           // Timer B Interrupt Enable

#endif 
