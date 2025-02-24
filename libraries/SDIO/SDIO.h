/**
 * \file
 * \brief Definitions for STM32 SDIO.
 * based on ST's https://github.com/stm32duino/STM32SD/blob/main/src/SDIO.c
 */

#ifndef __SDIO__
#define __SDIO__

#if defined(ARDUINO_ARCH_STM32)
#include "stm32_def.h"

#if defined(HAL_SD_MODULE_ENABLED) || defined(HAL_MMC_MODULE_ENABLED)

#include "core_debug.h"
#include "interrupt.h"
#include "PeripheralPins.h"
#include "stm32yyxx_ll_gpio.h"
#if defined(STM32F4)
#include "stm32F4xx_ll_sdmmc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "PinNames.h"
#include "variant.h"
#include "wiring_constants.h"

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

#ifndef SD_HW_FLOW_CTRL
  #define SD_HW_FLOW_CTRL          SD_HW_FLOW_CTRL_DISABLE
#endif

#ifndef SD_BUS_WIDE
  #define SD_BUS_WIDE              SD_BUS_WIDE_4B
#endif

#define SD_TRANSFER_OK                ((uint8_t)0x00)
#define SD_TRANSFER_BUSY              ((uint8_t)0x01)

#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION  <= 0x01060100)
#error "This library version required a STM32 core version > 1.6.1.\
Please update the core or install previous library version."
#endif

/* For backward compatibility */
#if defined(SD_TRANSCEIVER_MODE) && !defined(USE_SD_TRANSCEIVER)
#define USE_SD_TRANSCEIVER        1
#endif

/*SD Card information structure */

#define SDIO_CardInfo HAL_SD_CardInfoTypeDef
/* For backward compatibility */
#define SD_CardInfo SDIO_CardInfo
/*SD status structure definition */
#define MSD_OK                   ((uint8_t)0x00)
#define MSD_ERROR                ((uint8_t)0x01)
#define MSD_ERROR_SD_NOT_PRESENT ((uint8_t)0x02)

/* SD Exported Constants */
#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)
#define SD_DETECT_NONE           NUM_DIGITAL_PINS

/* Could be redefined in variant.h or using build_opt.h */
#ifndef SD_DATATIMEOUT
#define SD_DATATIMEOUT         100000000U
#endif

#if defined(USE_SD_TRANSCEIVER) && (USE_SD_TRANSCEIVER != 0U)
#ifndef SD_TRANSCEIVER_EN
#define SD_TRANSCEIVER_EN        NUM_DIGITAL_PINS
#endif

#ifndef SD_TRANSCEIVER_SEL
#define SD_TRANSCEIVER_SEL       NUM_DIGITAL_PINS
#endif
#endif

#ifndef GPIO_PIN_All
#define GPIO_PIN_All GPIO_PIN_ALL
#endif

/* Workaround while core does not defined *_NA for SDMMCx signals availability */
#if defined(SDMMC1) || defined(SDMMC2)
#if defined(STM32L4P5xx) || defined(STM32L4Q5xx) || defined(STM32L4R5xx) || defined(STM32L4R7xx) ||\
    defined(STM32L4R9xx) || defined(STM32L4S5xx) || defined(STM32L4S7xx) || defined(STM32L4S9xx)
#define STM32L4xx_PLUS
#endif
#if defined(STM32F7xx) || (defined(STM32L4xx) && !defined(STM32L4xx_PLUS))
#if !defined(SDMMC_CKIN_NA)
#define SDMMC_CKIN_NA
#endif
#if !defined(SDMMC_CDIR_NA)
#define SDMMC_CDIR_NA
#endif
#if !defined(SDMMC_D0DIR_NA)
#define SDMMC_D0DIR_NA
#endif
#if !defined(SDMMC_D123DIR_NA)
#define SDMMC_D123DIR_NA
#endif
#endif /* STM32F7xx || STM32L4xx_PLUS */
#endif /* SDMMC1 || SDMMC2 */

