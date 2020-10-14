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
// Second version date: 2020-10-14



/*===========================================================================*/

#include "sapi.h"

#define BAUDRATE_USB 19200
#define BAUDRATE_232 19200
#define BUFFER_SIZE   1024


typedef enum {SEEK, READ } States;



int main( void )
{
   float t1 = 0;
   float t2 = 0;
   float t3 = 0;

   float h1 = 0;
   float h2 = 0;
   float h3 = 0;

   int cs = 0;
   int temp_ref_1 = 0;
   int temp_ref_2 = 0;
   int temp_ref_3 = 0;
   int weight_1 = 0;
   int weight_2 = 0;
   int weight_3 = 0;
   int decrement_2 = 0;
   int decrement_1 = 0;
   int decrement_0 = 0;
   int increment_1 = 0;
   int increment_2 = 0;
   int increment_3 = 0;
   int min_speed = 0;
   int scale = 0;
   int delta_eq = 0;
   int sample_interval = 0;

   char buffer[BUFFER_SIZE];
   char lcdOut0[22];
   char lcdOut1[22];
   int idx;


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

   uartWriteString(UART_USB,"Display Adapter V 2.0\n");

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
/*        uartWriteByte( UART_USB, data[0] );
      } else if (0) {*/
         if (ledRS232 == ON) {
            gpioWrite( LED2, ON );
            ledRS232 = OFF;
         } else {
            gpioWrite( LED2, OFF);
            ledRS232 = ON;
         }

         if (data[0] == '{' && state != SEEK ) {
           state = SEEK;
         }

         switch (state) {
           case SEEK:
             if (data[0] == '{') {
               idx = 0;
               buffer[idx] = data[0];
               state = READ;
             } else {
               uartWriteByte( UART_USB, data[0] );
             }
           break;
           case READ:
             if (data[0] == '}') {
               buffer[idx + 1] = '}';
               buffer[idx + 2] = 0x0d;
               buffer[idx + 3] = 0;
               sscanf(buffer,"{ \"T1\" : \"%f\", \"H1\" : \"%f\", \"T2\" : \"%f\", \"H2\" : \"%f\", \"CS\" : \"%d\", \"R1\" : \"%d\", \"R2\" : \"%d\", \"W1\" : \"%d\", \"W2\" : \"%d\", \"D2\" : \"%d\", \"D1\" : \"%d\", \"D0\" : \"%d\", \"I1\" : \"%d\", \"I2\" : \"%d\", \"I3\" : \"%d\", \"MS\" : \"%d\", \"SC\" : \"%d\", \"DE\" : \"%d\", \"SI\" : \"%d\"}", &t1, &h1, &t2, &h2, &cs, &temp_ref_1, &temp_ref_2, &weight_1, &weight_2, &decrement_2, &decrement_1, &decrement_0, &increment_1, &increment_2, &increment_3, &min_speed, &scale, &delta_eq, &sample_interval);

               sprintf(buffer,"{ \"CS\" : \"%d\", \"T1\" : \"%.2f\", \"H1\" : \"%.2f\", \"T2\" : \"%.2f\", \"H2\" : \"%.2f\", \"T3\" : \"%.2f\", \"H3\" : \"%.2f\", \"R1\" : \"%d\", \"R2\" : \"%d\", \"R3\" : \"%d\", \"W1\" : \"%d\", \"W2\" : \"%d\", \"W3\" : \"%d\", \"D2\" : \"%d\", \"D1\" : \"%d\", \"D0\" : \"%d\", \"I1\" : \"%d\", \"I2\" : \"%d\", \"I3\" : \"%d\", \"MS\" : \"%d\", \"SC\" : \"%d\", \"DE\" : \"%d\", \"SI\" : \"%d\"}", cs, t1, h1, t2, h2, t3, h3, temp_ref_1, temp_ref_2, temp_ref_3, weight_1, weight_2,weight_3, decrement_2, decrement_1, decrement_0, increment_1, increment_2, increment_3, min_speed, scale, delta_eq, sample_interval);

               uartWriteString( UART_USB, buffer);

               sprintf(lcdOut0, "%d %.1f %.1f\0", cs,t1,t2);
               sprintf(lcdOut1, "    %d   %d\0", temp_ref_1, temp_ref_2);

               lcdClearAndHome();
               lcdSendStringRaw(lcdOut0);
               lcdGoToXY( 0, 1 );
               lcdSendStringRaw(lcdOut1);

               state = SEEK;
             } else {
               ++idx;
               buffer[idx] = data[0];
             }
           break;
         }
      }
   }
   return 0;
}
