/*
 * Copyright (c) 2016 Frederic Pillon <frederic.pillon@st.com> for
 * STMicroelectronics. All right reserved.
 * Interface utility of the sd module for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */
#include "wiring_time.h"
#include "core_debug.h"
#include "stm32_def.h"
#include "utility/sdio_com.h"
#include "PinAF_STM32F1.h"
#include "pinconfig.h"
#include "stm32yyxx_ll_sdmmc.h"
#include "sdio_com.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Initializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
 __weak void SDIO_MspInit(sd_t *obj, void *Params)
 {
   UNUSED(Params);
   SD_HandleTypeDef *hsd = obj->sd;
 #if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION <= 0x02050000)
   /* Configure SD GPIOs */
   const PinMap *map = PinMap_SD;
   while (map->pin != NC) {
     pin_function(map->pin, map->function);
     map++;
   }
 #else
   /* Configure SD GPIO pins */
   pinmap_pinout(obj->pin_d0, PinMap_SD_DATA0);
   if (obj->busMode == SDIO_MODE_4B) {
     pinmap_pinout(obj->pin_d1, PinMap_SD_DATA1);
     pinmap_pinout(obj->pin_d2, PinMap_SD_DATA2);
     pinmap_pinout(obj->pin_d3, PinMap_SD_DATA3);
   }
   pinmap_pinout(obj->pin_cmd, PinMap_SD_CMD);
   pinmap_pinout(obj->pin_sck, PinMap_SD_CK);
 #if defined(SDMMC1) || defined(SDMMC2)
 #if !defined(SDMMC_CKIN_NA)
   if (obj->pin_ckin != NC) {
     pinmap_pinout(obj->pin_ckin, PinMap_SD_CKIN);
   }
 #endif
 #if !defined(SDMMC_CDIR_NA)
   if (obj->pin_cdir != NC) {
     pinmap_pinout(obj->pin_cdir, PinMap_SD_CDIR);
   }
 #endif
 #if !defined(SDMMC_D0DIR_NA)
   if (obj->pin_d0dir != NC) {
     pinmap_pinout(obj->pin_d0dir, PinMap_SD_D0DIR);
   }
 #endif
 #if !defined(SDMMC_D123DIR_NA)
   if (obj->pin_d123dir != NC) {
     pinmap_pinout(obj->pin_d123dir, PinMap_SD_D123DIR);
   }
 #endif
 #endif /* SDMMC1 || SDMMC2 */
 #endif /* !STM32_CORE_VERSION || (STM32_CORE_VERSION <= 0x02050000) */
   /* Enable SD clock */
 #if defined(SDMMC1) || defined(SDMMC2)
 #if defined(SDMMC1)
   if (hsd->Instance == SDMMC1) {
     __HAL_RCC_SDMMC1_CLK_ENABLE();
   }
 #endif
 #if defined(SDMMC2)
   if (hsd->Instance == SDMMC2) {
     __HAL_RCC_SDMMC2_CLK_ENABLE();
   }
 #endif
 #else
   UNUSED(hsd);
   __HAL_RCC_SDIO_CLK_ENABLE();
 #endif
 }
 
 /**
   * @brief  DeInitializes the SD MSP.
   * @param  hsd: SD handle
   * @param  Params : pointer on additional configuration parameters, can be NULL.
   */
 __weak void SDIO_MspDeInit(sd_t * obj, void *Params)
 {
   UNUSED(Params);
   /* DeInit GPIO pins can be done in the application
      (by surcharging this __weak function) */
 #if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION <= 0x02050000)
   const PinMap *map = PinMap_SD;
   while (map->pin != NC) {
     HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(map->pin), STM_GPIO_PIN(map->pin));
     map++;
   }
 #else
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d0), STM_GPIO_PIN(obj->pin_d0));
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d1), STM_GPIO_PIN(obj->pin_d1));
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d2), STM_GPIO_PIN(obj->pin_d2));
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d3), STM_GPIO_PIN(obj->pin_d3));
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_cmd), STM_GPIO_PIN(obj->pin_cmd));
   HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_sck), STM_GPIO_PIN(obj->pin_sck));
 #if defined(SDMMC1) || defined(SDMMC2)
 #if !defined(SDMMC_CKIN_NA)
   if (obj->pin_ckin != NC) {
     HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_ckin), STM_GPIO_PIN(obj->pin_ckin));
   }
 #endif
 #if !defined(SDMMC_CDIR_NA)
   if (obj->pin_cdir != NC) {
     HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_cdir), STM_GPIO_PIN(obj->pin_cdir));
   }
 #endif
 #if !defined(SDMMC_D0DIR_NA)
   if (obj->pin_d0dir != NC) {
     HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d0dir), STM_GPIO_PIN(obj->pin_d0dir));
   }
 #endif
 #if !defined(SDMMC_D123DIR_NA)
   if (obj->pin_d123dir != NC) {
     HAL_GPIO_DeInit((GPIO_TypeDef *)STM_PORT(obj->pin_d123dir), STM_GPIO_PIN(obj->pin_d123dir));
   }
 #endif
 #endif /* SDMMC1 || SDMMC2 */
 #endif /* !STM32_CORE_VERSION || (STM32_CORE_VERSION <= 0x02050000) */
 
   /* Disable SD clock */
 #if defined(SDMMC1) || defined(SDMMC2)
 #if defined(SDMMC1)
   if (hsd->Instance == SDMMC1) {
     __HAL_RCC_SDMMC1_CLK_DISABLE();
   }
 #endif
 #if defined(SDMMC2)
   if (hsd->Instance == SDMMC2) {
     __HAL_RCC_SDMMC2_CLK_DISABLE();
   }
 #endif
 #else
   __HAL_RCC_SDIO_CLK_DISABLE();
 #endif
 }
 
 /**
  * @brief  Initializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
 __weak void SDIO_Detect_MspInit(sd_t * obj, void *Params)
 {
   UNUSED(Params);

   /* GPIO configuration in input for uSD_Detect signal */
 #ifdef LL_GPIO_SPEED_FREQ_VERY_HIGH
   LL_GPIO_SetPinSpeed(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
 #else
   LL_GPIO_SetPinSpeed(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_SPEED_FREQ_HIGH);
 #endif
   LL_GPIO_SetPinMode(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_MODE_INPUT);
   LL_GPIO_SetPinPull(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_PULL_UP);
 }
 
 /**
  * @brief  DeInitializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
 __weak void SDIO_Detect_MspDeInit(sd_t * obj, void *Params)
 {
   UNUSED(Params);

   /* GPIO configuration in analog to saves the consumption */
   LL_GPIO_SetPinSpeed(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_SPEED_FREQ_LOW);
 #ifndef LL_GPIO_PULL_NO
   /* For STM32F1xx */
   LL_GPIO_SetPinPull(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_MODE_FLOATING);
 #else
   LL_GPIO_SetPinPull(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_PULL_NO);
 #endif
   LL_GPIO_SetPinMode(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_MODE_ANALOG);
 }

