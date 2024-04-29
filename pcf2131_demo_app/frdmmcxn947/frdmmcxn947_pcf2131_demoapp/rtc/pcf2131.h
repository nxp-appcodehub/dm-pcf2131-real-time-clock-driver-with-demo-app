/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * @file pcf2131.h
 * @brief This file contains the PCF2131 RTC register definitions, access macros, and
 * device access functions.
 */
#ifndef PCF2131_H_
#define PCF2131_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Select 0 for SPI 1 for I2C */
#define  I2C_ENABLE   		0

/**
 **
 **  @brief The PCF2131 RTC Internal Register Map.
 */
enum
{
	PCF2131_CTRL1 = 0x00,
	PCF2131_CTRL2 = 0x01,
	PCF2131_CTRL3 = 0x02,
	PCF2131_CTRL4 = 0x03,
	PCF2131_CTRL5 = 0x04,
	PCF2131_SR_RESET = 0x05,
	PCF2131_100TH_SECOND = 0x06,
	PCF2131_SECOND = 0x07,
	PCF2131_MINUTE = 0x08,
	PCF2131_HOUR = 0x09,
	PCF2131_DAY = 0x0A,
	PCF2131_WEEKEND = 0x0B,
	PCF2131_MONTH = 0x0C,
	PCF2131_YEAR = 0x0D,
	PCF2131_SECOND_ALARM = 0x0E,
	PCF2131_MINUTE_ALARM = 0x0F,
	PCF2131_HOUR_ALARM = 0x10,
	PCF2131_DAY_ALARM = 0x11,
	PCF2131_WEEKDAY_ALARM = 0x12,
	PCF2131_CLKOUT_CTL = 0x13,
	PCF2131_TIMESTAMP1_CTL = 0x14,
	PCF2131_TIMESTAMP1_SECONDS = 0x15,
	PCF2131_TIMESTAMP1_MINUTES = 0x16,
	PCF2131_TIMESTAMP1_HOURS = 0x17,
	PCF2131_TIMESTAMP1_DAYS = 0x18,
	PCF2131_TIMESTAMP1_MONTHS = 0x19,
	PCF2131_TIMESTAMP1_YEARS = 0x1A,
	PCF2131_TIMESTAMP2_CTL = 0x1B,
	PCF2131_TIMESTAMP2_SECONDS = 0x1C,
	PCF2131_TIMESTAMP2_MINUTES = 0x1D,
	PCF2131_TIMESTAMP2_HOURS = 0x1E,
	PCF2131_TIMESTAMP2_DAYS = 0x1F,
	PCF2131_TIMESTAMP2_MONTHS = 0x20,
	PCF2131_TIMESTAMP2_YEARS = 0x21,
	PCF2131_TIMESTAMP3_CTL = 0x22,
	PCF2131_TIMESTAMP3_SECONDS = 0x23,
	PCF2131_TIMESTAMP3_MINUTES = 0x24,
	PCF2131_TIMESTAMP3_HOURS = 0x25,
	PCF2131_TIMESTAMP3_DAYS = 0x26,
	PCF2131_TIMESTAMP3_MONTHS = 0x27,
	PCF2131_TIMESTAMP3_YEARS = 0x28,
	PCF2131_TIMESTAMP4_CTL = 0x29,
	PCF2131_TIMESTAMP4_SECONDS = 0x2A,
	PCF2131_TIMESTAMP4_MINUTES = 0x2B,
	PCF2131_TIMESTAMP4_HOURS = 0x2C,
	PCF2131_TIMESTAMP4_DAYS = 0x2D,
	PCF2131_TIMESTAMP4_MONTHS = 0x2E,
	PCF2131_TIMESTAMP4_YEARS = 0x2F,
	PCF2131_AGING_OFFSET = 0x30,
	PCF2131_INT_A_MASK1 = 0x31,
	PCF2131_INT_A_MASK2 = 0x32,
	PCF2131_INT_B_MASK1 = 0x33,
	PCF2131_INT_B_MASK2 = 0x34,
	PCF2131_WATCHDOG_TIMER_CTL = 0x35,
	PCF2131_WATCHDOG_TIMER_VAL = 0x36,
};



