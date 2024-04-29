/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file  pcf2132_rtc_demoapp.c
 *  @brief The pcf2131_rtc_demoapp.c file implements the ISSDK PCF2131 SPI and I2C RTC driver
 *         example demonstration for SPI and I2C Mode with interrupt mode and EDMA mode.
 */

//-----------------------------------------------------------------------
// SDK Includes
//-----------------------------------------------------------------------
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"

//-----------------------------------------------------------------------
// ISSDK Includes
//-----------------------------------------------------------------------

#include "issdk_hal.h"
#include "gpio_driver.h"
#include "systick_utils.h"

//-----------------------------------------------------------------------
// CMSIS Includes
//-----------------------------------------------------------------------
#include "pcf2131.h"
#include "pcf2131_drv.h"
#include "Driver_GPIO.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (RTE_I2C2_DMA_EN || RTE_SPI1_DMA_EN)
#define EXAMPLE_DMA_BASEADDR (DMA0)
#define EXAMPLE_DMA_CLOCK    kCLOCK_Dma0
#endif

//-----------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------
#define PCF2131_INTA_IRQ		GPIO3_IRQn
#define PCF2131_INTA_ISR		GPIO3_IRQHandler

// Seize of RX/TX buffer
#define PCF2131_DATA_SIZE       2

#define ERROR_NONE       0
#define ERROR            1

/*! @brief Default Register settings. */
const registerwritelist_t pcf2131ConfigDefault[] = {
		/* Set 12h mode. */
		{PCF2131_CTRL1, PCF2131_12h_Mode, PCF2131_CTRL1_12_HOUR_24_HOUR_MODE_MASK},
		/* Disable 100th Second. */
		{PCF2131_CTRL1, PCF2131_100TH_S_DIS, PCF2131_CTRL1_100TH_S_DIS_MASK},
		/* generate a pulsed signal on MSF flag. */
		__END_WRITE_DATA__};

/*! @brief Address of Second Register for Time. */
const registerreadlist_t pcf2131timedata[] = {{.readFrom = PCF2131_100TH_SECOND, .numBytes = PCF2131_TIME_SIZE_BYTE}, __END_READ_DATA__};

/*! @brief Address of Second Register for Alarm. */
const registerreadlist_t pcf2131alarmdata[] = {{.readFrom = PCF2131_SECOND_ALARM, .numBytes = PCF2131_ALARM_TIME_SIZE_BYTE}, __END_READ_DATA__};

/*! @brief Address of Second Register for Time Stamp. */
const registerreadlist_t pcf2131TsSw1data[] = {{.readFrom = PCF2131_TIMESTAMP1_SECONDS, .numBytes = PCF2131_TS_SIZE_BYTE}, __END_READ_DATA__};
const registerreadlist_t pcf2131TsSw2data[] = {{.readFrom = PCF2131_TIMESTAMP2_SECONDS, .numBytes = PCF2131_TS_SIZE_BYTE}, __END_READ_DATA__};
const registerreadlist_t pcf2131TsSw3data[] = {{.readFrom = PCF2131_TIMESTAMP3_SECONDS, .numBytes = PCF2131_TS_SIZE_BYTE}, __END_READ_DATA__};
const registerreadlist_t pcf2131TsSw4data[] = {{.readFrom = PCF2131_TIMESTAMP4_SECONDS, .numBytes = PCF2131_TS_SIZE_BYTE}, __END_READ_DATA__};

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------

GENERIC_DRIVER_GPIO *pGpioDriver = &Driver_GPIO_KSDK;
static gpioConfigKSDK_t gpioConfigDefault = {
		.pinConfig = {kGPIO_DigitalInput, 1}, .portPinConfig = {0}, .interruptMode = kGPIO_InterruptFallingEdge};


void PCF2131_INTA_ISR(void)
{
	/* Clear external interrupt flag. */
	GPIO_GpioClearInterruptFlags(INTA_PIN.base, 1U << INTA_PIN.pinNumber);
	PRINTF("\r\n Interrupt Occurred on INTA Pin....Please Clear the interrupt\r\n");

	SDK_ISR_EXIT_BARRIER;
}


/*! -----------------------------------------------------------------------
 *  @brief       Initialize PCF2131 Interrupt INTA Source and Enable IRQ
 *  @details     This function initializes PCF2131 interrupt pin
 *  @return      void  There is no return value.
 *  -----------------------------------------------------------------------*/
void init_pcf2131_wakeup_intA(void)
{
	pGpioDriver->pin_init(&INTA_PIN, GPIO_DIRECTION_IN, NULL, NULL, NULL);
	EnableIRQ(PCF2131_INTA_IRQ);
}