/* Private Functions */
/**
  * @brief  return clock freq of an SD instance
  * @param  sd_inst : SD instance
  * @retval clock freq of the instance else SystemCoreClock
  */
uint32_t sd_getClkFreqInst(SD_TypeDef *sd_inst)
{
  uint32_t sd_freq = SystemCoreClock;
  if (sd_inst != NP) {
#if defined(STM32C0xx) || defined(STM32F0xx) || defined(STM32G0xx) || \
    defined(STM32U0xx)
    /* SDx source CLK is PCKL1 */
    sd_freq = HAL_RCC_GetPCLK1Freq();
#else
#if defined(SDIO_BASE)
    if (sd_inst == SDIO) {
#if defined(RCC_PERIPHCLK_SDIO)
      sd_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDIO);
      if (sd_freq == 0)
#endif
      {
        sd_freq = HAL_RCC_GetPCLK2Freq();
      }
    }
#endif // SDIO_BASE
#if defined(SDMMC_BASE)
    if (sd_inst == SDMMC) {
#if defined(RCC_PERIPHCLK_SDMMC)
#ifdef RCC_PERIPHCLK_SDMMC
      sd_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC);
#endif
      if (sd_freq == 0)
#endif
      {
        sd_freq = HAL_RCC_GetPCLK1Freq();
      }
    }
#endif // SDMMC_BASE
#if defined(SDMMC1_BASE)
    if (sd_inst == SDMMC1) {
#if defined(RCC_PERIPHCLK_SDMMC1)
#ifdef RCC_PERIPHCLK_SDMMC1
      sd_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC1);
#endif
      if (sd_freq == 0)
