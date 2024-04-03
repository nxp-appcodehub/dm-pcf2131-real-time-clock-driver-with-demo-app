Hardware requirements
===================
- Mini/micro C USB cable
- FRDM-MCXN947 board
- PCF2131-ARD board (https://www.nxp.com/part/PCF2131-ARD#/)
- Personal Computer

Board settings
============
Since the examples use SPI, Pins 1-2,3-4,5-6,7-8,9-10 of J2 on PCF2131 should be connected.
J14 Pins 2-3 should be connected to select default operating voltage level i.e. "+3V3".
J15 Pins 1-2,3-4,5-6,7-8 should be OFF to disconnect Pull Up Resistors.

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

LOGS:
=============
  ISSDK PCF2131 RTC driver example demonstration for SPI with Interrupt Mode.

 Successfully Applied PCF2131 Sensor Configuration


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 1

 RTC Started


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 4

 1. 12H mode

 2. 24H mode

 Enter your choice :- 2

 24h mode is set

 1. Enable centi-seconds granularity

 2. Disable centi-seconds granularity

 Enter your choice :- 2

 centi seconds Disabled

Enter Day value between 1 to 31 :- 1

Enter Month value between 1 to 12 :- 4

Enter Year value between 0 to 99 :- 24

Enter Weekday value between 0 to 6 (0 - Sunday.....6 - Saturday)-1

Enter Hour value between 0 to 23 :- 13

Enter Minute value between 0 to 59 :- 28

Enter Second value between 0 to 59 :- 23


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 3

 TIME :- 13:28:27:0 24H Mode

 DATE [DD/MM/YY]:- 01/04/24

 MONDAY

 
 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 5

 Enter Switch Number :- 2

 Switch 2 Time Stamp Enabled


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 7

 Enter Switch Number :- 2

 Time Stamp recorded on Switch 2

 TIME :- 13:32:51 AM

 DATE [DD/MM/YY] :- 01/04/24


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 6

 Enter Switch Number :- 2

 Switch 2 Time Stamp Disabled


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 8

 1. Clear Pre-scaler

 2. Clear TimeStamp

 3. Clear Pre-scaler and TimeStamp

 Enter your choice :- 3

 Software Reset Done


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


 Enter your choice :- 2

 RTC Stopped


 *********** Main Menu ***************

 1. RTC Start

 2. RTC Stop

 3. Get Time

 4. Set Time

 5. Enable Time Stamp on Switches

 6. Disable Time Stamp on Switches

 7. Get Time Stamp on Switches

 8. Software Reset

 9. Exit


Note 1: This example provides CMSIS Compliant APIs to configure different operating modes of PCF2131 RTC. It also provides APIs to perform below mentioned operations: 
1. RTC Start 
2. RTC Stop
3. Get Time
4. Set Time
5. Get Recorded Timestamps on Switches
6. Software Reset 

Note2: PCF2131_SPI_Initialize() API should be called first in order to use other APIs for differnt device features.
User can refer pcf2131_drv.h header file for more information.
