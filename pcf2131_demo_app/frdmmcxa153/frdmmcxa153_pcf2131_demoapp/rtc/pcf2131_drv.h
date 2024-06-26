/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file pcf2131_drv.h
 * @brief The pcf2131_drv.h file describes the PCF2131 driver interface and structures.
 */

#ifndef PCF2131_DRV_H_
#define PCF2131_DRV_H_

/* Standard C Includes */
#include <stdint.h>

#include "pcf2131.h"

/* ISSDK Includes */
#if (I2C_ENABLE)
#include "sensor_io_i2c.h"
#include "register_io_i2c.h"
#else
#include "sensor_io_spi.h"
#include "register_io_spi.h"
#endif

/*--------------------------------
 ** Enum: IntMask
 ** @brief: To mask interrupts
 ** ------------------------------*/
typedef enum INTMASK
{
	INT_UNMASK = 0x00,
	INT_MASK = 0x01,
}IntMask;

/*--------------------------------
 ** Enum: IntStatus
 ** @brief: store Interrupt status
 ** ------------------------------*/
typedef enum ALARMTYPE
{
	A_Seconds = 0x01,
	A_Minute = 0x02,
	A_Hour = 0x03,
	A_Day = 0x04,
	A_Weekday = 0x05,
}AlarmType;

/*--------------------------------
 ** Enum: IntStatus
 ** @brief: store Interrupt status
 ** ------------------------------*/
typedef enum INTSTATUS
{
	intEnable = 0x01,      /* Interrupt Enable */
	intDisable = 0x00,     /* Interrupt Disable */
	intClear = 0x00,       /* Clear the Interrupt */
}IntStatus;

/*--------------------------------
 ** Enum: IntState
 ** @brief: store Interrupt state
 ** ------------------------------*/
typedef enum INTSTATE
{
	nointOccurred = 0x00,    /* No Interrupt Occurred */
	intOccurred = 0x01,      /* Interrupt Occurred */
}IntState;

/*--------------------------------
 ** Enum: Mode12h_24h
 ** @brief: store 12h/24h mode
 ** ------------------------------*/
typedef enum MODE12H_24H
{
	mode24H = 0x00,         /* 24 Hour Mode Set*/
	mode12H = 0x01,         /* 12 Hour Mode Set */
}Mode12h_24h;

/*--------------------------------
 ** Enum: S100thMode
 ** @brief 100th Second Mode Enable/
 ** 		  Disable
 ** ------------------------------*/
typedef enum S100THMODE
{
	s100thEnable = 0x00,     /* 100th Second Mode Enabled */
	s100thDisable = 0x01,    /* 100th Second Mode Disabled */
}S100thMode;

/*--------------------------------
 ** Enum: RtcOnOff
 ** @brief RTC on/off
 ** ------------------------------*/
typedef enum RTCONOFF
{
	rtcStart = 0x00,        /* Start Real Time Clock */
	rtcStop = 0x01,         /* Stop Real Time Clock */
}RtcOnOff;

/*--------------------------------
 ** Enum: LowBatInt
 ** @brief Low Battery Interrupt
 ** 		  Enable/Disable
 ** ------------------------------*/
typedef enum LOWBATINT
{
	lbIntEnable = 0x01,      /* Low Battery Interrupt Enable */
	lbIntDisable = 0x00,     /* Low Battery Interrupt Disable */
}LowBatInt;

/*--------------------------------
 ** Enum: BatSoTs
 ** @brief Enable/Disable Time Stamp on
 **        Battery Switch Over
 ** ------------------------------*/
typedef enum BATSOTS
{
	SoTsEnable = 0x01,      /* Battery Switch Over Time Stamp Enable */
	SoTsDisable = 0x00,     /* Battery Switch Over Time Stamp Disable */
}BatSoTs;

/*--------------------------------
 ** Enum: SwTsNum
 ** @brief Switch Number 1/2/3/4
 ** ------------------------------*/
typedef enum SWTSNUM
{
	sw1Ts = 0x00,          /* Timestamp Switch 1 */
	sw2Ts = 0x01,          /* Timestamp Switch 2 */
	sw3Ts = 0x02,          /* Timestamp Switch 3 */
	sw4Ts = 0x03,          /* Timestamp Switch 4 */
}SwTsNum;