#endif
      {
        sd_freq = HAL_RCC_GetPCLK1Freq();
      }
    }
#endif // SDMMC1_BASE
#if defined(SDMMC2_BASE)
    if (sd_inst == SDMMC2) {
#if defined(RCC_PERIPHCLK_SDMMC2)
#ifdef RCC_PERIPHCLK_SDMMC2
      sd_freq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC2);
#endif
      if (sd_freq == 0)
#endif
      {
        sd_freq = HAL_RCC_GetPCLK1Freq();
      }
    }
#endif // SDMMC2_BASE

#endif
  }
  return sd_freq;
}

/**
  * @brief  return clock freq of an SD instance
  * @param  obj : pointer to sd_t structure
  * @retval clock freq of the instance else SystemCoreClock
  */
uint32_t sd_getClkFreq(sd_t *obj)
{
  SD_TypeDef *sd_inst = NP;
  uint32_t sd_freq = SystemCoreClock;

  if (obj != NULL) {
    sd_inst = pinmap_peripheral(obj->pin_sck, PinMap_SD_CK);

    if (sd_inst != NP) {
      sd_freq = sd_getClkFreqInst(sd_inst);
    }
  }
  return sd_freq;
}

/**
  * @brief  SD initialization function
  * @param  obj : pointer to sd_t structure
  * @param  speed : sd output speed
  * @param  mode : one of the sd modes
  * @param  msb : set to 1 in msb first
  * @retval None
  */