#define PCF2131_TBOOT_MAX  2                 /*!< Maximum safe value for TBOOT1/2 in ms (1ms, 2ms)= ~2ms. */
#define PCF2131_REG_SIZE_BYTE  (1)           /*!< size of register of PCF2131 in byte. */


/*--------------------------------
 ** Register: Control_1
 ** Enum: PCF2131_CTRL1
 ** --
 ** Offset : 0x00 control and status register 1.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t si : 1; /*  Second interrupt 0-disabled 1-enabled */

		uint8_t mi : 1; /*  Minute interrupt 0- disabled 1-enabled */

		uint8_t mode_12_24 : 1;/*  0-24 hour mode selected 1- 12 hour mode selected */

		uint8_t por_ovrd : 1; /*  Power on reset override 0-disabled 1-enabled */

		uint8_t s_dis_100th : 1; /*  0- 100th seconds counter enabled 1- 100th seconds counter disabled */

		uint8_t stop : 1;     /*  0- RTC source Clock runs 1- RTC clock is stopped */

		uint8_t tc_dis : 1;   /*  0- Temperature Compensation Enabled 1- Temperature Compensation disabled */

		uint8_t ext_test : 1; /*  0- Normal Mode 1- External Clock Test Mode. */
	} b;
	uint8_t w;
} PCF2131_CTRL_1;

/*
 * Control_1 - Bit field mask definitions
 */
#define PCF2131_CTRL1_EXT_TEST_MASK   ((uint8_t)0x80)
#define PCF2131_CTRL1_EXT_TEST_SHIFT  ((uint8_t)7)

#define PCF2131_CTRL1_TC_DIS_MASK     ((uint8_t)0x40)
#define PCF2131_CTRL1_TC_DI_SHIFT     ((uint8_t)6)

#define PCF2131_CTRL1_START_STOP_MASK       ((uint8_t)0x20)
#define PCF2131_CTRL1_START_STOP_SHIFT      ((uint8_t)5)

#define PCF2131_CTRL1_100TH_S_DIS_MASK ((uint8_t)0x10)
#define PCF2131_CTRL1_100TH_S_DIS_SHIFT ((uint8_t)4)

#define PCF2131_CTRL1_POR_OVRD_MASK    ((uint8_t)0x08)
#define PCF2131_CTRL1_POR_OVRD_SHIFT   ((uint8_t)3)

#define PCF2131_CTRL1_12_HOUR_24_HOUR_MODE_MASK ((uint8_t)0x04)
#define PCF2131_CTRL1_12_HOUR_24_HOUR_MODE_SHIFT ((uint8_t)2)

#define PCF2131_CTRL1_MI_MASK          ((uint8_t)0x02)
#define PCF2131_CTRL1_MI_SHIFT         ((uint8_t)1)

#define PCF2131_CTRL1_SI_MASK          ((uint8_t)0x01)
#define PCF2131_CTRL1_SI_SHIFT         ((uint8_t)0)

/*--------------------------------
 ** Register: Control_2
 ** Enum: PCF2131_CTRL2
 ** --
 ** Offset : 0x01 control and status register 2.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)*/

		uint8_t AIE : 1; /* 0- Alarm Interrupt Disabled 1- Alarm Interrupt Enabled */

		uint8_t _reserved_2 : 1;/*  Reserved Bit (Unused) */

		uint8_t _reserved_3 : 1; /*  Reserved Bit (Unused)* */

		uint8_t af : 1; /*  0- No Alarm Interrupt Triggered 1- Flag Set When Alarm Triggered (flag must be cleared to clear interrupt) */

		uint8_t _reserved_4 : 1; /*  Reserved Bit (Unused)  */

		uint8_t wdtf : 1; /*  0- No watchdog timer interrupt generated 1- Flag set when watchdog timer interrupt generated (flag cannot be cleared by command (read-only) */

		uint8_t msf : 1; /*  0- No minute or second interrupt generated 1- Flag set when minute or second interrupt generated (flag must be cleared to clear interrupt) */
	} b;
	uint8_t w;
} PCF2131_CTRL_2;

/*
 * Control_2 - Bit field mask definitions
 */
#define PCF2131_CTRL2_MSF_MASK        ((uint8_t)0x80)
#define PCF2131_CTRL2_MSF_SHIFT       ((uint8_t)7)

