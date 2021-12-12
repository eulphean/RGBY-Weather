#include <Ramp.h>
#include <DmxSimple.h>

// States
// A Sunrise
// B Noon
// C Sunset
// D Midnight

char currentState = '-'; 
boolean hasReset = false; 

// Initialize ramp variables for each color. 
rampFloat redRamp; 
rampFloat greenRamp; 
rampFloat blueRamp;
rampFloat yellowRamp;

// Sunrise colorssys
int yellow[7] = {250, 150, 150, 250, 250, 250, 250};
int red[7] =    {0, 0, 0, 0, 0, 0, 100};
int green[7] =  {250, 250, 250, 250, 250, 100, 0};
int blue[7] =   {250, 250, 150, 100, 0, 0, 50};

// Noon colors
int noonYellow[2] = {50, 0};
int noonRed[2]    = {250, 0};
int noonGreen[2]  = {0, 200};
int noonBlue[2]   = {0, 200};

int finalColorIdx = 0; 
// Time is in milliseconds
long sunriseFade = 600000; // 10 min 
long noonFade = 300000; // 5min
void initController() {
  // Initialize DMX pins. 
  DmxSimple.usePin(3);
  DmxSimple.maxChannel(4);

  // Turn off all the lights. 
  DmxSimple.write(1, 255); // Red
  DmxSimple.write(2, 255); // Yellow
  DmxSimple.write(3, 255); // Green
  DmxSimple.write(4, 255); // Blue
}

void setup() {
  Serial.begin(9600); // set the baud rate

  // Initialize DMX controller. 
  initController();
}
void loop() {
  char inByte = ' ';
  if(Serial.available()){ // only send data back if data has been sent
    char inByte = Serial.read(); // read the incoming data

    // Don't update state if the current state is same as new state
    if (inByte != currentState) {
      Serial.print("Updating State to: ");
      Serial.println(inByte); 
      updateCurrentState(inByte);
    }
   }

   if (currentState == 'A') {
    executeSunrise(); // Update values of 
   } else if (currentState == 'B') {
    executeNoon();
   } else if (currentState == 'C') {
    executeSunset();
   } else if (currentState == 'D') {
    executeMidnight();
   }
}

