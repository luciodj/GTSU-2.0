/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    uart_to_tcp_bridge.c

  Summary:
    Module for Microchip TCP/IP Stack
     -Transmits all incoming TCP bytes on a socket out the UART
      module, all incoming UART bytes out of the TCP socket.
     -Reference: None (hopefully AN833 in the future)

  Description:
    UART <-> TCP Bridge Example

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#define __UART2TCPBRIDGE_C_

#include "tcpip/tcpip.h"

// TCP listen port and UART baud rate settings.  Due to a current C18
// preprocessor bug (tested on compiler 3.30), the BAUD_RATE definition needs
// to be placed above the #if defined(STACK_USE_UART2TCP_BRIDGE) test.
// Otherwise, it won't compile when STACK_USE_UART2TCP_BRIDGE is UNdefined.
#define UART2TCPBRIDGE_PORT 9761
#define BAUD_RATE           19200

#if defined(STACK_USE_UART2TCP_BRIDGE)

// Comment this define out if we are the server.
// Insert the appropriate address if we are the client.
//#define USE_REMOTE_TCP_SERVER "192.168.2.50"

// Ring buffers for transfering data to and from the UART ISR:
//  - (Head pointer == Tail pointer) is defined as an empty FIFO
//  - (Head pointer == Tail pointer - 1), accounting for wraparound,
//    is defined as a completely full FIFO.  As a result, the max data
//    in a FIFO is the buffer size - 1.
static uint8_t vUARTRXFIFO[65];
static uint8_t vUARTTXFIFO[17];
static volatile uint8_t *RXHeadPtr = vUARTRXFIFO, *RXTailPtr = vUARTRXFIFO;
static volatile uint8_t *TXHeadPtr = vUARTTXFIFO, *TXTailPtr = vUARTTXFIFO;


/*********************************************************************
 * Function:        void UART2TCPBridgeInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the UART peripheral for this application
 *
 * Note:            Uses interrupts
 ********************************************************************/
void UART2TCPBridgeInit(void)
{
    // Initilize UART
#if defined(__XC8)
    TXSTA = 0x20;
    RCSTA = 0x90;

    #define CLOSEST_SPBRG_VALUE ((GetPeripheralClock()+2ul*BAUD_RATE)/BAUD_RATE/4-1)
    #define BAUD_ACTUAL (GetPeripheralClock()/(CLOSEST_SPBRG_VALUE+1))
    #if (BAUD_ACTUAL > BAUD_RATE)
        #define BAUD_ERROR (BAUD_ACTUAL-BAUD_RATE)
    #else
        #define BAUD_ERROR (BAUD_RATE-BAUD_ACTUAL)
    #endif
    #define BAUD_ERROR_PRECENT  ((BAUD_ERROR*100+BAUD_RATE/2)/BAUD_RATE)
    #if BAUD_ERROR_PRECENT > 2
        // Use high speed (Fosc/4) 16-bit baud rate generator
        BAUDCONbits.BRG16 = 1;
        TXSTAbits.BRGH = 1;
        SPBRGH = ((GetPeripheralClock()+BAUD_RATE/2)/BAUD_RATE-1)>>8 & 0xFF;
        SPBRG = ((GetPeripheralClock()+BAUD_RATE/2)/BAUD_RATE-1) & 0xFF;
    #else
        // See if we can use the high baud (Fosc/16) 8-bit rate setting
        #if ((GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1) <= 255
            SPBRG = (GetPeripheralClock()+2*BAUD_RATE)/BAUD_RATE/4 - 1;
            TXSTAbits.BRGH = 1;
        #else   // Use the low baud rate 8-bit setting
            SPBRG = (GetPeripheralClock()+8*BAUD_RATE)/BAUD_RATE/16 - 1;
        #endif
    #endif

    // Use high priority interrupt
    IPR1bits.TXIP = 1;

#else
    UARTTX_TRIS = 0;
    UARTRX_TRIS = 1;
    UMODE = 0x8000;         // Set UARTEN.  Note: this must be done before setting UTXEN

    #if defined(__XC16)
        USTA = 0x0400;      // UTXEN set
        #define CLOSEST_UBRG_VALUE ((GetPeripheralClock()+8ul*BAUD_RATE)/16/BAUD_RATE-1)
        #define BAUD_ACTUAL (GetPeripheralClock()/16/(CLOSEST_UBRG_VALUE+1))
    #else   //defined(__XC32)
        IPC8bits.U2IP = 6;      // Priority level 6
        USTA = 0x00001400;      // RXEN set, TXEN set
        #define CLOSEST_UBRG_VALUE ((GetPeripheralClock()+8ul*BAUD_RATE)/16/BAUD_RATE-1)
        #define BAUD_ACTUAL (GetPeripheralClock()/16/(CLOSEST_UBRG_VALUE+1))
    #endif

    #define BAUD_ERROR ((BAUD_ACTUAL > BAUD_RATE) ? BAUD_ACTUAL-BAUD_RATE : BAUD_RATE-BAUD_ACTUAL)
    #define BAUD_ERROR_PRECENT  ((BAUD_ERROR*100+BAUD_RATE/2)/BAUD_RATE)
    #if (BAUD_ERROR_PRECENT > 3)
        #warning UART frequency error is worse than 3%
    #elif (BAUD_ERROR_PRECENT > 2)
        #warning UART frequency error is worse than 2%
    #endif

    UBRG = CLOSEST_UBRG_VALUE;
#endif

}


