/**
Copyright (c) 2016 Nikita Pchelin

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* PINs */
const int PIN_MODE_BUTTON = 3; // The pin to which the mode button is hooked up.
const int PIN_RED_LED = 11; // The pin that controls the RED component of the LED.
const int PIN_BLUE_LED = 10; // The pin that controls the BLUE component of the LED.
const int PIN_GREEN_LED = 9; // The pin that controls the GREEN component of the LED.

/* Button behaviour. */
const long debounceDelay = 50; // Debounce time delay.
int modeButtonState = LOW; // Current state of the mode button.
int prev_reading = LOW; // Previous read state of the mode button.
unsigned long lastDebounceTime = 0; // The last time the reading changed.
const int longPressDelay = 1000;  // how much time in ms needs to pass before this is considered a long press.
boolean longPressInProgress = false; // indicates if long press is currently in progress.


/* Light modes and behaviour */
const int LIGHT_MODE_SINGLE_SOLID = 0; // display single solid color, does not change until mode button is clicked.
const int LIGHT_MODE_SINGLE_PULSATE = 1; // pulsate the single solid color, does not change until mode button is clicked.
const int LIGHT_MODE_MULTIPLE_PULSATE = 2; // pulsate the single solid color, and switch to the next one over automatically.

int selectedMode = 0; // Indicates current light mode.
int selectedColor = 0; // Indicates current light color.

const long COLOR_PULSE_STEPS = 5000; // how many "steps" the lightning should happen over - each step takes at least 1ms.
int currentStep = -1; // current lightning step.
int oldR, oldG, oldB = -1; // old color.
int newR, newG, newB = -1; // new color.

const int COLORS[5][3] = { // The array of colors (R, G, B) for the light.
  {255, 0, 0}, // Red
  {0, 255, 0}, // Green
  {0, 0, 255}, // Blue
  {180, 161, 0}, // Yellow
  {34, 0, 225} // Purple
};

/* Sleep variables. */
boolean isAsleep = false; // Indicates whether we are currently asleep or not.
unsigned long sleepTimer = 0; // Sleep timer from which the count starts.
const long SLEEP_INTERVALS[3] = {900000, 1800000, 3600000}; // 15 / 30 / 60 minutes delay.
int selectedSleepInterval = -1; // The sleep interval that is currently selected.

void setup() {
  pinMode(PIN_MODE_BUTTON, INPUT);

  // Init mode and color.
  selectedMode = 0;
  selectedColor = 0;
  lightRGB(0, 0, 0);
}

void loop() {
  // Read the states of the pin.
  int reading = digitalRead(PIN_MODE_BUTTON);

  // If the current reading is different from the reading obtained in the previous cycle, start
  // counting the delay.
  if (reading != prev_reading) {   
    lastDebounceTime = millis();  
  }

  // Remember the reading taken in this loop.
  prev_reading = reading;

  // If the time interval specified by the debounce delay has passed and the
  // most recent reading is not the same as the current button state 
  // we consider it a successful reading.
  if (((millis() - lastDebounceTime) > debounceDelay  && (reading != modeButtonState))) { 
    // Update the button stae.
    modeButtonState = reading;

    // If the state is HIGH, assume that this is a long press.
    if (modeButtonState == HIGH){
      longPressInProgress = true;
    } else {
      // If the state is LOW, and long press was in progress (but has not
      // completed, handle this as a short press.
      if (longPressInProgress) {
        longPressInProgress = false;
        handleShortPress();
      }
    }
  } 

  // If the long press in progress, and we've surpassed the long press delay required,
  // we can handle this as a long press.
  if (longPressInProgress && ((millis() - lastDebounceTime) > longPressDelay)) { 
    handleLongPress();
    longPressInProgress = false;        
  }

  // If we are awake and a sleep interval was selected, and the required time has elapsed, go to sleep.
  // Note that here we take advantage of the short-circuit evaluation. If we are not awake, the
  // last condition won't be checked so we should not worry about the overflow.
  if (isAsleep == false && selectedSleepInterval != -1 && millis() - sleepTimer > SLEEP_INTERVALS[selectedSleepInterval]){
    isAsleep = true;
    lightRGB(0, 0, 0);
  }

  // If we are not asleep,setup the lightning.
  if (!isAsleep) {
    switch (selectedMode) {
      // For solid color, just use the values of the currently selected
      // color from the color table.
      case LIGHT_MODE_SINGLE_SOLID:
        lightRGB(COLORS[selectedColor][0], COLORS[selectedColor][1], COLORS[selectedColor][2]);
        break;
  
      // For single pulsating color, do the initial setup, the pulsating is between
      // (0, 0, 0) and the target color.
      case LIGHT_MODE_SINGLE_PULSATE:
        if (currentStep == -1) {
          oldR = 0; newR = COLORS[selectedColor][0];
          oldG = 0; newG = COLORS[selectedColor][1];
          oldB = 0; newB = COLORS[selectedColor][2];
  
          currentStep = 1;
        }
  
        // Light up!
        lightStep(true);
        break;
  
      // For multiple pulsating colors, do the initial setup, the pulsating is between
      // the fest color and the following color.
      case LIGHT_MODE_MULTIPLE_PULSATE:
        if (currentStep == -1) {
          oldR = COLORS[selectedColor][0]; newR = COLORS[selectedColor + 1][0];
          oldG = COLORS[selectedColor][1]; newG = COLORS[selectedColor + 1][1];
          oldB = COLORS[selectedColor][2]; newB = COLORS[selectedColor + 1][2];
  
          // Advance color selection because once this cycke is over,
          // we will go from color 1 to color 2 in the table.
          selectedColor = 1;
          currentStep = 1;
        }
  
        // Light up!
        lightStep(false);
        break;
    }
  }
}