void sd_init(sd_t *obj, uint32_t speed, SDIOMode mode, uint8_t msb)
{
  int8_t sd_state;
  if (obj == NULL) {
    return;
  }

  SD_HandleTypeDef *handle = &(obj->handle);
  uint32_t sd_freq = 0;
  uint32_t pull = 0;

  SD_TypeDef *sd_d0 = NP;
  SD_TypeDef *sd_d1 = NP;
  SD_TypeDef *sd_d2 = NP;
  SD_TypeDef *sd_d3 = NP;
  SD_TypeDef *sd_cmd = NP;
  SD_TypeDef *sd_sck = NP;

  /* If a pin is not defined, use the first pin available in the associated PinMap_SD_* arrays */
  if (obj->pin_d0 == NC) {
    obj->pin_d0 = PinMap_SD_DATA0[0].pin;
    if (obj->busMode == SDIO_MODE_4B) {
      obj->pin_d1 = PinMap_SD_DATA1[0].pin;
      obj->pin_d2 = PinMap_SD_DATA2[0].pin;
      obj->pin_d3 = PinMap_SD_DATA3[0].pin;
    }
  }
  if (obj->pin_cmd == NC) {
    obj->pin_cmd = PinMap_SD_CMD[0].pin;
  }
  if (obj->pin_sck == NC) {
    obj->pin_sck = PinMap_SD_CK[0].pin;
  }
#if defined(SDMMC1) || defined(SDMMC2)
#if !defined(SDMMC_CKIN_NA)
  if (obj->pin_ckin == NC) {
    obj->pin_ckin = PinMap_SD_CKIN[0].pin;
  }
#endif
#if !defined(SDMMC_CDIR_NA)
  if (obj->pin_cdir == NC) {
    obj->pin_cdir = PinMap_SD_CDIR[0].pin;
  }
#endif
#if !defined(SDMMC_D0DIR_NA)
  if (obj->pin_d0dir == NC) {
    obj->pin_d0dir = PinMap_SD_D0DIR[0].pin;
  }
#endif
#if !defined(SDMMC_D123DIR_NA)
  if (obj->pin_d123dir == NC) {
    obj->pin_d123dir = PinMap_SD_D123DIR[0].pin;
  }
#endif
#endif /* SDMMC1 || SDMMC2 */

  // Determine the SD to use
  /* Get SD instance from pins */
  sd_d0 = pinmap_peripheral(obj->pin_d0, PinMap_SD_DATA0);
  if (obj->busMode == SDIO_MODE_4B) {
    sd_d1 = pinmap_peripheral(obj->pin_d1, PinMap_SD_DATA1);
    sd_d2 = pinmap_peripheral(obj->pin_d2, PinMap_SD_DATA2);
    sd_d3 = pinmap_peripheral(obj->pin_d3, PinMap_SD_DATA3);  
  }
  sd_cmd = pinmap_peripheral(obj->pin_cmd, PinMap_SD_CMD);
  sd_sck = pinmap_peripheral(obj->pin_sck, PinMap_SD_CK);

  /* Pins Dx/cmd/CK must not be NP. */
  if (sd_d0 == NP || ((obj->busMode == SDIO_MODE_4B) && (sd_d1 == NP || sd_d2 == NP || sd_d3 == NP)) ||
      sd_cmd == NP || sd_sck == NP) {
    core_debug("ERROR: at least one SD pin has no peripheral\n");
  } else {
    SD_TypeDef *sd_d01 = pinmap_merge_peripheral(sd_d0, sd_d1);
    SD_TypeDef *sd_d23 = pinmap_merge_peripheral(sd_d2, sd_d3);
    SD_TypeDef *sd_cx = pinmap_merge_peripheral(sd_cmd, sd_sck);
    SD_TypeDef *sd_dx = pinmap_merge_peripheral(sd_d01, sd_d23);
    SD_TypeDef *sd_base = pinmap_merge_peripheral(sd_dx, sd_cx);
    if (sd_d01 == NP  || ((obj->busMode == SDIO_MODE_4B) && (sd_d23 == NP)) ||
        sd_cx == NP || sd_dx == NP || sd_base == NP) {
      core_debug("ERROR: SD pins mismatch\n");
      return;
    }
    obj->sd = sd_base;
#if defined(SDMMC1) || defined(SDMMC2)
#if !defined(SDMMC_CKIN_NA)
    if (obj->pin_ckin != NC) {
      SD_TypeDef *sd_ckin = pinmap_peripheral(obj->pin_ckin, PinMap_SD_CKIN);
      if (pinmap_merge_peripheral(sd_ckin, sd_base) == NP) {
        core_debug("ERROR: SD CKIN pin mismatch\n");
        return;
    }
    }
#endif
#if !defined(SDMMC_CDIR_NA)
    if ((res && obj->pin_cdir != NC)) {
      SD_TypeDef *sd_cdir = pinmap_peripheral(obj->pin_cdir, PinMap_SD_CDIR);
      if (pinmap_merge_peripheral(sd_cdir, sd_base) == NP) {
        core_debug("ERROR: SD CDIR pin mismatch\n");
        return;
    }
    }
#endif
#if !defined(SDMMC_D0DIR_NA)
    if (res && (obj->pin_cdir != NC)) {
      SD_TypeDef *sd_d0dir = pinmap_peripheral(obj->pin_d0dir, PinMap_SD_D0DIR);
      if (pinmap_merge_peripheral(sd_d0dir, sd_base) == NP) {
        core_debug("ERROR: SD DODIR pin mismatch\n");
        return;
    }
    }
#endif
#if !defined(SDMMC_D123DIR_NA)
    if (res && (obj->pin_cdir != NC)) {
      SD_TypeDef *sd_d123dir = pinmap_peripheral(obj->pin_d123dir, PinMap_SD_D123DIR);
      if (pinmap_merge_peripheral(sd_d123dir, sd_base) == NP) {
        core_debug("ERROR: SD D123DIR pin mismatch\n");
        return;
    }
    }
#endif
#endif /* SDMMC1 || SDMMC2 */
    /* Are all pins connected to the same SDx instance? */
    if (&obj->handle == NP) {
      core_debug("ERROR: SD pins mismatch\n");
      return;
    }
  }

  /* Fill default value */
  obj->handle.Instance               = obj->sd;
  //handle->Init.Mode              = SD_MODE_MASTER;

  sd_freq = sd_getClkFreqInst(&obj->handle);
  handle->Init.ClockDiv = (sd_freq / speed) - 1;

  /* Check if SD is not yet initialized */
  if (handle->State == HAL_SD_STATE_RESET) {
    /* uSD device interface configuration */
#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION <= 0x02050000)
    obj->.sd = SD_INSTANCE;
#else
    if (!SDIO_GetInstance()) {
      sd_state = SD_ERROR;
    }
#endif /* !STM32_CORE_VERSION || (STM32_CORE_VERSION <= 0x02050000) */

    handle->Init.ClockEdge           = SD_CLK_EDGE;
#if defined(SD_CLK_BYPASS)
    handle->Init.ClockBypass         = SD_CLK_BYPASS;
#endif
    handle->Init.ClockPowerSave      = SD_CLK_PWR_SAVE;
    handle->Init.BusWide             = SD_BUS_WIDE_1B;
    handle->Init.HardwareFlowControl = SD_HW_FLOW_CTRL;
    handle->Init.ClockDiv            = SD_CLK_DIV;

    if ((sd_state == SD_OK) && (obj->SD_detect_ll_gpio_pin != LL_GPIO_PIN_ALL)) {
      /* Msp SD Detect pin initialization */
      SDIO_Detect_MspInit(obj, NULL);
      if (sd_IsDetected(obj)) { /* Check if SD card is present */
        sd_state = SD_ERROR;
      }
    }
    if (sd_state == SD_OK) {
      /* Msp SD initialization */
      SDIO_MspInit(obj, NULL);

      /* HAL SD initialization */
      if (HAL_SD_Init(&obj->handle) != HAL_OK) {
        sd_state = SD_ERROR;
      }

      /* Enable DMA if user requested */
      if ((sd_state == SD_OK) && obj->useDMA) {
        DMA_HandleTypeDef uSdDma = { 0 };
        /* Required power up waiting time before starting the SD initialization  sequence */
        HAL_Delay(2);
        __DMA2_CLK_ENABLE();
        uSdDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
        uSdDma.Init.PeriphInc = DMA_PINC_DISABLE;
        uSdDma.Init.MemInc = DMA_MINC_ENABLE;
        uSdDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        uSdDma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        uSdDma.Init.Mode = DMA_PFCTRL;
        uSdDma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        uSdDma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
        uSdDma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        uSdDma.Init.MemBurst = DMA_MBURST_INC4;
        uSdDma.Init.PeriphBurst = DMA_PBURST_INC4;

        uSdDma.Instance = DMA2_Stream6;
        uSdDma.Init.Channel = DMA_CHANNEL_4;

        HAL_DMA_DeInit(&uSdDma);

        HAL_DMA_Init(&uSdDma);
        __HAL_LINKDMA(&obj->handle, hdmatx, uSdDma);

        uSdDma.Init.Direction = DMA_PERIPH_TO_MEMORY;
        __HAL_LINKDMA(&obj->handle, hdmarx, uSdDma);
      }

      /* Configure SD Bus width */
      if ((sd_state == SD_OK)  && (obj->busMode != SDIO_MODE_1B)) {
        /* Enable wide operation */
        uint32_t mode = SDIO_BUS_WIDE_4B;
#if defined(SDIO_BUS_WIDE_8B)
        if (obj->busMode == SDIO_MODE_8B) {
          mode = SDIO_BUS_WIDE_8B;
        }
#endif
        if (HAL_SD_ConfigWideBusOperation(&obj->handle, mode) != HAL_OK) {
          sd_state = SD_ERROR;
        }
      }
    }
  }
#if defined SDIO_BASE
  // Enable SD clock
  if (obj->handle.Instance == SDIO) {
    __HAL_RCC_SDIO_CLK_ENABLE();
    __HAL_RCC_SDIO_FORCE_RESET();
    __HAL_RCC_SDIO_RELEASE_RESET();
  }
#endif

#if defined SDMMC_BASE
  if (obj->handle.Instance == SDMMC) {
    __HAL_RCC_SDMMC_CLK_ENABLE();
    __HAL_RCC_SDMMC_FORCE_RESET();
    __HAL_RCC_SDMMC_RELEASE_RESET();
  }
#endif

#if defined SDMMC1_BASE
  if (obj->handle.Instance == SDMMC1) {
    __HAL_RCC_SDMMC1_CLK_ENABLE();
    __HAL_RCC_SDMMC1_FORCE_RESET();
    __HAL_RCC_SDMMC1_RELEASE_RESET();
  }
#endif

#if defined SDMMC2_BASE
  if (obj->handle.Instance == SDMMC2) {
    __HAL_RCC_SDMMC2_CLK_ENABLE();
    __HAL_RCC_SDMMC2_FORCE_RESET();
    __HAL_RCC_SDMMC2_RELEASE_RESET();
  }
#endif
  HAL_SD_Init(handle);

  /* In order to set correctly the SD polarity we need to enable the peripheral */
  __HAL_SD_ENABLE(handle);
}