/*--------------------------------
 ** Enum: SwRst
 ** @brief Software Reset
 ** ------------------------------*/
typedef enum SWRST
{
	CPR = 0x00,            /* Clear Prescaler*/
	CTS = 0x01,            /* Clear Timestamp */
	SR = 0x02,             /* Software Reset (also enabled CTS and CPR)*/
}SwRst;

/*--------------------------------
 ** Enum: AmPm
 ** @brief Store AM PM mode
 ** ------------------------------*/
typedef enum AMPM
{
	AM = 0x00,             /* AM in 12H Mode */
	PM = 0x01,             /* PM in 12H Mode */
	h24 = 0x02,            /* 24H Mode*/
}AmPm;

/*--------------------------------
 ** Enum: TsOnOff
 ** @brief TimeStamp on/off
 ** ------------------------------*/
typedef enum TS_ONOFF
{
	Ts_On = 0x00,          /* TimeStamp On */
	Ts_Off = 0x01,         /* TimeStamp Off */
}TsOnOff;

/*--------------------------------
 ** Enum: SaveTs
 ** @brief TimeStamp save mode
 ** ------------------------------*/
typedef enum TSMODE
{
	Ts_first = 0x00,       /* Save First TimeStamp */
	Ts_latest = 0x01,      /* Save Latest (Last Saved) TimeStamp */
}TsMode;

/*--------------------------------
 ** Enum: IntSrc
 ** @brief Interrupt Source
 ** ------------------------------*/
typedef enum INTSRC
{
	IntA = 0x00,           /* Interrupt from INTA */
	IntB = 0x01,           /* Interrupt from INTB */
}IntSrc;

/*--------------------------------
 ** Enum: WeekDays
 ** @brief store Weekday
 ** ------------------------------*/
typedef enum WEEKDAYS
{
	Sunday = 0x00,         /* 0 Corresponds to Sunday */
	Monday  = 0x01,        /* 1 Corresponds to Monday */
	Tuesday = 0x02,        /* 2 Corresponds to Tuesday */
	Wednesday = 0x03,      /* 3 Corresponds to Wednesday */
	Thursday  = 0x04,      /* 4 Corresponds to Thursday */
	Friday   = 0x05,       /* 5 Corresponds to Friday */
	Saturday = 0x06,       /* 6 Corresponds to Saturday */
}WeekDays;

/*--------------------------------
 ** Enum: Months
 ** @brief Months
 ** ------------------------------*/
typedef enum MONTHS
{
	January = 0x00,         /* 0 Corresponds to January */
	February = 0x01,        /* 1 Corresponds to February */
	March  = 0x02,          /* 2 Corresponds to March */
	April = 0x03,           /* 3 Corresponds to April */
	May = 0x05,             /* 4 Corresponds to May */
	June = 0x06,            /* 5 Corresponds to June */
	July = 0x07,            /* 6 Corresponds to July */
	August = 0x08,          /* 7 Corresponds to August */
	September = 0x09,       /* 8 Corresponds to September */
	October = 0x0A,         /* 9 Corresponds to October */
	November = 0x0B,        /* 10 Corresponds to November */
	December = 0x0C,        /* 11 Corresponds to December */
}Months;

/*! @brief This structure defines the Alarm Data in Seconds, Minutes, Hours, Days, Weekdays.*/
typedef struct
{
	uint8_t  second;
	uint8_t  minutes;
	uint8_t  hours;
	uint8_t  days;
	uint8_t  weekdays;
	AmPm     ampm;
} pcf2131_alarmdata_t;

/*! @brief This structure defines the Time Data in 100th Seconds to Years.*/
typedef struct
{
	uint8_t  second_100th;
	uint8_t  second;
	uint8_t  minutes;
	uint8_t  hours;
	uint8_t  days;
	uint8_t  weekdays;
	uint8_t  months;
	uint8_t  years;
	AmPm     ampm;
} pcf2131_timedata_t;