/*********************************************************************
 * Function:        void UART2TCPBridgeTask(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void UART2TCPBridgeTask(void)
{
    static enum _BridgeState
    {
        SM_HOME = 0,
        SM_SOCKET_OBTAINED
    } BridgeState = SM_HOME;
    static TCP_SOCKET MySocket = INVALID_SOCKET;
    uint16_t wMaxPut, wMaxGet, w;
    uint8_t *RXHeadPtrShadow, *RXTailPtrShadow;
    uint8_t *TXHeadPtrShadow, *TXTailPtrShadow;


    switch(BridgeState)
    {
        case SM_HOME:
            #if defined(USE_REMOTE_TCP_SERVER)
                // Connect a socket to the remote TCP server
                MySocket = TCPOpen((uint32_t)USE_REMOTE_TCP_SERVER, TCP_OPEN_ROM_HOST, UART2TCPBRIDGE_PORT, TCP_PURPOSE_UART_2_TCP_BRIDGE);
            #else
                MySocket = TCPOpen(0, TCP_OPEN_SERVER, UART2TCPBRIDGE_PORT, TCP_PURPOSE_UART_2_TCP_BRIDGE);
            #endif

            // Abort operation if no TCP socket of type TCP_PURPOSE_UART_2_TCP_BRIDGE is available
            // If this ever happens, you need to go add one to tcpip_config.h
            if(MySocket == INVALID_SOCKET)
                break;

            // Eat the first TCPWasReset() response so we don't
            // infinitely create and reset/destroy client mode sockets
            TCPWasReset(MySocket);

            // We have a socket now, advance to the next state
            BridgeState = SM_SOCKET_OBTAINED;
            break;

        case SM_SOCKET_OBTAINED:
            // Reset all buffers if the connection was lost
            if(TCPWasReset(MySocket))
            {
                // Optionally discard anything in the UART FIFOs
                //RXHeadPtr = vUARTRXFIFO;
                //RXTailPtr = vUARTRXFIFO;
                //TXHeadPtr = vUARTTXFIFO;
                //TXTailPtr = vUARTTXFIFO;

                // If we were a client socket, close the socket and attempt to reconnect
                #if defined(USE_REMOTE_TCP_SERVER)
                    TCPDisconnect(MySocket);
                    MySocket = INVALID_SOCKET;
                    BridgeState = SM_HOME;
                    break;
                #endif
            }

            // Don't do anything if nobody is connected to us
            if(!TCPIsConnected(MySocket))
                break;


            // Make sure to clear UART errors so they don't block all future operations
            #if defined(__XC8)
            if(RCSTAbits.OERR)
            {
                RCSTAbits.CREN = 0;
                RCSTAbits.CREN = 1;
                LED1_IO ^= 1;
            }
            if(RCSTAbits.FERR)
            {
                uint8_t dummy = RCREG;
                LED2_IO ^= 1;
            }
            #else
            if(U2STAbits.OERR)
                U2STAbits.OERR = 0;
            #endif


            // Read FIFO pointers into a local shadow copy.  Some pointers are volatile
            // (modified in the ISR), so we must do this safely by disabling interrupts
            RXTailPtrShadow = (uint8_t*)RXTailPtr;
            TXHeadPtrShadow = (uint8_t*)TXHeadPtr;
            #if defined(__XC8)
            PIE1bits.RCIE = 0;
            PIE1bits.TXIE = 0;
            #else
            IEC1bits.U2RXIE = 0;
            IEC1bits.U2TXIE = 0;
            #endif
            RXHeadPtrShadow = (uint8_t*)RXHeadPtr;
            TXTailPtrShadow = (uint8_t*)TXTailPtr;
            #if defined(__XC8)
            PIE1bits.RCIE = 1;
            if(TXHeadPtrShadow != TXTailPtrShadow)
                PIE1bits.TXIE = 1;
            #else
            IEC1bits.U2RXIE = 1;
            if(TXHeadPtrShadow != TXTailPtrShadow)
                IEC1bits.U2TXIE = 1;
            #endif

            //
            // Transmit pending data that has been placed into the UART RX FIFO (in the ISR)
            //
            wMaxPut = TCPIsPutReady(MySocket);  // Get TCP TX FIFO space
            wMaxGet = RXHeadPtrShadow - RXTailPtrShadow;    // Get UART RX FIFO byte count
            if(RXHeadPtrShadow < RXTailPtrShadow)
                wMaxGet += sizeof(vUARTRXFIFO);
            if(wMaxPut > wMaxGet)               // Calculate the lesser of the two
                wMaxPut = wMaxGet;
            if(wMaxPut)                         // See if we can transfer anything
            {
                // Transfer the data over.  Note that a two part put
                // may be needed if the data spans the vUARTRXFIFO
                // end to start address.
                w = vUARTRXFIFO + sizeof(vUARTRXFIFO) - RXTailPtrShadow;
                if(wMaxPut >= w)
                {
                    TCPPutArray(MySocket, RXTailPtrShadow, w);
                    RXTailPtrShadow = vUARTRXFIFO;
                    wMaxPut -= w;
                }
                TCPPutArray(MySocket, RXTailPtrShadow, wMaxPut);
                RXTailPtrShadow += wMaxPut;

                // No flush.  The stack will automatically flush and do
                // transmit coallescing to minimize the number of TCP
                // packets that get sent.  If you explicitly call TCPFlush()
                // here, latency will go down, but so will max throughput
                // and bandwidth efficiency.
            }

            //
            // Transfer received TCP data into the UART TX FIFO for future transmission (in the ISR)
            //
            wMaxGet = TCPIsGetReady(MySocket);  // Get TCP RX FIFO byte count
            wMaxPut = TXTailPtrShadow - TXHeadPtrShadow - 1;// Get UART TX FIFO free space
            if(TXHeadPtrShadow >= TXTailPtrShadow)
                wMaxPut += sizeof(vUARTTXFIFO);
            if(wMaxPut > wMaxGet)               // Calculate the lesser of the two
                wMaxPut = wMaxGet;
            if(wMaxPut)                         // See if we can transfer anything
            {
                // Transfer the data over.  Note that a two part put
                // may be needed if the data spans the vUARTTXFIFO
                // end to start address.
                w = vUARTTXFIFO + sizeof(vUARTTXFIFO) - TXHeadPtrShadow;
                if(wMaxPut >= w)
                {
                    TCPGetArray(MySocket, TXHeadPtrShadow, w);
                    TXHeadPtrShadow = vUARTTXFIFO;
                    wMaxPut -= w;
                }
                TCPGetArray(MySocket, TXHeadPtrShadow, wMaxPut);
                TXHeadPtrShadow += wMaxPut;
            }

            // Write local shadowed FIFO pointers into the volatile FIFO pointers.
            #if defined(__XC8)
            PIE1bits.RCIE = 0;
            PIE1bits.TXIE = 0;
            #else
            IEC1bits.U2RXIE = 0;
            IEC1bits.U2TXIE = 0;
            #endif
            RXTailPtr = (volatile uint8_t*)RXTailPtrShadow;
            TXHeadPtr = (volatile uint8_t*)TXHeadPtrShadow;
            #if defined(__XC8)
            PIE1bits.RCIE = 1;
            if(TXHeadPtrShadow != TXTailPtrShadow)
                PIE1bits.TXIE = 1;
            #else
            IEC1bits.U2RXIE = 1;
            if(TXHeadPtrShadow != TXTailPtrShadow)
                IEC1bits.U2TXIE = 1;
            #endif

            break;
    }
}


#if defined(__XC8)
/*********************************************************************
 * Function:        void UART2TCPBridgeISR(void)
 *
 * PreCondition:    UART interrupt has occured
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            This function is supposed to be called in the ISR
 *                  context.
 ********************************************************************/
