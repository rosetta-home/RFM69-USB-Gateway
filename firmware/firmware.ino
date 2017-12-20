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
char data[100];
char _rssi[5];
char END_FRAME = 'n';
char TOUCHSTONE_FRAME = ',';

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
  if(Serial.available()){
    int total_bytes = 10;
    char incoming_data[total_bytes];
    int touchstone = Serial.parseInt();
    int comma[1] = { Serial.read() };
    Serial.readBytesUntil(END_FRAME, incoming_data, total_bytes);
    radio.sendWithRetry(touchstone, incoming_data, strlen(incoming_data), 8, 0);
    delay(10);
    memset(incoming_data, 0, sizeof incoming_data);
    
  }
  if (radio.receiveDone())
  {
    //Serial.println("Got Data");
    int rssi = radio.RSSI;

    if (radio.DATALEN > 0)
    {
      for (byte i = 0; i < radio.DATALEN; i++)
        data[i] = (char)radio.DATA[i];
    }

    dtostrf(rssi, 3, 0, _rssi);
    strcat(data, ",r:");
    strcat(data, _rssi);

    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
    }

    Serial.println(data);
    delay(1);

    memset(data, 0, sizeof data);
    memset(_rssi, 0, sizeof _rssi);

    Blink(LED,10);
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}
