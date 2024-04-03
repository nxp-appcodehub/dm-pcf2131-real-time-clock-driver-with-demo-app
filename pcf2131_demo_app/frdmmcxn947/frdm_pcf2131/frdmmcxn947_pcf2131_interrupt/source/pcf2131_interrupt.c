/*
 * Copyright 2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file  pcf2132_spi.c
 *  @brief The pcf2131_spi.c file implements the ISSDK PCF2131 SPI RTC driver
 *         example demonstration for SPI Mode with interrupt mode.
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
#include "pcf2131_drv.h"
#include "systick_utils.h"

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
	{PCF2131_WATCHDOG_TIMER_CTL, PCF2131_WD_TS_TP, PCF2131_WD_TI_TP_MASK},
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


/*!@brief       Print time data.
*  @details     Print time and date.
*  @param[in]   timeData   Structure holding time and date.
*  @constraints None
*
*  @reentrant   No
*  @return      No
*/
void printTime(pcf2131_timedata_t timeData)
{
	PRINTF("\r\n TIME :- %d:%d:%d:%d", timeData.hours, timeData.minutes, timeData.second, timeData.second_100th );

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

/*!@brief       Print time stamp data.
*  @details     Print time stamp data for corresponding Switch.
*  @param[in]   timeStamp   Structure holding time stamp data.
*  @constraints None
*
*  @reentrant   No
*  @return      No
*/
void printTimeStamp(pcf2131_timestamp_t timeStamp)
{
	PRINTF("\r\n TIME :- %d:%d:%d",timeStamp.hours, timeStamp.minutes, timeStamp.second);
	if(timeStamp.ampm == AM)
		PRINTF(" AM\r\n");
	else if(timeStamp.ampm == PM)
		PRINTF(" PM\r\n");
	else
		PRINTF(" 24H Mode\r\n");

	PRINTF("\r\n DATE [DD/MM/YY] :- %02d/%02d/%02d\r\n",timeStamp.days, timeStamp.months, timeStamp.years );
}

/*!@brief       Set Time data.
*  @details     Set Time and date in corresponding Registers.
*  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
*  @param[out]  timeData   		Structure holding time stamp data.
*  @constraints None
*
*  @reentrant   No
*  @return      Status of the operation.
*/
int32_t setTime(pcf2131_spi_sensorhandle_t *pcf2131Driver, pcf2131_timedata_t *timeData)
{
	int32_t status;
	uint8_t temp;
	Mode12h_24h mode12_24;
	S100thMode s100thmode;

	/* Get Days from User and update its internal Time Structure */
	do{
		PRINTF("\r\nEnter Day value between 1 to 31 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 31)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while(temp < 1 || temp > 31);
	timeData->days = temp;

	/* Get Months from User and update its internal Time Structure */
	do{
		PRINTF("\r\nEnter Month value between 1 to 12 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 1 || temp > 12)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while(temp < 1 || temp > 12);
	timeData->months = temp;

	/* Get Years from User and update its internal Time Structure */
	do{
		PRINTF("\r\nEnter Year value between 0 to 99 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 99)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
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

		PRINTF("\r\nEnter Weekday value between 0 to 6 (0 - Sunday.....6 - Saturday)-");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);


		if(temp < 0 || temp > 6)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while(temp < 0 || temp > 6);
	timeData->weekdays = temp;
	/* Get hour from User and update its internal Time Structure */
	do
	{
	/* Get 12h/24h hour format */
		PCF2131_SPI_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
		if(mode12_24 ==  mode24H)
			PRINTF("\r\nEnter Hour value between 0 to 23 :- ");
		else
			PRINTF("\r\nEnter Hour value between 1 to 12 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
 
		if((mode12_24 ==  mode24H) && (temp < 0 || temp > 23))
			PRINTF("\r\nInvalid Value\r\n");
 
		if((mode12_24 ==  mode12H) && (temp < 1 || temp > 12))
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	}
	while((mode12_24 ==  mode24H) && (temp < 0 || temp > 23) ||
			(mode12_24 ==  mode12H) && (temp < 1 || temp > 12));
	timeData->hours = temp;
 
	/* Get Minutes from User and update its internal Time Structure */
	do{
		PRINTF("\r\nEnter Minute value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
				PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	  }
	  while(temp < 0 || temp > 59);
	timeData->minutes = temp;

	/* Get Second from User and update its internal Time Structure */
	do{
		PRINTF("\r\nEnter Second value between 0 to 59 :- ");
		SCANF("%d",&temp);
		PRINTF("%d\r\n",temp);
		if(temp < 0 || temp > 59)
			PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
	   }
	   while(temp < 0 || temp > 59);
	timeData->second = temp;

	/* Get 100th Second mode*/
	status = PCF2131_SPI_Sec100TH_Mode_Get(pcf2131Driver, &s100thmode);
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
			PRINTF("\r\nEnter 1/100th Second value between 0 to 99 :- ");
			SCANF("%d",&temp);
			PRINTF("%d\r\n",temp);
			if(temp < 0 || temp > 99)
				PRINTF("\r\nInvalid Value, Please enter correct value\r\n");
		}
		while(temp < 0 || temp > 99);
		timeData->second_100th = temp;
	}

 
	/* Get 12h/24h hour format */
    PCF2131_SPI_12h_24h_Mode_Get(pcf2131Driver, &mode12_24);
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
    status = PCF2131_SPI_Rtc_Stop(pcf2131Driver);
    if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n RTC Stop Failed\r\n");
		return ERROR;
	}
 
    /* Set Time */
    status = PCF2131_SPI_SetTime(pcf2131Driver,timeData);
    if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Set Time Failed\r\n");
		return ERROR;
	}
 
    /* Start RTC */
    status = PCF2131_SPI_Rtc_Start(pcf2131Driver);
    if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n RTC Start Failed\r\n");
		return ERROR;
	}
 
    return ERROR_NONE;
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
	uint8_t temp;
	PRINTF("\r\n Enter Switch Number :- ");
	temp = GETCHAR();
    temp -= 48;
	PRINTF("%d\r\n",temp);

	if( temp >= 1 && temp <=4)
		return temp;
	else
	{
		PRINTF("\r\nInvalid Switch Number\r\n");
		return 0;
	}
}

/*!@brief       Set 100th Second mode.
*  @details     Enable/Disable 100th Second mode.
*  @param[in]   None.
*  @constraints None
*
*  @reentrant   No
*  @return      Switch Number.
*/
void  set100thSecond(pcf2131_spi_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;
	PRINTF("\r\n 1. Enable centi-seconds granularity\r\n");
	PRINTF("\r\n 2. Disable centi-seconds granularity\r\n");

	PRINTF("\r\n Enter your choice :- ");
	SCANF("%d",&temp);
	PRINTF("%d\r\n",temp);

	switch(temp)
	{
		case 1:
			status = PCF2131_SPI_Sec100TH_Mode(pcf2131Driver,s100thEnable);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n centi seconds Enabled Failed\r\n");
			}
			else
				PRINTF("\r\n centi seconds Enabled\r\n");
			break;
		case 2:
			status = PCF2131_SPI_Sec100TH_Mode(pcf2131Driver,s100thDisable);
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
/*!@brief       Get Time data.
*  @details     Get Time and date from corresponding Registers and
*  				store back in internal structure.
*  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
*  @param[out]  timeData   		Structure holding time data.
*  @constraints None
*
*  @reentrant   No
*  @return      Status of the operation.
*/
int32_t getTime(pcf2131_spi_sensorhandle_t *pcf2131Driver, pcf2131_timedata_t *timeData)
{
	int32_t status;

	/* Get Time */
	status = PCF2131_SPI_GetTime(pcf2131Driver,(const registerreadlist_t *)&pcf2131timedata , timeData);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n Get Time Failed\r\n");
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
int32_t getTimeStamp(pcf2131_spi_sensorhandle_t *pcf2131Driver, pcf2131_timestamp_t *timeStamp)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n Enter Switch Number :- ");
    SCANF("%d",&temp);
	PRINTF("%d\r\n",temp);

	switch(temp)
	{
		case 1:  /* Get Time Stamp for Sw1  */
			status = PCF2131_SPI_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw1data , (temp -1),  timeStamp);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Time Stamp for Switch 1 Failed\r\n");
				return ERROR;
			}
			PRINTF("\r\n Time Stamp recorded on Switch 1\r\n");
			break;
		case 2: /* Get Time Stamp for Sw2  */
			status = PCF2131_SPI_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw2data , (temp -1),  timeStamp);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Time Stamp for Switch 2 Failed\r\n");
				return ERROR;
			}
			PRINTF("\r\n Time Stamp recorded on Switch 2\r\n");
			break;
		case 3: /* Get Time Stamp for Sw3  */
			status = PCF2131_SPI_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw3data , (temp -1),  timeStamp);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Time Stamp for Switch 3 Failed\r\n");
				return ERROR;
			}
			PRINTF("\r\n Time Stamp recorded on Switch 3\r\n");
			break;
		case 4: /* Get Time Stamp for Sw4  */
			status = PCF2131_SPI_GetTs(pcf2131Driver,(const registerreadlist_t *)&pcf2131TsSw4data , (temp -1),  timeStamp);
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

