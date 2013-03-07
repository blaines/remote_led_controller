#include <IRremote.h>
/* Raw IR commander
 
 This sketch/program uses the Arduno and a PNA4602 to 
 decode IR received.  It then attempts to match it to a previously
 recorded IR signal
 
 Code is public domain, check out www.ladyada.net and adafruit.com
 for more tutorials! 
 */

// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
// http://arduino.cc/en/Hacking/PinMapping168 for the 'raw' pin mapping
//#define DEBUG true
#define METHODS true

#define IRpin_PIN      PIND
#define IRpin          2
IRrecv irrecv(IRpin);

decode_results results;

// Metro ledMetro = Metro(250); 

#define REDPIN 5
#define GREENPIN 6
#define BLUEPIN 9

bool powerState = true;
bool visibilityState = true;

uint8_t brightnessLevel = 100;  // brightness level
float redLevel = 0;           // 0 - 255 before brightness
float greenLevel = 0;         // 0 - 255 before brightness
float blueLevel = 0;          // 0 - 255 before brightness

uint8_t animationFrames= 0;
uint8_t animationCurrentFrame= 0;
float animationRedStep= 0;
float animationGreenStep = 0;
float animationBlueStep = 0;

unsigned long lastCommand;
unsigned long colorUpdateMillis = 0;
int animVar = 1;

#include "ircodes.h"

void setup(void) {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Ready!");

  changePowerState(false);

  irrecv.enableIRIn();
}

void loop(void) {
  int command;
  unsigned long currentMillis = millis();

  if (irrecv.decode(&results)) {
    if(results.value == 0xFFFFFFFF) {
      command = lastCommand;
    } else {
      Serial.print("IR: ");
      Serial.println(results.value, HEX);
      command = results.value;
      lastCommand = command;
    }
    switch(command) {
    case 0xFF3AC5:
    case 0x28526:
      ledIncreaseBrightnessButton();
    break;
    case 0xFFBA45:
      ledDecreaseBrightnessButton();
    break;
    case 0xFF827D:
      ledPlayButton();
    break;
    case 0xFF02FD:
      ledPowerButton();
    break;
    case 0xA8E05FBB:
    case 0xFF1AE5:
      ledRedButton();
    break;
    case 0xFF9A65:
      ledGreenButton();
    break;
    case 0xFFA25D:
      // transitionRGBValues(0, 0, 255);
      ledBlueButton();
    break;
    case 0xFF22DD:
      ledWhiteButton();
    break;
    // Red Column - R1
    case 0xFF38C71:
    case 0x5B83B61B:
      transitionRGBValues(255, 32, 0);
    break;
    // Red Column - R2
    case 0xFF0AF5:
    case 0xB08CB7DF:
      transitionRGBValues(255, 64, 0);
    break;
    // Red Column - R3
    case 0xFF38C7:
    case 0x488F3CBB:
      transitionRGBValues(255, 128, 0);
    break;
    // Red Column - R4 (Yellow)
    case 0x3D9AE3F7:
    case 0xFF18E7:
      transitionRGBValues(255, 255, 0);
    break;
    // case 0xFF0AF5:
      // transitionRGBValues(255, 128, 0);
    // break;
    case 0xFF28D7:
      if(redLevel < 255) {
        transitionRGBValues(redLevel+5, greenLevel, blueLevel);
      }
    break;
    case 0xFF08F7:
      if(redLevel > 0) {
        transitionRGBValues(redLevel-5, greenLevel, blueLevel);
      }
    break;
    case 0xFFA857:
      if(greenLevel < 255) {
        transitionRGBValues(redLevel, greenLevel+5, blueLevel);
      }
    break;
    case 0xFF8877:
      if(greenLevel > 0) {
        transitionRGBValues(redLevel, greenLevel-5, blueLevel);
      }
    break;
    case 0xFF6897:
      if(blueLevel < 255) {
        transitionRGBValues(redLevel, greenLevel, blueLevel+5);
      }
    break;
    case 0xFF48B7:
      if(blueLevel > 0) {
        transitionRGBValues(redLevel, greenLevel, blueLevel-5);
      }
    break;
    case 0xFF30CF:
  if(lastCommand == 0x30CF) {
    unsigned long animationStartMillis = millis();
    unsigned long animationKeyMillis = (currentMillis - animationStartMillis);

    Serial.println(animationKeyMillis);
    Serial.println(currentMillis);
    Serial.println(animationStartMillis);
    if((currentMillis - animationStartMillis) > 0) {
      transitionRGBValues(0, 128, 255);
    }
    if((currentMillis - animationStartMillis) > (1275*2)) {
      transitionRGBValues(255, 128, 0);
    }
    if((currentMillis - animationStartMillis) > (1275*2)) {
      lastCommand = 0;
    }
  }
    break;
    }
    irrecv.resume(); // Receive the next value
  }
  
  animate();

}

