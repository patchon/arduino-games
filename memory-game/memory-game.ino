// pins for the buttns
int ledbtnwhite   = 2;
int ledbtnblue    = 4;
int ledbtnyellow  = 6;
int ledbtngreen   = 8;
int ledbtnred     = 10;
int ledbtns[]     = { ledbtnwhite, ledbtnblue, ledbtnyellow, ledbtngreen, ledbtnred };
int ledbtnscount  = 5;

// pins for controlling the leds
int ledpinwhite   = 3;
int ledpinblue    = 5;
int ledpinyellow  = 7;
int ledpingreen   = 9;
int ledpinred     = 11;
int ledpins[]     = { ledpinwhite, ledpinblue, ledpinyellow, ledpingreen, ledpinred };
int ledpinscount  = 5;

// global variables
int  btnpinpressed = 0;
int  ledstolight   = 3;
int  gamepincnt = 0;
bool btnpressed    = false;
bool gamestarted   = false;
int validseqlength = 255;
int validseq[255];

// setup function will set correct pinmode on pins
// and play the led initial sequence
void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println("= = = = = = = ");
  Serial.println("start of setup");

  for (int pin = 0; pin < ledpinscount; pin++) {
    Serial.print("setting pinmode output on pin ");
    Serial.println(ledpins[pin]);
    pinMode(ledpins[pin], OUTPUT);
  }

  for (int pin = 0; pin < ledbtnscount; pin++) {
    Serial.print("setting pinmode input_pullup on pin ");
    Serial.println(ledbtns[pin]);
    pinMode(ledbtns[pin], INPUT_PULLUP);
  }

  init_seq(5, 3);
}

// init_seq will flash leds back and forth in a nice sequence
// and blink them all to visualize that the game is starting
void init_seq(int pace, int reps){
  Serial.print("starting init sequence with pace "); Serial.print(pace); Serial.print("ms and "); Serial.print(reps);Serial.println(" repetitions");

  for (int rep = 0; rep < reps; rep++){
    for (int pin = 0; pin < ledpinscount; pin++) {
      Serial.print("pin iterator = "); Serial.print(pin); Serial.print(", setting pin "); Serial.print(ledpins[pin]); Serial.println(" to high");
      digitalWrite(ledpins[pin], HIGH);
      delay(pace);
      Serial.print("setting pin "); Serial.print(ledpins[pin]); Serial.println(" to low");
      digitalWrite(ledpins[pin], LOW);
    }

    for (int pin = ledpinscount-2; pin >= 1; pin--) {
      Serial.print("pin iterator = "); Serial.print(pin); Serial.print(", setting pin "); Serial.print(ledpins[pin]); Serial.println(" to high");
      digitalWrite(ledpins[pin], HIGH);
      delay(pace);
      Serial.print("setting pin "); Serial.print(ledpins[pin]); Serial.println(" to low");
      digitalWrite(ledpins[pin], LOW);
    }
  }

  for (int i = 0; i < reps*2; i++){
    turn_on_all_leds();
    delay(pace);
    turn_off_all_leds();
  }

  delay(2000);
}

// wrapper function to set defined pin high
void turn_on_led_on_pin(int pin){
  Serial.print("turning on led on pin "); Serial.println(pin);
  digitalWrite(pin, HIGH);
}

// wrapper function to set defined pin low
void turn_off_led_on_pin(int pin){
  Serial.print("turning off led on pin "); Serial.println(pin);
  digitalWrite(pin, LOW);
}

// wrapper function to set all led-pins high
void turn_on_all_leds(){
  for (int pin = 0; pin < ledpinscount; pin++) {
    Serial.print("setting pin "); Serial.print(ledpins[pin]); Serial.println(" to high");
    digitalWrite(ledpins[pin], HIGH);
  }
}

// wrapper function to set all led-pins low
void turn_off_all_leds(){
  for (int pin = 0; pin < ledpinscount; pin++) {
    Serial.print("setting pin "); Serial.print(ledpins[pin]); Serial.println(" to low");
    digitalWrite(ledpins[pin], LOW);
  }
}

