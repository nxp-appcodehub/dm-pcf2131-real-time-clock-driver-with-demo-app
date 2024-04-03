/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file pcf2131_drv.c
 * @brief The pcf2131_drv.c file implements the PCF2131 RTC driver interfaces.
 */

//-----------------------------------------------------------------------
// ISSDK Includes
//-----------------------------------------------------------------------
#include "gpio_driver.h"
#include "pcf2131_drv.h"
#include "systick_utils.h"

//-----------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------
uint8_t pcf2131_spiRead_CmdBuffer[PCF2131_SPI_MAX_MSG_SIZE] = {0};
uint8_t pcf2131_spiRead_DataBuffer[PCF2131_SPI_MAX_MSG_SIZE] = {0};
uint8_t pcf2131_spiWrite_CmdDataBuffer[PCF2131_SPI_MAX_MSG_SIZE] = {0};


//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------

uint8_t DecimaltoBcd(uint8_t val)
{
	return (((val/10) << 4) | (((val) % 10) & 0x0f));
}

uint8_t BcdToDecimal(uint8_t val)
{
	return (((val) >> 4) * 10 + ((val) & 0x0f)) ;
}


void PCF2131_SPI_ReadPreprocess(void *pCmdOut, uint32_t offset, uint32_t size)
{
    spiCmdParams_t *pSlaveCmd = pCmdOut;

    uint8_t *pWBuff = pcf2131_spiRead_CmdBuffer;
    uint8_t *pRBuff = pcf2131_spiRead_DataBuffer;

    /* Formatting for Read command of PCF2131 SENSOR. */
    *(pWBuff) = offset | PCF2131_SPI_RD_CMD; /* offset is the internal register address of the sensor at which Read performed. */

    /* Create the slave read command. */
    pSlaveCmd->size = size + PCF2131_SPI_CMD_LEN;
    pSlaveCmd->pWriteBuffer = pWBuff;
    pSlaveCmd->pReadBuffer = pRBuff;
}

void PCF2131_SPI_WritePreprocess(void *pCmdOut, uint32_t offset, uint32_t size, void *pWritebuffer)
{
    spiCmdParams_t *pSlaveCmd = pCmdOut;

    uint8_t *pWBuff = pcf2131_spiWrite_CmdDataBuffer;
    uint8_t *pRBuff = pcf2131_spiWrite_CmdDataBuffer + size + PCF2131_SPI_CMD_LEN;

    /* Formatting for Write command of PCF2131 SENSOR. */
    *(pWBuff) =   offset & PCF2131_SPI_WR_CMD; /* offset is the internal register address of the sensor at which write is performed. */

    /* Copy the slave write command */
    memcpy(pWBuff + PCF2131_SPI_CMD_LEN, pWritebuffer, size);

    /* Create the slave command. */
    pSlaveCmd->size = size + PCF2131_SPI_CMD_LEN;
    pSlaveCmd->pWriteBuffer = pWBuff;
    pSlaveCmd->pReadBuffer = pRBuff;
}

