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

#include <string.h>

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
#include <support.h>
#include <ir.h>

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

#define EKMC_MAX 0x0Fff;
uint16_t EKMC1601111=0;

int cycle_event()
{

}

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

int time=0;
int ir_time=0;
uint16_t gtime=0;
//ISR_NONBLOCK;
//ISR_BLOCK;

int r_its=100;
int g_its=0;
int b_its=0;

uint8_t leds_status=0;

ISR(TIMER0_OVF_vect) {
  if(ir_time <= (9000*2))
    ir_time+=32;

  gtime++;
  if((gtime%64)==0){
    if(EKMC1601111>0){
      EKMC1601111--;;
    }
    /*
    if(leds_status==0){
      if(r_its>0) r_its--;
      else
        if(g_its>0) g_its--;
        else
          if(b_its>0) b_its--;
    }else{
      if(g_its<0xff) g_its++;
      else
        if(b_its<0xff) b_its++;
        else
          if(r_its<0xff) r_its++;
    }*/
  }
  time++;
  if(time>=256){
    if(r_its!=0)
      SET_R();
    if(g_its!=0)
      SET_G();
    if(b_its!=0)
      SET_B();
    time=0;
  }

  if(time==r_its) CLR_R();
  if(time==g_its) CLR_G();
  if(time==b_its) CLR_B();
}

ir_event ev;
int ftime=0;

ISR(INT0_vect)
{
  ev.time=ir_time;
  ev.stat=(!((PIND>>2)&1));
  ir_time=0;
  //if(ev.stat == 1)
  //printf("%i,%i\n", (int) ev.time, (int) ev.stat);
  ir_set_event(&ev);
}

ISR(INT1_vect)
{
  if((PIND>>3)&3)
    EKMC1601111=EKMC_MAX;
}  //*/

int timer0_init()
{
  //TCCR0A= (1<<COM0A1) | (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
  //TCCR0A= (1<<COM0A1)  | (1<<WGM01) | (1<<WGM00);
  TCCR0A= (1<<WGM01) | (1<<WGM00);
  //TCCR0A= (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
  TCCR0B= (1<<CS00); //1
  //TCCR0B= (1<<CS01);//8
  //TCCR0B= (1<<CS01) | (1<<CS00);//64
  //TCCR0B= (1<<CS02);//128
  //TCCR0B= (1<<CS02) | (1<<CS00); //1024
  //TCCR0B= (1<<CS00);

  TIMSK0 = (1<<TOIE0);

  OCR0A = 0x01;
  OCR0B = 0x50;
  power_timer0_enable();

  DDRBIT(D,6,1);


}

int extint_init()
{
  EICRA |= (1<<ISC10) | (1<<ISC00);
  EICRA |= (1<<ISC10) | (1<<ISC00);
  EIMSK |= (1<<INT0) | (1<<INT1);
}

int ir_update=0;

int ir_dumpresult(char* array, int size){
  static uint16_t code=0;
  int ret=0;
  if(size==0)
    ret=1;
  uint8_t a=array[0];
  uint8_t b=array[1];
  if( ((a == 0x40) && 
      (b == 0xff)) || ret){
    if(!ret)
      code=(uint8_t)array[2]<<8|(uint8_t)array[3];
    ir_update=1;
    switch(code){
      case 0x07f8:
        if(r_its<256)
          r_its++;
        break;
      case 0x45ba:
        if(r_its>0)
          r_its--;
        break;
      case 0x08f7:
        if(g_its<256)
          g_its++;
        break;
      case 0x00ff:
        if(g_its>0)
          g_its--;
        break;
      case 0x09f6:
        if(b_its<256)
          b_its++;
        break;
      case 0x1de2:
        if(b_its>0)
          b_its--;
        break;
    }
  }
  return 0;
}
        

void leds_switchon(){
  leds_status=1;
  //r_its=0xff;
  //g_its=0xff;
  //b_its=0xff;
}
void leds_switchoff(){
  leds_status=0;
  //r_its=0x00;
  //g_its=0x00;
  //b_its=0x00;
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
    //CONFIG INTPUT FOR TSOP
    DDRD&=~(1<<2);//
    PORTD&=~(1<<2);//


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

    uart_init();
    uart_stdio();

    twi_init();
    timer0_init();
    extint_init();

    ir_init();

    int i=0;
    int BH1772_init=0;
    int sens;
    sei();
    printf("===HELLO====\n");
    for(;;){
      ///*
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
      ///////////////////////
      //*
      if(i%2){
        PORTD |= (1<<5);
      }else{
        PORTD &= ~(1<<5);
      }//*/
      
      if(BH1772_init) {
        sens = BH1772GLC_get_shot(AMB_PREF);
        if(sens == 0) { //dark or error
          int res = BH1772GLC_initcheck(AMB_PREF);
          if(res!=2) { //not init, maybe error, or power reset.
            BH1772_init=0;
          }
        }
      }   // */
      /*
      if (leds_status==0){
          if(sens<=20)
            leds_switchon();
      }
      if(sens>=60)
        leds_switchoff();

      if(EKMC1601111==0){
        leds_switchoff();
      }*/
      if(BH1772_init) {
        r_its=sens/10; 
        printf("Current ALS is: %i (%x),EKMS: %i\n", sens, sens, EKMC1601111);
        if(ir_update){
          ir_update=0;
          printf("RGB: %i, %i, %i\n", r_its, g_its, b_its);
        }
      }else{
      //puts("A");
        _delay_ms(1000);
      }
      cycle_event();
      i++;
    }

  return res;
}