#define PCF2131_CTRL2_WDTF_MASK       ((uint8_t)0x40)
#define PCF2131_CTRL2_WDTF_SHIFT      ((uint8_t)6)

#define PCF2131_CTRL2_AF_MASK         ((uint8_t)0x10)
#define PCF2131_CTRL2_AF_SHIFT        ((uint8_t)4)


#define PCF2131_CTRL2_AIE_MASK        ((uint8_t)0x02)
#define PCF2131_CTRL2_AIE_SHIFT       ((uint8_t)1)

/*--------------------------------
 ** Register: Control_3
 ** Enum: PCF2131_CTRL3.
 ** --
 ** Offset : 0x02 control and status register 3.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t blie : 1;/*  0- No interrupt generated from battery low flag (BLF) 1- Interrupt generated when BLF is set*/

		uint8_t bie : 1; /*   0- No interrupt generated from the battery flag (BF) 1- Interrupt generated when BF is set */

		uint8_t blf : 1; /* 0- Battery status: OK 1- Battery Status: LOW   */

		uint8_t bf : 1; /*  0- No battery switch-over interrupt occurred 1- Flag set when battery switch-over occurs (flag must be cleared to clear interrupt) */

		uint8_t btse : 1; /*  0- No timestamp when battery switch-over occurs 1- Time-stamped when battery switch-over occurs */

		uint8_t pwrmng : 3; /* Control of the battery switch-over, battery low detection, and extra power fail detection functions */


	} b;
	uint8_t w;
} PCF2131_CTRL_3;

/*
 * Control_3 - Bit field mask definitions
 */
#define PCF2131_CTRL3_PWRMNG_MASK       ((uint8_t)0xE0)
#define PCF2131_CTRL3_PWRMNG_SHIFT      ((uint8_t)5)

#define PCF2131_CTRL3_BTSE_MASK         ((uint8_t)0x10)
#define PCF2131_CTRL3_BTSE_SHIFT        ((uint8_t)4)

#define PCF2131_CTRL3_BF_MASK           ((uint8_t)0x08)
#define PCF2131_CTRL3_BF_SHIFT          ((uint8_t)3)

#define PCF2131_CTRL3_BLF_MASK          ((uint8_t)0x04)
#define PCF2131_CTRL3_BLF_SHIFT         ((uint8_t)2)

#define PCF2131_CTRL3_BIE_MASK          ((uint8_t)0x02)
#define PCF2131_CTRL3_BIE_SHIFT         ((uint8_t)1)

#define PCF2131_CTRL3_BLIE_MASK         ((uint8_t)0x01)
#define PCF2131_CTRL3_BLIE_SHIFT        ((uint8_t)0)

/*--------------------------------
 ** Register: Control_4
 ** Enum: PCF2131_CTRL4.
 ** --
 ** Offset : 0x03 control and status register 4.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t _reserved_1 : 1; /*  Reserved Bit (Unused) */

		uint8_t _reserved_2 : 1;/*  Reserved Bit (Unused) */

		uint8_t _reserved_3 : 1; /* Reserved Bit (Unused) */

		uint8_t _reserved_4 : 1; /*  Reserved Bit (Unused) */

		uint8_t tsf4 : 1; /*  0- No timestamp interrupt generated when pin ~TS4 1- Flag set when ~TS4 input is driven to ground (flag must be cleared to clear interrupt) */

		uint8_t tsf3 : 1; /* 0- No timestamp interrupt generated when pin ~TS3 1- Flag set when ~TS3 input is driven to ground (flag must be cleared to clear interrupt)  */

		uint8_t tsf2 : 1; /*  0- No timestamp interrupt generated when pin ~TS2 1- Flag set when ~TS2 input is driven to ground (flag must be cleared to clear interrupt)  */

		uint8_t tsf1 : 1; /*  0- No timestamp interrupt generated when pin ~TS1 1- Flag set when ~TS1 input is driven to ground (flag must be cleared to clear interrupt)  */

	} b;
	uint8_t w;
} PCF2131_CTRL_4;

/*
 * Control_4 - Bit field mask definitions
 */
