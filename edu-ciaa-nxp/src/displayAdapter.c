/*
   Copyright 2020, Carlos Pantelides.
   All rights reserved.


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

// First version date: 2020-07-12

/*

  FSM

  seek    #    seekT1              lcdClearAndHome();
  seekT1  :    readT1

  readT1  ,    seekH1 (print ' ')  lcdSendStringRaw(space);
  readT1  *    (print it)          lcdSendStringRaw(data);

  seekH1  :    readH1

  readH1  ,    seekT2               lcdGoToXY( 0, 1 );
  readH1  *    (print it)           lcdSendStringRaw(data);

  seekT2  :    readT2

  readT2  ,    seekH2 (print ' ')   lcdSendStringRaw(space);
  readT2  *    (print it)           lcdSendStringRaw(data);

  seekH2  :    readH2

  readH2  ,    seekCS (print ' ')   lcdSendStringRaw(space);
  readH2  *    (print it)           lcdSendStringRaw(data);

  seekCS  :    readCS

  readCS  eol  seek,
  readCS  *    (print it)           lcdSendStringRaw(data);
*/


/*===========================================================================*/

#include "sapi.h"

#define BAUDRATE_USB 19200
#define BAUDRATE_232 19200



typedef enum {SEEK, SEEKT1, READT1, SEEKH1, READH1, SEEKT2, READT2, SEEKH2, READH2, SEEKCS, READCS } States;




int main( void )
{
   // ---------- CONFIGURACIONES ------------------------------

   States state;
   state = SEEK;

   boardConfig();

   uartConfig( UART_USB, BAUDRATE_USB );
   uartConfig( UART_232, BAUDRATE_232 );

   lcdInit( 16, 2, 5, 8 );

   bool ledRS232 = OFF;
   bool ledUSB   = OFF;

   char data[]  = { 0x0,0x0 };
   char space[] = { 0x20,0x0 };
   char dot[]   = { '.',0x0 };

   for ( int i = 0; i<10; ++i) {
     gpioWrite( LED1, ON );
     gpioWrite( LED2, OFF);
     delay(100);
     gpioWrite( LED1, OFF );
     gpioWrite( LED2, ON);
     delay(100);


   }

   uartWriteString(UART_USB,"Display Adapter V 1.0\n");

   lcdCursorSet( LCD_CURSOR_OFF );
   lcdClearAndHome();
   lcdSendStringRaw("Seeking...");


   // ---------- REPETIR POR SIEMPRE --------------------------
   while( TRUE ) {

      if ( uartReadByte( UART_USB, (uint8_t * )&data[0] ) ){
         if ( ledUSB == ON) {
            gpioWrite( LED1, ON );
            ledUSB = OFF;
         } else {
            gpioWrite( LED1, OFF );
            ledUSB = ON;
         }

         // echo
         uartWriteByte( UART_USB, data[0] );
         uartWriteByte( UART_232, data[0] );
     }

      if(  uartReadByte( UART_232, (uint8_t * )&data[0] ) ){
         if (ledRS232 == ON) {
            gpioWrite( LED2, ON );
            ledRS232 = OFF;
         } else {
            gpioWrite( LED2, OFF);
            ledRS232 = ON;
         } 
 
//         lcdSendStringRaw(dot);
         if (data[0] == '#' && state != SEEK ) {
           state = SEEK;
         }

         switch (state) {
           case SEEK:
             if (data[0] == '#') {
               lcdClearAndHome();
               state = SEEKT1;
             }
           break;
           case SEEKT1:
             if (data[0] == ':') {
               state = READT1;
             }
           break;
           case READT1:
             if (data[0] == ',') {
               lcdSendStringRaw(space);
               state = SEEKH1;
             } else {
               lcdSendStringRaw(data);
             }
           break;
           case SEEKH1:
             if (data[0] == ':') {
               state = READH1;
             }
           break;
           case READH1:
             if (data[0] == ',') {
               lcdGoToXY( 0, 1 );
               state = SEEKT2;
             } else {
               lcdSendStringRaw(data);
             }
           break;
           case SEEKT2:
             if (data[0] == ':') {
               state = READT2;
             }
           break;
           case READT2:
             if (data[0] == ',') {
               lcdSendStringRaw(space);
               state = SEEKH2;
             } else {
               lcdSendStringRaw(data);
             }
           break;
           case SEEKH2:
             if (data[0] == ':') {
               state = READH2;
             }
           break;
           case READH2:
             if (data[0] == ',') {
               lcdSendStringRaw(space);
               state = SEEKCS;
             } else {
               lcdSendStringRaw(data);
             }
           break;
           case SEEKCS:
             if (data[0] == ':') {
               state = READCS;
             }
           break;
           case READCS:
             if (data[0] == 0x0d || data[0] == 0x0a ) {
               state = SEEK;
             } else {
               lcdSendStringRaw(data);
             }
           break;
           default:

           break;
         }

         uartWriteByte( UART_USB, data[0] );
      }
   }
   return 0;
}