/**
  * @brief This function is implemented to deinitialize the SD interface
  *        (IOs + SD block)
  * @param  obj : pointer to sd_t structure
  * @retval None
  */
void sd_deinit(sd_t *obj)
{
  if (obj == NULL) {
    return;
  }
  uint8_t sd_state = SD_OK;

#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION <= 0x02050000)
  obj->sd = SD_INSTANCE;
#else
  if (!SDIO_GetInstance()) {
    sd_state = SD_ERROR;
  } else
#endif
  {
    /* HAL SD deinitialization */
    if (HAL_SD_DeInit(&obj->handle) != HAL_OK) {
      sd_state = SD_ERROR;
    }

    /* Msp SD deinitialization */
    SDIO_MspDeInit(&obj->handle, NULL);

    if (obj->SD_detect_ll_gpio_pin != LL_GPIO_PIN_ALL) {
      SDIO_Detect_MspDeInit(&obj->handle, NULL);
    }
  }

  #if defined SDIO_BASE
  // Enable SD clock
  if (obj->handle.Instance == SDIO) {
    __HAL_RCC_SDIO_FORCE_RESET();
    __HAL_RCC_SDIO_RELEASE_RESET();
    __HAL_RCC_SDIO_CLK_DISABLE();
  }
#endif

#if defined SDMMC_BASE
  if (obj->handle.Instance == SDMMC) {
    __HAL_RCC_SDMMC_FORCE_RESET();
    __HAL_RCC_SDMMC_RELEASE_RESET();
    __HAL_RCC_SDMMC2_CLK_DISABLE();
  }
#endif

#if defined SDMMC1_BASE
  if (obj->handle.Instance == SDMMC1) {
    __HAL_RCC_SDMMC1_FORCE_RESET();
    __HAL_RCC_SDMMC1_RELEASE_RESET();
    __HAL_RCC_SDMMC2_CLK_DISABLE();
  }
#endif

#if defined SDMMC2_BASE
  if (obj->handle.Instance == SDMMC2) {
    __HAL_RCC_SDMMC2_FORCE_RESET();
    __HAL_RCC_SDMMC2_RELEASE_RESET();
    __HAL_RCC_SDMMC2_CLK_DISABLE();
  }
#endif
    return sd_state;
}