/*! @brief This structure defines the Timestamp related data.*/
typedef struct
{
	uint8_t  second;
	uint8_t  minutes;
	uint8_t  hours;
	uint8_t  days;
	uint8_t  months;
	uint8_t  years;
	AmPm     ampm;
} pcf2131_timestamp_t;

/*! @def    PCF2131_TIME_SIZE_BYTE
 *  @brief  The size of time. */
#define PCF2131_TIME_SIZE_BYTE    (8)

/*! @def    PCF2131_ALARM_TIME_SIZE_BYTE
 *  @brief  The size of Alarm time. */
#define PCF2131_ALARM_TIME_SIZE_BYTE    (5)

/*! @def    PCF2131_TS_SIZE_BYTE
 *  @brief  The size of timestam time. */
#define PCF2131_TS_SIZE_BYTE    (6)

/*! @def    PCF2131_SPI_MAX_MSG_SIZE
 *  @brief  The MAX size of SPI message. */
#define PCF2131_SPI_MAX_MSG_SIZE (64)

/*! @def    PCF2131_SPI_CMD_LEN
 *  @brief  The size of the Sensor specific SPI Header. */
#define PCF2131_SPI_CMD_LEN (1)

/*! @def    PCF2131_SS_ACTIVE_VALUE
 *  @brief  Is the Slave Select Pin Active Low or High. */
#define PCF2131_SS_ACTIVE_VALUE SPI_SS_ACTIVE_LOW

/*! @def    PCF2131_12h_Mode
 *  @brief  By default 12h mode Enable. */
#define PCF2131_12h_Mode    (0x04)

/*! @def    PCF2131_100TH_S_DIS
 *  @brief  By default 100th_second mode disable. */
#define PCF2131_100TH_S_DIS     (0x10)

/*! @def    PCF2131_WD_TS_TP
 *  @brief  Generate a pulsed signal on INTA/B when MSF flag is set. */
#define PCF2131_WD_TS_TP        (0x20)

/*! @def    PCF2131_SPI_WR_CMD
 *  @brief  write command of PCF2131 SENSOR */
#define PCF2131_SPI_WR_CMD        (0x7F)

/*! @def    PCF2131_SPI_RD_CMD  (0x7F)
 *  @brief  Read command of PCF2131 SENSOR */
#define PCF2131_SPI_RD_CMD        (0x80)


/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief This defines the sensor specific information for SPI.
 */
typedef struct
{
	registerDeviceInfo_t deviceInfo;      /*!< SPI device context. */
#if (I2C_ENABLE)
	ARM_DRIVER_I2C *pCommDrv;        /*!< Pointer to the i2c driver. */
#else
	ARM_DRIVER_SPI *pCommDrv;             /*!< Pointer to the spi driver. */
#endif
	bool isInitialized;                   /*!< Whether sensor is intialized or not.*/
#if (I2C_ENABLE)
	uint16_t slaveAddress;           /*!< slave address.*/
#else
	spiSlaveSpecificParams_t slaveParams; /*!< Slave Specific Params.*/
#endif
}  pcf2131_sensorhandle_t;

/*******************************************************************************
 * APIs
 ******************************************************************************/

/*! @brief       Preprocesses a read command for the PCF2131 RTC.
 *  @details     Prepares a read command to be sent to the RTC.
 *  @param[in]   pCmdOut  		Pointer to the command output buffer.
 *  @param[in]   offset   		Offset for the read command.
 *  @param[in]   size     		Size of the read command.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant  No
 */
void PCF2131_ReadPreprocess(void *pCmdOut, uint32_t offset, uint32_t size);

/*! @brief       Preprocesses a write command for the PCF2131 RTC.
 *  @details     Prepares a write command to be sent to the RTC.
 *  @param[in]   pCmdOut  		Pointer to the command output buffer.
 *  @param[in]   offset  		Offset for the write command.
 *  @param[in]   size     		Size of the write command.
 *  @param[in] 	 pWritebuffer 	Pointer to the buffer containing data to be written.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant  No
 */
void PCF2131_WritePreprocess(void *pCmdOut, uint32_t offset, uint32_t size, void *pWritebuffer);