/*!@brief       Software Reset.
*  @details     Clear Prescaler, Time Stamp and Software Reset.
*  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
*  @constraints None
*
*  @reentrant   No
*  @return      No
*/
void swReset(pcf2131_spi_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n 1. Clear Pre-scaler\r\n");
	PRINTF("\r\n 2. Clear TimeStamp\r\n");
	PRINTF("\r\n 3. Clear Pre-scaler and TimeStamp\r\n");

	PRINTF("\r\n Enter your choice :- ");
    SCANF("%d",&temp);
	PRINTF("%d\r\n",temp);

	switch(temp)
	{
		case 1: /*! Clear Pre-scaler. */
			status = PCF2131_SPI_SwRst(pcf2131Driver, CPR);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Clear Pre-scaler Failed\r\n");
			}
			else
				PRINTF("\r\n Pre-scaler Cleared\r\n");
			break;
		case 2: /*! Clear Time Stamp. */
			status = PCF2131_SPI_SwRst(pcf2131Driver, CTS);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Clear Time Stamp Failed\r\n");
			}
			else
				PRINTF("\r\n Time Stamp Cleared\r\n");
			break;
		case 3: /*! Software Reset. */
			status = PCF2131_SPI_SwRst(pcf2131Driver, SR);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n Software Reset Failed\r\n");
			}
			else
				PRINTF("\r\n Software Reset Done\r\n");
			break;
		default:
			PRINTF("\r\nInvalid Switch Number\r\n");
			break;
	}
}

