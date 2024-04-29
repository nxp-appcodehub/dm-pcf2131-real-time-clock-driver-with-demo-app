/*
 * Copyright 2022, 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! File: frdm_stbi_pcf2131_shield.h
* @brief The frdm_stbi_pcf2131_shield.h file declares arduino pin mapping for frdm_stbi_pcf2131_shield expansion board.
*/

#ifndef _FRDM_PCF2131_SHIELD_H_
#define _FRDM_PCF2131_SHIELD_H_

/* The shield name */
#define SHIELD_NAME "FRDM-PCF2131"

// PCF2131 RTC Information

#define PCF2131_CS       D10
#define PCF2131_MOSI     D11
#define PCF2131_MISO     D12
#define PCF2131_SCLK     D13

#define PCF2131_I2C_ADDR 0x53

#endif /* _FRDM_PCF2131_SHIELD_H_ */
