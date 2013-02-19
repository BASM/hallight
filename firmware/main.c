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

#define DEBUG
#ifdef DEBUG
#include <stdio.h>
#else
#define printf(...) {}
#endif

#include <inttypes.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/twi.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>

#include <twi.h>
#include <uart.h>

#define TERM_I2C_PREF 0x48 /* 1001 000 */
#define AMB_PREF 0x38 /* 1001 000 */

int BH1772GLC_init(uint8_t addr)
{
  int err=0;
  if((err=twi_req_write(addr, 0x40))){
    printf("RW on: %i\r\n", err);
    return -1;
  }
  twi_p_write(2);
  twi_p_stop();
  return 0;
}

int BH1772GLC_initcheck(uint8_t addr)
{
      //0x38
  int res,err;
  (void)err;
  if(twi_req_read(addr, 0x40)){
    printf("Error to read term: %i\r\n", twi_err);
    return -1;
  }
  res=twi_p_read(LAST);
  twi_p_stop();
  //if(res<0) error
  return res;
}

int BH1772GLC_get_shot(uint8_t addr)
{
      //0x38
  int res,a,b,err;
  if(err=twi_req_write(addr, 0x44)){
    printf("RW on: %i\r\n", err);
    return 0;
  }
  twi_p_write(2);
  twi_p_stop();

  if(twi_req_read(addr, 0x4C)){
    printf("Error to read term :-(\r\n");
    return 0;
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

    int BH1772_init=0;

    int i=0;
    for(;;){
      int sens;

      if(BH1772_init == 0){
        res = BH1772GLC_init(AMB_PREF);
        if(res==0){
          res = BH1772GLC_initcheck(AMB_PREF);
        }
        if(res==2) { 
          printf("Module BH1772 init success\r\n");
          BH1772_init=1;
        }
      }

      if(BH1772GLC_init) {
        sens = BH1772GLC_get_shot(AMB_PREF);
        if(sens == 0) { //dark or error
          int res = BH1772GLC_initcheck(AMB_PREF);
          if(res!=2) { //not init, maybe error, or power reset.
            BH1772_init=0;
          }
        }else{
          printf("Current ALS is: %i (%x)\r\n", sens, sens);
        }
      }
    }

  return res;
}

