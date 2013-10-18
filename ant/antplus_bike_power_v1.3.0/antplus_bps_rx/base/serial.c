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

#include "serial.h"
#include "config.h"
#include "timer.h"
#include "antmessage.h"


#if defined(SERIAL_UART_ASYNC) || defined(USE_UART_DEBUG)
#define BAUD_57600_INDEX   1
#define BAUD_50000_INDEX   0


typedef struct 
{  
      UCHAR ucBR0;
      UCHAR ucBR1;
      UCHAR ucMCTL;

} UART_BAUD_CONTROL;


// Register settings for Baud rates. 
// Reduce the size of this array to reduce 
// RAM
static const UART_BAUD_CONTROL asBaudControl[8] =
{   
      //   { 0x06, 0x00, 0x0C },                              // 4800  baud using 32768 Hz clock
      //   { 0xD0, 0x00, 0x00 },                              // 38400 baud using 8 MHz clock
      //   { 0xA0, 0x01, 0x00 },                              // 19200 baud using 8 MHz clock
      { 0xA0, 0x00, 0x00 },                              // 50000 baud using 8 MHz clock
      //   { 0x1B, 0x00, 0x04 },                              // 1200  baud using 32768 Hz clock
      //   { 0x03, 0x00, 0x06 },                              // 9600  baud using 32768 Hz clock
      //   { 0x0D, 0x00, 0x0A },                              // 2400  baud using 32768 Hz clock
      { 0x8A, 0x00, 0x00 }                               // 57600 baud using 8 MHz clock
};
#endif

// Local defines
#define SEN_ASSERT_TIMEOUT             ((UCHAR)3)                                   // wait 30us for SEN to be asserted in response to SMSGRDY assertion                   

#define IS_SEN_ASSERTED()              ((SYNC_SEN_IN & SYNC_SEN_BIT) == 0)          // active low
#define IS_RTS_ASSERTED()              (ASYNC_RTS_IN & ASYNC_RTS_BIT)

#define IS_SMSGRDY_ASSERTED()          ((SYNC_SMSGRDY_OUT & SYNC_SMSGRDY_BIT) == 0) // active low
#define SYNC_SMSGRDY_ASSERT()          (SYNC_SMSGRDY_OUT &= ~SYNC_SMSGRDY_BIT)      // active low
#define SYNC_SMSGRDY_DEASSERT()        (SYNC_SMSGRDY_OUT |= SYNC_SMSGRDY_BIT)

#define SYNC_SCLK_EDGE_DETECT()        (SYNC_SCLK_IFG & SYNC_SCLK_BIT)              // this must be an edge interrupt flag
#define SYNC_SCLK_EDGE_CLEAR()         (SYNC_SCLK_IFG  &= ~SYNC_SCLK_BIT)           // clear the interrupt flag

#define SYNC_SRDY_ASSERT()             (SYNC_SRDY_OUT &= ~SYNC_SRDY_BIT)            // active low
#define SYNC_SRDY_DEASSERT()           (SYNC_SRDY_OUT |= SYNC_SRDY_BIT)

/// this is a delay of at least 2.5us
#define SYNC_SRDY_PULSE()              { SYNC_SRDY_ASSERT();\
      TIMER_SRDY_PULSE_DELAY();\
      TIMER_SRDY_PULSE_DELAY();\
      SYNC_SRDY_DEASSERT(); }

#define ASYNC_SLEEP_ASSERT()           (ASYNC_SLEEP_OUT |= ASYNC_SLEEP_BIT)         // active high
#define ASYNC_SLEEP_DEASSERT()         (ASYNC_SLEEP_OUT &= ~ASYNC_SLEEP_BIT)  


#define SERIAL_QUEUE_BUFFER_LENGTH     ((UCHAR)21)                                  // 15 assumes that an exteneded message is the longest that will be recieved.                                               
#define SERIAL_QUEUE_RX_SIZE           ((UCHAR)4)                                   // Same for TX and RX buffers
#define SERIAL_QUEUE_TX_SIZE           ((UCHAR)2)                                   // Same for TX and RX buffers


// Local structures
typedef struct
{
      UCHAR aucBuffer[SERIAL_QUEUE_BUFFER_LENGTH];  //Stores message data
} SERIAL_RX_MESSAGE;

typedef struct
{
      BOOL bGoToSleep;                                         //Signals whether ANT can go to sleep after this message is sent (TRUE) or not (FALSE)
      UCHAR aucBuffer[SERIAL_QUEUE_BUFFER_LENGTH];  //Stores message data
} SERIAL_TX_MESSAGE;

typedef struct
{
      UCHAR ucMessageCount;
      UCHAR ucHead;
      UCHAR ucTail;
      BOOL bQueueOverflow;
      SERIAL_RX_MESSAGE aucMessages[SERIAL_QUEUE_RX_SIZE];
} SERIAL_QUEUE_RX;

typedef struct
{
      UCHAR ucMessageCount;
      UCHAR ucHead;
      UCHAR ucTail;
      SERIAL_TX_MESSAGE aucMessages[SERIAL_QUEUE_TX_SIZE];
} SERIAL_QUEUE_TX;

// Externed variables
extern volatile USHORT usLowPowerMode;

//Local variables
static SERIAL_QUEUE_RX sqRxMessages = {0, 0, 0, 0, {{0}}};
static SERIAL_QUEUE_TX sqTxMessages = {0, 0, 0, {0, {0}}};
static UCHAR aucRxMessageBuffer[SERIAL_QUEUE_BUFFER_LENGTH];

// Local Functions
#if defined(SERIAL_BYTE_SYNCHRONOUS) || defined(SERIAL_BIT_SYNCHRONOUS)

volatile BOOL bSCLK_Detected = FALSE;    // Flag used to indicate if a SCLK edge has been detected
#if defined(SERIAL_BYTE_SYNCHRONOUS)
static BOOL bTheGotByte;
static UCHAR ucTheRxByte;
#endif

