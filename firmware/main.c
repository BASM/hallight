/*
 * main.c  - Main of firmware
 * Creation Date: 2013-05-20
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
    printf("RW on: %i\n", err);
    return -1;
  }
  twi_p_write(2);
  twi_p_stop();
  
  if((err=twi_req_write(addr, 0x5A))){
    printf("RW on: %i\n", err);
    return -1;
  }
  twi_p_write(254);
  twi_p_stop();
  return 0;
}

int BH1772GLC_initcheck(uint8_t addr)
{
      //0x38
  int res,err;
  (void)err;
  if(twi_req_read(addr, 0x40)){
    printf("Error to read term: %i\n", twi_err);
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
    printf("RW on: %i\n", err);
    return 0;
  }
  twi_p_write(2);
  twi_p_stop();

  if(twi_req_read(addr, 0x4C)){
    printf("Error to read term :-(\n");
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

    //CONFIG INPUT FOR EKMC1601111
    DDRD&=~(1<<3);
    //PORTC&=~(1<<0);
    PORTD&=~(1<<3);//


#define PINOUT(port, num) \
    DDR##port |= (1<<num);

#define PSET(port, num) \
    PORT##port |= (1<<num);

#define PCLR(port, num) \
    PORT##port &= ~(1<<num);
    
    DDRD |= (1<<5);

    //R:
    DDRB |= (1<<1);
    //G:
    DDRB |= (1<<3);
    //B:
    DDRD |= (1<<6);

#define SET_R() \
    PORTB |= (1<<1);
#define CLR_R() \
    PORTB &= ~(1<<1);
#define SET_G() \
    PORTB |= (1<<3);
#define CLR_G() \
    PORTB &= ~(1<<3);
#define SET_B() \
    PORTD |= (1<<6);
#define CLR_B() \
    PORTD &= ~(1<<6);

    uart_init();
    uart_stdio();

    twi_init();

    int BH1772_init=0;

    int i=0;
    int EKMC1601111=0;
    int sens;
    for(;;){
      switch(i%5){
        case 0:
          CLR_R();
          CLR_G();
          CLR_B();
          break;
        case 1:
          SET_R();
          CLR_G();
          CLR_B();
          break;
        case 2:
          CLR_R();
          SET_G();
          CLR_B();
          break;
        case 3:
          CLR_R();
          CLR_G();
          SET_B();
          break;
        case 4:
          SET_R();
          SET_G();
          SET_B();
          break;
      }


      if(i%2){
        PORTD |= (1<<5);
      }else{
        PORTD &= ~(1<<5);
      }

      EKMC1601111=(PIND>>3)&3;

      if(BH1772_init == 0){
        res = BH1772GLC_init(AMB_PREF);
        if(res==0){
          res = BH1772GLC_initcheck(AMB_PREF);
        }
        if(res==2) { 
          printf("Module BH1772 init success\n");
          BH1772_init=1;
        }
      }

          //*
      if(BH1772GLC_init) {
        sens = BH1772GLC_get_shot(AMB_PREF);
        if(sens == 0) { //dark or error
          int res = BH1772GLC_initcheck(AMB_PREF);
          if(res!=2) { //not init, maybe error, or power reset.
            BH1772_init=0;
          }
        }else{
        }
      }   // */
      printf("Current ALS is: %i (%x),EKMS: %i\n", sens, sens, EKMC1601111);
      //puts("A");
      //_delay_ms(1000);
      i++;
    }

  return res;
}

