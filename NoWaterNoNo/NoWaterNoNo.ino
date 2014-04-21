/*
 2014-04-14
 k2OS (k2os@labitat) / René Mikkelsen
 Modified the original print and code to add 433MHz functionality. The frequency of the acoustic alarm will be
 reduced as well, as where these are going, there might not be anyone but a very annoyed upstairs neightbour to 
 hear parts of the day. No - it would be so much cooler if they could email me (or whoever wants to know) that
 water has been detected.
 Obviously this tiny can't do it alone, so there will be a central unit that listens for all these little 'tiny's
 sending out messages if water is detected (and the occasional heartbeat, to make sure they're not out of battery).

 Measurements with a uCurrent show a powerusage of 4.6uA at idle and ~4.5mA when sending messages - roughly the 
 same when sampling.
 On a 3.7v / 700mAh nokia battery these will last 'forever' (or until the battery has discharged itself, most likely).

 If anyone uses this, pass the beer onto Nathan, as I can't drink beer (oh snap).

 1-14-2013
 Spark Fun Electronics
 Nathan Seidle
 
 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 When the ATtiny senses water between two pins, go crazy. Make noise, blink LED.
 
 Created to replace the water sensor in my Nauticam 17401 underwater enclosure. The original board
 ate up CR2032s, sorta kinda worked some of the time, and had pretty low quality assembly. Did I mention it goes for $100?!
 We have the technology. We can make it better!
 
 To use this code you must configure the ATtiny to run at 8MHz so that serial and other parts of the code work correctly.
 
 We take a series of readings of the water sensor at power up. We then wait for a deviation of more than
 100 from the average before triggering the alarm.
 
 The alarm will run for a minimum of 2 seconds before shutting down.
 
 The original board had the following measurements @ 3.21V:
 In alarm mode: ~30mA with LED on and making sound
 Off: 10nA. Really? Wow. 
 
 This firmware uses some power saving to get the average consumption down to ~50uA average. With a CR2032 @ 200mAh 
 this should allow it to run for 4,000hrs or 166 days. This is easily extended by increasing the amount of time
 between water checks (currently 1 per second).
 
 */

#include <avr/sleep.h> //Needed for sleep_mode
#include <avr/wdt.h> //Needed to enable/disable watch dog timer
#include <VirtualWire.h>

// radio stuff
int RF_TX_PIN = 3;
const char *heartbeat = "SENSE00OK";
const char *water = "SENSE00WATER";
int helpAlerted = 0; // keeps track of wether we've send alert or not

//Pin definitions for ATtiny
const byte buzzer1 = 1;
const byte buzzer2 = 2;
const byte statLED = 0;
const byte waterSensor = A2; // pin 4

//This is the average analog value found during startup. Usually ~995
//When hit with water, the analog value will drop to ~400. A diff of 100 is good.
int waterAvg = 0; 
int maxDifference = 100; //A diff of more than 100 in the analog value will trigger the system.
int wdt_counter = 0;
//This runs each time the watch dog wakes us up from sleep
ISR(WDT_vect) {
  //Don't do anything. This is just here so that we wake up.
  wdt_counter++;
}

void setup()
{
  
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(statLED, OUTPUT);
  pinMode(RF_TX_PIN, OUTPUT);
  vw_set_tx_pin(RF_TX_PIN); // Setup transmit pin
  vw_setup(2000); // Transmission speed in bits per second.
  delay(10);

  //pinMode(waterSensor, INPUT_PULLUP);
  pinMode(4, INPUT); //When setting the pin mode we have to use 2 instead of A1
  digitalWrite(4, HIGH); //Hack for getting around INPUT_PULLUP

  //Take a series of readings from the water sensor and average them
  waterAvg = 0;
  for(int x = 0 ; x < 8 ; x++)  {
    waterAvg += analogRead(waterSensor);

    //During power up, blink the LED to let the world know we're alive
    if(digitalRead(statLED) == LOW)
      digitalWrite(statLED, HIGH);
    else
      digitalWrite(statLED, LOW);
    delay(50);
  }
  waterAvg /= 8;
  //During power up, beep the buzzer to verify function
  alarmSound();
  delay(100);
  alarmSound();

  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(statLED, LOW);

  // send heartbeat - the function will send it x times
  sendMsg(heartbeat);
  //Power down various bits of hardware to lower power usage  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); //Power down everything, wake up from WDT
  sleep_enable();
}