#define PCF2131_CTRL4_TSF1_MASK      ((uint8_t)0x80)
#define PCF2131_CTRL4_TSF1_SHIFT     ((uint8_t)7)

#define PCF2131_CTRL4_TSF2_MASK      ((uint8_t)0x40)
#define PCF2131_CTRL4_TSF2_SHIFT     ((uint8_t)6)

#define PCF2131_CTRL4_TSF3_MASK     ((uint8_t)0x20)
#define PCF2131_CTRL4_TSF3_SHIFT     ((uint8_t)5)

#define PCF2131_CTRL4_TSF4_MASK      ((uint8_t)0x10)
#define PCF2131_CTRL4_TSF4_SHIFT     ((uint8_t)4)

/*--------------------------------
 ** Register: Control_5
 ** Enum: PCF2131_CTRL5.
 ** --
 ** Offset : 0x04 control and status register 5.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t _reserved_1 : 1; /*  Reserved Bit (Unused) */

		uint8_t _reserved_2 : 1; /*  Reserved Bit (Unused) */

		uint8_t _reserved_3 : 1; /*  Reserved Bit (Unused) */

		uint8_t _reserved_4 : 1; /*  Reserved Bit (Unused) */

		uint8_t tsie4 : 1; /*  0- No interrupt generated from timestamp flag of TS4. 1- Interrupt generated when timestamp flag set of TS4 */

		uint8_t tsie3 : 1; /*  0- No interrupt generated from timestamp flag of TS3. 1- Interrupt generated when timestamp flag set of TS3 */

		uint8_t tsie2 : 1; /*  0- No interrupt generated from timestamp flag of TS2. 1- Interrupt generated when timestamp flag set of TS2 */

		uint8_t tsie1  : 1; /* 0- No interrupt generated from timestamp flag of TS1. 1- Interrupt generated when timestamp flag set of TS1 */


	} b;
	uint8_t w;
} PCF2131_CTRL_5;

/*
 * Control_5 - Bit field mask definitions
 */
#define PCF2131_CTRL5_TSIE1_MASK  ((uint8_t)0x80)
#define PCF2131_CTRL5_TSIE1_SHIFT ((uint8_t)7)

#define PCF2131_CTRL5_TSIE2_MASK  ((uint8_t)0x40)
#define PCF2131_CTRL5_TSIE2_SHIFT ((uint8_t)6)

#define PCF2131_CTRL5_TSIE3_MASK ((uint8_t)0x20)
#define PCF2131_CTRL5_TSIE3_SHIFT ((uint8_t)5)

#define PCF2131_CTRL5_TSIE4_MASK  ((uint8_t)0x10)
#define PCF2131_CTRL5_TSIE4_SHIFT ((uint8_t)4)

/*--------------------------------
 ** Register: SR_RESET
 ** Enum: PCF2131_SR_RESET.
 ** --
 ** Offset : 0x05 Software Reset register.
 ** ------------------------------*/
typedef uint8_t PCF2131_SR_Reset_t;      /* Software Reset */

/*
 * SR_Reset - Bit field mask definitions
 */
#define PCF2131_TRIGGER_SW_RESET  (0x2C)
#define PCF2131_CPR               (0xA4)
#define PCF2131_CTS               (0x25)
#define PCF2131_RESET_MASK        ((uint8_t)0xFF)

/*--------------------------------
 ** Register: 100th_Seconds
 ** Enum: PCF2131_100TH_SECOND.
 ** --
 ** Offset : 0x06 100th_Seconds register.
 ** ------------------------------*/
typedef uint8_t PCF2131_100TH_SECONDS; /* 100th Seconds coded in BCD format (0-99)*/

/*
 * 100th_Seconds - Bit field mask definitions
 */
#define PCF2131_SECONDS_100TH_MASK    ((uint8_t)0xFF)
#define PCF2131_SECONDS_100TH_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Seconds
 ** Enum: PCF2131_SECOND.
 ** --
 ** Offset : 0x07 Seconds register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t seconds : 7;  /* Actual seconds coded in BCD format (0-59)*/
		uint8_t osf : 1;      /* 0- Clock integrity is guaranteed 1- Clock integrity is not guaranteed*/
	} b;
	uint8_t w;
} PCF2131_SECONDS;

