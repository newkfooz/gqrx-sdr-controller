#include <ProTrinketHidCombo.h>

// Keyboard___________________________________________________________________
const int Shutt_1 = 4;                     // Shuttle Wheel Pin 1
const int Shutt_2 = 5;                     // Shuttle Wheel Pin 2
const int Shutt_3 = 6;                     // Shuttle Wheel Pin 3
const int Shutt_4 = 8;                     // Shuttle Wheel Pin 4
int decimal = 0;
byte flagShuttle = 0;

const int Jog_A = 12;                      // Jog Wheel Pin A
const int Jog_B = 13;                      // Jog Wheel Pin B
int count = 0;
int lastCount = 0;
int aState;
int aLastState;

// Mouse___________________________________________________________________
// set pin numbers for joystick axes, and click-button
const int mouseButton = 9;   // input pin for the mouse pushButton
const int xAxis  = A2;         // joystick X axis to A1 
const int yAxis  = A3;         // joystick Y axis to A0

// parameters for reading the joystick
int range = 10;               // output range of X or Y movement (zero to range)
int responseDelay = 5;        // response delay of the mouse, in ms
int threshold = range/5;      // resting threshold
int center = range/2;         // resting position value


boolean mouseButtonPressed = false; // whether or not mouse button pressed
int  lastReading = 0;       // last joystick/mouse button reading
long debounceTime = 0;      // last time the mouse button was toggled
long debounce = 50;         // debounce time, increase if the mouse clicks rapidly



//////////////////////////////////////////// SETUP //////////////////////////////////////////
void setup() {
    pinMode(mouseButton, INPUT_PULLUP);          // mouse button on joystick
        
    pinMode(Shutt_1, INPUT_PULLUP);
    pinMode(Shutt_2, INPUT_PULLUP);
    pinMode(Shutt_3, INPUT_PULLUP);
    pinMode(Shutt_4, INPUT_PULLUP);
    pinMode(Jog_A, INPUT_PULLUP);
    pinMode(Jog_B, INPUT_PULLUP);
    // remember, the buttons are active-low, they read LOW when they are not pressed

    aLastState = digitalRead(Jog_A);
    
    // start USB stuff
    TrinketHidCombo.begin();
}


//////////////////////////////////////////// LOOP //////////////////////////////////////////
void loop() {
    
    TrinketHidCombo.poll(); // check if USB needs anything done, do every 10 ms or so
    mouse();
    readShuttle();
    readJog();
    
}


////##################################### FUNCTIONS ####################################////
////************************************************************************************////

void readShuttle(){
    decimal = (digitalRead(Shutt_1)==LOW?1:0)+          // transform the shuttle wheel's pins binary values in a decimal complessive value
              (digitalRead(Shutt_2)==LOW?2:0)+
              (digitalRead(Shutt_3)==LOW?4:0)+
              (digitalRead(Shutt_4)==LOW?8:0);
    //Serial.println(decimal);
    if (decimal != 12){
        if (decimal < 2 && flagShuttle == 0){
            //set cursor on the left
            TrinketHidCombo.pressKey(0, KEYCODE_ARROW_LEFT);         // this should type the "<--" Key
            TrinketHidCombo.pressKey(0, 0);                       // this releases the key 
            flagShuttle = 1;                                         //pressKey expects 2 values (modifier, keycode)!!!!!
        }
        if (decimal < 10 && decimal > 8 && flagShuttle == 0){
            //set cursor on the right
            TrinketHidCombo.pressKey(0, KEYCODE_ARROW_RIGHT);         // this should type the "-->" Key
            TrinketHidCombo.pressKey(0, 0);                       // this releases the key 
            flagShuttle = 1;
        }
    }
    else {
        flagShuttle = 0;
    }
}

void readJog(){
    aState = digitalRead(Jog_A);
    if (aState != aLastState){
        if (digitalRead(Jog_B) != aState){
            count++;
            if (count == lastCount+2){
                TrinketHidCombo.pressKey(0, KEYCODE_ARROW_UP);         // this should type the UP Key 
                TrinketHidCombo.pressKey(0, 0);                       // this releases the key 
                lastCount = count;
            }
            
        }else{
            count--;
            if (count == lastCount-2){
                TrinketHidCombo.pressKey(0, KEYCODE_ARROW_DOWN);         // this should type the DOWN Key
                TrinketHidCombo.pressKey(0, 0);                       // this releases the key 
                lastCount = count;
            }   
        }
    }
    aLastState = aState;
}

void mouse(){
  int buttonState;  // State of the mouse left button switch on joystick
  int xReading, yReading; // readings of the joystick movements
  int buttonReading;      // reading of the joystick (left mouse) button

  // read and scale the two joystick readings, one for each axis
  xReading = readAxis(xAxis);
  yReading = readAxis(yAxis);


  // Read the joystick button as the left mouse button
  buttonReading = digitalRead(mouseButton);  // read the mouse left button (push joystick)
  if(buttonReading != lastReading) {         // switch changed
     debounceTime = millis();                // reset debounce timer
  }
  if((millis() - debounceTime) > debounce) {
     buttonState = buttonReading;
     if(buttonState == LOW) {
        mouseButtonPressed = true;
     }
     else {
        mouseButtonPressed = false;
     }
  } 
  lastReading = buttonReading;

  
  //  move the mouse:
  if (mouseButtonPressed) {  // if joystick pressed down, indicate that too
         TrinketHidCombo.mouseMove(xReading, yReading, MOUSEBTN_LEFT_MASK);  //Clickleftmousebutton
  }
  else {
         TrinketHidCombo.mouseMove(xReading, yReading, 0);  // move, no mouse button press
  } 
  delay(responseDelay);  // wait between mouse readings
}

// Reads a joystick axis (x or y) and scales the analog input range to a range from 0 to <range>
int readAxis(int thisAxis) { 
  int reading = analogRead(thisAxis);  // read the analog input

  // map the reading from the analog input range to the output range
  reading = map(reading, 0, 1023, 0, range);

  // if the output reading is outside from the rest position threshold, use it
  int distance = center - reading;

  if (abs(distance) < threshold) { // if distance not to threshold, no move
    distance = 0;                  // prevents tiny jitters due to readings
  } 
  return distance;   // return the distance for this axis
}
