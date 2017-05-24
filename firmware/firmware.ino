#include <RFM69.h>         //get it here: http://github.com/lowpowerlab/rfm69
#include <SPIFlash.h>      //get it here: http://github.com/lowpowerlab/spiflash
#include <SPI.h>           //comes with Arduino IDE (www.arduino.cc)
#include "config.h"

#define NETWORKID     101
#define FREQUENCY     RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define LED             9
#define FLASH_CS        8
#define SERIAL_BAUD 115200
#define SERIAL_EN     //comment out if you don't want any serial verbose output
#define ACK_TIME       30  // # of ms to wait for an ack
//*****************************************************************************************************************************

RFM69 radio;
SPIFlash flash(FLASH_CS, 0xEF30); //EF40 for 16mbit windbond chip

void setup()
{
  pinMode(10, OUTPUT);
  Serial.begin(115200);

  radio.initialize(FREQUENCY,NODEID,NETWORKID);
#ifdef IS_RFM69HW
  radio.setHighPower(); //uncomment only for RFM69HW!
#endif
  radio.encrypt(ENCRYPTKEY);
  flash.initialize();
}

void loop()
{
  if (radio.receiveDone())
  {
    char data[100];
    char _rssi[5];
    int rssi = radio.RSSI;

    if (radio.DATALEN > 0)
    {
      for (byte i = 0; i < radio.DATALEN; i++)
        data[i] = (char)radio.DATA[i];
    }

    dtostrf(rssi, 3, 0, _rssi);
    strcat(data, ",r:");
    strcat(data, _rssi);

    Serial.println(data);
    Serial.flush();
    blink(LED,10);
    if (radio.ACKRequested()) radio.sendACK();
  }
}

void blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