static UCHAR Synchronous_ReadByte(void);
static void Synchronous_WriteByte(UCHAR ucByte);

static void Synchronous_Init();
static void Synchronous_Transaction();

#elif defined(SERIAL_UART_ASYNC)
typedef enum
{
   UART_STATE_WFSYNC,         // Wait for sync
   UART_STATE_WFSIZE,         // Wait for size
   UART_STATE_BODY,           // Reading in the body of the message
   UART_STATE_WFCHKSUM        // Wait for checksum
} UartRxState;

static void Asynchronous_Transaction();
static void Asynchronous_ProcessByte(UCHAR ucByte_);
#endif // SERIAL_UART_ASYNC

#if defined(SERIAL_UART_ASYNC) || defined(USE_UART_DEBUG)
static void Asynchronous_Init(UCHAR ucBaudRate_);
#endif // SERIAL_UART_ASYNC || USE_UART_DEBUG



////////////////////////////////////////////////////////////////////////////
// Serial_Init
//
// Initiates serial driver, queues and state machine.
//  
// Returns TRUE if successful.
//
////////////////////////////////////////////////////////////////////////////
BOOL Serial_Init()
{

   // Synchronous MUST be initialized before asynchronous (if doing sync to ANT and
   // asynch for printf. This is because the synch init changes some gpio's to an output
   // on startup to configure synch mode on ANT. This off course messes up what the gpio's
   // must be for asynch, hence the asynch init must be called after the synch init.
#if defined(SERIAL_BYTE_SYNCHRONOUS) || defined(SERIAL_BIT_SYNCHRONOUS)
   Synchronous_Init();
#endif
#if defined(SERIAL_UART_ASYNC) || defined(USE_UART_DEBUG)
   Asynchronous_Init(BAUD_50000_INDEX);
#endif   

   sqTxMessages.ucHead = 0;                     //Reset the Tx queue
   sqTxMessages.ucTail = 0;
   sqTxMessages.ucMessageCount = 0;


   sqRxMessages.ucHead = 0;                     //Reset the Rx queue
   sqRxMessages.ucTail = 0;
   sqRxMessages.ucMessageCount = 0;
   sqRxMessages.bQueueOverflow = FALSE;

   return(TRUE);   
}

/////////////////////////////////////////////////////////////////////////////
// SyncSerial_SyncReset
//
// This function performs the synchronous reset sequence for the ANT chip
//
// \return Boolean value indicating weather or not the ANT chip was successfully reset
/////////////////////////////////////////////////////////////////////////////
BOOL Serial_SyncReset(void)
{   
   SYNC_SMSGRDY_DEASSERT();
   // Add delay to wait for ANT module to finish power up and get ready for incoming messages
   // The current system setup results in a delay of ~1ms; a minimum of 0.5ms 
   // are required for ANT to properly reset.
   Timer_DelayTime(100);                               // 1 ms

   // perform the Synchronous serial reset as described in Interfacing with
   // ANT General Purpose Chipsets and Modules document

   // Assert SRDY (SRDY = 0)
   SYNC_SRDY_ASSERT();

   // make sure there is a 250us delay between SRDY falling edge and SMSGRDY falling edge
   Timer_DelayTime(30);                                // 300 us delay

   // Assert SMSGRDY (SMSGRDY = 0)
   SYNC_SMSGRDY_ASSERT();

   // Wait for ANT to lower SEN to indicate it is ready
   Timer_DelayTime(50000);                               // wait 500 ms

   sqTxMessages.ucHead = 0;                     //Reset the Tx queue
   sqTxMessages.ucTail = 0;
   sqTxMessages.ucMessageCount = 0;


   sqRxMessages.ucHead = 0;                     //Reset the Rx queue
   sqRxMessages.ucTail = 0;
   sqRxMessages.ucMessageCount = 0;
   sqRxMessages.bQueueOverflow = FALSE;

   // raise SRDY (SRDY = 1)
   SYNC_SRDY_DEASSERT();

   return IS_SEN_ASSERTED();
}



////////////////////////////////////////////////////////////////////////////
// Serial_Transaction
//
// Send, recieve data. Data will be sent if there is data to send and recieved 
// if there is room in the queue to recieve it.
//
// Returns TRUE if tx transaction initiated.
// 
////////////////////////////////////////////////////////////////////////////
void Serial_Transaction()
{
#if defined(SERIAL_BYTE_SYNCHRONOUS) || defined(SERIAL_BIT_SYNCHRONOUS)
   Synchronous_Transaction();
#elif defined(SERIAL_UART_ASYNC)
   Asynchronous_Transaction();
#endif  
   return;                                            // return whether we received a message or not
}



////////////////////////////////////////////////////////////////////////////
// Serial_Get_Tx_Buffer
//
// Try to get a free buffer from the transmit buffer.
//
// Return pointer to buffer if successful, NULL otherwise.
// 
////////////////////////////////////////////////////////////////////////////
UCHAR* Serial_Get_Tx_Buffer()
{
   if(sqTxMessages.ucMessageCount < SERIAL_QUEUE_TX_SIZE)
   {
      return(sqTxMessages.aucMessages[sqTxMessages.ucHead].aucBuffer);
   }
   return(NULL);

}

////////////////////////////////////////////////////////////////////////////
// Serial_Put_Tx_Buffer
//
// Puts the currently held buffer to queue
// 
////////////////////////////////////////////////////////////////////////////
void Serial_Put_Tx_Buffer(BOOL bGoToSleep_)
{
   sqTxMessages.aucMessages[sqTxMessages.ucHead].bGoToSleep = bGoToSleep_;
   sqTxMessages.ucMessageCount++;
   sqTxMessages.ucHead = ((sqTxMessages.ucHead + 1) & (SERIAL_QUEUE_TX_SIZE-1));
}