/* Default SDx pins definitions */
#ifndef SDX_D0
#define SDX_D0           PNUM_NOT_DEFINED
#endif
#ifndef SDX_D1
#define SDX_D1           PNUM_NOT_DEFINED
#endif
#ifndef SDX_D2
#define SDX_D2           PNUM_NOT_DEFINED
#endif
#ifndef SDX_D3
#define SDX_D3           PNUM_NOT_DEFINED
#endif
#ifndef SDX_CMD
#define SDX_CMD          PNUM_NOT_DEFINED
#endif
#ifndef SDX_CK
#define SDX_CK           PNUM_NOT_DEFINED
#endif
#if defined(SDMMC1) || defined(SDMMC2)
#ifndef SDX_CKIN
#define SDX_CKIN         PNUM_NOT_DEFINED
#endif
#ifndef SDX_CDIR
#define SDX_CDIR         PNUM_NOT_DEFINED
#endif
#ifndef SDX_D0DIR
#define SDX_D0DIR        PNUM_NOT_DEFINED
#endif
#ifndef SDX_D123DIR
#define SDX_D123DIR      PNUM_NOT_DEFINED
#endif
#endif /* SDMMC1 || SDMMC2 */

#ifndef MAKE_REG_MASK
#define MAKE_REG_MASK(m, s) (((uint32_t)(((uint32_t)(m) << (s)))))
#endif
#ifndef MAKE_REG_GET
#define MAKE_REG_GET(x, m, s) (((uint32_t)(((uint32_t)(x) >> (s)) & (m))))
#endif
#ifndef MAKE_REG_SET
#define MAKE_REG_SET(x, m, s) (((uint32_t)(((uint32_t)(x) & (m)) << (s))))
#endif

typedef struct {
  PinName pin_d0;
  PinName pin_d1;
  PinName pin_d2;
  PinName pin_d3;
  PinName pin_cmd;
  PinName pin_ck;
#if defined(SDMMC1) || defined(SDMMC2)
  PinName pin_ckin;
  PinName pin_cdir;
  PinName pin_d0dir;
  PinName pin_d123dir;
#endif
} SD_PinName_t;

extern SD_PinName_t SD_PinNames;

/* SD Exported Functions */
uint8_t SDIO_Init(bool useDma = false);
uint8_t SDIO_DeInit(void);
#if defined(USE_SD_TRANSCEIVER) && (USE_SD_TRANSCEIVER != 0U)
uint8_t SDIO_TransceiverPin(GPIO_TypeDef *enport, uint32_t enpin, GPIO_TypeDef *selport, uint32_t selpin);
#endif
uint8_t SDIO_DetectPin(PinName p, uint32_t level);
uint8_t SDIO_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t SDIO_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t SDIO_Erase(uint64_t StartAddr, uint64_t EndAddr);
uint8_t SDIO_GetCardState(void);
bool    SDIO_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
uint8_t SDIO_IsDetected(void);

/* These __weak function can be surcharged by application code in case the current settings (e.g. DMA stream)
   need to be changed for specific needs */
void    SDIO_MspInit(SD_HandleTypeDef *hsd, void *Params);
void    SDIO_MspDeInit(SD_HandleTypeDef *hsd, void *Params);
void    SDIO_Detect_MspInit(SD_HandleTypeDef *hsd, void *Params);
void    SDIO_Detect_MspDeInit(SD_HandleTypeDef *hsd, void *Params);
#if defined(USE_SD_TRANSCEIVER) && (USE_SD_TRANSCEIVER != 0U)
void    SDIO_Transceiver_MspInit(SD_HandleTypeDef *hsd, void *Params);
void    SDIO_Transceiver_MspDeInit(SD_HandleTypeDef *hsd, void *Params);
#endif

#ifdef __cplusplus
}
#endif

#endif  // defined(HAL_SD_MODULE_ENABLED) || defined(HAL_MMC_MODULE_ENABLED)
#endif  // defined(ARDUINO_ARCH_STM32)
#endif  // __SDIO__
