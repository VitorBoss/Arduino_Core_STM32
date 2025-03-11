/**
 * based on ST's https://github.com/stm32duino/STM32SD/blob/main/src/SDIO.c
 */
#include "SDIO.h"

SDIOClass SD;

/**
  * @brief  Default Constructor. Uses pin configuration of default SDIO
  *         defined in the variant*.h.
  *         To create another SDIO instance attached to another SDIO
  *         peripheral gave the pins as parameters to the constructor.
  * @note   All pins must be attached to the same SDIO peripheral.
  *         See datasheet of the microcontroller.
  * @param  pin_d0: SDIO D0 pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_DATA0[0] pin of the default SDIO peripheral.
  * @param  pin_d1: SDIO D1 pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_DATA1[0] pin of the default SDIO peripheral.
  * @param  pin_d2: SDIO d2 pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_DATA2[0] pin of the default SDIO peripheral.
  * @param  pin_d3: SDIO D3 pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_DATA3[0] pin of the default SDIO peripheral.
  * @param  pin_cmd: SDIO CMD pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_CMD[0] pin of the default SDIO peripheral.
  * @param  sclk: SDIO clock pin. Accepted format: number or Arduino format (Dx)
  *         or ST format (Pxy). Default is PinMap_SD_CK[0] pin of the default SDIO peripheral.
  */
 SDIOClass::SDIOClass(uint32_t pin_d0, uint32_t pin_d1, uint32_t pin_d2, uint32_t pin_d3, uint32_t pin_cmd, uint32_t sclk)
 {
   memset((void *)&_sd, 0, sizeof(_sd));
   _sd.pin_d0 = digitalPinToPinName(pin_d0);
   _sd.pin_d1 = digitalPinToPinName(pin_d1);
   _sd.pin_d2 = digitalPinToPinName(pin_d2);
   _sd.pin_d3 = digitalPinToPinName(pin_d3);
   _sd.pin_cmd = digitalPinToPinName(pin_cmd);
   _sd.pin_sck = digitalPinToPinName(sclk);
 }
 
 /**
  * @brief  Initialize the SDIO instance.
  * @param  useDMA: Use DMA peropheral for SDIO data transfer
  */
void SDIOClass::begin(bool useDMA)
{
  sd_init(&_sd, SDIO_SPEED_CLOCK_DEFAULT, SDIO_MODE_4B, useDMA);
}

/**
 * @brief  Deinitialize the SDIO instance and stop it.
 */
void SDIOClass::end(void)
{
  sd_deinit(&_sd);
}


/**
 * @brief  Return true if intput pin is valid
 * @param ulPin: board pin. Accepted format: number or Arduino format (Dx)
 *         or ST format (Pxy).
 * @param ulPin: pin status when SD is present, use 0 and non 0 values
  * @retval status of the operation
 */
bool SDIOClass::setDetectionInput(uint32_t ulPin, uint32_t ulLvl)
{
  bool ret = false;
  PinName p = digitalPinToPinName(ulPin);
  if (p != NC) {
    uint32_t level = STM_LL_GPIO_PIN(p);
    if (ulLvl == 0) {
      level = 0;
    }
    ret = sd_setDetectionInput(&_sd, p, level);
  }
  return ret;
}

/**
 * @brief  Set a card insertion/remove callback
 * @retval status of the operation
 */
bool SDIOClass::isDetectionCallbakc(void (*callback)(void))
{
  return sd_setDetectionInterrupt(&_sd, callback);
}

/**
 * @brief  Return true if intput signal matches configured level,
 *         if not configured always return false
  * @retval status of the detection pin
 */
bool SDIOClass::isDetected(void)
{
  return sd_IsDetected(&_sd);
}

/**
 * @brief  Return true if SD peripheral is busy when DMA is enabled
 * @retval status of DMA transfer busy or error flags
 */
bool SDIOClass::isBusy(void)
{
  bool ret = false;
  if (_sd.useDMA) {
    ret = sd_isBusy(&_sd);
  }
  return ret;
}

/**
  * @brief This function is implemented by user to receive data over
  *         SD interface
  * @param  buf : buffer to receive data
  * @param  address : data address in the card to be read
  * @param  blocks : number of blocks of the data to receive
  * @retval status of the receive operation
  */
 bool SDIOClass::read(void *buf, uint32_t address, size_t blocks)
{
  bool ret = false;
  if (sd_read(&_sd, (uint8_t *)buf, address, blocks) == 0) {
    ret = true;
  }
  return ret;
}

/**
  * @brief This function is implemented by user to write data over
  *         SD interface
  * @param  buf : buffer with data to be write
  * @param  address : data address in the card to write
  * @param  blocks : number of blocks of the data to write
  * @retval status of the write operation
  */
 bool SDIOClass::write(void *buf, uint32_t address, size_t blocks)
{
  bool ret = false;
  if (sd_write(&_sd, (uint8_t *)buf, address, blocks) == 0) {
    ret = true;
  }
  return ret;
}

/**
  * @brief This function is implemented by user to write data over
  *         SD interface
  * @param  startAddress : data address in the card to start erase
  * @param  endAddress : data address in the card to end erase
  * @retval status of the write operation
  */
 bool SDIOClass::erase(uint32_t startAddress, uint32_t endAddress)
{
  bool ret = false;
  if (sd_erase(&_sd, startAddress, endAddress) == 0) {
    ret = true;
  }
  return ret;
}

/**
  * @brief  Deprecated function.
  *         Configure the data mode (clock polarity and clock phase)
  * @param  mode: SDIO_MODE_1B, SDIO_MODE_4B, SDIO_MODE_8B
  */
 void SDIOClass::setDataMode(uint8_t mode)
 {
   setDataMode((SDIOMode)mode);
 }
 
 void SDIOClass::setDataMode(SDIOMode mode)
 {
   _sd.busMode = mode;
   sd_init(&_sd, SDIO_SPEED_CLOCK_DEFAULT, mode, _sd.useDMA);
 }
 
/**
  * @brief  Not implemented.
  */
 void SDIOClass::usingInterrupt(int interruptNumber)
 {
   UNUSED(interruptNumber);
 }
 
 /**
   * @brief  Not implemented.
   */
 void SDIOClass::notUsingInterrupt(int interruptNumber)
 {
   UNUSED(interruptNumber);
 }
 
 /**
   * @brief  Not implemented.
   */
 void SDIOClass::attachInterrupt(void)
 {
   // Should be enableInterrupt()
 }
 
 /**
   * @brief  Not implemented.
   */
 void SDIOClass::detachInterrupt(void)
 {
   // Should be disableInterrupt()
 }
 