/*!@brief        Print Alarm Time.
 *  @details     Print Alarm Time set by user.
 *  @param[in]   timeAlarm   Structure holding alarm data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void printAlarmTime(pcf2131_alarmdata_t timeAlarm)
{
	PRINTF("\r\n TIME :- %d:%d:%d", timeAlarm.hours, timeAlarm.minutes, timeAlarm.second);

	if(timeAlarm.ampm == AM)
		PRINTF(" AM \r\n");
	else if(timeAlarm.ampm == PM)
		PRINTF(" PM \r\n");
	else
		PRINTF(" 24H Mode \r\n");

	PRINTF("\r\n DATE :- %d \r\n",timeAlarm.days);

	switch(timeAlarm.weekdays)
	{
	case 0:
		PRINTF("\r\n SUNDAY\r\n");
		break;
	case 1:
		PRINTF("\r\n MONDAY\r\n");
		break;
	case 2:
		PRINTF("\r\n TUESDAY\r\n");
		break;
	case 3:
		PRINTF("\r\n WEDNESDAY\r\n");
		break;
	case 4:
		PRINTF("\r\n THURSDAY\r\n");
		break;
	case 5:
		PRINTF("\r\n FRIDAY\r\n");
		break;
	case 6:
		PRINTF("\r\n SATURDAY\r\n");
		break;
	default:
		break;
	}
}

/*!@brief        Print Time.
 *  @details     Print Time set by user.
 *  @param[in]   timeAlarm   Structure holding time data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void printTime(pcf2131_timedata_t timeData)
{
	PRINTF("\r\n TIME :- %02d:%02d:%02d:%02d", timeData.hours, timeData.minutes, timeData.second, timeData.second_100th );

	if(timeData.ampm == AM)
		PRINTF(" AM\r\n");
	else if(timeData.ampm == PM)
		PRINTF(" PM\r\n");
	else
		PRINTF(" 24H Mode\r\n");

	PRINTF("\r\n DATE [DD/MM/YY]:- %02d/%02d/%02d\r\n",timeData.days, timeData.months, timeData.years );

	switch(timeData.weekdays)
	{
	case 0:
		PRINTF("\r\n SUNDAY\r\n");
		break;
	case 1:
		PRINTF("\r\n MONDAY\r\n");
		break;
	case 2:
		PRINTF("\r\n TUESDAY\r\n");
		break;
	case 3:
		PRINTF("\r\n WEDNESDAY\r\n");
		break;
	case 4:
		PRINTF("\r\n THURSDAY\r\n");
		break;
	case 5:
		PRINTF("\r\n FRIDAY\r\n");
		break;
	case 6:
		PRINTF("\r\n SATURDAY\r\n");
		break;
	default:
		break;
	}
}

/*!@brief        Print time stamp data.
 *  @details     Print time stamp data for corresponding Switch.
 *  @param[in]   timeStamp   Structure holding time stamp data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void printTimeStamp(pcf2131_timestamp_t timeStamp)
{
	PRINTF("\r\n TIME :- %02d:%02d:%02d",timeStamp.hours, timeStamp.minutes, timeStamp.second);
	if(timeStamp.ampm == AM)
		PRINTF(" AM\r\n");
	else if(timeStamp.ampm == PM)
		PRINTF(" PM\r\n");
	else
		PRINTF(" 24H Mode\r\n");

	PRINTF("\r\n DATE [DD/MM/YY] :- %02d/%02d/%02d\r\n",timeStamp.days, timeStamp.months, timeStamp.years );
}

/*!@brief       Get Switch Number.
 *  @details     Get Switch Number on which Time stamp is recorded.
 *  @param[in]   None.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Switch Number.
 */
int32_t getSwNum(void)
{
	uint8_t swnum;
	PRINTF("\r\n Enter Switch Number :- ");
	do{
		swnum = GETCHAR();
		swnum -= 48;
		PRINTF("%d\r\n",swnum);
		if( swnum >= 1 && swnum <=4)
			return swnum;
		else
			PRINTF("\r\nInvalid Switch Number\r\n");
	}
	while(swnum < 0 || swnum > 4);

}

/*!@brief        Get INT Source.
 *  @details     Get Source on which interrupt is to be generated.
 *  @param[in]   None.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      PIN Number.
 */