////////////////////////////////////////////////////////////////////////////
// Serial_Get_Rx_Buffer
//
// Try to get a free buffer from the transmit buffer.
//
// Return pointer to buffer if successful, NULL otherwise.
// 
////////////////////////////////////////////////////////////////////////////
UCHAR* Serial_Get_Rx_Buffer()
{
   if(sqRxMessages.ucMessageCount < SERIAL_QUEUE_RX_SIZE)
   {
      return(sqRxMessages.aucMessages[sqRxMessages.ucHead].aucBuffer);
   }
   return(NULL);

}

////////////////////////////////////////////////////////////////////////////
// Serial_Put_Rx_Buffer
//
// Puts the currently held buffer to queue
// 
////////////////////////////////////////////////////////////////////////////
void Serial_Put_Rx_Buffer()
{
   sqRxMessages.ucMessageCount++;
   sqRxMessages.ucHead = ((sqRxMessages.ucHead + 1) & (SERIAL_QUEUE_RX_SIZE-1));
}



////////////////////////////////////////////////////////////////////////////
// Serial_Read_Rx_Buffer
//
// Returns a pointer to any data in the QUEUE 
// 
////////////////////////////////////////////////////////////////////////////
UCHAR* Serial_Read_Rx_Buffer()
{

   if(sqRxMessages.ucMessageCount > 0)
   {
      UCHAR i;
      //Copy the message content over to a buffer that we can return
      for(i = 0; i < SERIAL_QUEUE_BUFFER_LENGTH; i++)
         aucRxMessageBuffer[i] = sqRxMessages.aucMessages[sqRxMessages.ucTail].aucBuffer[i];
      //Release the message buffer so it can be used to store a new message
      sqRxMessages.ucTail = ((sqRxMessages.ucTail + 1) & (SERIAL_QUEUE_RX_SIZE-1));
      sqRxMessages.bQueueOverflow = FALSE;  //Set overflow flag
      sqRxMessages.ucMessageCount--;
      return aucRxMessageBuffer;
   }
   return(NULL);

}

////////////////////////////////////////////////////////////////////////////
// Serial_Scan_Rx_Top
//
// Copies message data from the QUEUE to a buffer. Returns buffer pointer.
//
////////////////////////////////////////////////////////////////////////////
UCHAR* Serial_Scan_Rx_Top()
{
   if(sqRxMessages.ucMessageCount > 0)
   {
      UCHAR ucQueueTop = (sqRxMessages.ucHead - 1) & (SERIAL_QUEUE_RX_SIZE-1);
      //Copy the message content over to a buffer that we can return
      memcpy(aucRxMessageBuffer, sqRxMessages.aucMessages[ucQueueTop].aucBuffer, SERIAL_QUEUE_BUFFER_LENGTH);
      return aucRxMessageBuffer;
   }
   return(NULL);
}

////////////////////////////////////////////////////////////////////////////
// Serial_Read_Rx_Top
//
// Copies message data from the QUEUE to a buffer returns buffer pointer.
// Frees up the message slot in the queue after the data has been copied out.
// 
////////////////////////////////////////////////////////////////////////////
UCHAR* Serial_Read_Rx_Top()
{
   if(sqRxMessages.ucMessageCount > 0)
   {
      UCHAR ucQueueTop = (sqRxMessages.ucHead - 1) & (SERIAL_QUEUE_RX_SIZE-1);
      //Copy the message content over to a buffer that we can return
      memcpy(aucRxMessageBuffer, sqRxMessages.aucMessages[ucQueueTop].aucBuffer, SERIAL_QUEUE_BUFFER_LENGTH);
      //Release the message buffer so it can be used to store a new message
      sqRxMessages.ucHead = (sqRxMessages.ucHead- 1) & (SERIAL_QUEUE_RX_SIZE-1);
      sqRxMessages.bQueueOverflow = FALSE;  //Set overflow flag
      sqRxMessages.ucMessageCount--;
      return aucRxMessageBuffer;
   }
   return(NULL);
}


////////////////////////////////////////////////////////////////////////////
// Serial_Flush_Tx
//
// Flush the TX queue
// 
////////////////////////////////////////////////////////////////////////////
void Serial_Flush_Tx()
{

   sqTxMessages.ucHead = 0;
   sqTxMessages.ucTail = 0;
   sqTxMessages.ucMessageCount = 0;
}

////////////////////////////////////////////////////////////////////////////
// Serial_Flush_Rx
//
// Flush the RX queue
// 
////////////////////////////////////////////////////////////////////////////
void Serial_Flush_Rx()
{
   sqRxMessages.ucHead = 0;
   sqRxMessages.ucTail = 0;
   sqRxMessages.ucMessageCount = 0;
   sqRxMessages.bQueueOverflow = FALSE;
}

////////////////////////////////////////////////////////////////////////////
// Serial_Rx_Buffer_Overflow
//
// Checks whether a Rx buffer overflow has recently occurred.
//
// Returns TRUE if overflow has occurred.
//
////////////////////////////////////////////////////////////////////////////
BOOL Serial_Rx_Buffer_Overflow()
{
   return sqRxMessages.bQueueOverflow;
}

