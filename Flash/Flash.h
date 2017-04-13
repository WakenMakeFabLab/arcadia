#include <SPI.h>
#include <Arduino.h>

#ifndef _FLASH_H_
#define _FLASH_H_

#define CS 10

#define WRITE_STATUS 0x01
#define PAGE_PROG 0x02
#define READ_DATA 0x03
#define WRITE_DISABLE 0x04
#define READ_STATUS 0x05
#define WRITE_ENABLE 0x06
#define ERASE_CHIP 0xC7

#define MAX_PAGE_MS 10
#define MAX_CHIPERASE_MS 128000

#define BYTES_PAGE 256
class Flash {
 public:
  Flash(void);
  unsigned long Flash::write(uint32_t addr, byte* buf, uint16_t len);
  unsigned long Flash::read(unsigned long addr, byte* buf, unsigned long qty);
  void Flash::eraseChip();
  bool Flash::isBusy();
  byte Flash::readStatusRegister();
  byte Flash::readManufactorID();
  void Flash::unprotect();
  void Flash::writeEnable();
  void Flash::writeDisable();
};

#endif // _FLASH_H_