/*
 *  Seconds - Bit field mask definitions
 */
#define PCF2131_OSF_MASK    ((uint8_t)0x80)
#define PCF2131_OSF_SHIFT    ((uint8_t)7)

#define PCF2131_SECONDS_MASK    ((uint8_t)0x7F)
#define PCF2131_SECONDS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Minutes
 ** Enum: PCF2131_MINUTES.
 ** --
 ** Offset : 0x08 Minutes register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t minutes : 7; /* Actual minutes coded in BCD format (0-59)  */
		uint8_t _reserved_ : 1; /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_MINUTES;

/*
 * Minutes - Bit field mask definitions
 */
#define PCF2131_MINUTES_MASK    ((uint8_t)0x7F)
#define PCF2131_MINUTES_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Hours
 ** Enum: PCF2131_HOURS.
 ** --
 ** Offset : 0x09 Hours register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t hours : 6 ;         /* Actual hours coded in BCD format when in 12/24-hour mode*/
		uint8_t _reserved_1 : 1;    /* Reserved Bit (Unused)  */
		uint8_t _reserved_2 : 1;    /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_HOURS;

/*
 * Hours - Bit field mask definitions
 */
#define PCF2131_HOURS_MASK        ((uint8_t)0x3F)
#define PCF2131_HOURS_MASK_12H    ((uint8_t)0x1F)

#define PCF2131_HOURS_SHIFT       ((uint8_t)0)
#define PCF2131_AM_PM_SHIFT       ((uint8_t)5)

/*--------------------------------
 ** Register: Days
 ** Enum: PCF2131_DAYS.
 ** --
 ** Offset : 0x0A Days register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t days: 6; /* Actual day coded in BCD format (1-31)  */
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused)  */

	} b;
	uint8_t w;
} PCF2131_DAYS;

/*
 * Days - Bit field mask definitions
 */
#define PCF2131_DAYS_MASK    ((uint8_t)0x3F)
#define PCF2131_DAYS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Weekdays
 ** Enum: PCF2131_WEEKDAYS.
 ** --
 ** Offset : 0x0B Weekdays register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t weekdays : 3; /* Actual weekday value (0-6)*/
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_3 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_4 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_5 : 1; /* Reserved Bit (Unused)  */
	} b;
	uint8_t w;
} PCF2131_WEEKDAYS;

/*
 * Weekdays - Bit field mask definitions
 */
#define PCF2131_WEEKDAYS_MASK    ((uint8_t)0x07)
#define PCF2131_WEEKDAYS_SHIFT    ((uint8_t)0)


/*--------------------------------
 ** Register: Months
 ** Enum: PCF2131_MONTHS.
 ** --
 ** Offset : 0x0C Months register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t months : 5; /* Actual month coded in BCD format (1-12) */
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused)  */
		uint8_t _reserved_3 : 1; /* Reserved Bit (Unused)  */
	} b;
	uint8_t w;
} PCF2131_MONTHS;

/*
 * Months - Bit field mask definitions
 */
#define PCF2131_MONTHS_MASK    ((uint8_t)0x1F)
#define PCF2131_MONTHS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Years
 ** Enum: PCF2131_YEAR.
 ** --
 ** Offset : 0x0D Years register.
 ** ------------------------------*/
typedef uint8_t PCF2131_YEARS; /* Actual year coded in BCD format*/

/*
 * Years - Bit field mask definitions
 */
#define PCF2131_YEARS_MASK    ((uint8_t)0xFF)
#define PCF2131_YEARS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Second_alarm
 ** Enum: PCF2131_SECONDS_ALARM.
 ** --
 ** Offset : 0x0E Second_alarm register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t seconds_alarm : 7;     /* Second alarm information coded in BCD format */
		uint8_t ae_s : 1;              /* 0- Second Alarm Enabled 1- Second Alarm Disabled*/
	} b;
	uint8_t w;
} PCF2131_SECONDS_ALARM;

/*
 * Second_alarm - Bit field mask definitions
 */
#define PCF2131_AE_S_MASK             ((uint8_t)0x80)
#define PCF2131_AE_S_SHIFT            ((uint8_t)7)

