// include lcd header
#include <LiquidCrystal.h>

// init the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// constants for the swedish characters
byte AwithRing[8] = {
  B00100, B01010, B01110, B00001, B01111, B10001, B01111
};

byte AwithDots[8] = {
  B01010, B00000, B01110, B00001, B01111, B10001, B01111
};
  
byte OwithDots[8] = {
  B01010, B00000, B01110,B10001, B10001, B10001, B01110
};

byte BigAwithRing[8] = {
  0b00100, 0b01010, 0b01110, 0b10001, 0b11111, 0b10001, 0b10001, 0b00000
};

byte BigAwithDots[8] = {
  0b01010, 0b00000, 0b01110, 0b10001, 0b11111, 0b10001, 0b10001, 0b00000
};

byte BigOwithDots[8] = {
  0b01010, 0b01110, 0b10001, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000
};

// welcome messages
String msg_welcome = "Välkommen !";
String msg_start = "Tryck på knappen"; 

// globlal variable used to determine if key is pressed or not
bool btnpressed = false;

// list of random words to display
String words[] = { 
  "Morot", "Äpple", "Päron", "Banan", "Ananas", "Mandarin", "Vindruvor",
  "Bil", "Båt", "Flygplan", "Motorcykel", "Buss", "Lastbil", "Moped",
  "Fotboll", "Basketboll", "Tennis", "Pingis", "Hockey", "Höjdhoppning",
  "Hamburgare", "Pannkakor", "Köttbullar", "Glass", "Pizza", "Bullar", 
  "Bio", "Fröken", "Träd", "Lampa", "Kotte", "Leksaker", "Docka", "Säng"};

// helper function to replace swedish chars with our custom glyphs
void printmsg(String str){
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
  Serial.println("setting pinmode input_pullup on pin 2");

  // configure pin 2 to "report" LOW when pulled to gnd
  pinMode(2, INPUT_PULLUP);

  // configure pin 3 to be an "output pin"
  pinMode(3, OUTPUT);
  
  // seed the random generator
  randomSeed(analogRead(0));

  // configure the dimensions of the lcd and print our
  // welcome message
  lcd.begin(16, 2);
  printmsg(msg_welcome);

  // move cursor on lcd and print starting 
  lcd.setCursor(0, 1);
  printmsg(msg_start);
}

// loop function for running the game
void loop() {

    // if reading of pin 2 reports low, and global button variable
    // is false, it means that we are pressing the button 
    if (digitalRead(2) == LOW && btnpressed == false){
      Serial.println("btn pressed");

      // light up led on pin 3 
      digitalWrite(3, HIGH);
      
      // we use this variable to prevent multiple keypresses while 
      // holding down the button 
      btnpressed = true;

      // get random number within the range 
      int rnd = random(sizeof(words) / sizeof(String));
      Serial.print("random number set to "); Serial.println(rnd);
      Serial.print("word to print is thus "); Serial.println(words[rnd]);

      // clear lcd, set cursor position and print random word
      lcd.clear();
      lcd.setCursor(0, 0);
      printmsg(words[rnd]);

    }

    // if reading of pin 2 reports high, and global button variable
    // is true, it means we just release the button, 
    if (digitalRead(2) == HIGH && btnpressed == true){
      Serial.println("btn released");

      // turn off led on pin 3 
      digitalWrite(3, LOW);
      
      // reset state of global button variable
      btnpressed = false;
    }
}