#if defined(SERIAL_BYTE_SYNCHRONOUS) || defined(SERIAL_BIT_SYNCHRONOUS)
////////////////////////////////////////////////////////////////////////////
// Synchronous_Init
//
// Initialize the synchronous serial interface.
// - put ANT module into synch serial mode
// - reset module (RST pin if > AT3)
// - configure SPI and required gpio's
// - do synchronous reset
// 
////////////////////////////////////////////////////////////////////////////
void Synchronous_Init()
{
#if defined (PIN_RESET)
   // Go int RESET
   SYSTEM_RST_SEL &= ~SYSTEM_RST_BIT;           // Set as output
   SYSTEM_RST_DIR |= SYSTEM_RST_BIT;            // Set as output
   SYSTEM_RST_OUT &= ~SYSTEM_RST_BIT;           // Set low to reset module 
#endif

   // Set configuration pins
   SERIAL_IOSEL_SEL &= ~SERIAL_IOSEL_BIT;       // Set as output
   SERIAL_IOSEL_DIR |= SERIAL_IOSEL_BIT;        // Set as output
   SERIAL_IOSEL_OUT |= SERIAL_IOSEL_BIT;        // Serial, not SensRcore   

   SERIAL_PORTSEL_SEL &= ~SERIAL_PORTSEL_BIT;   // Set as output
   SERIAL_PORTSEL_DIR |= SERIAL_PORTSEL_BIT;    // Set as output
   SERIAL_PORTSEL_OUT |= SERIAL_PORTSEL_BIT;    // Port select HI for SYNC serial

   SERIAL_SFLOW_SEL &= ~SERIAL_SFLOW_BIT;       // Set as output
   SERIAL_SFLOW_DIR |= SERIAL_SFLOW_BIT;        // Set as output

#if defined(SERIAL_BIT_SYNCHRONOUS)
   SERIAL_SFLOW_OUT |= SERIAL_SFLOW_BIT;        // Select bit flow control
#else // SERIAL_BIT_SYNCHRONOUS
   SERIAL_SFLOW_OUT &= ~SERIAL_SFLOW_BIT;       // Select byte flow control
#endif // SERIAL_BYTE_SYNCHRONOUS      

   //
   // Set up serial pins
   //

#if defined(SERIAL_BIT_SYNCHRONOUS)
   SYNC_SIN_SEL &= ~SYNC_SIN_BIT;               // Enable SIN as GPIO
   SYNC_SOUT_SEL &= ~SYNC_SOUT_BIT;             // Enable SOUT as GPIO
   SYNC_SCLK_SEL &= ~SYNC_SCLK_BIT;             // Enable SCLK as GPIO
   SYNC_SCLK_IES &= ~SYNC_SCLK_BIT;             // interrupt on the low-to-high transition
   SYNC_SCLK_IFG &= ~SYNC_SCLK_BIT;             // clear the interrupt flag		
   SYNC_SCLK_IE |= SYNC_SCLK_BIT;               // Enable SCLK intterupt

#elif defined (SERIAL_BYTE_SYNCHRONOUS)
   // UCI Register Init
   BYTE_SYNC_UCI_CTL0 = UCCKPL | UCSYNC;        // Set UCI to synchronous with clock normally high           
   SYNC_SIN_SEL |= SYNC_SIN_BIT;                // Enable SIN to use UCI B0 SIMO pin
   SYNC_SOUT_SEL |= SYNC_SOUT_BIT;              // Enable SOUT to use UCI B0 SOMI pin
   SYNC_SCLK_SEL |= SYNC_SCLK_BIT;              // Enable the UCSCLK functionality

#endif 
   //   // SEN
   SYNC_SEN_DIR &= ~SYNC_SEN_BIT;               // set the direction to be an input

   // SMSGRDY
   SYNC_SMSGRDY_DIR |= SYNC_SMSGRDY_BIT;        // set SMSGRDY as an output
   SYNC_SMSGRDY_DEASSERT();                     // make sure it is deasserted so that it doesn't misguide ANT

   // SIN
   SYNC_SIN_DIR &= ~SYNC_SIN_BIT;               // set SIN as an input

   // SCLK
   SYNC_SCLK_DIR &= ~SYNC_SCLK_BIT;             // set SCLK as an input

   // SOUT
   SYNC_SOUT_DIR |= SYNC_SOUT_BIT;              // Set SOUT as an output

   // SRDY
   SYNC_SRDY_DIR |= SYNC_SRDY_BIT;              // set SRDY as an output
   SYNC_SRDY_DEASSERT();                        // put SRDY in a known state

#if defined (PIN_RESET)
   SYSTEM_RST_OUT |= SYSTEM_RST_BIT;            // ** Take ANT out of RESET **
   Timer_DelayTime(50000);                      // Delay for 500ms
#endif

#if defined (SYNCH_RESET)
   while(!Serial_SyncReset());                  // Do a synchronous reset
   SYNC_SMSGRDY_DEASSERT();                     // make sure it is deasserted so that it doesn't misguide ANT
#endif

#if defined(SERIAL_BYTE_SYNCHRONOUS) 
   BYTE_SYNC_UCI_CTL1 &= ~UCSWRST;              // Enable SPI
#endif     

   // Enable SEN interrupt
   SYNC_SEN_IES |= SYNC_SEN_BIT;                // interrupt on high-to-low transition
   SYNC_SEN_IFG &= ~SYNC_SEN_BIT;               // reset the interrupt flag
   SYNC_SEN_IE  |= SYNC_SEN_BIT;                // enable the SEN interrupt to allow wake up from sleep                                       

#if defined(SERIAL_BYTE_SYNCHRONOUS)   
   // Enable RECV interrupt
   BYTE_SYNC_UCI_IFR &= ~BYTE_SYNC_RX_INT;      // Enable RX interrupt only
   BYTE_SYNC_UCI_IER |= BYTE_SYNC_RX_INT;       // Enable RX interrupt only

   bTheGotByte = FALSE;
#endif

}