#define PCF2131_SECONDS_ALARM_MASK     ((uint8_t)0x7F)
#define PCF2131_SECONDS_ALARM_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Minute_alarm
 ** Enum: PCF2131_MINUTES_ALARM.
 ** --
 ** Offset : 0x0F Minute_alarm register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t minutes_alarm : 7; /* Minute alarm information coded in BCD format */
		uint8_t ae_m : 1;  /* 0- Minute Alarm Enabled 1- Minute Alarm Disabled*/

	} b;
	uint8_t w;
} PCF2131_MINUTES_ALARM;

/*
 * PCF2131_MINUTES_ALARM - Bit field mask definitions
 */
#define PCF2131_AE_M_MASK             ((uint8_t)0x80)
#define PCF2131_AE_M_SHIFT            ((uint8_t)7)

#define PCF2131_MINUTES_ALARM_MASK     ((uint8_t)0x7F)
#define PCF2131_MINUTES_ALARM_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Hour_alarm
 ** Enum: PCF2131_HOURS_ALARM.
 ** --
 ** Offset : 0x10 Hour_alarm register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t hour_alarm : 6; /* Hour alarm information coded in BCD format when in 12/24-hour mode*/
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)*/
		uint8_t ae_h : 1; /* 0- Hour Alarm Enabled 1- Hour Alarm Disabled */
	} b;
	uint8_t w;
} PCF2131_HOURS_ALARM;

/*
 * Hour_alarm - Bit field mask definitions
 */
#define PCF2131_AE_H_MASK                ((uint8_t)0x80)
#define PCF2131_AE_H_SHIFT               ((uint8_t)7)

#define PCF2131_HOURS_ALARM_MASK         ((uint8_t)0x3F)
#define PCF2131_HOURS_ALARM_MASK_12H     ((uint8_t)0x1F)

/*--------------------------------
 ** Register: Day_alarm
 ** Enum: PCF2131_DAYS_ALARM.
 ** --
 ** Offset : 0x11 Day_alarm register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t days_alarm : 6; /* Day alarm information coded in BCD format*/
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)*/
		uint8_t ae_d : 1;  /* 0- Day Alarm Enabled 1- Day Alarm Disabled */

	} b;
	uint8_t w;
} PCF2131_DAYS_ALARM;

/*
 * Day_alarm - Bit field mask definitions
 */
#define PCF2131_AE_D_MASK             ((uint8_t)0x80)
#define PCF2131_AE_D_SHIFT            ((uint8_t)7)

#define PCF2131_DAYS_ALARM_MASK     ((uint8_t)0x3F)
#define PCF2131_DAYS_ALARM_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Weekday_alarm
 ** Enum: PCF2131_WEEKDAY_ALARM.
 ** --
 ** Offset : 0x12 Weekday_alarm register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t weekdays_alarm : 3; /* Weekday alarm information */
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused)*/
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused)*/
		uint8_t _reserved_3 : 1; /* Reserved Bit (Unused)*/
		uint8_t _reserved_4 : 1; /* Reserved Bit (Unused)*/
		uint8_t ae_w : 1; /* 0- Weekday Alarm Enabled 1- Weekday Alarm Disabled */
	} b;
	uint8_t w;
} PCF2131_WEEKDAYS_ALARM;

/*
 * Weekday_alarm - Bit field mask definitions
 */
#define PCF2131_AE_W_MASK             ((uint8_t)0x80)
#define PCF2131_AE_W_SHIFT            ((uint8_t)7)

#define PCF2131_WEEKDAYS_ALARM_MASK     ((uint8_t)0x07)
#define PCF2131_WEEKDAYS_ALARM_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Sec_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_SECONDS.
 ** --
 ** Offset : 0x15/0x1C/0x23/0x2A Sec_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t seconds_ts : 7; /* Second timestamp information coded in BCD format */
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_SECONDS_TS;

/*
 * Sec_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_SECONDS_TS_MASK    ((uint8_t)0x7F)
#define PCF2131_SECONDS_TS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Min_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_MINUTES.
 ** --
 ** Offset : 0x16/0x1D/0x24/0x2B Min_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t minutes_ts : 7; /* Minute timestamp information coded in BCD format */
		uint8_t _reserved_ : 1; /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_MINUTES_TS;

