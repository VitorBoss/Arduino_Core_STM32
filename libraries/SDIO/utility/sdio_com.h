/*
 * Copyright (c) 2016 Frederic Pillon <frederic.pillon@st.com> for
 * STMicroelectronics. All right reserved.
 * Header utility of the sd module for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDIO_COM_H
#define __SDIO_COM_H

/* Includes ------------------------------------------------------------------*/
#include "stm32_def.h"
#include "PeripheralPins.h"

#ifdef __cplusplus
extern "C" {
#endif


///@brief specifies the SDIO speed bus in HZ.
#define SDIO_SPEED_CLOCK_DEFAULT     25000000

/* Definition for BSP SD */
#if defined(SDMMC1) || defined(SDMMC2)
  #ifndef SD_INSTANCE
    #if defined(SDMMC1)
      #define SD_INSTANCE              SDMMC1
      #define IRQ_SDHC                 SDMMC1_IRQn
    #else
      #define SD_INSTANCE              SDMMC2
      #define IRQ_SDHC                 SDMMC2_IRQn
    #endif
  #endif

  #define SD_CLK_EDGE              SDMMC_CLOCK_EDGE_RISING
  #if defined(SDMMC_CLOCK_BYPASS_DISABLE)
    #define SD_CLK_BYPASS            SDMMC_CLOCK_BYPASS_DISABLE
  #endif
  #define SD_CLK_PWR_SAVE          SDMMC_CLOCK_POWER_SAVE_DISABLE
  #define SD_BUS_WIDE_1B           SDMMC_BUS_WIDE_1B
  #define SD_BUS_WIDE_4B           SDMMC_BUS_WIDE_4B
  #define SD_BUS_WIDE_8B           SDMMC_BUS_WIDE_8B
  #define SD_HW_FLOW_CTRL_ENABLE   SDMMC_HARDWARE_FLOW_CONTROL_ENABLE
  #define SD_HW_FLOW_CTRL_DISABLE  SDMMC_HARDWARE_FLOW_CONTROL_DISABLE

  #ifndef SD_CLK_DIV
    #if defined(SDMMC_TRANSFER_CLK_DIV)
      #define SD_CLK_DIV               SDMMC_TRANSFER_CLK_DIV
    #else
      #define SD_CLK_DIV               SDMMC_NSpeed_CLK_DIV
    #endif
  #endif

  #if defined(USE_SD_TRANSCEIVER) && (USE_SD_TRANSCEIVER != 0U)
    #if defined(SDMMC_TRANSCEIVER_ENABLE)
      #define SD_TRANSCEIVER_ENABLE    SDMMC_TRANSCEIVER_ENABLE
      #define SD_TRANSCEIVER_DISABLE   SDMMC_TRANSCEIVER_DISABLE
    #else
      #define SD_TRANSCEIVER_ENABLE    SDMMC_TRANSCEIVER_PRESENT
      #define SD_TRANSCEIVER_DISABLE   SDMMC_TRANSCEIVER_NOT_PRESENT
    #endif
  #endif

#elif defined(SDIO)
  #define SD_INSTANCE              SDIO
  #define IRQ_SDHC                 SDMMC1_IRQn
  #define SD_CLK_EDGE              SDIO_CLOCK_EDGE_RISING
  #if defined(SDIO_CLOCK_BYPASS_DISABLE)
    #define SD_CLK_BYPASS            SDIO_CLOCK_BYPASS_DISABLE
  #endif
  #define SD_CLK_PWR_SAVE          SDIO_CLOCK_POWER_SAVE_DISABLE
  #define SD_BUS_WIDE_1B           SDIO_BUS_WIDE_1B
  #define SD_BUS_WIDE_4B           SDIO_BUS_WIDE_4B
  #define SD_BUS_WIDE_8B           SDIO_BUS_WIDE_8B
  #define SD_HW_FLOW_CTRL_ENABLE   SDIO_HARDWARE_FLOW_CONTROL_ENABLE
  #define SD_HW_FLOW_CTRL_DISABLE  SDIO_HARDWARE_FLOW_CONTROL_DISABLE
  #ifndef SD_CLK_DIV
    #define SD_CLK_DIV               SDIO_TRANSFER_CLK_DIV
  #endif
#else
  #error "Unknown SD_INSTANCE"
#endif

// Defines a default timeout delay in milliseconds for the SDIO transfer
#ifndef SD_TRANSFER_TIMEOUT
#define SD_TRANSFER_TIMEOUT 1000
#elif SD_TRANSFER_TIMEOUT <= 0
#error "SDIO_TRANSFER_TIMEOUT cannot be less or equal to 0!"
#endif

///@brief specifies the SDIO mode to use
//Mode SDIO_MODE_1B: Used pin D0, CMD and SCK
//Mode SDIO_MODE_4B: Used pin D0, D1, D2, D3, CMD and SCK
typedef enum {
    SDIO_MODE_1B = 0,
    SDIO_MODE_4B = 1,
#if defined(SDIO_BUS_WIDE_8B)
    SDIO_MODE_8B = 2,
#endif
} SDIOMode;

///@brief SDIO errors
typedef enum {
    SD_OK = 0,
    SD_TIMEOUT = 1,
    SD_ERROR = 2
} sd_status_e;

/* Exported types ------------------------------------------------------------*/
struct sd_s {
  SD_HandleTypeDef handle;
  SD_TypeDef *sd;
  PinName pin_d0;
  PinName pin_d1;
  PinName pin_d2;
  PinName pin_d3;
  PinName pin_cmd;
  PinName pin_sck;
  #if defined(SDMMC1) || defined(SDMMC2)
  PinName pin_ckin;
  PinName pin_cdir;
  PinName pin_d0dir;
  PinName pin_d123dir;
  #endif
  GPIO_TypeDef *SD_detect_gpio_port;
  uint32_t SD_detect_ll_gpio_pin;
  uint32_t SD_detect_level;
  bool useDMA;
  SDIOMode busMode;
  };
  typedef struct sd_s sd_t;
    
  
/* Exported functions ------------------------------------------------------- */
void sd_init(sd_t *obj, uint32_t speed, SDIOMode mode, uint8_t dmaEnabled);
void sd_deinit(sd_t *obj);
sd_status_e sd_read(sd_t *obj, const uint8_t *rx_buffer, uint32_t address, uint16_t len);
sd_status_e sd_write(sd_t *obj, const uint8_t *tx_buffer, uint32_t address, uint16_t len);
sd_status_e sd_erase(sd_t *obj, uint32_t start_address, uint32_t end_address);
uint32_t sd_getClkFreq(sd_t *obj);
bool sd_setDetectionInput(sd_t *obj, PinName p, uint32_t level);
bool sd_setDetectionInterrupt(sd_t *obj, void (*callback)(void));
bool sd_IsDetected(sd_t *obj);
bool sd_IsBusy(sd_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* __SDIO_COM_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