int32_t getIntSrc(void)
{
	uint8_t intsrc;
	PRINTF("\r\n Enter Interrupt PIN :- \r\n");
	PRINTF("\r\n 1. INTA PIN\r\n");
	PRINTF("\r\n 2. INTB PIN\r\n");
	PRINTF("\r\nFRDM-MCXA153 supports interrupts on INTA PIN only\r\n");
	PRINTF("\r\nPlease Select 1\r\n");
	PRINTF("\r\n Enter Your Choice :- ");
	do{
		intsrc = GETCHAR();
		intsrc -= 48;
		PRINTF("%d\r\n",intsrc);
		if(intsrc == 1 || intsrc == 2)
			return intsrc;
		if(intsrc < 0 || intsrc > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(intsrc < 0 || intsrc > 2);
}

/*!@brief        Get Alarm Type.
 *  @details     Get Alarm Type (Second,Minute,Hour,Day,Weekday).
 *  @param[in]   None.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Alarm Type.
 */
int32_t getAlarmType(void)
{
	uint8_t  alarmmode;
	PRINTF("\r\n Enter Alarm Type :-\r\n ");
	PRINTF("\r\n 1. Second Alarm\r\n");
	PRINTF("\r\n 2. Minute Alarm\r\n");
	PRINTF("\r\n 3. Hour Alarm\r\n");
	PRINTF("\r\n 4. Day Alarm\r\n");
	PRINTF("\r\n 5. Weekday Alarm\r\n");
	do{
		alarmmode = GETCHAR();
		alarmmode -= 48;
		PRINTF("%d\r\n",alarmmode);
		if( alarmmode >= 1 && alarmmode <=4)
			return alarmmode;
		else
			PRINTF("\r\nInvalid Alarm Type\r\n");
	}
	while(alarmmode < 0 || alarmmode > 5);
}

/*!@brief        Clear interrupts.
 *  @details     Clear interrupts (Seconds, Minute,
 *  			 Timestamps,Alarm).
 *  @param[in]   pcf2131Driver   Pointer to spi sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t clearInterrupts(pcf2131_sensorhandle_t *pcf2131Driver)
{
	IntState intstate;
	int32_t status;

	status = PCF2131_Check_MinSecInt(pcf2131Driver, &intstate);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Minutes or Seconds Interrupt Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Minutes or Seconds interrupt occurred: %x \r\n", intstate);
		status = PCF2131_Clear_MinSecInt(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Minutes or Seconds Interrupt clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Minutes or Seconds Interrupt cleared\r\n");
	}

	status = PCF2131_Check_SW_TsInt(pcf2131Driver,sw1Ts, &intstate);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Interrupt at switch 1 Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Interrupt at switch 1 occurred: %x \r\n", intstate);
		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, sw1Ts);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Interrupt at switch 1 clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Interrupt at switch 1 cleared\r\n");
	}

	status = PCF2131_Check_SW_TsInt(pcf2131Driver,sw2Ts, &intstate );
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Interrupt at switch 2 Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Interrupt at switch 2 occurred: %x \r\n", intstate);
		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, sw2Ts);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Interrupt at switch 2 clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Interrupt at switch 2 cleared\r\n");
	}


	status = PCF2131_Check_SW_TsInt(pcf2131Driver,sw3Ts, &intstate );
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Interrupt at switch 3 Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Interrupt at switch 3 occurred: %x \r\n", intstate);
		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, sw3Ts);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Interrupt at switch 3 clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Interrupt at switch 3 cleared\r\n");
	}


	status = PCF2131_Check_SW_TsInt(pcf2131Driver,sw4Ts, &intstate );
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Interrupt at switch 4 Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Interrupt at switch 4 occurred: %x \r\n", intstate);
		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, sw4Ts);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Interrupt at switch 4 clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Interrupt at switch 4 cleared\r\n");
	}

	status = PCF2131_Check_AlarmInt(pcf2131Driver, &intstate );
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Check Alarm Interrupt Failed\r\n");
		return ERROR;
	}
	if(intstate == 0x01)
	{
		PRINTF("\r\n Alarm Interrupt occurred: %x \r\n", intstate);
		status = PCF2131_Clear_AlarmInt(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Alarm Interrupt clear Failed\r\n");
			return ERROR;
		}
		else
			PRINTF("\r\n Alarm Interrupt cleared\r\n");
	}
}

/*!@brief        Set Alarm Time.
 *  @details     Set Alarm Time by taking input from user.
 *  @param[in]   pcf2131Driver   Pointer to spi sensor handle structure.
 *  @param[in]   timeAlarm       Alarm Time to be set by user.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t setAlarmTime(pcf2131_sensorhandle_t *pcf2131Driver, pcf2131_alarmdata_t *timeAlarm )
{
	int32_t status;
	uint8_t temp;
	Mode12h_24h mode12_24;

	/* Get Days from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Day value between 1 to 31 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 31)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 1 || temp > 31);
	timeAlarm->days = temp;

	/* Get Weekdays from User and update its internal Time Structure */
	/* 0 - Sunday
	 * 1 - Monday
	 * 2 - Tuesday
	 * 3 - Wednesday
	 * 4 - Thursday
	 * 5 - Friday
	 * 6 - Saturday
	 */
	do{

		PRINTF("\r\n Enter Weekday value between 0 to 6 (0 - Sunday.....6 - Saturday)-");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);

		if(temp < 0 || temp > 6)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 6);
	timeAlarm->weekdays = temp;

	/* Get hour from User and update its internal Time Structure */
	do
	{
		/* Get 12h/24h hour format */
		PCF2131_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
		if(mode12_24 ==  mode24H)
			PRINTF("\r\n Enter Hour value between 0 to 23 :- ");
		else
			PRINTF("\r\n Enter Hour value between 1 to 12 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);

		if((mode12_24 ==  mode24H) && (temp < 0 || temp > 23))
			PRINTF("\r\nInvalid Value\r\n");

		if((mode12_24 ==  mode12H) && (temp < 1 || temp > 12))
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while((mode12_24 ==  mode24H) && (temp < 0 || temp > 23) ||
			(mode12_24 ==  mode12H) && (temp < 1 || temp > 12));
	timeAlarm->hours = temp;

	/* Get Minutes from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Minute value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 59);
	timeAlarm->minutes = temp;

	/* Get Second from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Second value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 59);
	timeAlarm->second = temp;

	/* Get 12h/24h hour format */
	PCF2131_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
	if(mode12_24 !=  mode24H)
	{
		PRINTF("\r\n 1. AM \r\n");
		PRINTF("\r\n 2. PM \r\n");

		PRINTF("\r\n Enter Your choice :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		switch(temp)  /* Update AM/PM */
		{
		case 1:
			timeAlarm->ampm =  AM;
			break;
		case 2:
			timeAlarm->ampm = PM;
			break;
		default:
			PRINTF("\r\nInvalid choice\r\n");
			break;
		}
	}
	else
		timeAlarm->ampm = h24;

	/* Set Time */
	status = PCF2131_SetAlarmTime(pcf2131Driver,timeAlarm);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Set Time Failed\r\n");
		return ERROR;
	}

	return ERROR_NONE;
}

/*!@brief        Set Time.
 *  @details     Set Time by taking input from user.
 *  @param[in]   pcf2131Driver   Pointer to spi sensor handle structure.
 *  @param[in]   timeData        Time to be set by user.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t setTime(pcf2131_sensorhandle_t *pcf2131Driver, pcf2131_timedata_t *timeData)
{
	int32_t status;
	uint8_t temp;
	Mode12h_24h mode12_24;
	S100thMode s100thmode;

	/* Get Days from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Day value between 1 to 31 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 31)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while(temp < 1 || temp > 31);
	timeData->days = temp;

	/* Get Months from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Month value between 1 to 12 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 1 || temp > 12)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 1 || temp > 12);
	timeData->months = temp;

	/* Get Years from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Year value between 0 to 99 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 99)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 99);
	timeData->years = temp;

	/* Get Weekdays from User and update its internal Time Structure */
	/* 0 - Sunday
	 * 1 - Monday
	 * 2 - Tuesday
	 * 3 - Wednesday
	 * 4 - Thursday
	 * 5 - Friday
	 * 6 - Saturday
	 */
	do{

		PRINTF("\r\n Enter Weekday value between 0 to 6 (0 - Sunday.....6 - Saturday)-");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);


		if(temp < 0 || temp > 6)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 6);
	timeData->weekdays = temp;
	/* Get hour from User and update its internal Time Structure */
	do
	{
		/* Get 12h/24h hour format */
		PCF2131_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
		if(mode12_24 ==  mode24H)
			PRINTF("\r\n Enter Hour value between 0 to 23 :- ");
		else
			PRINTF("\r\n Enter Hour value between 1 to 12 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);

		if((mode12_24 ==  mode24H) && (temp < 0 || temp > 23))
			PRINTF("\r\nInvalid Value\r\n");

		if((mode12_24 ==  mode12H) && (temp < 1 || temp > 12))
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while((mode12_24 ==  mode24H) && (temp < 0 || temp > 23) ||
			(mode12_24 ==  mode12H) && (temp < 1 || temp > 12));
	timeData->hours = temp;

	/* Get Minutes from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Minute value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 59);
	timeData->minutes = temp;

	/* Get Second from User and update its internal Time Structure */
	do{
		PRINTF("\r\n Enter Second value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 59);
	timeData->second = temp;

	/* Get 100th Second mode*/
	status = PCF2131_Sec100TH_Mode_Get(pcf2131Driver, &s100thmode);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Get 100th Second Failed\r\n");
		return ERROR;
	}
	if(s100thmode == s100thEnable)
	{
		/* Get 1/100th Second from User and update its internal Time Structure */
		do
		{
			PRINTF("\r\n Enter 1/100th Second value between 0 to 99 :- ");
			SCANF("%d",&temp);
			PRINTF("%d\r\n",temp);
			if(temp < 0 || temp > 99)
				PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
		}
		while(temp < 0 || temp > 99);
		timeData->second_100th = temp;
	}


	/* Get 12h/24h hour format */
	PCF2131_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
	if(mode12_24 !=  mode24H)
	{
		PRINTF("\r\n 1. AM \r\n");
		PRINTF("\r\n 2. PM \r\n");

		PRINTF("\r\n Enter Your choice :- ");
		do
		{
			SCANF("%d",&temp);
			PRINTF("%d\r\n",temp);
			if(temp < 0 || temp > 2)
				PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
		}
		while(temp < 0 || temp > 2);

		switch(temp)  /* Update AM/PM */
		{
		case 1:
			timeData->ampm =  AM;
			break;
		case 2:
			timeData->ampm = PM;
			break;
		default:
			PRINTF("\r\nInvalid choice\r\n");
			break;
		}
	}
	else
		timeData->ampm = h24;

	/* Stop RTC */
	status = PCF2131_Rtc_Stop(pcf2131Driver);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n RTC Stop Failed\r\n");
		return ERROR;
	}

	/* Set Time */
	status = PCF2131_SetTime(pcf2131Driver,timeData);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Set Time Failed\r\n");
		return ERROR;
	}

	/* Start RTC */
	status = PCF2131_Rtc_Start(pcf2131Driver);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n RTC Start Failed\r\n");
		return ERROR;
	}

	return ERROR_NONE;
}
/*!@brief        Set 100th Second functionality.
 *  @details     Set 100th Second functionality.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void  set100thSecond(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;
	PRINTF("\r\n 1. Enable centi-seconds granularity\r\n");
	PRINTF("\r\n 2. Disable centi-seconds granularity\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1:
		status = PCF2131_Sec100TH_Mode(pcf2131Driver,s100thEnable);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n centi seconds Enabled Failed\r\n");
		}
		else
			PRINTF("\r\n centi seconds Enabled\r\n");
		break;
	case 2:
		status = PCF2131_Sec100TH_Mode(pcf2131Driver,s100thDisable);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n centi seconds Disabled Failed\r\n");
		}
		else
			PRINTF("\r\n centi seconds Disabled\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Option\r\n");
		break;

	}
}

/*!@brief        Get Time data.
 *  @details     Get Time and date from corresponding Registers and
 *  				store back in internal structure.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @param[out]  timeData   		Structure holding time data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t getTime(pcf2131_sensorhandle_t *pcf2131Driver, pcf2131_timedata_t *timeData)
{
	int32_t status;

	/* Get Time */
	status = PCF2131_GetTime(pcf2131Driver,(const registerreadlist_t *)&pcf2131timedata , timeData);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Get Time Failed\r\n");
		return ERROR;
	}
	return ERROR_NONE;
}

