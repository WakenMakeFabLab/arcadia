#include <Flash.h>

Flash flash;
void setup() {  
  char input = 0;

  Serial.begin(115200);

  flash.unprotect();

  Serial.println("Ready");
  
  while(1) {
    if (Serial.available() > 0) {
      input = Serial.read();
      if (input == 'd') //d=dump flash area
      {
        dumpFlash();
      }
      else if (input == 'e')
      {
        Serial.print("Erasing Flash chip ... ");
        flash.eraseChip();
        Serial.println("DONE");
      }
      else if (input == 'w')
      {
        while(flash.isBusy());
        Serial.println("Ready to write");
        writeFlash();
      }
    }
  }
}

void dumpFlash() {
  Serial.println("Flash content:");
  int counter = 0;
  byte read_buffer[256];
  for (int page = 0; page < 10; page++) {
    memset(read_buffer, 0, 256);
    flash.read(page*0x100, read_buffer, 256);
    Serial.print("Page:");
    Serial.println(page);
    for (int i = 0; i < 256; i++)
    {
       if ((i+1) % 16 == 0)
       {
         Serial.println(read_buffer[i]);
  
       } else {
         Serial.print(read_buffer[i]);
         Serial.print(" ");
       }
    }  
  }

  Serial.println();
}


void writeFlash() {
  uint8_t buffer[256];
  int c, index = 0;
  uint32_t capacity = 4194304,
                     address = 0,
                     bytesExpected = capacity,
                     bytesReceived = 0;
  for(;;) {
    // Buffer data until a full page is ready or last packet arrives.
    if((c = Serial.read()) >= 0) {
      buffer[index++] = c;
      Serial.println(c);
      // Bytes 0-4 indicate number of samples to follow
      if(++bytesReceived == 4) {
        bytesExpected = (((uint32_t)buffer[0] << 24) |
                         ((uint32_t)buffer[1] << 16) |
                         ((uint32_t)buffer[2] << 8) |
                                    buffer[3] ) + 4;

        
        Serial.print("Expecting bytes:");
        Serial.println(bytesExpected);
      }

      if((index >= sizeof(buffer)) || (bytesReceived >= bytesExpected)) {
        if(flash.write(address, buffer, sizeof(buffer))) {
          Serial.print('.');
        } else {
          Serial.print('X');
        }
        address += sizeof(buffer);
        if((address >= capacity) || (bytesReceived >= bytesExpected)) {
          Serial.print("Written: ");
          Serial.print(bytesReceived);
          Serial.println("bytes.");
          break;
        }
        index = 0;
      }
    }
  }
}

void loop() {
}

