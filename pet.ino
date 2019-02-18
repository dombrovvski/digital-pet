SYSTEM_THREAD(ENABLED);

#include <Adafruit_LEDBackpack_RK.h>
#include <Adafruit_GFX_RK.h>
#include <Adafruit_APDS9960_Particle.h>

Adafruit_APDS9960 apds;

//motor

int motorValue = 0;

Adafruit_8x8matrix matrix = Adafruit_8x8matrix();

uint16_t r, g, b, c;
int R,G,B;

int prev = 0; //let's watch for rising and falling edge!!
int btnState = 0;
int flag = 1;

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },
  rainy_bmp[] =
  { B11111111,
    B11111111,
    B11011011,
    B11111111,
    B11111111,
    B11000011,
    B10111101,
    B01111110};
    
Timer timer(1000, resetFlag);

    
//<----------------------------------SETUP------------------------------------------->

void setup() {
    
  Serial.begin(9600);
  waitUntil(Particle.connected);

  if(!apds.begin()){
    Serial.println("Helppppp we've failed to initialize! Please check my wiring :(");
  }
  else Serial.println("Device initialized!");
  
  pinMode(D5, INPUT); //apds
  
  matrix.begin(0x70); //start LED 
  matrix.setBrightness(10);
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  
  apds.enableColor(true);
  apds.enableGesture(true);
  
  
  pinMode (D3, OUTPUT);
  pinMode (D2, OUTPUT);
  
  pinMode (D6, OUTPUT); //slp
  
  pinMode(D7, INPUT); //button input
  
  Particle.variable("R", R); //setting uppercase as tracked so only btnpush rgb monitored
//   Particle.variable("G", G);
//   Particle.variable("B", B);
  
  Particle.function("sadWeather", weatherResponse); //register cloud function

  delay(500);
}

//<----------------------------------LOOOOOOP------------------------------------------->


void loop() {
    
  //wait for color data to be ready
  while(!apds.colorDataReady()){
    delay(5);
  }
  //get the data and print the different channels
  apds.getColorData(&r, &g, &b, &c);
  
  delay(500);
  
  motorValue = (r + g + b) ; 
  analogWrite(D6, motorValue); //slp
  delay(30);      
  
  pBehave();
  buttonTrigger();
  motorRun();
}

//<----------------------------------RESET FLAG------------------------------------------->


void resetFlag() {
  timer.stop();
  flag = 1;
 }

//<----------------------------------BUTTON TRIGGER------------------------------------------->


void buttonTrigger () {
    
  btnState = digitalRead(D4);
  
  if(prev == 1) {  
    if (btnState == 0) {
      timer.start();
    }
  }
  
  if (prev == 0 ) {
    if (btnState == 1) {
      if (!timer.isActive()) { //before: if (timer > 1000)
        flag = 1;
      } else {
        flag = 0;
      }
      timer.stop();
    }
  }
  
  if (flag == 1) {
    if (btnState == 1) {
      Serial.print(flag);
      R = (int) r; //cast uint16_t to double
//       G = (int) g;
// 	  B = (int) b;
      flag = 0;
    }
  }
  prev = btnState;
}

 

//<----------------------------------BEHAVIOR------------------------------------------->

void pBehave() {
  if (r > g && r > b ) {
    matrix.clear();
    matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    
  }
    
  if (b > g && b > r) {
    matrix.clear();
    matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    
  }
  
  if (g > r && g > b) {
    matrix.clear();
    matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_ON);
    matrix.writeDisplay();
    delay(100);
    
  }
}

//<----------------------------------MOTOR------------------------------------------->
void motorRun() {
    if (r > g && r > b ) {
      digitalWrite(D3, HIGH);
      digitalWrite(D2, LOW);
    }
    // if (b > g && b > r) {
    //   digitalWrite(ain1, LOW);
    //   digitalWrite(ain2, HIGH);
    // }
    // while (g > r && g > b) {
    // }
}

//<----------------------------------IFTTT-TRIGGERED ACTION------------------------------------------->

int weatherResponse(String command) {
  matrix.clear();
  matrix.drawBitmap(0, 0, rainy_bmp, 8, 8, LED_ON);
  matrix.writeDisplay();
  delay(1000);
  return 1;
}