/*!@brief        Get Alarm Time data.
 *  @details     Get Alarm Time from corresponding Registers and
 *  			 store back in internal structure.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @param[out]  timeAlarm   		Structure holding alarm data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t getAlarmTime(pcf2131_sensorhandle_t *pcf2131Driver, pcf2131_alarmdata_t *timeAlarm)
{
	int32_t status;

	/* Get Time */
	status = PCF2131_GetAlarmTime(pcf2131Driver,(const registerreadlist_t *)&pcf2131alarmdata , timeAlarm);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Get Alarm Failed\r\n");
		return ERROR;
	}
	return ERROR_NONE;
}

/*!@brief       Get Time Stamp.
 *  @details     Get Time stamp from corresponding Registers and
 *  				store back in internal structure.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @param[out]  timeStamp   	Structure holding time stamp data.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t getTimeStamp(pcf2131_sensorhandle_t *pcf2131Driver, pcf2131_timestamp_t *timeStamp)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n Enter Switch Number :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 4)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 4);

	switch(temp)
	{
	case 1:  /* Get Time Stamp for Sw1  */
		status = PCF2131_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw1data , (temp -1),  timeStamp);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Time Stamp for Switch 1 Failed\r\n");
			return ERROR;
		}
		PRINTF("\r\n Time Stamp recorded on Switch 1\r\n");
		break;
	case 2: /* Get Time Stamp for Sw2  */
		status = PCF2131_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw2data , (temp -1),  timeStamp);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Time Stamp for Switch 2 Failed\r\n");
			return ERROR;
		}
		PRINTF("\r\n Time Stamp recorded on Switch 2\r\n");
		break;
	case 3: /* Get Time Stamp for Sw3  */
		status = PCF2131_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw3data , (temp -1),  timeStamp);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Time Stamp for Switch 3 Failed\r\n");
			return ERROR;
		}
		PRINTF("\r\n Time Stamp recorded on Switch 3\r\n");
		break;
	case 4: /* Get Time Stamp for Sw4  */
		status = PCF2131_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw4data , (temp -1),  timeStamp);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Time Stamp for Switch 4 Failed\r\n");
			return ERROR;
		}
		PRINTF("\r\n Time Stamp recorded on Switch 4\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Switch Number\r\n");
		break;
	}
	return ERROR_NONE;
}