void loop() 
{
 ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
 setup_watchdog(9); //Setup watchdog to go off after 8sec
 sleep_mode(); //Go to sleep! Wake up 8sec later and check water
  //Check for water
  ADCSRA |= (1<<ADEN); //Enable ADC
  delay(10); // wait a tiny bit
  int waterDifference = abs(analogRead(waterSensor) - waterAvg);

  if(waterDifference > maxDifference) {
    wdt_disable(); //Turn off the WDT!!
    wdt_counter = 0;
    long startTime = millis(); //Record the current time
    long timeSinceBlink = millis(); //Record the current time for blinking
    digitalWrite(statLED, HIGH); //Start out with the uh-oh LED on

    //Loop until we don't detect water AND 2 seconds of alarm have completed
    while(waterDifference > maxDifference || (millis() - startTime) < 2000)
    {
      alarmSound(); //Make noise!! // maybe only do this once in a while instead of all the time

      if(millis() - timeSinceBlink > 100) //Toggle the LED every 100ms
      {
        timeSinceBlink = millis();

        if(digitalRead(statLED) == LOW) 
          digitalWrite(statLED, HIGH);
        else
          digitalWrite(statLED, LOW);
      }

      waterDifference = abs(analogRead(waterSensor) - waterAvg); //Take a new reading
      // add a timeout-thing here and send out audible alarm + radio once every X minutes
      if (!helpAlerted) {
        sendMsg(water); // send watermsg - we should probably limit this one to 1-2x transmissions and cut down on the
        helpAlerted = 1;              // audible alarm too?
      }            
    } //Loop until we don't detect water AND 2 seconds of alarm have completed
    sendMsg(heartbeat); // send OK-msg when the water goes away.
    helpAlerted = 0;
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(statLED, LOW); //No more alarm. Turn off LED
  } 
  if (wdt_counter >= 4) { // send heart beat every 4 times we wake up
    // send heart beat to control
    sendMsg(heartbeat);
    wdt_counter = 0;
  }
}

//This is just a unique (annoying) sound we came up with, there is no magic to it
//Comes from the Simon Says game/kit actually: https://www.sparkfun.com/products/10547
//250us to 79us
void alarmSound(void)
{
  // Toggle the buzzer at various speeds
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 5 ; y++)
    {
      digitalWrite(buzzer2, HIGH);
      digitalWrite(buzzer1, LOW);
      delayMicroseconds(x);

      digitalWrite(buzzer2, LOW);
      digitalWrite(buzzer1, HIGH);
      delayMicroseconds(x);
    }
  }
}

//Sets the watchdog timer to wake us up, but not reset
//0=16ms, 1=32ms, 2=64ms, 3=128ms, 4=250ms, 5=500ms
//6=1sec, 7=2sec, 8=4sec, 9=8sec
//From: http://interface.khm.de/index.php/lab/experiments/sleep_watchdog_battery/
void setup_watchdog(int timerPrescaler) {

  if (timerPrescaler > 9 ) timerPrescaler = 9; //Limit incoming amount to legal settings

  byte bb = timerPrescaler & 7; 
  if (timerPrescaler > 7) bb |= (1<<5); //Set the special 5th bit if necessary

  //This order of commands is important and cannot be combined
  MCUSR &= ~(1<<WDRF); //Clear the watch dog reset
  WDTCR |= (1<<WDCE) | (1<<WDE); //Set WD_change enable, set WD enable
  WDTCR = bb; //Set new watchdog timeout value
  WDTCR |= _BV(WDIE); //Set the interrupt enable, this will keep unit from resetting after each int
}

void sendMsg(const char *m) {
    for (int i = 0; i< 5; i++) {
    vw_send((uint8_t *)m, strlen(m));  // Send msg
    vw_wait_tx();
  }
}