////////////////////////////////////////////////////////////////////////////
// Synchronous_Transaction
//
// If there is a message in the serial queue, send it.
// If ANT has a message to send to us, put it the recieve queue.
// 
////////////////////////////////////////////////////////////////////////////
void Synchronous_Transaction()
{
   UCHAR ucCheckSum;
   UCHAR ucMesgLen;
   UCHAR ucIndex;
   UCHAR ucByte;
   UCHAR* pucTxBuffer;  

   if (sqTxMessages.ucMessageCount > 0)    // if we have a message to send
   {  
      pucTxBuffer = sqTxMessages.aucMessages[sqTxMessages.ucTail].aucBuffer;
      SYNC_SMSGRDY_ASSERT();                          // notify the ANT device that the Host wishes to send a message

      // the following retry loop is a workaround to avoid a possible loss of the MSGRDY assertion edge with the  nRF24AP1
      {
         UCHAR ucSenTimeout = SEN_ASSERT_TIMEOUT;

         do                                              
         {
            if (!(ucSenTimeout--))                    // check the retry timeout
            {
               ucSenTimeout = SEN_ASSERT_TIMEOUT;     // Reset the timeout for the SEN response 

               SYNC_SMSGRDY_DEASSERT();               // Pulse MSGRDY again if SEN has not been asserted by the timeout
               Timer_DelayTime(1);                    // 10us
               SYNC_SMSGRDY_ASSERT();            
            }

            Timer_DelayTime(1);                       // wait 10us   
         }
         while (!IS_SEN_ASSERTED());                  // check if SEN is asserted
      }
   }            

   while (IS_SMSGRDY_ASSERTED() || IS_SEN_ASSERTED()) // while we have a message to send or a message to receive
   {   
      while (!IS_SEN_ASSERTED());                     // make sure SEN is asserted

      // Read the sync Byte 
      ucByte     = Synchronous_ReadByte();            // read the sync byte
      ucCheckSum = ucByte;                            // initialize the checksum

      if ((ucByte == MESG_RX_SYNC) && (sqTxMessages.ucMessageCount> 0))                     // ANT is ready to receive our message and we have a message to send
      {
         SYNC_SMSGRDY_DEASSERT();                     // clear the message ready signal

         ucMesgLen = pucTxBuffer[0] + MESG_SAVED_FRAME_SIZE; // read the message length of the message to send and adjust for frame size
         ucIndex   = 0;

         do
         {
            Synchronous_WriteByte(pucTxBuffer[ucIndex]);       // write out message byte
            ucCheckSum ^= pucTxBuffer[ucIndex];                // update the checksum

         } while (++ucIndex < ucMesgLen);                      // loop through message

         Synchronous_WriteByte(ucCheckSum);                    // write out the checksum byte

         while (IS_SEN_ASSERTED());                   // Wait until SEN goes HIGH
         sqTxMessages.ucTail = (sqTxMessages.ucTail + 1) & (SERIAL_QUEUE_TX_SIZE-1);    // Update queue
         sqTxMessages.ucMessageCount--;
      }
      else if (ucByte == MESG_TX_SYNC)                // ANT wants to send us a message
      {
         UCHAR* pucRxBuffer = NULL;

         // Check if there is room to receive message
         if(sqRxMessages.ucMessageCount < SERIAL_QUEUE_RX_SIZE)
         {
            pucRxBuffer = sqRxMessages.aucMessages[sqRxMessages.ucHead].aucBuffer;

            // Read the second byte which is the length of the message 
            ucByte    = Synchronous_ReadByte();
            ucMesgLen = ucByte + MESG_SAVED_FRAME_SIZE;  // save the message length and correct for the frame size
            ucIndex   = 0;

            do
            {
               pucRxBuffer[ucIndex] = ucByte;            // save it in the receive buffer            
               ucCheckSum ^= ucByte;                     // update the checksum
               ucByte      = Synchronous_ReadByte();     // read out the next byte

            } while (++ucIndex < ucMesgLen);           

            if (ucCheckSum == ucByte)                    // validate the checksum of the message
            {
               sqRxMessages.ucHead = ((sqRxMessages.ucHead + 1) & (SERIAL_QUEUE_RX_SIZE-1));
               sqRxMessages.ucMessageCount++;
            }
         }
         else //Buffer overflow
         {
            sqRxMessages.bQueueOverflow = TRUE;  //Set overflow flag
            // Read in the ANT message and discard it so as not to interrupt the serial line states
            // Read the second byte which is the length of the message
            ucByte    = Synchronous_ReadByte();
            ucMesgLen = ucByte + MESG_SAVED_FRAME_SIZE;  // save the message length and correct for the frame size
            ucIndex   = 0;

            do
            {
               ucByte      = Synchronous_ReadByte();     // read out the next byte

            } while (++ucIndex < ucMesgLen);
         }
      }
   } 
}

////////////////////////////////////////////////////////////////////////////
// Synchronous_ReadByte
//
// Read byte from ANT
// 
////////////////////////////////////////////////////////////////////////////
#if defined(SERIAL_BYTE_SYNCHRONOUS)
UCHAR Synchronous_ReadByte(void)
{
   bTheGotByte = FALSE;          // Clear the interrupt status flag

   // Pulse to SRDY to show that we are ready for another byte
   SYNC_SRDY_PULSE();
   while (!bTheGotByte);
   bTheGotByte = FALSE;          // Clear the interrupt status flag

   return (ucTheRxByte);
}

#elif defined(SERIAL_BIT_SYNCHRONOUS)
UCHAR Synchronous_ReadByte(void)
{
   UCHAR ucByte    = 0;
   UCHAR ucBitMask = 0x01;
   bSCLK_Detected = FALSE;                            // Clear the interrupt status flag

   do
   {
      // Pulse to SRDY to show that we are ready for another bit
      SYNC_SRDY_PULSE();

      // Wait for the SCLK rising edge
      while (!bSCLK_Detected);
      bSCLK_Detected = FALSE;                         // Clear the interrupt status flag

      // Read the bit
      if (SYNC_SIN_IN & SYNC_SIN_BIT)
         ucByte |= ucBitMask;

      ucBitMask <<= 1;

   } while (ucBitMask);                               // Read the entire byte

   return (ucByte);
}
#endif // SERIAL_BIT_SYNCHRONOUS