/**
  * @brief This function is implemented by user to receive data over
  *         SD interface
  * @param  obj : pointer to sd_t structure
  * @param  rx_buffer : data to be received rx data
  * @param  address : data address in the card to be read
  * @param  blocks : number of blocks of the data to receive
  * @retval status of the receive operation (0) in case of error
  */
sd_status_e sd_read(sd_t *obj, const uint8_t *rx_buffer, uint32_t address, uint16_t blocks)
{
  sd_status_e ret = SD_OK;
  SD_TypeDef *_SD = obj->handle.Instance;
  uint8_t *buffer = (uint8_t *)rx_buffer;

  if (blocks == 0) {
    ret = SD_ERROR;
  } else {
    if (!obj->useDMA) {
      if (HAL_SD_ReadBlocks(_SD, buffer, address, blocks, SD_TRANSFER_TIMEOUT) == HAL_OK) {
        ret = SD_OK;
      } else {
        ret = SD_ERROR;
      }
    } else {
      if (HAL_SD_ReadBlocks_DMA(_SD, buffer, address, blocks) == HAL_OK) {
        ret = SD_OK;
      } else {
        ret = SD_ERROR;
      }
    }
  }
  return ret;
}

/**
  * @brief This function is implemented by user to send data over
  *         SD interface
  * @param  obj : pointer to sd_t structure
  * @param  tx_buffer : data to be sent tx data
  * @param  address : data address in the card to be read
  * @param  blocks : number of blocks of the data to receive
  * @retval status of the send operation (0) in case of error
  */
sd_status_e sd_write(sd_t *obj, const uint8_t *rx_buffer, uint32_t address, uint16_t blocks)
{
  sd_status_e ret = SD_OK;
  SD_TypeDef *_SD = obj->handle.Instance;
  uint8_t *buffer = (uint8_t *)rx_buffer;

  if (blocks == 0) {
    ret = SD_ERROR;
  } else {
    if (!obj->useDMA) {
      if (HAL_SD_WriteBlocks(_SD, buffer, address, blocks, SD_TRANSFER_TIMEOUT) == HAL_OK) {
        ret = SD_OK;
      } else {
        ret = SD_ERROR;
      }
    } else {
      if (HAL_SD_WriteBlocks_DMA(_SD, buffer, address, blocks) == HAL_OK) {
        ret = SD_OK;
      } else {
        ret = SD_ERROR;
      }
    }
  }
  return ret;
}