/*! @brief       Initializes the PCF2131 RTC.
 *  @details     Initializes the PCF2131 sensor and its handle.
 *  @param[in]   pSensorHandle  Pointer to sensor handle structure.
 *  @param[in]   pBus  			Pointer to CMSIS API compatible SPI bus object.
 *  @param[in]   index     		Index of the sensor.
 *  @param[in] 	 pSlaveSelect 	Pointer to the slave select pin.
 *  @constraints This should be the first API to be called.
 *				 Application has to ensure that previous instances of these APIs have exited before invocation.
 *  @reentrant   No
 *  @return      ::PCF2131_Initialize() returns the status
 */
#if (I2C_ENABLE)
int32_t PCF2131_Initialize(pcf2131_sensorhandle_t *pSensorHandle, ARM_DRIVER_I2C *pBus, uint8_t index, uint16_t sAddress);
#else
int32_t PCF2131_Initialize(pcf2131_sensorhandle_t *pSensorHandle, ARM_DRIVER_SPI *pBus, uint8_t index, void *pSlaveSelect);
#endif
/*! @brief       Sets an idle task for the PCF2131 RTC.
 *  @details     Sets a function to be called when the sensor is in idle state.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   idleTask  			Function Pointer to the idle task.
 *  @param[in]   userParam     		Pointer to user defined parameter for the idle task.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 */
void PCF2131_SetIdleTask(pcf2131_sensorhandle_t *pSensorHandle, registeridlefunction_t idleTask, void *userParam);

/*! @brief       Configures the PCF2131 RTC.
 *  @details     Initializes the PCF2131 sensor and its handle.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pRegWriteList      Pointer to the list of register write operations.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Configue() returns the status.
 */
int32_t PCF2131_Configure(pcf2131_sensorhandle_t *pSensorHandle, const registerwritelist_t *pRegWriteList);

/*! @brief       De-initializes the PCF2131 RTC.
 *  @details     De-initializes the PCF2131 sensor and its handle.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return     ::PCF2131_Deinit() returns the status.
 */