/*!@brief       Set mode (12h/24h).
*  @details     set 12 hour / 24 hour format.
*  @param[in]   pcf2131Driver   Pointer to sensor handle structure.
*  @constraints None
*
*  @reentrant   No
*  @return      No
*/
void setmode12h_24h(pcf2131_spi_sensorhandle_t *pcf2131Driver)
{
	uint8_t temp;
	int32_t status;

	PRINTF("\r\n 1. 12H mode\r\n");
	PRINTF("\r\n 2. 24H mode\r\n");

	PRINTF("\r\n Enter your choice :- ");
	SCANF("%d",&temp);
	PRINTF("%d\r\n",temp);

	switch(temp)
	{
		case 1: /* Set 12h clock format */
			status = PCF2131_SPI_12h_24h_Mode_Set(pcf2131Driver, mode12H);
			if (SENSOR_ERROR_NONE != status)
			{
				PRINTF("\r\n 12h clock format Set Failed\r\n");
			}
			else
				PRINTF("\r\n 12h mode is set\r\n");
			break;
		case 2: /* Set 24h clock format */
			status = PCF2131_SPI_12h_24h_Mode_Set(pcf2131Driver, mode24H);
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
	int32_t status;
	uint8_t data[PCF2131_DATA_SIZE];
	uint8_t character,swnum;

	pcf2131_timedata_t timeData;
	pcf2131_alarmdata_t timeAlarm;
	pcf2131_timestamp_t timeStamp;

	ARM_DRIVER_SPI *pSPIdriver = &SPI_S_DRIVER;
	pcf2131_spi_sensorhandle_t pcf2131Driver;

	/*! Initialize the MCU hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_SystickEnable();
	BOARD_InitDebugConsole();

	PRINTF("\r\n ISSDK PCF2131 RTC driver example demonstration for SPI with Interrupt Mode.\r\n");

	/*! Initialize the SPI driver. */
	status = pSPIdriver->Initialize(SPI_S_SIGNAL_EVENT);
	if (ARM_DRIVER_OK != status)
	{
	  PRINTF("\r\n SPI Initialization Failed\r\n");
	  return -1;
	}

	/*! Set the SPI Power mode. */
	status = pSPIdriver->PowerControl(ARM_POWER_FULL);
	if (ARM_DRIVER_OK != status)
	{
	  PRINTF("\r\n SPI Power Mode setting Failed\r\n");
	  return -1;
	}

	/*! Set the SPI Slave speed. */
	status = pSPIdriver->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0, SPI_S_BAUDRATE);
	if (ARM_DRIVER_OK != status)
	{
	  PRINTF("\r\n SPI Control Mode setting Failed\r\n");
	  return -1;
	}

	/*! Initialize the PCF2131 RTC driver. */
	status = PCF2131_SPI_Initialize(&pcf2131Driver, &SPI_S_DRIVER, SPI_S_DEVICE_INDEX, &PCF2131_CS);
	if (SENSOR_ERROR_NONE != status)
	{
	  PRINTF("\r\n PCF2131 Sensor Initialization Failed\r\n");
	  return -1;
	}

	/*! Configure the PCF2131 sensor driver. */
	status = PCF2131_SPI_Configure(&pcf2131Driver, pcf2131ConfigDefault);
	if (SENSOR_ERROR_NONE != status)
	{
		PRINTF("\r\n PCF2131 Sensor Configuration Failed, Err = %d\r\n", status);
		return -1;
	}
	PRINTF("\r\n Successfully Applied PCF2131 Sensor Configuration\r\n");

	do
	{
		PRINTF("\r\n");
		PRINTF("\r\n *********** Main Menu ***************\r\n");
		PRINTF("\r\n 1. RTC Start \r\n");
		PRINTF("\r\n 2. RTC Stop \r\n");
		PRINTF("\r\n 3. Get Time \r\n");
		PRINTF("\r\n 4. Set Time \r\n");
		PRINTF("\r\n 5. Enable Time Stamp on Switches\r\n");
		PRINTF("\r\n 6. Disable Time Stamp on Switches\r\n");
		PRINTF("\r\n 7. Get Time Stamp on Switches \r\n");
		PRINTF("\r\n 8. Software Reset \r\n");
		PRINTF("\r\n 9. Exit \r\n");
		PRINTF("\r\n");

		PRINTF("\r\n Enter your choice :- ");
        SCANF("%d",&character);
		PRINTF("%d\r\n",character);
		switch (character)
		{
				case 1: /* RTC Start */
					status = PCF2131_SPI_Rtc_Start(&pcf2131Driver);
					if (SENSOR_ERROR_NONE != status)
					{
						PRINTF("\r\n RTC Start Failed\r\n");
						continue;
					}
					PRINTF("\r\n RTC Started\r\n");
					break;
				case 2: /* RTC Stop */
					status = PCF2131_SPI_Rtc_Stop(&pcf2131Driver);
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
					swnum = getSwNum();
					if(swnum)
					{
						status = PCF2131_SPI_TimeStamp_On(&pcf2131Driver,(swnum - 1));
						if (SENSOR_ERROR_NONE != status)
						{
							PRINTF("\r\n Time Stamp Enable Failed\r\n");
							return -1;
						}
						PRINTF("\r\n Switch %d Time Stamp Enabled\r\n",swnum);
					}
					break;
				case 6: /* Disable Time Stamp */
					swnum = getSwNum();
					if(swnum)
					{
						status = PCF2131_SPI_TimeStamp_Off(&pcf2131Driver,(swnum - 1));
						if (SENSOR_ERROR_NONE != status)
						{
							PRINTF("\r\n Time Stamp Disable Failed\r\n");
							return -1;
						}
						PRINTF("\r\n Switch %d Time Stamp Disabled\r\n",swnum);
					}
					break;
				case 7: /* Get Time Stamp */
					memset(&timeStamp, '0',sizeof(pcf2131_timestamp_t));
					status = getTimeStamp(&pcf2131Driver, &timeStamp);    // Get time Stamp
					if (ERROR != status)
					{
						printTimeStamp(timeStamp);
					}
					break;
				case 8: /*  Software Reset */
					swReset(&pcf2131Driver);
					break;
				case 9:  /* Exit */
					PRINTF("\r\n .....Bye\r\n");
					exit(0);
					break;
				default:
					PRINTF("\r\n Invalid option...chose correct one from Main Menu\r\n");
					break;
		}
	}while(1);
	return 0;
}