int32_t PCF2131_SPI_Initialize(
		pcf2131_spi_sensorhandle_t *pSensorHandle, ARM_DRIVER_SPI *pBus, uint8_t index, void *pSlaveSelect)
{
    int32_t status;
    uint8_t reg;
    GENERIC_DRIVER_GPIO *pGPIODriver = &Driver_GPIO_KSDK;

    /*! Check the input parameters. */
    if ((pSensorHandle == NULL) || (pBus == NULL) || (pSlaveSelect == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Initialize the sensor handle. */
    pSensorHandle->pCommDrv = pBus;
    pSensorHandle->slaveParams.pReadPreprocessFN = PCF2131_SPI_ReadPreprocess;
    pSensorHandle->slaveParams.pWritePreprocessFN = PCF2131_SPI_WritePreprocess;
    pSensorHandle->slaveParams.pTargetSlavePinID = pSlaveSelect;
    pSensorHandle->slaveParams.spiCmdLen = PCF2131_SPI_CMD_LEN;
    pSensorHandle->slaveParams.ssActiveValue = PCF2131_SS_ACTIVE_VALUE;

    pSensorHandle->deviceInfo.deviceInstance = index;
    pSensorHandle->deviceInfo.functionParam = NULL;
    pSensorHandle->deviceInfo.idleFunction = NULL;

    /* Initialize the Slave Select Pin. */
    pGPIODriver->pin_init(pSlaveSelect, GPIO_DIRECTION_OUT, NULL, NULL, NULL);
    if (pSensorHandle->slaveParams.ssActiveValue == SPI_SS_ACTIVE_LOW)
    {
        pGPIODriver->set_pin(pSlaveSelect);
    }
    else
    {
        pGPIODriver->clr_pin(pSlaveSelect);
    }

    pSensorHandle->isInitialized = true;
    return SENSOR_ERROR_NONE;
}

void PCF2131_SPI_SetIdleTask(pcf2131_spi_sensorhandle_t *pSensorHandle,
                              registeridlefunction_t idleTask,
                              void *userParam)
{
    pSensorHandle->deviceInfo.functionParam = userParam;
    pSensorHandle->deviceInfo.idleFunction = idleTask;
}

int32_t PCF2131_SPI_Configure(pcf2131_spi_sensorhandle_t *pSensorHandle, const registerwritelist_t *pRegWriteList)
{
    int32_t status;

    /*! Validate for the correct handle and register write list.*/
    if ((pSensorHandle == NULL) || (pRegWriteList == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before applying configuration.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Apply the Sensor Configuration based on the Register Write List */
    status = Sensor_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
                              pRegWriteList);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_ReadData(pcf2131_spi_sensorhandle_t *pSensorHandle,
                              const registerreadlist_t *pReadList,
                              uint8_t *pBuffer)
{
    int32_t status;

    /*! Validate for the correct handle and register read list.*/
    if ((pSensorHandle == NULL) || (pReadList == NULL) || (pBuffer == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before reading sensor data.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Parse through the read list and read the data one by one. */
    status = Sensor_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
                             pReadList, pBuffer);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_READ;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Deinit(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Trigger RTC device reset.*/
    status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				PCF2131_SR_RESET, PCF2131_TRIGGER_SW_RESET, PCF2131_RESET_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }
    else
    {
        /*! De-initialize sensor handle. */
        pSensorHandle->isInitialized = false;
    }

    /* Wait for MAX of TBOOT ms after soft reset command,
     * to allow enough time for FXLS8974AF to complete its internal boot sequence and be ready for communication. */
    BOARD_DELAY_ms(PCF2131_TBOOT_MAX);

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_SecInt_Enable(pcf2131_spi_sensorhandle_t *pSensorHandle, IntSrc intsrc)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Second Interrupt Enable */
    status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
                PCF2131_CTRL1, (uint8_t)(intEnable << PCF2131_CTRL1_SI_SHIFT ), PCF2131_CTRL1_SI_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    if( intsrc == IntA)   /*! clear Second Interrupt Mask on INTA */
    {
		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_INT_A_MASK1, PCF2131_SI_ENB_MASK_VALUE, PCF2131_REG_MASK);
    }
    else  /*! clear Second Interrupt Mask on INTB */
    {
    	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
   			PCF2131_INT_B_MASK1, PCF2131_SI_ENB_MASK_VALUE, PCF2131_REG_MASK);
    }
   	if (ARM_DRIVER_OK != status)
   	{
   		return SENSOR_ERROR_WRITE;
   	}

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_SecInt_Disable(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Second Interrupt Disable */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1,(uint8_t)(intDisable << PCF2131_CTRL1_SI_SHIFT ), PCF2131_CTRL1_SI_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}


int32_t PCF2131_SPI_MinInt_Enable(pcf2131_spi_sensorhandle_t *pSensorHandle, IntSrc intsrc)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Minute Interrupt Enable */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1, (uint8_t)(intEnable << PCF2131_CTRL1_MI_SHIFT ), PCF2131_CTRL1_MI_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

	if( intsrc == IntA)   /*! clear Minute Interrupt Mask on INTA */
	{
		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_INT_A_MASK1, PCF2131_MI_ENB_MASK_VALUE, PCF2131_REG_MASK);
	}
	else   /*! clear Minute Interrupt Mask on INTA */
	{
		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_INT_B_MASK1, PCF2131_MI_ENB_MASK_VALUE, PCF2131_REG_MASK);
	 }
	 if (ARM_DRIVER_OK != status)
	 {
		return SENSOR_ERROR_WRITE;
	 }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_MinInt_Disable(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Minute Interrupt Disable */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1,(uint8_t)(intDisable << PCF2131_CTRL1_MI_SHIFT ), PCF2131_CTRL1_MI_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_12h_24h_Mode_Set(pcf2131_spi_sensorhandle_t *pSensorHandle, Mode12h_24h is_mode12h)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Set 12/24 mode */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1, (uint8_t)(((is_mode12h == mode12H) ? mode12H : mode24H ) << PCF2131_CTRL1_12_HOUR_24_HOUR_MODE_SHIFT ), PCF2131_CTRL1_12_HOUR_24_HOUR_MODE_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_12h_24h_Mode_Get(pcf2131_spi_sensorhandle_t *pSensorHandle, Mode12h_24h *pmode_12_24)
{
	 int32_t status;
	 PCF2131_CTRL_1 Ctrl1_Reg;

	 /*! Validate for the correct handle */
	 if ((pSensorHandle == NULL) || (pmode_12_24 == NULL))
	 {
		return SENSOR_ERROR_INVALID_PARAM;
	 }

	 /*! Check whether sensor handle is initialized before triggering sensor reset.*/
	 if (pSensorHandle->isInitialized != true)
	 {
		return SENSOR_ERROR_INIT;
	 }

	 /*! Get 12/24 mode */
	 status = Register_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			 PCF2131_CTRL1, PCF2131_REG_SIZE_BYTE, (uint8_t *)&Ctrl1_Reg);
	 if (ARM_DRIVER_OK != status)
	 {
		return SENSOR_ERROR_READ;
	 }
	 *pmode_12_24 = Ctrl1_Reg.b.mode_12_24;

	 return SENSOR_ERROR_NONE;
}



int32_t PCF2131_SPI_Sec100TH_Mode(pcf2131_spi_sensorhandle_t *pSensorHandle, S100thMode is_s100h)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! 100th Second mode Enable/Disable */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1, (uint8_t)(((is_s100h == s100thEnable) ? s100thEnable : s100thDisable) << PCF2131_CTRL1_100TH_S_DIS_SHIFT ), PCF2131_CTRL1_100TH_S_DIS_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Sec100TH_Mode_Get(pcf2131_spi_sensorhandle_t *pSensorHandle, S100thMode *s100_mode)
{
    int32_t status;
    PCF2131_CTRL_1 Ctrl1_Reg;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Get 100th Second mode */
	status = Register_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_CTRL1, PCF2131_REG_SIZE_BYTE, (uint8_t *)&Ctrl1_Reg);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }
    *s100_mode = Ctrl1_Reg.b.s_dis_100th;

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Rtc_Start(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Start RTC source clock */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL1, (uint8_t)(rtcStart << PCF2131_CTRL1_START_STOP_SHIFT ), PCF2131_CTRL1_START_STOP_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}


int32_t PCF2131_SPI_Rtc_Stop(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Stop RTC source clock */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_CTRL1, (uint8_t)(rtcStop << PCF2131_CTRL1_START_STOP_SHIFT), PCF2131_CTRL1_START_STOP_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Clear_MinSecInt(pcf2131_spi_sensorhandle_t *pSensorHandle)
{
    int32_t status;

    /*! Validate for the correct handle */
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! clear Minute/Second interrupt flag */
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_CTRL2, (uint8_t)(intClear << PCF2131_CTRL2_MSF_SHIFT), PCF2131_CTRL2_MSF_MASK);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Check_MinSecInt(pcf2131_spi_sensorhandle_t *pSensorHandle, IntState *pIntStatus)
{
    int32_t status;
    PCF2131_CTRL_2 Ctrl2_Reg;

    /*! Validate for the correct handle and Interrupt status read variable.*/
    if ((pSensorHandle == NULL) || (pIntStatus == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! Get Minute/Second interrupt flag */
	status = Register_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL2, PCF2131_REG_SIZE_BYTE, (uint8_t *)&Ctrl2_Reg);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_READ;
    }
    *pIntStatus = Ctrl2_Reg.b.msf ;

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Clear_SW_TsInt(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum swtsnum)
{
    int32_t status;

    /*! Validate for the correct handle.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch (swtsnum)
	{
		case sw1Ts: /* Clear SW1 Time Stamp Interrupt */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL4, (uint8_t)(intClear << PCF2131_CTRL4_TSF1_SHIFT), PCF2131_CTRL4_TSF1_MASK);
				break;
		case sw2Ts: /* Clear SW2 Time Stamp Interrupt */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL4, (uint8_t)(intClear << PCF2131_CTRL4_TSF2_SHIFT), PCF2131_CTRL4_TSF2_MASK);
				break;
		case sw3Ts: /* Clear SW3 Time Stamp Interrupt */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL4, (uint8_t)(intClear << PCF2131_CTRL4_TSF3_SHIFT), PCF2131_CTRL4_TSF3_MASK);
				break;
		case sw4Ts: /* Clear SW4 Time Stamp Interrupt */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL4, (uint8_t)(intClear << PCF2131_CTRL4_TSF4_SHIFT), PCF2131_CTRL4_TSF4_MASK);;
				break;
		default:
				break;
	 }
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_Check_SW_TsInt(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum swtsnum, IntState *pSwIntState)
{
    int32_t status;
    PCF2131_CTRL_4 Ctrl4_Reg;

    /*! Validate for the correct handle and Interrupt state read variable.*/
    if ((pSensorHandle == NULL) || (pSwIntState == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

	/*! read control Register 4 */
	status = Register_SPI_Read(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
				PCF2131_CTRL4, PCF2131_REG_SIZE_BYTE, (uint8_t *)&Ctrl4_Reg);
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_READ;
    }

    switch (swtsnum)
    {
		case sw1Ts: /* Get SW1 Time Stamp Interrupt flag */
				*pSwIntState = Ctrl4_Reg.b.tsf1 ;
				break;
		case sw2Ts:  /* Get SW2 Time Stamp Interrupt flag */
				*pSwIntState = Ctrl4_Reg.b.tsf2 ;
				break;
		case sw3Ts:  /* Get SW3 Time Stamp Interrupt flag */
				*pSwIntState = Ctrl4_Reg.b.tsf3 ;
				break;
		case sw4Ts:  /* Get SW4 Time Stamp Interrupt flag */
				*pSwIntState = Ctrl4_Reg.b.tsf4 ;
				break;
		default:
				break;
    }
    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_SW_TsInt_Enable(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum swtsnum, IntSrc intsrc )
{
    int32_t status;

    /*! Validate for the correct handle.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(swtsnum)
    {
    	case sw1Ts:   /*! SW1 time stamp INT Enable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intEnable << PCF2131_CTRL5_TSIE1_SHIFT ), PCF2131_CTRL5_TSIE1_MASK);
			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}

			if( intsrc == IntA)   /*! Clear SW1 time stamp INTA */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_INT_A_MASK2, PCF2131_TS1_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}
			else  /*! Clear SW1 time stamp INTB */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
					PCF2131_INT_B_MASK2, PCF2131_TS1_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}

			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}
    		break;
    	case sw2Ts:  /*! SW2 time stamp INT Enable */
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intEnable << PCF2131_CTRL5_TSIE2_SHIFT ), PCF2131_CTRL5_TSIE2_MASK);
			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}

			if( intsrc == IntA)   /*! Clear SW2 time stamp INTA */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_INT_A_MASK2, PCF2131_TS2_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}
			else  /*! Clear SW2 time stamp INTB */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
					PCF2131_INT_B_MASK2, PCF2131_TS2_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}
			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}
			break;
    	case sw3Ts:   /*! SW3 time stamp INT Enable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intEnable << PCF2131_CTRL5_TSIE3_SHIFT ), PCF2131_CTRL5_TSIE3_MASK);
			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}

			if( intsrc == IntA)   /*! Clear SW3 time stamp INTA */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_INT_A_MASK2, PCF2131_TS3_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}
			else  /*! Clear SW3 time stamp INTB */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
					PCF2131_INT_B_MASK2, PCF2131_TS3_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}

			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}
			break;
    	case sw4Ts:  /*! SW4 time stamp INT Enable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intEnable << PCF2131_CTRL5_TSIE4_SHIFT ), PCF2131_CTRL5_TSIE4_MASK);
			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}

			if( intsrc == IntA)   /*! Clear SW4 time stamp INTA */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_INT_A_MASK2, PCF2131_TS4_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}
			else  /*! Clear SW4 time stamp INTB */
			{
				status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
					PCF2131_INT_B_MASK2, PCF2131_TS4_ENB_MASK_VALUE, PCF2131_REG_MASK);
			}

			if (ARM_DRIVER_OK != status)
			{
				 return SENSOR_ERROR_WRITE;
			}
			break;
    default:
		return SENSOR_ERROR_INVALID_PARAM;
    }
    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_PCF2131_SPI_SW_TsInt_Disable(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum swtsnum)
{
    int32_t status;

    /*! Validate for the correct handle.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(swtsnum)
	{
		case sw1Ts:   /*! SW1 time stamp INT Disable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intDisable << PCF2131_CTRL5_TSIE1_SHIFT ), PCF2131_CTRL5_TSIE1_MASK);
			break;
		case sw2Ts:  /*! SW2 time stamp INT Disable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intDisable << PCF2131_CTRL5_TSIE2_SHIFT), PCF2131_CTRL5_TSIE2_MASK);
			break;
		case sw3Ts:   /*! SW3 time stamp INT Disable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intDisable << PCF2131_CTRL5_TSIE3_SHIFT), PCF2131_CTRL5_TSIE3_MASK);
			break;
		case sw4Ts:  /*! SW4 time stamp INT Disable */
			status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
						PCF2131_CTRL5, (uint8_t)(intDisable << PCF2131_CTRL5_TSIE4_SHIFT), PCF2131_CTRL5_TSIE4_MASK);
			break;
		default:
			return SENSOR_ERROR_INVALID_PARAM;
	}
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_SwRst(pcf2131_spi_sensorhandle_t *pSensorHandle, SwRst swrst)
{
    int32_t status;

    /*! Validate for the correct handle.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(swrst)
    {
    	case CPR:  /*! Trigger clear prescaler.*/
    		 status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				 PCF2131_SR_RESET, PCF2131_CPR, PCF2131_RESET_MASK);
    		break;
    	case CTS:  /*! Trigger clear Time stamp.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    		    				 PCF2131_SR_RESET, PCF2131_CTS, PCF2131_RESET_MASK);
    	    break;
    	case SR:   /*! Trigger sensor device reset.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    		    				 PCF2131_SR_RESET, PCF2131_TRIGGER_SW_RESET, PCF2131_RESET_MASK);
    	    break;
    	default:
    		break;
    }
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    /* Wait for MAX of TBOOT ms after soft reset command,
     * to allow enough time for PCF3121 to complete its internal boot sequence and be ready for communication. */
    BOARD_DELAY_ms(PCF2131_TBOOT_MAX);

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_TimeStamp_On(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum tsnum)
{
    int32_t status;

    /*! Validate for the correct.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(tsnum)
    {
    	case sw1Ts:  /*! Enable SW1 Timestamp.*/
    		 status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				 PCF2131_TIMESTAMP1_CTL, (Ts_On << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    		break;
    	case sw2Ts: /*! Enable SW2 Timestamp.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				PCF2131_TIMESTAMP2_CTL, (Ts_On << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	case sw3Ts:   /*! Enable SW3 Timestamp.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				PCF2131_TIMESTAMP3_CTL, (Ts_On << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	case sw4Ts:  /*! Enable SW4 Timestamp.*/
    	    status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    	    		PCF2131_TIMESTAMP4_CTL, (Ts_On << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	default:
    		break;
    }
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_TimeStamp_Off(pcf2131_spi_sensorhandle_t *pSensorHandle, SwTsNum tsnum)
{
    int32_t status;

    /*! Validate for the correct handle.*/
    if (pSensorHandle == NULL)
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(tsnum)
    {
    	case sw1Ts:  /*! Disable SW1 Timestamp.*/
    		 status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				 PCF2131_TIMESTAMP1_CTL, (Ts_Off << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    		break;
    	case sw2Ts:  /*! Disable SW2 Timestamp.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				PCF2131_TIMESTAMP2_CTL, (Ts_Off << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	case sw3Ts:  /*! Disable SW3 Timestamp.*/
    		status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    				PCF2131_TIMESTAMP3_CTL, (Ts_Off << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	case sw4Ts:  /*! Disable SW4 Timestamp.*/
    	    status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    	    		PCF2131_TIMESTAMP4_CTL, (Ts_Off << PCF2131_TSOFF_TS_SHIFT ), PCF2131_TSOFF_TS_MASK);
    	    break;
    	default:
    		break;
    }
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_GetTs(pcf2131_spi_sensorhandle_t *pSensorHandle, const registerreadlist_t *pcf2131tsdata, SwTsNum tsnum, pcf2131_timestamp_t *timestamp)
{
    int32_t status;
    Mode12h_24h mode12_24;

    /*! Validate for the correct handle and time Stamp status read variable.*/
    if ((pSensorHandle == NULL) || (timestamp == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    switch(tsnum)
    {
    	case sw1Ts:  /*! Get Timestamp SW1 .*/
    		status = PCF2131_SPI_ReadData(pSensorHandle, pcf2131tsdata, (uint8_t *)timestamp);
    		break;
    	case sw2Ts: /*! Get Timestamp SW2 .*/
    		status = PCF2131_SPI_ReadData(pSensorHandle, pcf2131tsdata, (uint8_t *)timestamp);
    	    break;
    	case sw3Ts:   /*! Get Timestamp SW3 .*/
    		status = PCF2131_SPI_ReadData(pSensorHandle, pcf2131tsdata, (uint8_t *)timestamp);
    	    break;
    	case sw4Ts: /*! Get Timestamp SW4 .*/
    		status = PCF2131_SPI_ReadData(pSensorHandle, pcf2131tsdata, (uint8_t *)timestamp);
    	    break;
    	default:
    		break;
    }
    if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

    /*! after read convert BCD to Decimal */
    timestamp->second = BcdToDecimal(timestamp->second & PCF2131_SECONDS_TS_MASK) ;
    timestamp->minutes = BcdToDecimal(timestamp->minutes & PCF2131_MINUTES_TS_MASK) ;
    PCF2131_SPI_12h_24h_Mode_Get(pSensorHandle, &mode12_24); /*! Update AM/PM */
	if(mode12_24 ==  mode24H)
	{
		timestamp->hours = BcdToDecimal(timestamp->hours & PCF2131_HOURS_TS_MASK) ;
		timestamp->ampm = h24;
	}
	else
	{
		if( (timestamp->hours >> 5) & 0x01 )
			timestamp->ampm = PM;
		else
			timestamp->ampm = AM;
		timestamp->hours =  BcdToDecimal(timestamp->hours & PCF2131_HOURS_TS_MASK_12H);
	}
	timestamp->days = BcdToDecimal(timestamp->days & PCF2131_DAYS_TS_MASK);
	timestamp->months = BcdToDecimal(timestamp->months & PCF2131_MONTHS_TS_MASK);
	timestamp->years = BcdToDecimal(timestamp->years) ;

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_GetTime(pcf2131_spi_sensorhandle_t *pSensorHandle, const registerreadlist_t *pcf2131timedata, pcf2131_timedata_t *time )
{
    int32_t status;
    Mode12h_24h mode12_24;

    /*! Validate for the correct handle and time read variable.*/
    if ((pSensorHandle == NULL) || (time == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Get time.*/
	status = PCF2131_SPI_ReadData(pSensorHandle, pcf2131timedata, ( uint8_t *)time );
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    /*! after read convert BCD to Decimal */
    time->second_100th = BcdToDecimal(time->second_100th);
    time->second = BcdToDecimal(time->second & PCF2131_SECONDS_MASK) ;
    time->minutes = BcdToDecimal(time->minutes & PCF2131_MINUTES_MASK) ;
    PCF2131_SPI_12h_24h_Mode_Get(pSensorHandle, &mode12_24);
    if(mode12_24 ==  mode24H)
    {
    	time->hours = BcdToDecimal(time->hours & PCF2131_HOURS_MASK) ;
       time->ampm = h24;
    }
    else   /* Set AM/PM */
    {
    	if( (time->hours >> 5) & 0x01 )
    		time->ampm = PM;
    	else
    		time->ampm = AM;
    	time->hours =  BcdToDecimal(time->hours & PCF2131_HOURS_MASK_12H);
    }
    time->days = BcdToDecimal(time->days & PCF2131_DAYS_MASK);
    time->weekdays = BcdToDecimal(time->weekdays & PCF2131_WEEKDAYS_MASK);
    time->months = BcdToDecimal(time->months & PCF2131_MONTHS_MASK);
    time->years = BcdToDecimal(time->years) ;

    return SENSOR_ERROR_NONE;
}

int32_t PCF2131_SPI_SetTime(pcf2131_spi_sensorhandle_t *pSensorHandle, pcf2131_timedata_t *time)
{
    int32_t status;
    Mode12h_24h mode12_24;

    /*! Validate for the correct handle and time read variable.*/
    if ((pSensorHandle == NULL) || (time == NULL))
    {
        return SENSOR_ERROR_INVALID_PARAM;
    }

    /*! Check whether sensor handle is initialized before triggering sensor reset.*/
    if (pSensorHandle->isInitialized != true)
    {
        return SENSOR_ERROR_INIT;
    }

    /*! Set 100th Second.*/
    status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
    			PCF2131_100TH_SECOND, DecimaltoBcd(time->second_100th & PCF2131_SECONDS_100TH_MASK), PCF2131_SECONDS_100TH_MASK);
    if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

    /*! Set Second.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_SECOND, DecimaltoBcd(time->second & PCF2131_SECONDS_MASK), PCF2131_SECONDS_MASK);
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Set Minutes.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_MINUTE, DecimaltoBcd(time->minutes & PCF2131_MINUTES_MASK ), PCF2131_MINUTES_MASK );
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Update AM/PM Bit.*/
	if(time->ampm == AM)
	{
		time->hours = DecimaltoBcd(time->hours & PCF2131_HOURS_MASK_12H);
		time->hours = ((time->hours) & (~(PM << PCF2131_AM_PM_SHIFT)));
	}
	else if(time->ampm == PM)
	{
		time->hours = DecimaltoBcd(time->hours & PCF2131_HOURS_MASK_12H);
		time->hours = (time->hours | (PM << PCF2131_AM_PM_SHIFT));
	}
	else
		time->hours = DecimaltoBcd(time->hours & PCF2131_HOURS_MASK);

	/*! Set Hour.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_HOUR, time->hours , PCF2131_HOURS_MASK );
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Set Day.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_DAY, DecimaltoBcd(time->days & PCF2131_DAYS_MASK) , PCF2131_DAYS_MASK);
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Set WeekDay.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_WEEKEND, DecimaltoBcd(time->weekdays &  PCF2131_WEEKDAYS_MASK ), PCF2131_WEEKDAYS_MASK);
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Set Months.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_MONTH, DecimaltoBcd(time->months & PCF2131_MONTHS_MASK) , PCF2131_MONTHS_MASK );
	if (ARM_DRIVER_OK != status)
	{
		return SENSOR_ERROR_WRITE;
	}

	/*! Set Year.*/
	status = Register_SPI_Write(pSensorHandle->pCommDrv, &pSensorHandle->deviceInfo, &pSensorHandle->slaveParams,
			PCF2131_YEAR, DecimaltoBcd(time->years & PCF2131_YEARS_MASK) , PCF2131_YEARS_MASK );
    if (ARM_DRIVER_OK != status)
    {
        return SENSOR_ERROR_WRITE;
    }

    return SENSOR_ERROR_NONE;
}