/*!@brief        Get Time Stamp on Switches.
 *  @details     Record Time stamp from corresponding Switches.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      Status of the operation.
 */
int32_t timeStampOnSwitches(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;
	uint8_t swnum;
	pcf2131_timestamp_t ltimeStamp;

	PRINTF("\r\n 1. Enable Timestamp on switches\r\n");
	PRINTF("\r\n 2. Disable Timestamp on switches\r\n");
	PRINTF("\r\n 3. Get Timestamp on switches\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 3)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 3);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		swnum = getSwNum();
		if(swnum)
		{
			status = PCF2131_TimeStamp_On(pcf2131Driver,(swnum - 1));
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Time Stamp Enable Failed\r\n");
				return -1;
			}
			PRINTF("\r\n Switch %d Time Stamp Enabled\r\n",swnum);
		}
		break;
	case 2: /*! Clear Time Stamp. */
		swnum = getSwNum();
		if(swnum)
		{
			status = PCF2131_TimeStamp_Off(pcf2131Driver,(swnum - 1));
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Time Stamp Disable Failed\r\n");
				return -1;
			}
			PRINTF("\r\n Switch %d Time Stamp Disabled\r\n",swnum);
		}
		break;

	case 3: /*! Software Reset. */
		memset(&ltimeStamp, '0',sizeof(pcf2131_timestamp_t));
		status = getTimeStamp(pcf2131Driver, &ltimeStamp);    // Get time Stamp
		if (ERROR != status)
		{
			printTimeStamp(ltimeStamp);
		}
		break;
	default:
		PRINTF("\r\nInvalid Switch Number\r\n");
		break;
	}
	return 0;
}
/*!@brief        Software Reset.
 *  @details     Clear Prescaler, Time Stamp and Software Reset.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void swReset(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n 1. Clear Pre-scaler\r\n");
	PRINTF("\r\n 2. Clear Timestamp\r\n");
	PRINTF("\r\n 3. Complete Software Reset\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 3)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 3);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		status = PCF2131_SwRst(pcf2131Driver, CPR);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Clear Pre-scaler Failed\r\n");
		}
		else
			PRINTF("\r\n Pre-scaler Cleared\r\n");
		break;
	case 2: /*! Clear Time Stamp. */
		status = PCF2131_SwRst(pcf2131Driver, CTS);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Clear Time Stamp Failed\r\n");
		}
		else
			PRINTF("\r\n Time Stamp Cleared\r\n");
		break;
	case 3: /*! Software Reset. */
		status = PCF2131_SwRst(pcf2131Driver, SR);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Software Reset Failed\r\n");
		}
		else
		{
			PRINTF("\r\n Pre-Scaler Cleared.....\r\n");
			PRINTF("\r\n Timestamps Cleared.....\r\n");
			PRINTF("\r\n Software Reset Done.....\r\n");
		}
		break;
	default:
		PRINTF("\r\nInvalid Switch Number\r\n");
		break;
	}
}

