Hardware requirements
===================
- Mini/micro C USB cable
- FRDM-MCXA153 board
- PCF2131-ARD board (https://www.nxp.com/part/PCF2131-ARD#/)
- Personal Computer

Board settings

Settings for SPI
Pins 1-2,3-4,5-6,7-8,9-10 of J2 on PCF2131 should be connected.
J14 Pins 2-3 should be connected to select default operating voltage level i.e. "+3V3".
J15 Pins 1-2,3-4,5-6,7-8 should be OFF to disconnect Pull Up Resistors.

Settings for I2C
Pins 1-2,3-4,5-6,7-8,9-10 of J1 on PCF2131 should be connected.
J14 Pins 2-3 should be connected to select default operating voltage level i.e. "+3V3".
J15 Pins 1-2,3-4,5-6,7-8 should be connected to connect Pull Up Resistors.


Settings for selection of EDMA/Interrupt Mode for both I2C/SPI:

I2C: Set RTE_I2C2_DMA_EN to 1 for EDMA mode and RTE_I2C2_DMA_EN to 0 for Interrupt mode in board/RTE_Device.h
SPI: Set RTE_SPI1_DMA_EN to 1 for EDMA mode and RTE_SPI1_DMA_EN to 0 for Interrupt mode in board/RTE_Device.h

Settings for Selection of I2C/SPI communication with shield board:

In rtc/pcf2131.h ,Set I2C_ENABLE to 1 for I2C and 0 for SPI

Prepare the Demo
===============
1.  Connect a USB cable between the host PC and the OpenSDA USB port on the target board.
2.  Open a serial terminal with the following settings:
    - 115200 baud rate
    - 8 data bits
    - No parity
    - One stop bit
    - No flow control
3.  Download the program to the target board.
4.  Either press the reset button on your board or launch the debugger in your IDE to begin running the demo.

Running the demo
===============
When the demo runs successfully, you can see the logs printed on the terminal based on the menu options user chooses.

NOTE: Only INTA pin is configurable for FRDN-MCXA153. 

LOGS:
=============
 ISSDK PCF2131 RTC driver example demonstration.

 Successfully Applied PCF2131 RTC Configuration


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 1

 RTC Started

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 3

 TIME :- 0:38:48:0 AM

 DATE [DD/MM/YY]:- 01/01/01

 MONDAY

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 3

 TIME :- 0:38:50:0 AM

 DATE [DD/MM/YY]:- 01/01/01

 MONDAY

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 9

 1.Enable Seconds Interrupt

 2.Disable Seconds Interrupt

 Enter your choice :- 1

 Enter Interrupt PIN :-

 1. INTA PIN

 2. INTB PIN

 Enter Your Choice :- 2

 Interrupt PIN B Selected

 Seconds Interrupt Cleared........ 1

 Seconds Interrupt Enabled

 Press Enter to goto Main Menu

 Interrupt Occurred on INTB Pin....Please Clear the interrupt


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 13

 Clearing Interrupts!!

 Minutes or Seconds interrupt occurred: 1

 Minutes or Seconds Interrupt cleared

 Press Enter to goto Main Menu

 Interrupt Occurred on INTB Pin....Please Clear the interrupt


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 13

 Clearing Interrupts!!

 Minutes or Seconds interrupt occurred: 1

 Minutes or Seconds Interrupt cleared

 Press Enter to goto Main Menu

 Interrupt Occurred on INTB Pin....Please Clear the interrupt


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 9

 1.Enable Seconds Interrupt

 2.Disable Seconds Interrupt

 Enter your choice :- 2

 Seconds Interrupt Disabled

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 4

 1. 12H mode

 2. 24H mode

 Enter your choice :- 1

 12h mode is set

 1. Enable centi-seconds granularity

 2. Disable centi-seconds granularity

 Enter your choice :- 1

 centi seconds Enabled

 Enter Day value between 1 to 31 :- 24

 Enter Month value between 1 to 12 :- 4

 Enter Year value between 0 to 99 :- 24

 Enter Weekday value between 0 to 6 (0 - Sunday.....6 - Saturday)-3

 Enter Hour value between 1 to 12 :- 4

 Enter Minute value between 0 to 59 :- 35

 Enter Second value between 0 to 59 :- 2

 Enter 1/100th Second value between 0 to 99 :- 78

 1. AM

 2. PM

 Enter Your choice :- 2

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 3

 TIME :- 4:35:9:32 PM

 DATE [DD/MM/YY]:- 24/04/24

 WEDNESDAY

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 13

 Clearing Interrupts!!

 Minutes or Seconds interrupt occurred: 1

 Minutes or Seconds Interrupt cleared

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :- 7

 Timestamp Interrupt Enabled

 1.Enable TimeStamp Interrupt

 2.Disable TimeStamp Interrupt

 Enter your choice :- 1

 Enter Switch Number :- 2

 Enter Interrupt PIN :-

 1. INTA PIN

 2. INTB PIN

 Enter Your Choice :- 1

 Interrupt PIN A Selected

 TimeStamp Interrupt Enabled

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit


 Enter your choice :-
 Interrupt Occurred on INTA Pin....Please Clear the interrupt
13

 Clearing Interrupts!!

 Interrupt at switch 2 occurred: 1

 Interrupt at switch 2 cleared

 Press Enter to goto Main Menu


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Record Timestamps

 6. Software Reset

 7. Timestamp Interrupt

 8. Minutes Interrupt

 9. Seconds Interrupt

 10. Get Alarm Time

 11. Set Alarm Time

 12. Alarm Interrupt

 13. Clear Interrupts

 14. Exit

Note: This example provides CMSIS Compliant APIs to configure different operating modes of PCF2131 RTC. It also provides APIs to perform below mentioned operations: 

1. RTC Start 
2. RTC Stop
3. Get Time
4. Set Time
5.  Get Recorded Timestamps on Switches
6. Software Reset 
7. Interrupts(Seconds,Minutes,Alarm,Timestamps) on INTA/INTB source
8. Clear Interrupts

Note2: PCF2131_Initialize() API should be called first in order to use other APIs for differnt device features.
User can refer pcf2131_drv.h header file for more information.
