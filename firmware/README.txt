For compilation you need:
* source env.sh
* compilate all libraries (on lib directory, see manual for each)
* make

For working you need fix CLOCK from atmega328p,
by default FUSE include CKDIV8 (div CLK to 8),
default CLOCK is 1 MHz.

For normal work you need unprogramm this bit:
* make fuse_atmega328P

It up clock to 8 MHz.

It needed for:
* correct decoding IR signals (NEC Procotol),
* for fast work this UART (38400 for 8 MHz)