/*!@brief        Seconds Interrupt.
 *  @details     Get Interrupt on INTA/INTB Pin every second.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void secondsInterrupt(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;
	IntSrc intsrc;
	IntState intstate;

	PRINTF("\r\n 1.Enable Seconds Interrupt\r\n");
	PRINTF("\r\n 2.Disable Seconds Interrupt\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		intsrc = getIntSrc();
		if (intsrc == 1)
			PRINTF("\r\n Interrupt PIN A Selected\r\n");
		else
			PRINTF("\r\n Interrupt PIN B Selected\r\n");
		status = PCF2131_Check_MinSecInt(pcf2131Driver, &intstate);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Seconds Interrupt Check Failed\r\n");
		}

		if(intstate == 0x01)
		{
			status = PCF2131_Clear_MinSecInt(pcf2131Driver);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Seconds Interrupt Clear Failed\r\n");
			}
			else
				PRINTF("\r\n Seconds Interrupt Cleared........ %x \r\n", intstate);
		}
		if (intsrc == 1)
			status = PCF2131_SecInt_Enable(pcf2131Driver, IntA);
		else
			status = PCF2131_SecInt_Enable(pcf2131Driver, IntB);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Seconds Interrupt Enabled Failed\r\n");
		}
		else
			PRINTF("\r\n Seconds Interrupt Enabled \r\n");

		break;
	case 2: /*! Clear Time Stamp. */
		status = PCF2131_SecInt_Disable(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Seconds Interrupt Disabl Failed\r\n");
		}
		else
			PRINTF("\r\n Seconds Interrupt Disabled\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Number\r\n");
		break;
	}
}

/*!@brief        Minutes Interrupt.
 *  @details     Get Interrupt on INTA/INTB Pin every Minute.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void minutesInterrupt(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	IntSrc intsrc;
	IntState intstate;
	int32_t status;

	PRINTF("\r\n 1.Enable Minutes Interrupt\r\n");
	PRINTF("\r\n 2.Disable Minutes Interrupt\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		intsrc = getIntSrc();
		if (intsrc == 1)
			PRINTF("\r\n Interrupt PIN A Selected\r\n");
		else
			PRINTF("\r\n Interrupt PIN B Selected\r\n");
		status = PCF2131_Check_MinSecInt(pcf2131Driver, &intstate);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Minutes Interrupt Check Failed\r\n");
		}

		if(intstate == 0x01)
		{
			status = PCF2131_Clear_MinSecInt(pcf2131Driver);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Minutes Interrupt clear Failed\r\n");
			}
			else
				PRINTF("\r\n Minutes Interrupt Cleared........ %x \r\n", intstate);
		}

		if (intsrc == 1)
			status = PCF2131_MinInt_Enable(pcf2131Driver, IntA);
		else
			status = PCF2131_MinInt_Enable(pcf2131Driver, IntB);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Minutes Interrupt Enable Failed\r\n");
		}
		else
			PRINTF("\r\n Minutes Interrupt Enabled \r\n");

		break;
	case 2: /*! Clear Time Stamp. */
		status = PCF2131_MinInt_Disable(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Minutes Interrupt Disable Failed\r\n");
		}
		else
			PRINTF("\r\n Minutes Interrupt Disabled\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Number\r\n");
		break;
	}
}

/*!@brief        TimeStamp Interrupt.
 *  @details     Get Interrupt on INTA/INTB Pin on SW1/SW2/SW3/SW4.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *  @reentrant   No
 *  @return      No
 */
void timeStampInterrupt(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	IntSrc intsrc;
	uint8_t swnum;
	int32_t status;

	PRINTF("\r\n 1.Enable TimeStamp Interrupt\r\n");
	PRINTF("\r\n 2.Disable TimeStamp Interrupt\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		swnum = getSwNum();
		intsrc = getIntSrc();
		if (intsrc == 1)
			PRINTF("\r\n Interrupt PIN A Selected\r\n");
		else
			PRINTF("\r\n Interrupt PIN B Selected\r\n");

		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, (swnum-1));
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Enabled Interrupt clear Failed\r\n");
		}

		if (intsrc == 1)
			status = PCF2131_SW_TsInt_Enable(pcf2131Driver, (swnum-1), IntA);
		else
			status = PCF2131_SW_TsInt_Enable(pcf2131Driver, (swnum-1), IntB);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n TimeStamp Interrupt Enable Failed\r\n");
		}
		else
			PRINTF("\r\n TimeStamp Interrupt Enabled\r\n");
		break;
	case 2: /*! Clear Time Stamp. */
		swnum = getSwNum();
		status = PCF2131_Clear_SW_TsInt(pcf2131Driver, (swnum-1));
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n TimeStamp Interrupt Clear Failed\r\n");
		}
		status = PCF2131_SW_TsInt_Disable(pcf2131Driver, (swnum-1));
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n TimeStamp Interrupt Disable Failed\r\n");
		}
		else
			PRINTF("\r\n TimeStamp Interrupt Disabled\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Number\r\n");
		break;
	}
}