////////////////////////////////////////////////////////////////////////////////////////////
// REMOTE
////////////////////////////////////////////////////////////////////////////////////////////

void ledIncreaseBrightnessButton() {
#ifdef METHODS
  Serial.println('ledIncreaseBrightnessButton');
#endif
  if(brightnessLevel < 100) {
    brightnessLevel += 10;
    updateRGBLevels();
  }
}

void ledDecreaseBrightnessButton() {
#ifdef METHODS
  Serial.println('ledDecreaseBrightnessButton');
#endif
  if(brightnessLevel > 10) {
    brightnessLevel -= 10;
    updateRGBLevels();
  }
  else if(brightnessLevel > 0) {
    brightnessLevel -= 1;
    updateRGBLevels();
  }
}

void ledPlayButton() {
  // while(1) {
    // ledMetro.interval(250+random(1500));
    // if (ledMetro.check() == 1) {
//      transitionRGBValues(0, random(64), 128+random(127));
    // }
  // }
}

void ledPowerButton() {
  if(powerState == true && visibilityState == false) {
    ledIncreaseBrightnessButton();
  }
  else if(powerState == true && visibilityState == true) {
    powerState = changePowerState(false);
  }
  else {
    powerState = changePowerState(true);
  }
}

void ledRedButton() {
  transitionRGBValues(255, 0, 0);
}

void ledGreenButton() {
  transitionRGBValues(0, 255, 0);
}

void ledBlueButton() {
  transitionRGBValues(0, 0, 255);
}

void ledWhiteButton() {
  transitionRGBValues(255, 255, 255);
}

////////////////////////////////////////////////////////////////////////////////////////////
// HELPERS
////////////////////////////////////////////////////////////////////////////////////////////

void powerStateOff() {
  if(powerState == true) {
    powerState = false;
    visibilityStateOff();
    Serial.println("Light is now OFF");
  }
}

void powerStateOn() {
  if(powerState == false) {
    powerState = true;
    Serial.println("Light is now ON");
  }
}

void visibilityStateOff() {
  if(visibilityState == true) {
    visibilityState = false;
    Serial.println("Light is not visibile");
  }
}

void visibilityStateOn() {
  if(visibilityState == false) {
    visibilityState = true;
    Serial.println("Light is visible");
  }
}

void checkPowerState() {
  if((redLevel > 0 || greenLevel > 0 || blueLevel > 0) && powerState == false) {
    powerStateOn();
  } 
  else if(redLevel == 0 && greenLevel == 0 && blueLevel == 0) {
    powerStateOff();
  }
}

void checkVisibilityState(int colorSum) {
  if(colorSum > 0) {
    visibilityStateOn();
  } 
  else {
    visibilityStateOff();
  }
}

boolean changePowerState(boolean b) {
#ifdef METHODS
  Serial.println("changePowerState");
#endif
  int on = 255;
  int off = 0;
  if(b) {
    transitionRGBValues(on, on, on);
  } 
  else {
    transitionRGBValues(off, off, off);
  }
  return b;
}

////////////////////////////////////////////////////////////////////////////////////////////
// LED CONTROL
////////////////////////////////////////////////////////////////////////////////////////////

void writeRGBValues(float r, float g, float b) {
  // Serial.println("writeRGBValues");
  if(r < 1) {
    redLevel = 1;
  } else {
    redLevel = r;  
  }

  if(g < 1) {
    greenLevel = 1;
  } else {
    greenLevel = g;  
  }

  if(b < 1) {
    blueLevel = 1;
  } else {
    blueLevel = b;  
  }

  r = safeBrightnessColorValue(redLevel);
  g = safeBrightnessColorValue(greenLevel);
  b = safeBrightnessColorValue(blueLevel);

  analogWrite(REDPIN, r);
  analogWrite(GREENPIN, g);
  analogWrite(BLUEPIN, b);
  checkPowerState();
  checkVisibilityState(r + g + b);
#ifdef DEBUG
  Serial.print("r: ");
  Serial.print(redLevel);
  Serial.print(" g: ");
  Serial.print(greenLevel);
  Serial.print(" b: ");
  Serial.println(blueLevel);
#endif
}

