//Temp to turn off compressor
#define COMP_ON 11
//Temp to turn off compressor
#define COMP_OFF 9
//TEMP OFFSET from externally measured temp in %
#define TEMPOFFSET 0.85
// which analog pin to connect
#define THERMISTORPIN A1
//RELAY PIN (Digital PIN)
#define RELAYPIN 7
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 50
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950
// the value of the 'other' resistor
#define SERIESRESISTOR 10000

//RGB LED Pins
#define LEDRED A3
#define LEDGREEN A6
#define LEDBLUE A5

//Time to keep led on
#define LEDDELAY 250

//COMPRESSOR RELAY PINN
#define COMPRESSORRELAY 4
//HEATER Relay PIN
#define HEATERRELAY 2
//Fluro Light Relay Pin
#define LIGHTRELAY 3
//Door Closed Relay
#define DOORRELAY 5

uint16_t samples[NUMSAMPLES];
bool COMP_RUN;



void setup(void) {
  Serial.begin(9600);
  pinMode (RELAYPIN, OUTPUT);
  pinMode (LEDRED, OUTPUT);
  pinMode (LEDGREEN, OUTPUT);
  pinMode (LEDBLUE, OUTPUT);
  pinMode (COMPRESSORRELAY, OUTPUT);
  pinMode (HEATERRELAY, OUTPUT);
  pinMode (LIGHTRELAY, OUTPUT);
  pinMode (DOORRELAY, OUTPUT);
  COMP_RUN=0;
}

void loop(void) {

  uint8_t i;
  float average;
  float measuredtemp;
  float correctedtemp;
  // take N samples in a row, with a slight delay
  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(THERMISTORPIN); ;
    delay(10);
  }


  // average all the samples out
  average = 0;
  for (i = 0; i < NUMSAMPLES; i++) {
    average += samples[i];
  }
  average /= NUMSAMPLES;

  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  //Serial.print("Measured Temp ");
  //Serial.print(steinhart);
  //Serial.println(" *C");

  correctedtemp = (steinhart * TEMPOFFSET);

  Serial.print("Thermistor resistance ");
  Serial.println(average);
  Serial.print("Measured Temp: ");
  Serial.print(steinhart);
  Serial.println(" *C");
  Serial.print("Corrected Temp: ");
  Serial.print(correctedtemp);
  Serial.println(" *C");
  Serial.print ("Compressor Running: ");
  Serial.println (COMP_RUN);

  if ((not (COMP_RUN)) and (correctedtemp > COMP_ON)) {
    //turn compressor on
    COMP_RUN = true;
    digitalWrite (COMPRESSORRELAY, HIGH);
    analogWrite (LEDRED, 0);
    analogWrite (LEDGREEN, 0);
    analogWrite (LEDBLUE, 0);
    delay(LEDDELAY);
    analogWrite (LEDRED, 255);
    delay(LEDDELAY);
  }
  
  else if ((COMP_RUN) and  (correctedtemp > COMP_OFF)) {
    analogWrite (LEDRED, 0);
    analogWrite (LEDGREEN, 0);
    analogWrite (LEDBLUE, 0);
    delay(LEDDELAY);
    analogWrite (LEDRED, 255);
    delay(LEDDELAY);
    
  }
  
  else if ((COMP_RUN) and (correctedtemp <= COMP_ON)) {
    analogWrite (LEDRED, 0);
    analogWrite (LEDGREEN, 0);
    analogWrite (LEDBLUE, 0);
    delay (LEDDELAY);
    analogWrite (LEDGREEN, 255);
    delay (LEDDELAY);
  }
  
  else if ((COMP_RUN) and (correctedtemp <= COMP_OFF)) {
    //turn compressor off
    COMP_RUN = false;
    digitalWrite (RELAYPIN, LOW);
    digitalWrite (COMPRESSORRELAY, HIGH);
    analogWrite (LEDRED, 0);
    analogWrite (LEDGREEN, 0);
    analogWrite (LEDBLUE, 0);
    delay (LEDDELAY);
    analogWrite (LEDBLUE, 255);
    delay (LEDDELAY);
  }
  
  else if (not (COMP_RUN) and (correctedtemp < COMP_ON)) {
    analogWrite (LEDRED, 0);
    analogWrite (LEDGREEN, 0);
    analogWrite (LEDBLUE, 0);
    delay(LEDDELAY);
    analogWrite (LEDBLUE, 255);
    delay(LEDDELAY);
  }


}
