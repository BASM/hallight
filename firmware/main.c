/*
 * main.c  - Main of firmware
 * Creation Date: 2012-05-20
 *
 * Copyright (C) 2012 Leonid Myravjev (asm@asm.pp.ru)
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


#include <stdio.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>

#include <uart.h>

#define TERM_I2C_PREF 0x48 /* 1001 000 */
#define AMB_PREF 0x38 /* 1001 000 */


typedef unsigned char uchar;

int twi_init(){
    power_twi_enable();
    TWBR=0xFF;
    TWSR=3;
    return 0;
}

int TwiWaitInt()
{
  uint32_t i=0;
  while (!(TWCR & (1<<TWINT)))
  {
    if(i++>=0xFFFF){
      TWCR = (1<<TWINT) | (1<<TWEN);
      if( (TWSR&0xF8) == 0 ) //BUS ERROR
        TWCR=0; //RESET
      return 1;
    }
    _delay_us(1);
  }
  return 0;
}

int TwiRequestWrite(uchar dev, uchar addr)
{
  //1. START
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  //2. Wait
  if(TwiWaitInt())         return 1;

  //3. Check TWI Status 
  if( (TWSR&0xF8) != 0x10) return 2;
 
  //3. Load SLA_W
  TWDR = dev<<1;
  TWCR = (1<<TWINT) | (1<<TWEN);
  
  //4. Wait (inc. ACK)
  if(TwiWaitInt())         return 3;

  //5. Check TWI Status
  if( (TWSR&0xF8) != 0x18) return 4;
  
  //5. Load ADDR
  TWDR = addr;
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
  TwiWaitInt();//)         return 5; 
  //if(TwiWaitInt())         return 5; 
  
  //6. Wait (inc. ACK)
  //if( (TWSR&0xF8) != 0x28) return 6;
  
 // FIXME need stop?
  // START
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
  if(TwiWaitInt())         return 7; 
  TWDR = (dev << 1); //Write bit
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt())         return 8; 
  //*/
  return 0;
}
int TwiRequestRead(uchar dev, uchar addr)
{
  //1. START
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  //2. Wait
  if(TwiWaitInt()){
    printf("No start\n");
    return 10;
  }
  //3. Check TWI Status 
  if( (TWSR&0xF8) != 0x10) return 1;
 
  //3. Load SLA_W
  TWDR = dev<<1;
  TWCR = (1<<TWINT) | (1<<TWEN);
  
  //4. Wait (inc. ACK)
  if(TwiWaitInt()) return 10;

  //5. Check TWI Status
  if( (TWSR&0xF8) != 0x18) return 2;
  
  //5. Load ADDR
  TWDR = addr;
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt()){
    printf("Addres on dev don't get\n");
    return 10; 
  }
  //6. Wait (inc. ACK)
  if( (TWSR&0xF8) != 0x28) return 3;
  
 // FIXME need stop?
  // START
  TWCR = (1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
  if(TwiWaitInt()) return 11; 
  TWDR = (dev << 1) | 1;
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt()) return 12; 
  //*/
  return 0;
}



uchar TwiRead()
{
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
   if(TwiWaitInt()) return 10;
   return TWDR;
}

uchar TwiReadStop()
{
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt()) return 10; 
  //TWCR = (1<<TWINT)|(1<<TWSTO)| (1<<TWEN);
  return TWDR;
}

uchar TwiWrite(unsigned char byte)
{
  TWDR = byte;
  //TWCR = (1<<TWINT) | (1<<TWEN);
  TWCR = (1<<TWINT) | (1<<TWEN);
   if(TwiWaitInt()) return 10;
   return 0;
}

uchar TwiWriteStop(unsigned char byte)
{
  TWDR = byte;
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
  if(TwiWaitInt())
    return 0;
  //TWCR = (1<<TWINT)|(1<<TWSTO)| (1<<TWEN);
  return 0;
}

int BH1772GLC_init(uchar addr)
{
  int res;
  if(res=TwiRequestWrite(addr, 0x40)){
    printf("Err to write 0x40: %i\r\n", res);
    return -1;
  }
  TwiWriteStop(2);
  return 0;
}

int BH1772GLC_initcheck(uchar addr)
{
      //0x38
  int res,a,b,err;
  (void)err;
  if(res=TwiRequestRead(addr, 0x40)){
    printf("Error to read term: %i\r\n", res);
    return -1;
  }
  res=TwiReadStop();
  return res;
}

int BH1772GLC_get_shot(uchar addr)
{
      //0x38
  int res,a,b,err;
  (void)err;
  if(TwiRequestWrite(addr, 0x44)){
    printf("Error to read term :-(\n");
    return -1;
  }
  TwiWriteStop(2);

  _delay_ms(1000);

  if(TwiRequestRead(addr, 0x4C)){
    printf("Error to read term :-(\n");
    return -1;
  }

  a=TwiRead();
  b=TwiReadStop();
  res=(a<<8)|b;

  return res;
}

int main(void)
{
    int res;
    wdt_disable();
    cli();

    DDRD=0xFE;
    uart_init();
    uart_stdio();

    twi_init();

    int i=0;
    for(;;){
      int term=0,sens=0;
      //int amb = _get_shot(TERM_I2C_PREF);
      //term = MAX6634_get_shot(TERM_I2C_PREF);
//      res = BH1772GLC_init(AMB_PREF);
//      printf("Init result: %i\r\n", res);
//      res = BH1772GLC_initcheck(AMB_PREF);
//      printf("Init check: %i\r\n", res);
      sens = BH1772GLC_get_shot(AMB_PREF);
      /* main event loop */
      printf("Hello world: %i, res: %i, sens: %i\r\n", i++, term, sens);
      //printf("Hello world: %i, res: %i\r\n", i++);

    }

  return res;
}