/*!@brief        Alarm Interrupt.
 *  @details     Get Interrupt on INTA/INTB Pin on set alarm.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *  @reentrant   No
 *  @return      No
 */
int32_t alarmInterrupt(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	IntSrc intsrc;
	AlarmType alarmtypeInp;
	int32_t status;

	PRINTF("\r\n 1.Enable Alarm Interrupt\r\n");
	PRINTF("\r\n 2.Disable Alarm Interrupt\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1: /*! Clear Pre-scaler. */
		alarmtypeInp = getAlarmType();
		intsrc = getIntSrc();
		if (intsrc == 1)
			PRINTF("\r\n Interrupt PIN A Selected\r\n");
		else
			PRINTF("\r\n Interrupt PIN B Selected\r\n");

		status = PCF2131_Clear_AlarmInt(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Alarm Interrupt clear Failed\r\n");
		}
		if (intsrc == 1)
			status = PCF2131_AlarmInt_Enable(pcf2131Driver, IntA, alarmtypeInp);
		else
			status = PCF2131_AlarmInt_Enable(pcf2131Driver, IntB, alarmtypeInp);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Alarm Interrupt Enable Failed\r\n");
		}
		else
			PRINTF("\r\n Alarm Interrupt Enabled\r\n");
		break;
	case 2: /*! Clear Time Stamp. */
		status = PCF2131_Clear_AlarmInt(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n  Alarm Interrupt Clear Failed\r\n");
		}

		status = PCF2131_AlarmInt_Disable(pcf2131Driver);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n Alarm Interrupt Disable Failed\r\n");
		}
		else
			PRINTF("\r\n Alarm Interrupt Disabled\r\n");
		break;
	default:
		PRINTF("\r\nInvalid Number\r\n");
		break;
	}
}
/*!@brief        Set mode (12h/24h).
 *  @details     set 12 hour / 24 hour format.
 *  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
 *  @constraints None
 *
 *  @reentrant   No
 *  @return      No
 */
void setmode12h_24h(pcf2131_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n 1. 12H mode\r\n");
	PRINTF("\r\n 2. 24H mode\r\n");

	PRINTF("\r\n Enter your choice :- ");
	do{
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 2)
			PRINTF("\r\n Invalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 2);

	switch(temp)
	{
	case 1: /* Set 12h clock format */
		status = PCF2131_12h_24h_Mode_Set(pcf2131Driver, mode12H);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n 12h clock format Set Failed\r\n");
		}
		else
			PRINTF("\r\n 12h mode is set\r\n");
		break;
	case 2: /* Set 24h clock format */
		status = PCF2131_12h_24h_Mode_Set(pcf2131Driver, mode24H);
		if (SENSOR_ERROR_NONE != status)
		{
			PRINTF("\r\n 24h clock format Set Failed\r\n");
		}
		else
			PRINTF("\r\n 24h mode is set\r\n");
		break;
	default:
		PRINTF("\r\n Invalid choice\r\n");
		break;
	}
}

/*! @brief      This is the The main function implementation.
 *  @details     This function invokes board initializes routines, then then brings up the RTC sensor and
 *               finally enters an endless loop to continuously Get/Set RTC data.
 *  @param[in]   void This is no input parameter.
 *  @constraints None
 *  @reentrant   No
 *  @return      Error.
 */

int main(void)
{
	pcf2131_timedata_t timeData;
	pcf2131_alarmdata_t timeAlarm;
	AlarmType alarmtype;
	pcf2131_timestamp_t timeStamp;
	int32_t status;
	uint8_t character;
	uint8_t data[PCF2131_DATA_SIZE];
	char dummy;
	pcf2131_sensorhandle_t pcf2131Driver;

	/* Enable EDMA for I2C/SPI */
#if (RTE_I2C2_DMA_EN || RTE_SPI1_DMA_EN)
	/* Enable DMA clock. */
	CLOCK_EnableClock(EXAMPLE_DMA_CLOCK);
	edma_config_t edmaConfig = {0};
	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(EXAMPLE_DMA_BASEADDR, &edmaConfig);
#endif

#if (I2C_ENABLE)
	ARM_DRIVER_I2C *pdriver = &I2C_S_DRIVER;
#else
	ARM_DRIVER_SPI *pdriver = &SPI_S_DRIVER;
#endif


	/* Release peripheral RESET */
	RESET_PeripheralReset(kLPSPI1_RST_SHIFT_RSTn);
	RESET_PeripheralReset(kLPI2C0_RST_SHIFT_RSTn);

	/*! Initialize the MCU hardware. */
	BOARD_InitPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();

	/*! Register ISR for INTA and INTB */
	init_pcf2131_wakeup_intA();

	/*! Initialize the driver. */

	PRINTF("\r\n ISSDK PCF2131 RTC driver example demonstration.\r\n");


	/*! Initialize the driver. */
#if (I2C_ENABLE)
	status = pdriver->Initialize(I2C_S_SIGNAL_EVENT);
#else
	status = pdriver->Initialize(SPI_S_SIGNAL_EVENT);
#endif
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n I2C Initialization Failed\r\n");
		return -1;
	}

	/*! Set the Power mode. */
	status = pdriver->PowerControl(ARM_POWER_FULL);
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n Driver Power Mode setting Failed\r\n");
		return -1;
	}

	/*! Set the Slave speed. */