void executeSunrise() {  
  // Once it's reached the final value, set new final value and animate to that
  if (redRamp.isFinished() && greenRamp.isFinished() 
        && blueRamp.isFinished() && yellowRamp.isFinished()) {

     // Set the next destination. 
     finalColorIdx = (finalColorIdx + 1) % 7; // Loop the colors. 
     Serial.print("Final color index: "); Serial.println(finalColorIdx);
     
     redRamp.go(red[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     greenRamp.go(green[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     blueRamp.go(blue[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     yellowRamp.go(yellow[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD);
  }
  
  // Write the current ramp value to the 
  DmxSimple.write(1, redRamp.update()); // Red
  DmxSimple.write(2, yellowRamp.update()); // Yellow
  DmxSimple.write(3, greenRamp.update()); // Green
  DmxSimple.write(4, blueRamp.update()); // Blue
}

void executeNoon() {
   if (redRamp.isFinished() && greenRamp.isFinished() 
        && blueRamp.isFinished() && yellowRamp.isFinished()) {

     // Set the next destination. 
     finalColorIdx = (finalColorIdx + 1); // Only 2 colors here
     if (finalColorIdx > 1) { 
      finalColorIdx = 0; 
     }
     
     Serial.print("Final color index: "); Serial.println(finalColorIdx);

//     Serial.print("Current, Final: ");
//     Serial.print(redRamp.update()); Serial.print(", ");
//     Serial.println(red[finalColorIdx]);
     
     redRamp.go(noonRed[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     greenRamp.go(noonGreen[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     blueRamp.go(noonBlue[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     yellowRamp.go(noonYellow[finalColorIdx], noonFade, LINEAR, ONCEFORWARD);
  }
//
//  Serial.print("Red, Yellow, Green, Blue:");
//  Serial.print(redRamp.update()); Serial.print(", ");
//  Serial.print(yellowRamp.update()); Serial.print(", ");
//  Serial.print(greenRamp.update()); Serial.print(", ");
//  Serial.println(blueRamp.update());
  
  // Write the current ramp value to the 
  DmxSimple.write(1, redRamp.update()); // Red
  DmxSimple.write(2, yellowRamp.update()); // Yellow
  DmxSimple.write(3, greenRamp.update()); // Green
  DmxSimple.write(4, blueRamp.update()); // Blue
}

void executeSunset() {
  // Once it's reached the final value, set new final value and animate to that
  if (redRamp.isFinished() && greenRamp.isFinished() 
        && blueRamp.isFinished() && yellowRamp.isFinished()) {

     // Set the next destination. 
     finalColorIdx = finalColorIdx -1; 
     if (finalColorIdx == -1) {
        finalColorIdx = 6; 
     }
     Serial.print("Final color index: "); Serial.println(finalColorIdx);
     
     redRamp.go(red[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     greenRamp.go(green[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     blueRamp.go(blue[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD); 
     yellowRamp.go(yellow[finalColorIdx], sunriseFade, LINEAR, ONCEFORWARD);
  }
  
  // Write the current ramp value to the 
  DmxSimple.write(1, redRamp.update()); // Red
  DmxSimple.write(2, yellowRamp.update()); // Yellow
  DmxSimple.write(3, greenRamp.update()); // Green
  DmxSimple.write(4, blueRamp.update()); // Blue
}

void executeMidnight() {
  if (redRamp.isFinished() && greenRamp.isFinished() 
        && blueRamp.isFinished() && yellowRamp.isFinished()) {

     // Set the next destination. 
     finalColorIdx = (finalColorIdx - 1); // Only 2 colors here 
     if (finalColorIdx == -1) {
        finalColorIdx = 1; 
     }
     
     Serial.print("Final color index: "); Serial.println(finalColorIdx);
     
     redRamp.go(noonRed[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     greenRamp.go(noonGreen[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     blueRamp.go(noonBlue[finalColorIdx], noonFade, LINEAR, ONCEFORWARD); 
     yellowRamp.go(noonYellow[finalColorIdx], noonFade, LINEAR, ONCEFORWARD);
  }
  
  // Write the current ramp value to the 
  DmxSimple.write(1, redRamp.update()); // Red
  DmxSimple.write(2, yellowRamp.update()); // Yellow
  DmxSimple.write(3, greenRamp.update()); // Green
  DmxSimple.write(4, blueRamp.update()); // Blue
}

void updateCurrentState(char inByte) {
  switch (inByte) {
    case 'A': { // Sunrise
      Serial.println("Initializing Sunrise State");
      currentState = inByte;
      
      // Initialize ramp variables. 
      redRamp.init(red[0]);
      greenRamp.init(green[0]);
      blueRamp.init(blue[0]);
      yellowRamp.init(yellow[0]);
      
      // Fire all the ramps in once sided direction. 
      redRamp.go(red[1], noonFade, LINEAR, ONCEFORWARD);
      greenRamp.go(green[1], noonFade, LINEAR, ONCEFORWARD);
      blueRamp.go(blue[1], noonFade, LINEAR, ONCEFORWARD); 
      yellowRamp.go(yellow[1], noonFade, LINEAR, ONCEFORWARD);

      finalColorIdx = 1; 

      Serial.println("Kicked off Ramp");
      
      break;
    }

    case 'B': { // Noon
      Serial.println("Initializing Noon State");
      currentState = inByte;
      
      // Initialize ramp variables. 
      redRamp.init(noonRed[0]);
      greenRamp.init(noonGreen[0]);
      blueRamp.init(noonBlue[0]);
      yellowRamp.init(noonYellow[0]);

      // Fire all the ramps in once sided direction. 
      redRamp.go(noonRed[1], sunriseFade, LINEAR, ONCEFORWARD);
      greenRamp.go(noonGreen[1], sunriseFade, LINEAR, ONCEFORWARD);
      blueRamp.go(noonBlue[1], sunriseFade, LINEAR, ONCEFORWARD); 
      yellowRamp.go(noonYellow[1], sunriseFade, LINEAR, ONCEFORWARD);

      finalColorIdx = 1; 

      Serial.println("Kicked off Ramp");
      
      break;
    }

    case 'C': { // Sunset
      Serial.println("Initializing Sunset State");
      currentState = inByte;

      finalColorIdx = 6;
      
      // Initialize ramp variables. 
      redRamp.init(red[finalColorIdx]);
      greenRamp.init(green[finalColorIdx]);
      blueRamp.init(blue[finalColorIdx]);
      yellowRamp.init(yellow[finalColorIdx]);
      
      // Fire all the ramps in once sided direction. 
      redRamp.go(red[finalColorIdx-1], noonFade, LINEAR, ONCEFORWARD);
      greenRamp.go(green[finalColorIdx-1], noonFade, LINEAR, ONCEFORWARD);
      blueRamp.go(blue[finalColorIdx-1], noonFade, LINEAR, ONCEFORWARD); 
      yellowRamp.go(yellow[finalColorIdx-1], noonFade, LINEAR, ONCEFORWARD);

      finalColorIdx = finalColorIdx-1; 

      Serial.println("Kicked off Ramp");
      
      break;
    }

    case 'D': { // Midnight
      Serial.println("Initializing Midnight State");
      currentState = inByte;
      
      // Initialize ramp variables. 
      redRamp.init(noonRed[1]);
      greenRamp.init(noonGreen[1]);
      blueRamp.init(noonBlue[1]);
      yellowRamp.init(noonYellow[1]);

      // Fire all the ramps in once sided direction. 
      redRamp.go(noonRed[0], sunriseFade, LINEAR, ONCEFORWARD);
      greenRamp.go(noonGreen[0], sunriseFade, LINEAR, ONCEFORWARD);
      blueRamp.go(noonBlue[0], sunriseFade, LINEAR, ONCEFORWARD); 
      yellowRamp.go(noonYellow[0], sunriseFade, LINEAR, ONCEFORWARD);

      finalColorIdx = 0; 

      Serial.println("Kicked off Ramp");
      
      break;
    }

    default: 
      break;
  }
}