int safeBrightnessColorValue(int color) {
  float brightnessPercent = brightnessLevel/100.0f;
  color = color * brightnessPercent;
  if(color > 255) {
    color = 255;
  }
  return color;
}

void updateRGBLevels() {
#ifdef METHODS
  Serial.println("updateRGBLevels");
#endif
  transitionRGBValues(redLevel, greenLevel, blueLevel);
}

void transitionRGBValues(int r, int g, int b) {
#ifdef METHODS
  Serial.println("transitionRGBValues");
#endif
  if(r == redLevel && g == greenLevel && b == blueLevel) {
    writeRGBValues(r, g, b);
    return;
  }
  float temp_redLevel = redLevel;
  float temp_greenLevel = greenLevel;
  float temp_blueLevel = blueLevel;

  float iterations = 0;
  float rxr = r-redLevel;
  float gxg = g-greenLevel;
  float bxb = b-blueLevel;

  float abs_rxr = abs(rxr);
  float abs_gxg = abs(gxg);
  float abs_bxb = abs(bxb);

  iterations = max(abs_rxr, max(abs_gxg, abs_bxb));

#ifdef DEBUG
  Serial.print("r: ");
  Serial.print(r);
  Serial.print(" g: ");
  Serial.print(g);
  Serial.print(" b: ");
  Serial.println(b);
  Serial.print("rxr: ");
  Serial.print(rxr);
  Serial.print(" gxg: ");
  Serial.print(gxg);
  Serial.print(" bxb: ");
  Serial.println(bxb);
  Serial.print("abs_rxr: ");
  Serial.print(abs_rxr);
  Serial.print(" abs_gxg: ");
  Serial.print(abs_gxg);
  Serial.print(" abs_bxb: ");
  Serial.println(abs_bxb);
  Serial.print("iterations: ");
  Serial.println(iterations);
#endif

  // float rxstep = (rxr/iterations);
  // float gxstep = (gxg/iterations);
  // float bxstep = (bxb/iterations);

  animationCurrentFrame = 0;
  animationFrames = iterations;
  animationRedStep = (rxr/iterations);
  animationGreenStep = (gxg/iterations);
  animationBlueStep = (bxb/iterations);



  // int rxval;
  // int gxval;
  // int bxval;

  // int i;
  // Serial.print("<");
  // Serial.print(">");
  // writeRGBValues(r, g, b);
//   for(i = 0; i <= iterations; i++) {
// //    Serial.print("-");
//     rxval = ( rxstep * i ) + temp_redLevel;
//     gxval = ( gxstep * i ) + temp_greenLevel;
//     bxval = ( bxstep * i ) + temp_blueLevel;
// #ifdef DEBUG
//     Serial.print("i ");
//     Serial.print(i);
//     Serial.print("  r ");
//     Serial.print(rxval);
//     Serial.print("  g ");
//     Serial.print(gxval);
//     Serial.print("  b ");
//     Serial.println(bxval);
//     Serial.print("  rstep ");
//     Serial.print(rxstep*i);
//     Serial.print("  gstep ");
//     Serial.print(gxstep*i);
//     Serial.print("  bstep ");
//     Serial.println(bxstep*i);
// #endif
//     writeRGBValues(rxval, gxval, bxval);
//     // delay(1);
//   }
#ifdef DEBUG
  Serial.print("animationCurrentFrame: ");
  Serial.print(animationCurrentFrame);
  Serial.print(" animationFrames: ");
  Serial.print(animationFrames);
  Serial.print(" animationRedStep: ");
  Serial.print(animationRedStep);
  Serial.print(" animationGreenStep: ");
  Serial.print(animationGreenStep);
  Serial.print(" animationBlueStep: ");
  Serial.println(animationBlueStep);
#endif
}

void animate() {
  if(animationFrames == 0) {
    return;
  }
  unsigned long currentMillis = millis();
  if(animationCurrentFrame < animationFrames && currentMillis >= colorUpdateMillis) {
    colorUpdateMillis = currentMillis + 5;
    writeRGBValues((animationRedStep + redLevel), (animationGreenStep + greenLevel), (animationBlueStep + blueLevel));

    animationCurrentFrame++;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////