////////////////////////////////////////////////////////////////////////////
// Synchronous_WriteByte
//
// Write byte to ANT
// 
////////////////////////////////////////////////////////////////////////////
#if defined(SERIAL_BYTE_SYNCHRONOUS)
void Synchronous_WriteByte(UCHAR ucByte)
{   
   // Wait until tx flag is enabled.
   while(!(BYTE_SYNC_UCI_IFR & BYTE_SYNC_TX_INT));

   // Pulse SRDY to indicate that the bit is ready for reading for ANT
   BYTE_SYNC_UCI_TXBUF = ucByte;
   SYNC_SRDY_PULSE();   
}

#elif defined(SERIAL_BIT_SYNCHRONOUS)
void Synchronous_WriteByte(UCHAR ucByte)
{
   UCHAR ucBitMask = 0x01;
   bSCLK_Detected = FALSE;                            // Clear the interrupt status flag

   do
   {   
      // Write the bit
      if (ucByte & ucBitMask)
         SYNC_SOUT_OUT |= SYNC_SOUT_BIT;
      else
         SYNC_SOUT_OUT &= ~SYNC_SOUT_BIT;

      // Pulse SRDY to indicate that the bit is ready for reading for ANT
      SYNC_SRDY_PULSE();

      // Wait for the SCLK rising edge that will indicate that the bit has been read
      while (!bSCLK_Detected);
      bSCLK_Detected = FALSE;                         // Clear the interrupt status flag
      ucBitMask <<= 1;                                // select the next bit

   } while (ucBitMask);                               // Write the entire byte
}
#endif // SERIAL_BIT_SYNCHRONOUS

#endif // SERIAL_BYTE_SYNCHRONOUS || SERIAL_BIT_SYNCHRONOUS


////////////////////////////////////////////////////////////////////////////
// Asynchronous_Init
//
// Initialize the asynchronous serial interface.
// - assumes that ANT is in ASYNCH mode using HW configuration of required pins
// - configure UART and required gpio's
// 
////////////////////////////////////////////////////////////////////////////
#if defined(SERIAL_UART_ASYNC) || defined(USE_UART_DEBUG)
void Asynchronous_Init(UCHAR ucBaudRate_)
{
#if defined (PIN_RESET)
   // Go int RESET
   SYSTEM_RST_SEL &= ~SYSTEM_RST_BIT;           // Set as output
   SYSTEM_RST_DIR |= SYSTEM_RST_BIT;            // Set as output
   SYSTEM_RST_OUT &= ~SYSTEM_RST_BIT;           // Set low to reset module
#endif
   // Set configuration pin  

   ASYNC_RTS_SEL &= ~ASYNC_RTS_BIT;                // Set as gpio
   ASYNC_RTS_DIR &= ~ASYNC_RTS_BIT;                // Set as input

   ASYNC_SUSPEND_SEL &= ~ASYNC_SUSPEND_BIT;        // Set as gpio
   ASYNC_SUSPEND_DIR |= ASYNC_SUSPEND_BIT;         // Set as output
   ASYNC_SUSPEND_OUT |= ASYNC_SUSPEND_BIT;         // Set high  

   ASYNC_SLEEP_SEL &= ~ASYNC_SLEEP_BIT;            // Set as output
   ASYNC_SLEEP_DIR |= ASYNC_SLEEP_BIT;             // Set as output
   ASYNC_SLEEP_OUT |= ASYNC_SLEEP_BIT;            // Set high

#if defined (SUSPEND_MODE_RESET)
   Timer_DelayTime(10);
   ASYNC_SLEEP_OUT |= ASYNC_SLEEP_BIT;              // Set high
   Timer_DelayTime(5);
   ASYNC_SUSPEND_OUT &= ~ASYNC_SUSPEND_BIT;         // Set low to enter SUSPEND mode
   Timer_DelayTime(25);
#endif


   UART_ASYNC_UCI_CTL1 = UCSSEL_2 | UCSWRST;       // Disable UART and select ACLK source
   UART_ASYNC_UCI_CTL0 = 0;                        // Configure UART no parity, LSB first, 8-bit, one stop bit

   UART_ASYNC_UCI_BR0 = asBaudControl[ucBaudRate_].ucBR0;    // Baud rate
   UART_ASYNC_UCI_BR1 = asBaudControl[ucBaudRate_].ucBR1;
   UART_ASYNC_UCI_MCTL = asBaudControl[ucBaudRate_].ucMCTL;

   ASYNC_RX_SEL |= ASYNC_RX_BIT;                   // Enable pin as UART RX
   ASYNC_RX_DIR &= ~ASYNC_RX_BIT;                  // Enable pin as input  

   ASYNC_TX_SEL |= ASYNC_TX_BIT;                   // Enable pin as UART TX
   ASYNC_TX_DIR |= ASYNC_TX_BIT;                   // Enable pin as output

   ASYNC_RTS_SEL &= ~ASYNC_RTS_BIT;                // Enable pin as GPIO
   ASYNC_RTS_DIR &= ~ASYNC_RTS_BIT;                // Enable pin as input

   UART_ASYNC_UCI_CTL1 &= ~UCSWRST;                // Enable UART
   UART_ASYNC_UCI_IFR &= ~UART_ASYNC_RX_INT;       // Clear the RX interrupt
   UART_ASYNC_UCI_IER |= UART_ASYNC_RX_INT;        // Enable the RX interrupt
#if defined (PIN_RESET)
   SYSTEM_RST_OUT |= SYSTEM_RST_BIT;               // ** Take ANT out of RESET **
   Timer_DelayTime(50000);                         // Delay for 500ms
#elif defined (SUSPEND_MODE_RESET)
   ASYNC_SUSPEND_OUT |= ASYNC_SUSPEND_BIT;         // Set high
   Timer_DelayTime(25);
   ASYNC_SLEEP_OUT &= ~ASYNC_SLEEP_BIT;            // Set low to exit SUSPEND mode and finish the reset
   while(IS_RTS_ASSERTED());                       // Wait until ANT wakes up out of SUSPEND
   ASYNC_SLEEP_OUT |= ASYNC_SLEEP_BIT;             // Set high to allow ANT to sleep until we need it
   while(!IS_RTS_ASSERTED());                      // Wait until ANT goes to SLEEP
#endif
}
#endif // SERIAL_UART_ASYNC || USE_UART_DEBUG