/**
 * Handles long press which is used to set a sleep timer.
 */
void handleLongPress(){
  // Capture when timer starts.
  sleepTimer = millis();

  // Advance to the next interval (loops through the possible values).
  if (selectedSleepInterval + 1 < sizeof(SLEEP_INTERVALS) / sizeof(SLEEP_INTERVALS[0])) {
    selectedSleepInterval++;
  } else {
    selectedSleepInterval = 0;
  }

  // Acknowledge first interval with 1 red blink, second with 2, third with 3...
  showAcknowledgementPattern((selectedSleepInterval + 1));
}

/**
 * Handles short press which is used to loop through colors.
 */
void handleShortPress(){
    // If we are asleep, cancel the timer and do nothing.
    if (isAsleep){
      isAsleep = false;
      sleepTimer = 0;
      selectedSleepInterval = -1;
    } else {
      switch (selectedMode) {
        // If the current mode is to display solid lights:
        case LIGHT_MODE_SINGLE_SOLID:
          // Move to the next color, unless the current color is the last color in the list.
          if (selectedColor + 1 < sizeof(COLORS) / sizeof(COLORS[0])) {
            selectedColor++;
            // If the current color is the last color in the list, move to the next state.
          } else {
            selectedMode = LIGHT_MODE_SINGLE_PULSATE;
            currentStep = -1;
            selectedColor = 0;
          }
          break;
    
        // If current mode is to pulsate a single light:
        case LIGHT_MODE_SINGLE_PULSATE:
          // Move to the next color, unless the current color is the last color in the list.
          if (selectedColor + 1 < sizeof(COLORS) / sizeof(COLORS[0])) {
            selectedColor++;
            currentStep = -1;
            // If the current color is the last color in the list, move to the next state.
          } else {
            selectedMode = LIGHT_MODE_MULTIPLE_PULSATE;
            currentStep = -1;
            selectedColor = 0;
          }
          break;
    
        // If the current mode is to pulsate through multiple lights:
        case LIGHT_MODE_MULTIPLE_PULSATE:
          // Move to the next state.
          selectedMode = LIGHT_MODE_SINGLE_SOLID;
          selectedColor = 0;
          break;
      }
  }
}

/**
 * Lighting processing for single pulsating light / crossfading lights.
 */
void lightStep(bool single_pulse) {
  // Calculate the appropriate value for each component and light up.
  lightRGB(calculateComponentValue(oldR, newR), calculateComponentValue(oldG, newG), calculateComponentValue(oldB, newB));

  // Delay of at least 1ms for each step.
  delay(1);

  // Increment the steo count.
  currentStep++;

  // Check if we've completed all steps.
  if ((currentStep > COLOR_PULSE_STEPS)) {

    // For a single pulse, we simply swap the values
    // of (0, 0, 0) and the current color.
    if (single_pulse == true) {
      swap( & oldR, & newR);
      swap( & oldG, & newG);
      swap( & oldB, & newB);

      // For multiple pulses, we shift old/new color by one.
    } else {

      int idx_oldC = selectedColor;
      int idx_newC = (selectedColor + 1 < sizeof(COLORS) / sizeof(COLORS[0])) ? selectedColor + 1 : 0;

      selectedColor = idx_newC;

      oldR = COLORS[idx_oldC][0]; newR = COLORS[idx_newC][0];
      oldG = COLORS[idx_oldC][1]; newG = COLORS[idx_newC][1];
      oldB = COLORS[idx_oldC][2]; newB = COLORS[idx_newC][2];

    }

    // Reset step count.
    currentStep = 1;
  }
}

/**
 * Calculate the component change delta and divide it by the number of steps to calculate
 * change per step. Then, multiply it by the current step to obtain the delta value between
 * the original color component value to the component value at the current step.
 */
int calculateComponentValue(int oldV, int newV) {
  int delta = round((abs(oldV - newV) / (COLOR_PULSE_STEPS * 1.0)) * currentStep);
  int calV = -1;

  // Going down.
  if (oldV > newV) {
    calV = oldV - delta;

    // Just a precaution to wrap up the values.
    if (calV < 0) {
      calV = 0;
    }
    // Or up.
  } else {
    calV = oldV + delta;

    // Just a precaution to wrap up the values.
    if (calV > 255) {
      calV = 255;
    }
  }

  return calV;
}

/**
 * Helper, swaps two integers.
 */
void swap(int * a, int * b) {
  int tmp = * a; * a = * b; * b = tmp;
}

/**
 * Helper, lights the RGB light.
 */
void lightRGB(int red, int green, int blue){
    analogWrite(PIN_RED_LED, red);
    analogWrite(PIN_GREEN_LED, green);
    analogWrite(PIN_BLUE_LED, blue);  
}

/**
 * Helper that shows an acknowledgement pattern - a number of
 * blinks specified by blink_count.
 */
void showAcknowledgementPattern(int blink_count) {
  for (int i=0; i < blink_count; i++){
      lightRGB(255, 0, 0);
      delay(200);
      
      lightRGB(0, 0, 0);
      delay(200);
  }
}
