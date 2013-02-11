EESchema Schematic File Version 2  date Пн. 11 февр. 2013 23:52:02
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:asm
EELAYER 43  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "11 feb 2013"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5400 4750 5400 4950
Wire Wire Line
	5200 4750 5200 4850
Wire Wire Line
	5950 2950 5950 3050
Wire Wire Line
	4150 5300 4150 5450
Wire Wire Line
	4150 4150 4150 4300
Connection ~ 4600 3100
Wire Wire Line
	4150 3750 4150 3100
Wire Wire Line
	4150 3100 4900 3100
Wire Wire Line
	4600 3100 4600 3400
Wire Wire Line
	4900 3600 4900 3500
Wire Wire Line
	4750 3950 4450 3950
Wire Wire Line
	4900 3100 4900 2650
Wire Wire Line
	4600 3900 4600 3950
Connection ~ 4600 3950
Wire Wire Line
	4150 4700 4150 4800
Wire Wire Line
	4950 2750 4900 2750
Connection ~ 4900 2750
Wire Wire Line
	5950 2750 6050 2750
Wire Wire Line
	6050 2750 6050 3300
Wire Wire Line
	6050 3300 5450 3300
Wire Wire Line
	5450 3300 5450 3450
Wire Wire Line
	5600 4750 5600 4850
Wire Wire Line
	5600 4850 5200 4850
Connection ~ 5400 4850
$Comp
L GND #PWR?
U 1 1 51194B5A
P 5400 4950
F 0 "#PWR?" H 5400 4950 30  0001 C CNN
F 1 "GND" H 5400 4880 30  0001 C CNN
	1    5400 4950
	1    0    0    -1  
$EndComp
Text Label 5700 2850 0    60   ~ 0
warning see datasheet!
Text Label 5500 3450 0    60   ~ 0
3.3v
$Comp
L GND #PWR?
U 1 1 511949EB
P 5950 3050
F 0 "#PWR?" H 5950 3050 30  0001 C CNN
F 1 "GND" H 5950 2980 30  0001 C CNN
	1    5950 3050
	1    0    0    -1  
$EndComp
$Comp
L IRU1015 U?
U 1 1 00000000
P 5450 2800
F 0 "U?" H 5350 2750 60  0000 C CNN
F 1 "IRU1015" H 5450 2950 60  0000 C CNN
	1    5450 2800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 51193DBA
P 4150 5450
F 0 "#PWR?" H 4150 5450 30  0001 C CNN
F 1 "GND" H 4150 5380 30  0001 C CNN
	1    4150 5450
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 51193DAE
P 4150 5050
F 0 "R?" V 4230 5050 50  0000 C CNN
F 1 "R68" V 4150 5050 50  0000 C CNN
	1    4150 5050
	1    0    0    -1  
$EndComp
$Comp
L LED D?
U 1 1 00000000
P 4150 4500
F 0 "D?" H 4150 4600 50  0000 C CNN
F 1 "LED" H 4150 4400 50  0000 C CNN
	1    4150 4500
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR?
U 1 1 00000000
P 4900 2650
F 0 "#PWR?" H 4900 2740 20  0001 C CNN
F 1 "+5V" H 4900 2740 30  0000 C CNN
	1    4900 2650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 00000000
P 4900 3600
F 0 "#PWR?" H 4900 3600 30  0001 C CNN
F 1 "GND" H 4900 3530 30  0001 C CNN
	1    4900 3600
	1    0    0    -1  
$EndComp
$Comp
L C C?
U 1 1 00000000
P 4900 3300
F 0 "C?" H 4950 3400 50  0000 L CNN
F 1 "C" H 4950 3200 50  0000 L CNN
	1    4900 3300
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 00000000
P 4600 3650
F 0 "R?" V 4680 3650 50  0000 C CNN
F 1 "R1000" V 4600 3650 50  0000 C CNN
	1    4600 3650
	1    0    0    -1  
$EndComp
$Comp
L MOSFET_N Q?
U 1 1 00000000
P 4250 3950
F 0 "Q?" H 4260 4120 60  0000 R CNN
F 1 "2N7002" H 4300 3750 60  0000 R CNN
	1    4250 3950
	-1   0    0    -1  
$EndComp
$Comp
L BH1772GLC U?
U 1 1 00000000
P 5450 4000
F 0 "U?" H 5450 3800 60  0000 C CNN
F 1 "BH1772GLC" H 5450 3900 60  0000 C CNN
	1    5450 4000
	1    0    0    -1  
$EndComp
$EndSCHEMATC