// function called on button press
void btn_pressed(int pin){
  btnpressed       = true;
  btnpinpressed    = pin;
  int pin_to_light = pin+1;

  digitalWrite(pin_to_light,HIGH);
  Serial.print("pressing btn on pin "); Serial.print(pin);
  Serial.print(" to light led on pin "); Serial.println(pin_to_light);
}

// function called when button is released
void btn_released(int pin){
  btnpressed          = false;
  btnpinpressed       = 0;
  int ledpin_to_turn_off = pin+1;

  digitalWrite(ledpin_to_turn_off,LOW);
  Serial.print("releasing btn on pin "); Serial.print(pin);
  Serial.print(" to turn off led on pin "); Serial.println(ledpin_to_turn_off);

  chk_if_correct_pin_is_pressed(ledpin_to_turn_off);
}

// function to check if we actually pressed the correct
// button according to our game
void chk_if_correct_pin_is_pressed(int pin){
   Serial.print("checking if correct button is pressed, gamepinctn is set to "); Serial.println(gamepincnt);

  if (validseq[gamepincnt] == pin){
    Serial.print("correct button pin pressed "); Serial.println(pin);
    gamepincnt++;
  }else{
    Serial.print("wrong button pin pressed, "); Serial.print(pin);Serial.print(" is not the same as ");Serial.println(validseq[gamepincnt]);
    reset_game();
  }
}

// function called to reset game when user input was wrong
void reset_game(){

  // flash the red light to indicate that the user pressed the wrong button
  for (int i = 0; i < 10; i++){
    digitalWrite(ledpins[4], HIGH);
    delay(50);
    digitalWrite(ledpins[4], LOW);
    delay(50);
  }
  delay(1000);

  // reset global variables
  gamepincnt  = 0;
  ledstolight = 3;
  for (int i; i < validseqlength; i++){
    validseq[i] = 0;
  }

  // run init sequence and initalize a new game
  init_seq(5, 3);
  init_game(ledstolight);
}

// function used to initialize game, called for every turn with
// the number of leds to light up (will be increased for every turn by +1)
void init_game(int ledstolight){

  Serial.print("game init with "); Serial.print(ledstolight); Serial.println(" leds to light");

  // seed the random generator and get a random number
  randomSeed(analogRead(0));
  int rnd = random(ledpinscount);

  // for each led we should light up
  for (int i = 0; i < ledstolight; i++){
    Serial.print("rnd led is set to ");Serial.println(rnd);

    // blink the selected random led
    turn_on_led_on_pin(ledpins[rnd]);
    delay(1000);
    turn_off_led_on_pin(ledpins[rnd]);
    delay(500);

    // save that led to our global array so we can check when the user
    // presses if it was correct or not
    validseq[i] = ledpins[rnd];
    Serial.print("saving correct led pin ");Serial.print(validseq[i]);Serial.println(" to validseq array");

    // generate new random number
    rnd = random(ledpinscount);
  }

  // game state variable
  gamestarted = true;
}


void loop() {

  // loop btn inputs and act on presses / releases
  for (int pin = 0; pin < ledbtnscount; pin++){
    if (digitalRead(ledbtns[pin]) == LOW && btnpressed == false){
      btn_pressed(ledbtns[pin]);
    }
    if (digitalRead(btnpinpressed) == HIGH && btnpressed == true){
      btn_released(btnpinpressed);
    }
  }

  // start game if not alreade started
  if (gamestarted == false){
    init_game(ledstolight);
  }else{
    // if we get here, and we have pressed the correct buttons
    // equal to the leds we should have lightened up, it means
    // that we passed this level
    if (gamepincnt == ledstolight){
      Serial.print("new level");
      gamepincnt = 0;
      ledstolight++;
      init_seq(2, 1);
      init_game(ledstolight);
    }
  }
}
