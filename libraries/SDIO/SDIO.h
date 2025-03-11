/**
 * \file
 * \brief Definitions for STM32 SDIO.
 * based on ST's https://github.com/stm32duino/STM32SD/blob/main/src/SDIO.c
 */

 #ifndef _SDIO_H_INCLUDED
 #define _SDIO_H_INCLUDED
 
 //#if defined(ARDUINO_ARCH_STM32)
 #include <Arduino.h>
 #include <stdio.h>
 extern "C" {
   #include "utility/sdio_com.h"
   }
   
 class SDIOClass {
   public:
     SDIOClass(uint32_t pin_d0 = NC, uint32_t pin_d1 = NC, uint32_t pin_d2 = NC, uint32_t pin_d3 = NC, uint32_t pin_cmd = NC, uint32_t sclk = NC);
 
     // setD0...D3/CMD/SCLK have to be called before begin()
     void setD0(uint32_t pin_d0)
     {
       _sd.pin_d0 = digitalPinToPinName(pin_d0);
     };
     void setD1(uint32_t pin_d1)
     {
       _sd.pin_d1 = digitalPinToPinName(pin_d1);
     };
     void setD2(uint32_t pin_d2)
     {
       _sd.pin_d2 = digitalPinToPinName(pin_d2);
     };
     void setD3(uint32_t pin_d3)
     {
       _sd.pin_d3 = digitalPinToPinName(pin_d3);
     };
     void setCMD(uint32_t pin_cmd)
     {
       _sd.pin_cmd = digitalPinToPinName(pin_cmd);
     };
     void setSCK(uint32_t sclk)
     {
       _sd.pin_sck = digitalPinToPinName(sclk);
     };
 
     void setD0(PinName pin_d0)
     {
       _sd.pin_d0 = pin_d0;
     };
     void setD1(PinName pin_d1)
     {
       _sd.pin_d1 = pin_d1;
     };
     void setD2(PinName pin_d2)
     {
       _sd.pin_d2 = pin_d2;
     };
     void setD3(PinName pin_d3)
     {
       _sd.pin_d3 = pin_d3;
     };
     void setCMD(PinName pin_cmd)
     {
       _sd.pin_cmd = pin_cmd;
     };
     void setSCK(PinName sclk)
     {
       _sd.pin_sck = sclk;
     };
 
     void begin(bool useDma = false);
     void end(void);
 
     /* Transfer functions: must be called after initialization of the SDIO
      * instance with begin().
      */
     bool read(void *buf, uint32_t address, size_t blocks);
     bool write(void *buf, uint32_t address, size_t blocks);
     bool erase(uint32_t startAddress, uint32_t endAddress);
 
     bool setDetectionInput(uint32_t ulPin, uint32_t ulLvl);
     bool isDetectionCallbakc(void (*callback)(void));
     bool isDetected(void);
     bool isBusy(void);
 
     void setDataMode(uint8_t);
     void setDataMode(SDIOMode);
 
     // Not implemented functions. Kept for compatibility.
     void usingInterrupt(int interruptNumber);
     void notUsingInterrupt(int interruptNumber);
     void attachInterrupt(void);
     void detachInterrupt(void);
 
     // Could be used to mix Arduino API and STM32Cube HAL API (ex: DMA). Use at your own risk.
     SD_HandleTypeDef *getHandle(void)
     {
       return &(_sd.handle);
     }
 
   protected:
     // SD instance
     sd_t         _sd;
 };
 extern SDIOClass SD;
 
 //#endif  // defined(ARDUINO_ARCH_STM32)
 #endif  // _SDIO_H_INCLUDED
 