////////////////////////////////////////////////////////////////////////////
// Asynchronous_WriteByte
//
// Write byte to ANT
// 
////////////////////////////////////////////////////////////////////////////
int Asynchronous_WriteByte(int iByte)
{

   // Wait until tx flag is enabled.
   while(!(UART_ASYNC_UCI_IFR & UART_ASYNC_TX_INT));

   // Pulse SRDY to indicate that the bit is ready for reading for ANT
   UART_ASYNC_UCI_TXBUF = iByte;

   return 1;
}


#if defined(SERIAL_UART_ASYNC)
////////////////////////////////////////////////////////////////////////////
// Asynchronous_Transaction
//
// If there is a message in the serial queue, send it.
// 
////////////////////////////////////////////////////////////////////////////
void Asynchronous_Transaction()
{
   if(sqTxMessages.ucMessageCount > 0)  // if we have a message to send
   {
      ASYNC_SLEEP_DEASSERT();       // keep ANT from sleeping

      //Wait until RTS drops
      while(IS_RTS_ASSERTED());

      UCHAR ucCheckSum = MESG_TX_SYNC;
      UCHAR* pucTxBuffer;
      UCHAR ucMesgLen;
      UCHAR ucIndex;

      pucTxBuffer = sqTxMessages.aucMessages[sqTxMessages.ucTail].aucBuffer;

      Asynchronous_WriteByte(MESG_TX_SYNC);

      ucMesgLen = pucTxBuffer[0] + MESG_SAVED_FRAME_SIZE;                           // read the message length of the message to send and adjust for frame size
      ucIndex   = 0;

      do
      {
         Asynchronous_WriteByte(pucTxBuffer[ucIndex]);                              // write out message byte
         ucCheckSum ^= pucTxBuffer[ucIndex];                                        // update the checksum

      } while (++ucIndex < ucMesgLen);                                              // loop through message


      Asynchronous_WriteByte(ucCheckSum);                                           // write out the checksum byte

      //Wait until RTS is high to signal that ANT has begun toggling RTS to show that it has begun processing the message
      while(!IS_RTS_ASSERTED());

      //Wait until RTS drops
      while(IS_RTS_ASSERTED());

      // Wait until tx flag is enabled.
      while(!(UART_ASYNC_UCI_IFR & UART_ASYNC_TX_INT));

      //Check if we can allow ANT to go to sleep after having sent this message
      if(sqTxMessages.aucMessages[sqTxMessages.ucTail].bGoToSleep)
         ASYNC_SLEEP_ASSERT();      // let ANT sleep

      sqTxMessages.ucMessageCount--;
      sqTxMessages.ucTail = (sqTxMessages.ucTail + 1) & (SERIAL_QUEUE_TX_SIZE-1);   // Update queue
   }
}


////////////////////////////////////////////////////////////////////////////
// Asynchronous_WriteBuffer
//
// Writes a UCHAR buffer of specified length to the UART. The UART TX process
// is not interrupt driven.
// 
////////////////////////////////////////////////////////////////////////////
void Asynchronous_WriteBuffer(char* pcBuffer_, int iLength_)
{
   int i;
   for (i = 0; i < iLength_; i++)
   {
      Asynchronous_WriteByte(pcBuffer_[i]);
   }
}


////////////////////////////////////////////////////////////////////////////
// Asynchronous_ProcessByte
//
// Process bytes recieve in the UART interrupt. Once a complete and valid
// ANT message is recieved, it is put into the RX serial queue.
// 
////////////////////////////////////////////////////////////////////////////
void Asynchronous_ProcessByte(UCHAR ucByte_)
{   
   static UCHAR ucCheckSum = 0;
   static UartRxState eTheState = UART_STATE_WFSYNC;
   static UCHAR ucTheLength = 0;
   static UCHAR ucIndex = 0;
   static UCHAR* pucTheBuffer = NULL;

   switch(eTheState)
   {
      case UART_STATE_WFSYNC:                         // Wiating for SYNC
      {
         if(ucByte_ == MESG_TX_SYNC)                  // Got SYNC
         {
            eTheState = UART_STATE_WFSIZE;            // Move to wiating for size
            ucCheckSum = MESG_TX_SYNC; 
            ucIndex = 0;         
         }
         break;
      }     
      case UART_STATE_WFSIZE:                         // Wiating for size
      {

         if(ucByte_ < SERIAL_QUEUE_BUFFER_LENGTH)     // Make sure size makes sense
         {
            pucTheBuffer = Serial_Get_Rx_Buffer();    // Try to get a buffer from the RX queue

            if(pucTheBuffer)
            {
               pucTheBuffer[ucIndex++] = ucByte_;
               ucCheckSum ^= ucByte_; 
               ucTheLength = ucByte_;                 // Move to waiting for body
               eTheState = UART_STATE_BODY;
            }
            else
            {
               eTheState = UART_STATE_WFSYNC;         // If no buffer in queue, drop this message and reset
               sqRxMessages.bQueueOverflow = TRUE;  //Set overflow flag
            }

         }
         else
         {
            eTheState = UART_STATE_WFSYNC;            // If the size is weird drop message and reset
         }

         break;
      }     
      case UART_STATE_BODY:                           // Waiting for body bytes
      {
         if(pucTheBuffer)
         {
            pucTheBuffer[ucIndex++] = ucByte_;
            ucCheckSum ^= ucByte_; 

            if(ucTheLength == 0)                      // Body will be collected up to checksum
            {
               eTheState = UART_STATE_WFCHKSUM;       // Move to waiting for checksum
            }
            else
            {         
               ucTheLength--;
            }      
         }
         break;
      }       
      case UART_STATE_WFCHKSUM:                       // Waiting for checksum byte
      {

         if( (ucByte_ == ucCheckSum) && !sqRxMessages.bQueueOverflow)                   // If the checksum is good
            Serial_Put_Rx_Buffer();                   // put buffer back in queue

         // Reset the state machine
         ucCheckSum = 0;
         eTheState = UART_STATE_WFSYNC;
         ucTheLength = 0;
         ucIndex = 0;
         pucTheBuffer = NULL;

         break;
      }


      default:                                        // Shouldn't happen
      {
         eTheState = UART_STATE_WFSYNC;
         break;
      }
   }
}
#endif   // SERIAL_UART_ASYNC


