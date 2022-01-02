// include lcd header
#include <LiquidCrystal.h>

// init the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

// constants for the swedish characters
byte AwithRing[8] = {B00100, B01010, B01110, B00001, B01111, B10001, B01111};

byte AwithDots[8] = {B01010, B00000, B01110, B00001, B01111, B10001, B01111};

byte OwithDots[8] = {B01010, B00000, B01110, B10001, B10001, B10001, B01110};

byte BigAwithRing[8] = {0b00100, 0b01010, 0b01110, 0b10001,
                        0b11111, 0b10001, 0b10001, 0b00000};

byte BigAwithDots[8] = {0b01010, 0b00000, 0b01110, 0b10001,
                        0b11111, 0b10001, 0b10001, 0b00000};

byte BigOwithDots[8] = {0b01010, 0b01110, 0b10001, 0b10001,
                        0b10001, 0b10001, 0b01110, 0b00000};

// global strings 
String msg_player_one = "Första spelaren";
String msg_player_two = "Andra spelaren";
String msg_start = " startar om ";
String msg_started = " Spelet startat ";
String msg_player_one_started = " för spelare  1";
String msg_player_two_started = " för spelare  2";
String msg_reaction_time = "Tid : ";
String msg_winner_one = "Spelare 1 vann !";
String msg_winner_two = "Spelare 2 vann !";
String msg_winner_msg = "   Grattis :)";
String msg_winner_none = " Det blev lika :)";
String msg_info_player_one = "Spelare 1 var";
String msg_info_player_two = "Spelare 2 var";
String msg_info_too_slow = "för långsam :(";

// global counters for each player
unsigned long time_player_start;
unsigned long time_player_current;
unsigned long time_player_elapsed;
unsigned long time_player_one;
unsigned long time_player_two;

// global counters for each round
unsigned long time_round_start;
unsigned long time_round_current;
unsigned long time_round_elapsed;

// global game state variables
int game_round = 0;
int rnd = 0;
bool game_started = false;;

// globlal variable used to determine if key is pressed or not
bool btnpressed = false;
int active_pin = 0;

// global vars for the random time generation
int rnd_msec_min = 3000;
int rnd_msec_max = 15000;
int player_timeout_msec = 30000;
int count_down_starter = 5;

// helper function to replace swedish chars with our custom glyphs
void printmsg(String str) {
  str.replace("å", "\1");
  str.replace("ä", "\2");
  str.replace("ö", "\3");
  str.replace("Å", "\4");
  str.replace("Ä", "\5");
  str.replace("Ö", "\6");
  lcd.print(str);
}

// setup function to init the lcd and the random generator
void setup() {
  // create the custom glyphs for the swedish chars
  lcd.createChar(1, AwithRing);     // å
  lcd.createChar(2, AwithDots);     // ä
  lcd.createChar(3, OwithDots);     // ö
  lcd.createChar(4, BigAwithRing);  // Å
  lcd.createChar(5, BigAwithDots);  // Ä
  lcd.createChar(6, BigOwithDots);  // Ö

  delay(1000);
  Serial.begin(9600);
  Serial.println("= = = = game starting = = = = ");
  Serial.println("setting pinmode input_pullup on pin 2,3");
  Serial.println("setting pinmode output on pin 4");

  // configure pin 2,3 to "report" LOW when pulled to gnd
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  // configure pin 4 to be an "output pin"
  pinMode(4, OUTPUT);
  
  // configure the dimensions of the lcd
  lcd.begin(16, 2);

  // initialize the game
  init_game();
}

// function called on button press
void btn_pressed(int pin) {
  bool pressed_to_early = false;
  String msg = "Spelare 1 var";
  int spacing;

  btnpressed = true;
  active_pin = pin;
  Serial.print("pressing btn on pin ");Serial.println(pin);

  // user presses the button 
  if (pin == 3){
    // calculate time for the user, from when led was lit
    time_player_current = millis();
    time_player_elapsed = time_player_current - time_player_start;

    // player one game
    if (game_round == 0){
      // if game was not started yet, user pressed to quickly
      if (game_started == false){
        pressed_to_early = true;
        time_player_one = 1;
        time_player_two = 0;
      }else{
        time_player_one = time_player_elapsed;
      }
    // player two game
    }else{
      // if game was not started yet, user pressed to quickly
      if (game_started == false){
        msg = "Spelare 2 var";
        pressed_to_early = true;
        time_player_one = 0;
        time_player_two = 1;
      }else{
        time_player_two = time_player_elapsed;
      }
    }

    // if either of the players were to quick, just print a message
    // and end game by increasing the game counter
    if (pressed_to_early == true){
      lcd.clear();
      lcd.print(msg);
      lcd.setCursor(0, 1);
      printmsg("för snabb :(");
      game_round++;
      return;
    }

    // we get here, just print the elapsed time for the user
    // code for handling "next game logic" is in the released button function
    Serial.print("took : ");Serial.print(time_player_elapsed);Serial.println(" ms");
    lcd.clear();
    printmsg(msg_reaction_time); 
    lcd.setCursor(6, 0);

    if (game_round == 0){
      lcd.print(time_player_one);
      spacing = floor (log10 (abs (time_player_one))) + 1;
    }else{
      lcd.print(time_player_two);
      spacing = floor (log10 (abs (time_player_two))) + 1;
    }
    
    lcd.setCursor(7+spacing, 0);
    lcd.print("ms");
  }  
}

