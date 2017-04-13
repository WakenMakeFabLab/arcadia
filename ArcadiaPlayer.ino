#include <Flash.h>

Flash flash;

unsigned long sampleIndex = 4;
uint32_t sampleSize = 0;

byte sample = 0;
byte read_buffer[256];
unsigned long bytesInBuffer = 0;

void setup() {
  Serial.begin(115200);
 
  for (byte i=0;i<8;i++) {
    pinMode(i, OUTPUT); //set digital pins 0-7 as outputs
  }

  sampleSize = getSamplesCount();

  Serial.print("Samples on flash: ");
  Serial.println(sampleSize);
  
  cli();//stop interrupts

  //set timer1 interrupt at ~44.1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 361;// = (16*10^6) / (44100*1) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 bit for 1 prescaler
  TCCR1B |= (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei(); //enable interrupts
}

ISR(TIMER1_COMPA_vect) {//timer1 interrupt ~44.1kHz to send audio data (it is really 44.199kHz)
  sample = getNextSample();
  PORTD = sample;//send a value stored in the array sine out to the DAC through digital pins 0-7
  if (sampleIndex == sampleSize){//reset index if it reaches the limit
    sampleIndex=0;
  }
}

uint32_t getSamplesCount() {
  byte buffer[4];
  memset(buffer, 0, 4);
  flash.read(0, buffer, 4);
  return (((uint32_t)buffer[0] << 24) |
          ((uint32_t)buffer[1] << 16) |
          ((uint32_t)buffer[2] << 8) |
                     buffer[3] );
}

byte getNextSample() {
  int bufIndex = sampleIndex%256;
  if (!bytesInBuffer || !bufIndex) {
    memset(read_buffer, 0, 256);
    bytesInBuffer = flash.read(sampleIndex, read_buffer, 256);
  }
  sampleIndex++;
  return read_buffer[bufIndex];
}

void loop() {
}

