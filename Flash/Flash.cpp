#include "Flash.h"

Flash::Flash() {
  pinMode(CS, OUTPUT);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

/* Writes the 'qty' quantity of bytes from 'buf' buffer into
 * 'addr' address.
 * Returns the quantity of written bytes.
 */
unsigned long Flash::write(uint32_t addr, byte* buf, uint16_t len)
{
  uint16_t n;
  uint16_t maxBytes = BYTES_PAGE-(addr%BYTES_PAGE);  // force the first set of bytes to stay within the first page
  uint16_t offset = 0;

  writeEnable();


  while (len>0)
  {
    // Wait until it's not busy for every write
    while (isBusy());

    n = (len<=maxBytes) ? len : maxBytes;

    digitalWrite(CS, LOW);
    SPI.transfer(PAGE_PROG);

    SPI.transfer((addr >> 16) & 0xFF);
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);

    for (uint16_t i = 0; i < n; i++) {
      SPI.transfer(buf[offset + i]);
    }
    digitalWrite(CS, HIGH);

    addr+=n;  // adjust the addresses and remaining bytes by what we've just transferred.
    offset +=n;
    len -= n;
    maxBytes = BYTES_PAGE;

    // Wait for max page program time
    delay(MAX_PAGE_MS);
    while (isBusy());
  }

  /* Disable write operations */
  writeDisable();
  return offset;
}

/* Reads the given quantity of bytes from 'addr' address into
 * 'buf' buffer.
 * Returns the quantity of read bytes.
 */
unsigned long Flash::read(unsigned long addr, byte* buf, unsigned long qty)
{
  unsigned long read = 0;

  while (isBusy());

  digitalWrite(CS, LOW);
  SPI.transfer(READ_DATA);

  SPI.transfer((addr >> 16) & 0xFF);
  SPI.transfer((addr >> 8) & 0xFF);
  SPI.transfer(addr & 0xFF);

  while (read < qty)
  {
    buf[read++] = SPI.transfer(0x00);
  }

  digitalWrite(CS, HIGH);
  return read;
}


void Flash::eraseChip()
{
  writeEnable();

  while (isBusy());

  digitalWrite(CS, LOW);
  SPI.transfer(ERASE_CHIP);
  digitalWrite(CS, HIGH);

  // Delay max erase time
  delay(MAX_CHIPERASE_MS);
  while (isBusy());
}

/* This function returns true if the Flash is busy writing.
 * Otherwise it returns false.
 */
bool Flash::isBusy()
{
  if (readStatusRegister() & 0x01)
  {
    return true;
  }
  return false;
}


/* This function reads the Flash status register */
byte Flash::readStatusRegister()
{
  byte val;
  digitalWrite(CS, LOW);
  SPI.transfer(READ_STATUS);
  val = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);

  return val;
}

byte Flash::readManufactorID()
{
  byte val;
  digitalWrite(CS, LOW);
  SPI.transfer(0x9F);
  val = SPI.transfer(0x00);
  val = SPI.transfer(0x00);
  val = SPI.transfer(0x00);
  val = SPI.transfer(0x00);
  digitalWrite(CS, HIGH);

  return val;
}

/* By default, the Flash starts with all the sectors write-protected.
 * This function disables the protection. */
void Flash::unprotect()
{
  writeEnable();
  digitalWrite(CS, LOW);
  SPI.transfer(WRITE_STATUS);
  SPI.transfer(0x00);
  digitalWrite(CS, HIGH);
}

/* Enables write/erase operations */
void Flash::writeEnable()
{

  digitalWrite(CS, LOW);
  SPI.transfer(WRITE_ENABLE);
  digitalWrite(CS, HIGH);
}

/* Disables write/erase operations */
void Flash::writeDisable()
{
  digitalWrite(10, LOW);
  SPI.transfer(WRITE_DISABLE);
  digitalWrite(10, HIGH);
}