void UART2TCPBridgeISR(void)
{
    // NOTE: All local variables used here should be declared static
    static uint8_t i;

    // Store a received byte, if pending, if possible
    if(PIR1bits.RCIF)
    {
        // Get the byte
        i = RCREG;

        // Clear the interrupt flag so we don't keep entering this ISR
        PIR1bits.RCIF = 0;

        // Copy the byte into the local FIFO, if it won't cause an overflow
        if(RXHeadPtr != RXTailPtr - 1)
        {
            if((RXHeadPtr != vUARTRXFIFO + sizeof(vUARTRXFIFO)) || (RXTailPtr != vUARTRXFIFO))
            {
                *RXHeadPtr++ = i;
                if(RXHeadPtr >= vUARTRXFIFO + sizeof(vUARTRXFIFO))
                    RXHeadPtr = (volatile uint8_t*)vUARTRXFIFO;
            }
        }
    }

    // Transmit a byte, if pending, if possible
    if(PIR1bits.TXIF)
    {
        if(TXHeadPtr != TXTailPtr)
        {
            TXREG = *TXTailPtr++;
            if(TXTailPtr >= vUARTTXFIFO + sizeof(vUARTTXFIFO))
                TXTailPtr = (volatile uint8_t*)vUARTTXFIFO;
        }
        else    // Disable the TX interrupt if we are done so that we don't keep entering this ISR
        {
            PIE1bits.TXIE = 0;
        }
    }
}