bool sd_isBusy(sd_t *obj)
{
  bool ret = false;
  if (obj->useDMA) {
    if (__HAL_SD_GET_FLAG(&obj->handle, SDIO_FLAG_TXACT | SDIO_FLAG_RXACT | SDIO_FLAG_CMDACT) == 0) {
      ret = true;
    }
  }
  return ret;
}

sd_status_e sd_erase(sd_t *obj, uint32_t start_address, uint32_t end_address)
{
  uint8_t sd_state = SD_OK;
  SD_TypeDef *_SD = obj->handle.Instance;

  if (HAL_SD_Erase(_SD, start_address, end_address) != HAL_OK) {
    sd_state = SD_ERROR;
  }
  return sd_state;
}

/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @retval Status
  */
 bool sd_setDetectionInterrupt(sd_t *obj, void (*callback)(void))
 {
   bool sd_state = false;
   if (obj->SD_detect_gpio_port != 0) {
     LL_GPIO_SetPinPull(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, LL_GPIO_PULL_UP);
     uint16_t SD_detect_gpio_pin = GPIO_PIN_All;
     switch (obj->SD_detect_ll_gpio_pin) {
       case LL_GPIO_PIN_0:
         SD_detect_gpio_pin = GPIO_PIN_0;
         break;
       case LL_GPIO_PIN_1:
         SD_detect_gpio_pin = GPIO_PIN_1;
         break;
       case LL_GPIO_PIN_2:
         SD_detect_gpio_pin = GPIO_PIN_2;
         break;
       case LL_GPIO_PIN_3:
         SD_detect_gpio_pin = GPIO_PIN_3;
         break;
       case LL_GPIO_PIN_4:
         SD_detect_gpio_pin = GPIO_PIN_4;
         break;
       case LL_GPIO_PIN_5:
         SD_detect_gpio_pin = GPIO_PIN_5;
         break;
       case LL_GPIO_PIN_6:
         SD_detect_gpio_pin = GPIO_PIN_6;
         break;
       case LL_GPIO_PIN_7:
         SD_detect_gpio_pin = GPIO_PIN_7;
         break;
       case LL_GPIO_PIN_8:
         SD_detect_gpio_pin = GPIO_PIN_8;
         break;
       case LL_GPIO_PIN_9:
         SD_detect_gpio_pin = GPIO_PIN_9;
         break;
       case LL_GPIO_PIN_10:
         SD_detect_gpio_pin = GPIO_PIN_10;
         break;
       case LL_GPIO_PIN_11:
         SD_detect_gpio_pin = GPIO_PIN_11;
         break;
       case LL_GPIO_PIN_12:
         SD_detect_gpio_pin = GPIO_PIN_12;
         break;
       case LL_GPIO_PIN_13:
         SD_detect_gpio_pin = GPIO_PIN_13;
         break;
       case LL_GPIO_PIN_14:
         SD_detect_gpio_pin = GPIO_PIN_14;
         break;
       case LL_GPIO_PIN_15:
         SD_detect_gpio_pin = GPIO_PIN_15;
         break;
       default:
         Error_Handler();
         break;
     }
     stm32_interrupt_enable(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin, callback, GPIO_MODE_IT_RISING_FALLING);
     sd_state = true;
   }
   return sd_state;
 }

bool sd_setDetectionInput(sd_t *obj, PinName p, uint32_t level)
{
    uint8_t sd_state = SD_OK;
    GPIO_TypeDef *port = set_GPIO_Port_Clock(STM_PORT(p));
    uint32_t pin = STM_LL_GPIO_PIN(p);
    if (port != 0) {
        obj->SD_detect_ll_gpio_pin = pin;
        obj->SD_detect_gpio_port = port;
        obj->SD_detect_level = level;
    } else {
      sd_state = SD_ERROR;
    }
    return sd_state;
}

bool sd_isDetected(sd_t *obj)
{
    uint8_t ret = false;
    if (obj->SD_detect_gpio_port != 0) {
        ret = LL_GPIO_IsInputPinSet(obj->SD_detect_gpio_port, obj->SD_detect_ll_gpio_pin) == obj->SD_detect_level;
    }
    return ret;
}

#ifdef __cplusplus
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