//------------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
/// INT_PORT2
///
/// This function handles all port 2 interrupts, these include:
/// -SCLK rising edge
/// -SEN falling edge 
///
/////////////////////////////////////////////////////////////////////////////
#pragma vector=PORT2_VECTOR
__interrupt void INT_PORT2(void)
{
   while (P2IFG)
   {

#if defined(SERIAL_BIT_SYNCHRONOUS)
      if (SYNC_SCLK_IFG & SYNC_SCLK_BIT)
      {
         SYNC_SCLK_IFG &= ~SYNC_SCLK_BIT; 
         bSCLK_Detected = TRUE;
      }  
      else
#endif
         if (SYNC_SEN_IFG & SYNC_SEN_BIT)
         {
            //////////////////////////////////////////////////////////////////////////////////////////////////
            // when interrupt exits, microprocessor must come out of sleep and run main, to process ANT events
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if (SYNC_SEN_IFG & SYNC_SEN_BIT)
            {
               //////////////////////////////////////////////////////////////////////////////////////////////////
               // when interrupt exits, microprocessor must come out of sleep and run main, to process ANT events
               //////////////////////////////////////////////////////////////////////////////////////////////////
               usLowPowerMode = 0;                                          // Prevent the Main loop from going to sleep if it was just about too (prevents a race condition)
               SYNC_SEN_IFG &= ~SYNC_SEN_BIT;
               __bic_SR_register_on_exit(CPUOFF | OSCOFF | SCG0 | SCG1);    // Cause MCU to wake up when interrupt exits
            }
         }        
         else
         {
            P2IFG = 0;                                                     // Clear all other port 1 interrupts.
         }
   }   
}



////////////////////////////////////////////////////////////////////////////
// INT_UCIAB0RX
//
// This function handles all UCIAB0 RX interrupts, these include:
//
// SPI RX - Recieve interrupt for SPI byte synchronous mode.
//
////////////////////////////////////////////////////////////////////////////
#pragma vector=USCIAB0RX_VECTOR
__interrupt void INT_UCIAB0RX(void)
{

   usLowPowerMode = 0;

#if defined (SERIAL_BYTE_SYNCHRONOUS)
   // Prevent the Main loop from going to sleep if it was just about too (prevents a race condition)
   bTheGotByte = TRUE;
   ucTheRxByte = BYTE_SYNC_UCI_RXBUF;
   BYTE_SYNC_UCI_IFR &= ~BYTE_SYNC_RX_INT;

#elif defined(SERIAL_UART_ASYNC)   
   Asynchronous_ProcessByte(UART_ASYNC_UCI_RXBUF);
#endif        

   __bic_SR_register_on_exit(CPUOFF | OSCOFF | SCG0 | SCG1);      // Cause MCU to wake up when interrupt exits
   return;
}

////////////////////////////////////////////////////////////////////////////
// INT_UCIAB0TX
//
// This function handles all UCIAB0 TX interrupts, these include:
//
// SPI TX - Transmit interrupt for SPI byte synchronous mode.
//
////////////////////////////////////////////////////////////////////////////
//#pragma vector=USCIAB0TX_VECTOR
//__interrupt void INT_UCIAB0TX(void)
//{
//
//   
//
//   if(eTheSerialState == STATE_SERIAL_TRANSMIT)   
//   {
//   
//      IOBOARD_LED0_OUT &= ~IOBOARD_LED0_BIT;    // Port select HI for SYNC serial
//   
//      // Check if writing from data bytes
//      if(stTheTxQueue.ucCurrentByte < (stTheTxQueue.astBuffer[stTheTxQueue.ucTail].aucBuffer[0]+2))
//      {
//      
//         stTheTxQueue.ucCheckSum ^= stTheTxQueue.astBuffer[stTheTxQueue.ucTail].aucBuffer[stTheTxQueue.ucCurrentByte];
//         BYTE_SYNC_UCI_TXBUF = stTheTxQueue.astBuffer[stTheTxQueue.ucTail].aucBuffer[stTheTxQueue.ucCurrentByte++];   // Wtite out the first byte the transmit register
//      }
//      else
//      { 
//         IOBOARD_LED3_OUT &= ~IOBOARD_LED3_BIT;    // Port select HI for SYNC serial
//      
//         usLowPowerMode = 0;                             // Prevent the Main loop from going to sleep if it was just about too (prevents a race condition)                                                 
//         BYTE_SYNC_UCI_TXBUF = stTheTxQueue.ucCheckSum;  // Write out the checksum
//         eTheSerialState = STATE_SERIAL_IDLE;            // Return to idle state
//         BYTE_SYNC_UCI_IER &= ~BYTE_SYNC_TX_INT;         // Disable TX interrupt
//         stTheTxQueue.ucTail++;                          // Release serial message buffer
//         __bic_SR_register_on_exit(CPUOFF | OSCOFF | SCG0 | SCG1); // Cause MCU to wake up when interrupt exits
//      }
//
//      SYNC_SRDY_PULSE();                                 // Pulse SRDY to initiate transfer of next byte
//   }
//   BYTE_SYNC_UCI_IFR &= ~BYTE_SYNC_TX_INT;               // Clear RX interrupt
//   
//   return;
//}