#else
/*********************************************************************
 * Function:        void _ISR _U2RXInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies bytes to and from the local UART TX and
 *                  RX FIFOs
 *
 * Note:            None
 ********************************************************************/
#if __C30_VERSION__ >= 300
void _ISR __attribute__((__no_auto_psv__)) _U2RXInterrupt(void)
#elif defined(__XC16)
void _ISR _U2RXInterrupt(void)
#else
void _U2RXInterrupt(void)
#endif
{
    uint8_t i;

    // Store a received byte, if pending, if possible
    // Get the byte
    i = U2RXREG;

    // Clear the interrupt flag so we don't keep entering this ISR
    IFS1bits.U2RXIF = 0;

    // Copy the byte into the local FIFO, if it won't cause an overflow
    if(RXHeadPtr != RXTailPtr - 1)
    {
        if((RXHeadPtr != vUARTRXFIFO + sizeof(vUARTRXFIFO)) || (RXTailPtr != vUARTRXFIFO))
        {
            *RXHeadPtr++ = i;
            if(RXHeadPtr >= vUARTRXFIFO + sizeof(vUARTRXFIFO))
                RXHeadPtr = vUARTRXFIFO;
        }
    }
}
/*********************************************************************
 * Function:        void _ISR _U2TXInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies bytes to and from the local UART TX and
 *                  RX FIFOs
 *
 * Note:            None
 ********************************************************************/
#if __C30_VERSION__ >= 300
void _ISR __attribute__((__no_auto_psv__)) _U2TXInterrupt(void)
#elif defined(__XC16)
void _ISR _U2TXInterrupt(void)
#else
void _U2TXInterrupt(void)
#endif
{
    // Transmit a byte, if pending, if possible
    if(TXHeadPtr != TXTailPtr)
    {
        // Clear the TX interrupt flag before transmitting again
        IFS1bits.U2TXIF = 0;

        U2TXREG = *TXTailPtr++;
        if(TXTailPtr >= vUARTTXFIFO + sizeof(vUARTTXFIFO))
            TXTailPtr = vUARTTXFIFO;
    }
    else    // Disable the TX interrupt if we are done so that we don't keep entering this ISR
    {
        IEC1bits.U2TXIE = 0;
    }
}


#if defined(__XC32)
void __attribute((interrupt(ipl6), vector(_UART2_VECTOR), nomips16)) U2Interrupt(void)
{
    if(IFS1bits.U2RXIF)
        _U2RXInterrupt();
    if(IEC1bits.U2TXIE)
    {
        if(IFS1bits.U2TXIF)
            _U2TXInterrupt();
    }
}
#endif

#endif  // end of ISRs

#endif  //#if defined(STACK_USE_UART2TCP_BRIDGE)
