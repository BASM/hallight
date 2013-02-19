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
#include <util/twi.h>

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

#define TWI_ERRVERB
#ifdef TWI_ERRVERB
int twi_err=0;
#define TWI_ERR(a) twi_err=a
#else
#define TWI_ERR()
#endif

int twi_p_start(){
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  if(TwiWaitInt())             {TWI_ERR(1); return 1;}
  if( (TW_STATUS != TW_START) &&
      (TW_STATUS != TW_REP_START) ){
    printf("STAT: %x\n", TW_STATUS);
    TWI_ERR(2); return 2;}
  return 0;
}

int twi_p_restart(){
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
  if(TwiWaitInt())              {TWI_ERR(3); return 3;}
  if(TW_STATUS != TW_REP_START) {TWI_ERR(4); return 4;}
  return 0;
}

/* Type:
 * * TW_READ
 * * TW_WRITE
 */
int twi_p_sla(char dev, int type)
{
  TWDR = (dev<<1) | type;
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt())                 {TWI_ERR(5); return 5;}
  if(type==TW_WRITE) {
    if(TW_STATUS != TW_MT_SLA_ACK) {TWI_ERR(6); return 6;}
  }else{//type==TW_READ
    if(TW_STATUS != TW_MR_SLA_ACK) {TWI_ERR(7); return 7;}
  }
  return 0;
}

int twi_p_write(uint8_t data)
{
  TWDR = data;
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt())                {TWI_ERR(7); return 7;}
  if(TW_STATUS != TW_MT_DATA_ACK) {TWI_ERR(8); return 8;}
  return 0;
}

#define NOLAST 0
#define LAST 1
int twi_p_read(int last)
{
  if(last==NOLAST)
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  else
    TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt())                {TWI_ERR(9);  return -9;}

  if(last==NOLAST){
    if(TW_STATUS != TW_MR_DATA_ACK) {TWI_ERR(10); return -10;}
  }else{
    if(TW_STATUS != TW_MR_DATA_NACK) {TWI_ERR(11); return -11;}
  }

  return TWDR;
}


int twi_p_stop()
{
  TWCR = (1<<TWINT)|(1<<TWSTO)| (1<<TWEN);
  return 0;
}


int TwiRequestWrite(uchar dev, uchar addr)
{
  if(twi_p_start()) return 1;
  if(twi_p_sla(dev, TW_WRITE)) return 2;
  if(twi_p_write(addr)) return 3;
  twi_p_stop();
  _delay_us(100);
  
  if(twi_p_restart()) return 4;
  if(twi_p_sla(dev, TW_WRITE)) return 5;
  if(twi_p_write(addr)) return 6;

  return 0;
}
int TwiRequestRead(uchar dev, uchar addr)
{
  if(twi_p_start()) return 1;
  if(twi_p_sla(dev, TW_WRITE)) return 2;
  if(twi_p_write(addr)) return 3;

  if(twi_p_restart()) return 4;
  if(twi_p_sla(dev, TW_READ)) return 5;
  return 0;
}



uchar TwiReadStop()
{
  TWCR = (1<<TWINT) | (1<<TWEN);
  if(TwiWaitInt()) return 10; 
  //TWCR = (1<<TWINT)|(1<<TWSTO)| (1<<TWEN);
  return TWDR;
}

uchar TwiWriteStop()
{
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
  return 0;
}

int BH1772GLC_init(uchar addr)
{
  int err=0;
  if((err=TwiRequestWrite(addr, 0x40))){
    printf("RW on: %i\r\n", err);
    return -1;
  }
  twi_p_write(2);
  twi_p_stop();
  return 0;
}

int BH1772GLC_initcheck(uchar addr)
{
      //0x38
  int res,err;
  (void)err;
  if(TwiRequestRead(addr, 0x40)){
    printf("Error to read term: %i\r\n", twi_err);
    return -1;
  }
  res=twi_p_read(LAST);
  twi_p_stop();
  //if(res<0) error
  return res;
}

int BH1772GLC_get_shot(uchar addr)
{
      //0x38
  int res,a,b,err;
  if(err=TwiRequestWrite(addr, 0x44)){
    printf("RW on: %i\r\n", err);
    return -1;
  }
  twi_p_write(2);
  twi_p_stop();

  if(TwiRequestRead(addr, 0x4C)){
    printf("Error to read term :-(\n");
    return -1;
  }

  a=twi_p_read(NOLAST);
  b=twi_p_read(LAST);
  res=(b<<8)|a;
  twi_p_stop();

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
      printf("=================\r\n");
      res = BH1772GLC_init(AMB_PREF);
      printf("Init result: %i\r\n", res);
      res = BH1772GLC_initcheck(AMB_PREF);
      printf("Init check: %x (err=%i)\r\n", res, twi_err);
     
      sens = BH1772GLC_get_shot(AMB_PREF);
      printf("Hello world: %i, res: %i, sens: %i (%x)\r\n", i++, term, sens, sens);
      //_delay_ms(5000);
      /* main event loop */
      //printf("Hello world: %i, res: %i\r\n", i++);

    }

  return res;
}