/*
 * Min_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_MINUTES_TS_MASK    ((uint8_t)0x7F)
#define PCF2131_MINUTES_TS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Hour_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_HOURS.
 ** --
 ** Offset : 0x17/0x1E/0x25/0x2C Hour_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t hours_ts : 6 ; /* Hour timestamp information coded in BCD format (12-24 Hour Mode)*/
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused) */
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_HOURS_TS;

/*
 * Hour_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_HOURS_TS_MASK      ((uint8_t)0x3F)
#define PCF2131_HOURS_TS_MASK_12H  ((uint8_t)0x1F)
#define PCF2131_HOURS_TS_SHIFT     ((uint8_t)0)

/*--------------------------------
 ** Register: Day_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_DAYS.
 ** --
 ** Offset : 0x18/0x1F/0x26/0x2D Day_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t days_ts: 6; /* Day timestamp information coded in BCD format*/
		uint8_t _reserved_1 : 1;  /* Reserved Bit (Unused) */
		uint8_t _reserved_2 : 1;  /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_DAYS_TS;

/*
 * Day_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_DAYS_TS_MASK    ((uint8_t)0x3F)
#define PCF2131_DAYS_TS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Mon_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_MONTHS.
 ** --
 ** Offset : 0x19/0x20/0x27/0x2E Mon_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t months_ts : 5; /* Month timestamp information coded in BCD format */
		uint8_t _reserved_1 : 1; /* Reserved Bit (Unused) */
		uint8_t _reserved_2 : 1; /* Reserved Bit (Unused) */
		uint8_t _reserved_3 : 1; /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} PCF2131_MONTHS_TS;

/*
 * Mon_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_MONTHS_TS_MASK    ((uint8_t)0x1F)
#define PCF2131_MONTHS_TS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: Year_timestp1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_YEARS.
 ** --
 ** Offset : 0x1A/0x21/0x28/0x2F Year_timestp1/2/3/4 register.
 ** ------------------------------*/
typedef uint8_t PCF2131_YEARS_TS; /* Year timestamp information coded in BCD format*/

/*
 * Year_timestp1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_YEARS_TS_MASK      ((uint8_t)0xFF)
#define PCF2131_YEARS_TS_SHIFT     ((uint8_t)0)

/*--------------------------------
 ** Register: Timestp_ctl1/2/3/4
 ** Enum: PCF2131_TIMESTAMP1/2/3/4_CTL.
 ** --
 ** Offset : 0x14/0x1B/0x22/0x29 Timestp_ctl1/2/3/4 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t subsec_ts : 5;                  /* 1/16 second TS information coded in BCD format if 100TH_S_DIS = '1'
                                                   else 1/20 second TS information coded in BCD format */
		uint8_t _reserved_1 : 1;                /* Reserved Bit (Unused) */
		uint8_t tsoff : 1;                      /* Timestamp Monitoring to store first timestamp event */
		uint8_t tsm : 1;                        /* Timestamp Monitoring to store last timestamp event*/
	} b;
	uint8_t w;
} PCF2131_CTRL_TS;

/*
 ** Timestp_ctl1/2/3/4 - Bit field mask definitions
 */
#define PCF2131_TSM_TS_MASK        ((uint8_t)0x80)
#define PCF2131_TSM_TS_SHIFT       ((uint8_t)7)

#define PCF2131_TSOFF_TS_MASK      ((uint8_t)0x40)
#define PCF2131_TSOFF_TS_SHIFT     ((uint8_t)6)

#define PCF2131_SUBSEC_TS_MASK     ((uint8_t)0x1F)
#define PCF2131_SUBSEC_TS_SHIFT    ((uint8_t)0)

/*--------------------------------
 ** Register: INT_A/B_MASK_1
 ** Enum: PCF2131_INT_A/B_MASK1.
 ** --
 ** Offset : 0x31/0x33 INT_A/B_MASK_1 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t blie : 1;         /* battery low flag interrupt mask */
		uint8_t bie : 1;          /* battery flag interrupt mask */
		uint8_t aie : 1;          /* alarm interrupt mask */
		uint8_t wd_cd : 1;        /* watchdog interrupt mask */
		uint8_t si : 1;           /* second interrupt mask */
		uint8_t mi : 1;           /* minute interrupt mask */
		uint8_t _reserved_1 : 1;  /* */
		uint8_t _reserved_2 : 1;  /* */
	} b;
	uint8_t w;
} INT_MASK1;