#if (I2C_ENABLE)
	status = pdriver->Control(ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_FAST);
#else
	status = pdriver->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0, SPI_S_BAUDRATE);
#endif
	if (ARM_DRIVER_OK != status)
	{
		PRINTF("\r\n Driver Control Mode setting Failed\r\n");
		return -1;
	}

	/*! Initialize the PCF2131 RTC driver. */
#if (I2C_ENABLE)
	status = PCF2131_Initialize(&pcf2131Driver, &I2C_S_DRIVER, I2C_S_DEVICE_INDEX, PCF2131_I2C_ADDR);
#else
	status = PCF2131_Initialize(&pcf2131Driver, &SPI_S_DRIVER, SPI_S_DEVICE_INDEX, &PCF2131_CS);
#endif
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n PCF2131 RTC Initialization Failed\r\n");
		return -1;
	}

	/*! Configure the PCF2131 RTC driver. */
	status = PCF2131_Configure(&pcf2131Driver, pcf2131ConfigDefault);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n PCF2131 RTC Configuration Failed, Err = %d\r\n", status);
		return -1;
	}

	PRINTF("\r\n Successfully Applied PCF2131 RTC Configuration\r\n");

	do
	{
		PRINTF("\r\n");
		PRINTF("\r\n *********** Main Menu ***************\r\n");
		PRINTF("\r\n 1. RTC Start \r\n");
		PRINTF("\r\n 2. RTC Stop \r\n");
		PRINTF("\r\n 3. Get Time \r\n");
		PRINTF("\r\n 4. Set Time \r\n");
		PRINTF("\r\n 5. Record Timestamps\r\n");
		PRINTF("\r\n 6. Software Reset \r\n");
		PRINTF("\r\n 7. Timestamp Interrupt \r\n");
		PRINTF("\r\n 8. Minutes Interrupt \r\n");
		PRINTF("\r\n 9. Seconds Interrupt\r\n");
		PRINTF("\r\n 10. Get Alarm Time \r\n");
		PRINTF("\r\n 11. Set Alarm Time \r\n");
		PRINTF("\r\n 12. Alarm Interrupt \r\n");
		PRINTF("\r\n 13. Clear Interrupts\r\n");
		PRINTF("\r\n 14. Exit \r\n");
		PRINTF("\r\n");

		PRINTF("\r\n Enter your choice :- ");
		SCANF("%d",&character);
		PRINTF("%d\r\n",character);

		switch (character)
		{
		case 1: /* RTC Start */
			status = PCF2131_Rtc_Start(&pcf2131Driver);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n RTC Start Failed\r\n");
				continue;
			}
			PRINTF("\r\n RTC Started\r\n");
			break;
		case 2: /* RTC Stop */
			status = PCF2131_Rtc_Stop(&pcf2131Driver);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n  RTC Stop Failed\r\n");
				continue;
			}
			PRINTF("\r\n RTC Stopped\r\n");
			break;
		case 3: /* Get Time */
			memset(&timeData, '0',sizeof(pcf2131_timedata_t));
			status = getTime(&pcf2131Driver, &timeData);
			if (ERROR  != status)
			{
				printTime(timeData);  // print time data
			}
			break;
		case 4: /* Set Time */
			setmode12h_24h(&pcf2131Driver);  /* Set 12h/24h mode */
			set100thSecond(&pcf2131Driver);  /* Set centi-seconds mode */
			memset(&timeData, '0',sizeof(pcf2131_timedata_t));
			setTime(&pcf2131Driver, &timeData);    // set time data
			break;
		case 5: /* Enable Time Stamp */
			timeStampOnSwitches(&pcf2131Driver);
			break;
		case 6: /*  Software Reset */
			swReset(&pcf2131Driver);
			break;
		case 7:  /* Timestamp Interrupts */
			PRINTF("\r\n Timestamp Interrupt Enabled\r\n");
			timeStampInterrupt(&pcf2131Driver);
			break;
		case 8:
			minutesInterrupt(&pcf2131Driver);
			break;
		case 9: /* Seconds Interrupt */
			secondsInterrupt(&pcf2131Driver);
			break;
		case 10: /* Get Alarm Time */
			memset(&timeAlarm, '0',sizeof(pcf2131_alarmdata_t));
			status = getAlarmTime(&pcf2131Driver, &timeAlarm);
			if (ERROR  != status)
			{
				printAlarmTime(timeAlarm);  // print time data
			}
			break;
		case 11: /* Set Alarm Time */
			memset(&timeAlarm, '0',sizeof(pcf2131_alarmdata_t));
			setAlarmTime(&pcf2131Driver, &timeAlarm);    // set time data
			break;
		case 12:  /* Alarm Interrupt */
			alarmInterrupt(&pcf2131Driver);
			break;
		case 13:  /* Clear Interrupts */
			PRINTF("\r\n Clearing Interrupts!!\r\n");
			clearInterrupts(&pcf2131Driver);
			break;
		case 14:  /* Exit */
			PRINTF("\r\n .....Bye\r\n");
			exit(0);
			break;
		default:
			PRINTF("\r\n Invalid option...chose correct one from Main Menu\r\n");
			break;
		}
		PRINTF("\r\n Press Enter to goto Main Menu\r\n");
		do
		{
			dummy = GETCHAR();
		} while(dummy != 13);

	}while(1);
	return 0;
}