int32_t PCF2131_Deinit(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Enables the second interrupt source for the PCF2131 RTC.
 *  @details     Enables the specified interrupt source for second interrupt generation.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   intsrc  			Source of the interrupt to be enabled.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SecInt_Enable() returns the status.
 */
int32_t PCF2131_SecInt_Enable(pcf2131_sensorhandle_t *pSensorHandle, IntSrc intsrc);

/*! @brief       Disables the second interrupt source for the PCF2131 RTC.
 *  @details     Disables the specified interrupt source for second interrupt generation.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SecInt_Disable() returns the status.
 */
int32_t PCF2131_SecInt_Disable(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Check the status of minute/second interrupt.
 *  @details     Checks whether minute/second interrupt is active or not.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[out]  pIntStatus       	Pointer to store the interrupt status.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_MinSecInt() returns the status.
 */
int32_t PCF2131_Check_MinSecInt(pcf2131_sensorhandle_t *pSensorHandle, IntState *pIntStatus);

/*! @brief       Clears the minute/second interrupt.
 *  @details     Clears the minute/second interrupt status.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Clear_MinSecInt() returns the status.
 */
int32_t PCF2131_Clear_MinSecInt(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Enables the minute/second interrupt for the PCF2131 RTC.
 *  @details     Enables the specified interrupt source for minute/second interrupt generation.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   intsrc  			Source of the interrupt to be enabled.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return     ::PCF2131_MinInt_Enable() returns the status.
 */
int32_t PCF2131_MinInt_Enable(pcf2131_sensorhandle_t *pSensorHandle, IntSrc intsrc);

/*! @brief       Disables the minute/second interrupt for the PCF2131 RTC.
 *  @details     Disables the specified interrupt source for minute/second interrupt generation.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_MinInt_Disable() returns the status.
 */
int32_t PCF2131_MinInt_Disable(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Start the real time clock (RTC)  for the PCF2131 RTC.
 *  @details     Start the real time clock (RTC) of the RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Rtc_Start() returns the status.
 */
int32_t PCF2131_Rtc_Start(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Stops the real time clock (RTC) for the PCF2131 RTC.
 *  @details     Stops the real time clock (RTC) of the RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Rtc_Stop() returns the status.
 */
int32_t PCF2131_Rtc_Stop(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Performs a Software Reset for the PCF2131 RTC.
 *  @details     Resets the RTC using software reset.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   swrst  			Type of software reset to perform.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SwRst() returns the status.
 */
int32_t PCF2131_SwRst(pcf2131_sensorhandle_t *pSensorHandle, SwRst swrst);

/*! @brief       Turns on the timestamp feature for the PCF2131 RTC.
 *  @details     Enables the timestamp feature for the specified timestamp number.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   tsnum  			Number of the timestamp to be enabled.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_TimeStamp_On() returns the status.
 */
int32_t PCF2131_TimeStamp_On(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum tsnum);

/*! @brief       Turns off the timestamp feature for the PCF2131 RTC.
 *  @details     Disables the timestamp feature for the specified timestamp number.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   tsnum  			Number of the timestamp to be disabled.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_TimeStamp_Off() returns the status.
 */
int32_t PCF2131_TimeStamp_Off(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum tsnum);

/*! @brief       Get the current time from the PCF2131 RTC.
 *  @details     Reads the current time from sensor registers.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pcf2131timedata    Pointer to the list of registers read operationsfor time data.
 *  @param[out]  time   			Pointer to store the read time data.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_GetTime() returns the status.
 */
int32_t PCF2131_GetTime(pcf2131_sensorhandle_t *pSensorHandle,const registerreadlist_t  *pcf2131timedata, pcf2131_timedata_t *time );

/*! @brief       Sets the time from the PCF2131 RTC.
 *  @details     Sets the current time in the RTC registers.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   time    			Pointer to the time data to be set.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SetTime() returns the status.
 */
int32_t PCF2131_SetTime(pcf2131_sensorhandle_t *pSensorHandle, pcf2131_timedata_t *time);

/*! @brief       Gets the timestamp from the PCF2131 RTC.
 *  @details     Reads the timestampfor the specified timestamp number.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pcf2131tsdata    	Pointer to the list of register read operations for timestamp data.
 *  @param[in]   tsnum    			Number of the timestamp to be read.
 *  @param[out]  timestamp    	    Pointer to store the read timestamp data.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_GetTs() returns the status.
 */
int32_t PCF2131_GetTs(pcf2131_sensorhandle_t *pSensorHandle,const registerreadlist_t *pcf2131tsdata, SwTsNum tsnum, pcf2131_timestamp_t *timestamp);

/*! @brief       Sets the time mode for the PCF2131 RTC.
 *  @details     Sets the 12-hour or 24-hour mode the PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   is_mode12h   		Mode Selection: 12 hour/24 hour.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_12h_24h_Mode_Set() returns the status.
 */
int32_t PCF2131_12h_24h_Mode_Set(pcf2131_sensorhandle_t *pSensorHandle, Mode12h_24h is_mode12h);

/*! @brief       Gets the current time mode from the PCF2131 RTC.
 *  @details     Retrieves the current time mode (12-hour or 24-hour) from the PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[out]  pmode_12_24   		Pointer to store the current time mode.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_12h_24h_Mode_Get() returns the status.
 */
int32_t PCF2131_12h_24h_Mode_Get(pcf2131_sensorhandle_t *pSensorHandle, Mode12h_24h *pmode_12_24);

/*! @brief       Sets the 100th second mode for the PCF2131 RTC.
 *  @details     Sets the 100th second mode(Enable/Disable) for the PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[out]  is_s100h   		100th Second Mode: Enable or Disable.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Sec100TH_Mode() returns the status.
 */
int32_t PCF2131_Sec100TH_Mode(pcf2131_sensorhandle_t *pSensorHandle, S100thMode is_s100h);

/*! @brief       Gets the 100th second mode for the PCF2131 RTC.
 *  @details     Gets the 100th second mode(Enable/Disable) for the PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[out]  is_s100h   		100th Second Mode: Enable or Disable.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Sec100TH_Mode_Get() returns the status.
 */
int32_t PCF2131_Sec100TH_Mode_Get(pcf2131_sensorhandle_t *pSensorHandle, S100thMode *s100_mode);

/*! @brief       Enables or Disables battery switch-over for the timestamps for PCF2131 RTC.
 *  @details     Enables or Disables the battery switch-over timestamp functionality.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   is_enabled			Enable/Disable the battery switch-over timestamp.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Bat_SwitchOverTS() returns the status.
 */
int32_t PCF2131_Bat_SwitchOverTS(pcf2131_sensorhandle_t *pSensorHandle, BatSoTs is_enabled);

/*! @brief       Enables TimeStamp Interrupts for PCF2131 RTC.
 *  @details     Enables TimeStamp Interrupts functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   SwTsNum			Switch number on which timestamp interrupt is to be generated.
 *  @param[in]   IntSrc             Interrupt Source (INTA or INTB)
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SW_TsInt_Enable() returns the status.
 */
int32_t PCF2131_SW_TsInt_Enable(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum swtsnum, IntSrc intsrc);

/*! @brief       Disables TimeStamp Interrupts for PCF2131 RTC.
 *  @details     Disables TimeStamp Interrupts functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   swtsnum			Switch number on which timestamp interrupt is to be generated.
 *  @param[in]   intsrc             Interrupt Source (INTA or INTB)
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SW_TsInt_Disable() returns the status.
 */
int32_t PCF2131_SW_TsInt_Disable(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum swtsnum);

/*! @brief       Clears TimeStamp Interrupts for PCF2131 RTC.
 *  @details     Clears TimeStamp Interrupts for PCF2131 RTC on INTA/INTB pin .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   swtsnum			Switch number on which timestamp interrupt is to be generated.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Clear_SW_TsInt() returns the status.
 */
int32_t PCF2131_Clear_SW_TsInt(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum swtsnum);

/*! @brief       Checks TimeStamp Interrupts for PCF2131 RTC.
 *  @details     Checks TimeStamp Interrupts for PCF2131 RTC on INTA/INTB pin .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   swtsnum			Switch number on which timestamp interrupt is to be generated.
 *  @param[in]   pSwIntStatus			Interrupt state checks whether interrupt occurred or not.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_SW_TsInt() returns the status.
 */
int32_t PCF2131_Check_SW_TsInt(pcf2131_sensorhandle_t *pSensorHandle, SwTsNum swtsnum, IntState *pSwIntStatus);

/*! @brief       Clears Alarm Interrupts for PCF2131 RTC.
 *  @details     Clears Alarm Interrupts for PCF2131 RTC on INTA/INTB pin .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Clear_AlarmInt() returns the status.
 */
int32_t PCF2131_Clear_AlarmInt(pcf2131_sensorhandle_t *pSensorHandle);


/*! @brief       Checks Alarm Interrupts for PCF2131 RTC.
 *  @details     Checks Alarm Interrupts for PCF2131 RTC on INTA/INTB pin .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pAlarmState			Interrupt state checks whether interrupt occurred or not.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_AlarmInt() returns the status.
 */
int32_t PCF2131_Check_AlarmInt(pcf2131_sensorhandle_t *pSensorHandle, IntState *pAlarmState);

/*! @brief       Enables Alarm Interrupts for PCF2131 RTC.
 *  @details     Enables Alarm Interrupts functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   intsrc             Interrupt Source (INTA or INTB)
 *  @param[in]   alarmtype          Alarm Type (Second,Minute,Hour,Day,Weekday).
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_AlarmInt_Enable() returns the status.
 */
int32_t PCF2131_AlarmInt_Enable(pcf2131_sensorhandle_t *pSensorHandle, IntSrc intsrc, AlarmType alarmtype);

/*! @brief       Disables Alarm Interrupts for PCF2131 RTC.
 *  @details     Disables Alarm Interrupts functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_AlarmInt_Disable() returns the status.
 */
int32_t PCF2131_AlarmInt_Disable(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Get Alarm Time for PCF2131 RTC.
 *  @details     Get Alarm functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pcf2131alarmdata   Alarm Data
 *  @param[in]   alarmtime          Time to be set for alarm functionality
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_GetAlarmTime() returns the status.
 */
int32_t PCF2131_GetAlarmTime(pcf2131_sensorhandle_t *pSensorHandle,const registerreadlist_t *pcf2131alarmdata , pcf2131_alarmdata_t *alarmtime);


/*! @brief       Get Alarm Time for PCF2131 RTC.
 *  @details     Get Alarm functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pcf2131alarmdata   Alarm Data
 *  @param[in]   alarmtime          Time to be set for alarm functionality
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_SetAlarmTime() returns the status.
 */
int32_t PCF2131_SetAlarmTime(pcf2131_sensorhandle_t *pSensorHandle, pcf2131_alarmdata_t *alarmtime);

//Battery Switch Over


/*! @brief       Disable Battery Switch Over Timestamps for PCF2131 RTC.
 *  @details     Disable Battery Switch Over Timestamps functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Bat_SwitchOverTS_Disable() returns the status.
 */
int32_t PCF2131_Bat_SwitchOverTS_Disable(pcf2131_sensorhandle_t *pSensorHandle);
/*! @brief       Enable Battery Switch Over Timestamps for PCF2131 RTC.
 *  @details     Enable Battery Switch Over Timestamps functionality for PCF2131 RTC .
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   intsrc 			Interrupt Source (INTA or INTB)
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Bat_SwitchOverTS_Enable() returns the status.
 */
int32_t PCF2131_Bat_SwitchOverTS_Enable(pcf2131_sensorhandle_t *pSensorHandle,IntSrc intsrc);

/*! @brief       Clear Interrupts on Battery Switch Over for PCF2131 RTC.
 *  @details     Clear Interrupts on Battery Switch Over for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Clear_SwitchOverInt() returns the status.
 */
int32_t PCF2131_Clear_SwitchOverInt(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Checks Interrupts on Battery Switch Over for PCF2131 RTC.
 *  @details    Checks Interrupts on Battery Switch Over for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pSoStatus			Battery Switch Over Status
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_SwitchOverInt() returns the status.
 */
int32_t PCF2131_Check_SwitchOverInt(pcf2131_sensorhandle_t *pSensorHandle, IntStatus *pSoStatus);

/*! @brief       Enable/Disable Battery Switch Over Interrupt for PCF2131 RTC.
 *  @details     Enable/Disable Battery Switch Over Interrupt for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   is_enabled			Battery Switch Over Interrupt Status
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Bat_SwitchOverInt() returns the status.
 */
int32_t PCF2131_Bat_SwitchOverInt(pcf2131_sensorhandle_t *pSensorHandle, IntStatus is_enabled);

//Battery Low Status

/*! @brief       Checks whether Battery Low Status functionality is enabled for PCF2131 RTC.
 *  @details     Checks whether Battery Low Status functionality is enabled for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_LowBatStatusEnable() returns the status.
 */
int32_t PCF2131_Check_LowBatStatusEnable(pcf2131_sensorhandle_t *pSensorHandle);

/*! @brief       Checks Battery Low Status  for PCF2131 RTC.
 *  @details     Checks Battery Low Status  for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   pLowBatStatus      Battery Low Status Flag.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_Check_LowBatStatus() returns the status.
 */
int32_t PCF2131_Check_LowBatStatus(pcf2131_sensorhandle_t *pSensorHandle, IntStatus *pLowBatStatus);

/*! @brief       Generate Interrupt if battery status is low for PCF2131 RTC.
 *  @details     Generate Interrupt if battery status is low for PCF2131 RTC.
 *  @param[in]   pSensorHandle  	Pointer to sensor handle structure.
 *  @param[in]   is_enabled      	Low Battery Interrupt is Enabled or Disabled.
 *  @constraints This can be called any number of times only after PCF2131_Initialize().
 *				 Application has to ensure that previous instances of these APIs have exited before invocation
 *  @reentrant   No
 *  @return      ::PCF2131_LowBatInt() returns the status.
 */
int32_t PCF2131_LowBatInt(pcf2131_sensorhandle_t *pSensorHandle, LowBatInt is_enabled);
#endif /* PCF2131_DRV_H_ */