/*
 ** INT_A/B_MASK_1 - Bit field mask definitions
 */
#define PCF2131_MI_ENB_MASK_VALUE        ((uint8_t)0xDF)
#define PCF2131_MI_DIS_MASK              ((uint8_t)0x20)
#define PCF2131_MI_SHIFT                 ((uint8_t)0x05)

#define PCF2131_SI_ENB_MASK_VALUE        ((uint8_t)0xEF)
#define PCF2131_SI_DIS_MASK              ((uint8_t)0x10)
#define PCF2131_SI_SHIFT                 ((uint8_t)0x04)

#define PCF2131_WD_CD_ENB_MASK_VALUE     ((uint8_t)0xF7)
#define PCF2131_WD_CD_DIS_MASK           ((uint8_t)0x08)
#define PCF2131_WD_CD_SHIFT              ((uint8_t)0x03)

#define PCF2131_AIE_ENB_MASK_VALUE       ((uint8_t)0xFB)
#define PCF2131_AIE_DIS_MASK             ((uint8_t)0x04)
#define PCF2131_AIE_SHIFT                ((uint8_t)0x02)

#define PCF2131_BIE_ENB_MASK_VALUE       ((uint8_t)0xFD)
#define PCF2131_BIE_DIS_MASK             ((uint8_t)0x02)
#define PCF2131_BIE_SHIFT                ((uint8_t)0x01)

#define PCF2131_BLIE_ENB_MASK_VALUE      ((uint8_t)0xFE)
#define PCF2131_BLIE_DIS_MASK            ((uint8_t)0x01)
#define PCF2131_BLIE_SHIFT               ((uint8_t)0x00)

#define PCF2131_REG_MASK              ((uint8_t)0xFF)

/*--------------------------------
 ** Register: INT_A/B_MASK_2
 ** Enum: PCF2131_INT_A/B_MASK2.
 ** --
 ** Offset : 0x32/0x34 INT_A/B_MASK_2 register.
 ** ------------------------------*/
typedef union
{
	struct
	{
		uint8_t tsie4 : 1;             /* time stamp 4 interrupt mask */
		uint8_t tsie3 : 1;             /* time stamp 3 interrupt mask */
		uint8_t tsie2 : 1;             /* time stamp 2 interrupt mask */
		uint8_t tsie1 : 1;             /* time stamp 1 interrupt mask */
		uint8_t _reserved_1 : 1;       /* Reserved Bit (Unused) */
		uint8_t _reserved_2 : 1;       /* Reserved Bit (Unused) */
		uint8_t _reserved_3 : 1;       /* Reserved Bit (Unused) */
		uint8_t _reserved_4 : 1;       /* Reserved Bit (Unused) */
	} b;
	uint8_t w;
} INT_MASK2;

/*
 ** INT_MASK2 - Bit field mask definitions
 */
#define PCF2131_TS1_ENB_MASK_VALUE        ((uint8_t)0xF7)
#define PCF2131_TS1_DIS_MASK        	  ((uint8_t)0x08)
#define PCF2131_TS1_SHIFT                 ((uint8_t)0x03)

#define PCF2131_TS2_ENB_MASK_VALUE        ((uint8_t)0xFB)
#define PCF2131_TS2_DIS_MASK              ((uint8_t)0x04)
#define PCF2131_TS2_SHIFT                 ((uint8_t)0x02)

#define PCF2131_TS3_ENB_MASK_VALUE        ((uint8_t)0xFD)
#define PCF2131_TS3_DIS_MASK              ((uint8_t)0x02)
#define PCF2131_TS3_SHIFT                 ((uint8_t)0x01)

#define PCF2131_TS4_ENB_MASK_VALUE        ((uint8_t)0xFE)
#define PCF2131_TS4_DIS_MASK              ((uint8_t)0x01)
#define PCF2131_TS4_SHIFT                 ((uint8_t)0x00)

#define  PCF2131_WD_TI_TP_MASK            ((uint8_t)0x20)   /* Watchdog TI TP bit */

#endif /* PCF2131_H_ */