// function called when button is released
void btn_released(int pin) {
  btnpressed = false;
  active_pin = 0;
  Serial.print("releasing btn on pin ");
  Serial.println(pin);
  delay(2000);

  // user releases the buton
  if (pin == 3){    

    // if its the first player, just increase the game round counter
    // and initialize a new game 
    if (game_round == 0){
      game_round++;
    }else{

      // if its the second player though, print the winner
      print_winner();
   
      // reset the game round counter to restart the game
      game_round = 0;
    }
    
    // reinitialize the game   
    init_game();
  }
}

// wrapper function to calculate and print the winner
void print_winner(){
  
  lcd.clear();
  if (time_player_one == time_player_two){
    printmsg(msg_winner_none); 
  }else if (time_player_one < time_player_two){
    printmsg(msg_winner_one); 
  }else{
    printmsg(msg_winner_two); 
  }
  
  lcd.setCursor(0, 1);
  printmsg(msg_winner_msg);
  delay(2000);   
}

// function used to initialize the game, the game_round counter
// is used to determine player
void init_game() {
  // reset all game round/state variables used
  game_started = false;
  time_round_start = 0;
  time_round_current = 0;
  time_round_elapsed = 0;
  
  // clear display, turn of led, seed random generator 
  lcd.clear();
  digitalWrite(4, LOW);
  randomSeed(analogRead(0));

  if (game_round == 0){
    printmsg(msg_player_one);
  }else{
    printmsg(msg_player_two);
  }
  
  lcd.setCursor(0, 1);
  printmsg(msg_start);

  // show a simple countdown for the user
  for (int cnt=count_down_starter; cnt>0; cnt--){
    Serial.println(cnt);
    lcd.setCursor(msg_start.length()+1, 1);
    lcd.print(cnt);
    delay(1000);
  }

  // clear display, show message and set random time to wait for
  // led to light up 
  lcd.clear();
  printmsg(msg_started);
  lcd.setCursor(0, 1);

  if (game_round == 0){
    printmsg(msg_player_one_started);
  }else{
    printmsg(msg_player_two_started);
  }

  rnd = random(rnd_msec_min,rnd_msec_max);
  Serial.print("random sleep is set to ");Serial.print(rnd);Serial.println(" ms");
  time_round_start = millis();  
}



// loop function for running the game
void loop() {
  
  // loop btn inputs and act on presses / releases
  for (int pin = 2; pin < 4; pin++) {
    if (digitalRead(pin) == LOW && btnpressed == false) {
      btn_pressed(pin);
    }
    if (digitalRead(active_pin) == HIGH && btnpressed == true) {
      btn_released(active_pin);
    }
  }

  // set current round time, calculate the elapsed time so we now
  // when to light up the led 
  time_round_current = millis();
  time_round_elapsed = time_round_current - time_round_start;
  
  if (time_round_elapsed > rnd && game_started == false){
    digitalWrite(4, HIGH);
    Serial.println("led is lit, waiting for user reaction");
    time_player_start = millis();
    game_started = true;
  }
  
  // if the user never presses, just declare winner and restart game
  if (time_round_elapsed > player_timeout_msec && game_started == true){
    Serial.println("user to slow");
    lcd.clear();
    
    // player one game
    if (game_round == 0){
      lcd.print(msg_info_player_one);
      time_player_one = 1;
      time_player_two = 0;
    }else{
      // player two game
      lcd.print(msg_info_player_two);
      time_player_one = 0;
      time_player_two = 1;
    }
       
    lcd.setCursor(0, 1);
    printmsg(msg_info_too_slow);

    // reset the game round counter to restart the game
    delay(2000);

    print_winner();
    game_round = 0;
    init_game();
  }
}
