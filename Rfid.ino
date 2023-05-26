//Code by Botan Denis of the class 3ATT-2022/2023 IIS Levi-Ponti(Levi)

//This code was made for an NFC/RFID controlled RGB lamp

//Libraries required for the functioning of the code
#include <SPI.h>
#include <MFRC522.h>
 
//Defines the pins required for the functioning of the code
#define SS_PIN 10
#define RST_PIN 9
#define R 3
#define G 5
#define B 6
#define SWITCH 2

//Defines the access codes of the cards
const char card0[12] = "60 9B 05 41";
const char card1[12] = "B0 79 55 80";
const char card2[12] = "60 AC 0E 41";
const char card3[12] = "B8 6C B2 3D";

//Defines the RGB values to feed into the led() function, the initial value of the advancedMode loop and the variable used to tore the value state of the SWITCH
int vec[7][3] = {/*WHITE*/{255, 255, 255},/*RED*/ {255, 0, 0},/*ORANGE*/ {255, 64, 0},/*YELLOW*/ {255, 255, 0},/*GREEN*/ {0, 255, 0},/*BLU*/ {0, 0, 255}, /*Purple*/ {255, 0, 255}}, i = -1, sw;

bool advancedMode = false, selector = true;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup(){
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus

  mfrc522.PCD_Init();   // Initiate MFRC522
  
  //Serial.println("Approximate your  card to the reader");
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(SWITCH, INPUT);
  
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);

}
void  loop(){
  sw = digitalRead(SWITCH);
  if(sw == HIGH){ //advancedMode setter
    //Serial.println("Manual mode engaged");
    advancedMode = true;
  } else {
    //Serial.println("manual mode unengaged");
    advancedMode = false;
  }
  
  // Look for new cards
  if( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  // Select one of the cards
  if( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID  tag :");
  String content= "";
  byte letter;
  for(byte i = 0; i <  mfrc522.uid.size; i++){
     Serial.print(mfrc522.uid.uidByte[i] < 0x10  ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i]  < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i],  HEX));
  }
  
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  if(advancedMode == false){ //checks if the manual mode is enabled
    //Main mode
    if(content.substring(1) == card0){
      Serial.println("WHITE");
      led(255, 255, 255);
    } else if(content.substring(1) == card1){
      Serial.println("RED");
      led(255, 0, 0);
    } else if(content.substring(1) == card2){
      Serial.println("GREEN");
      led(0, 255, 0);
    } else if(content.substring(1) == card3){
      Serial.println("BLUE");
      led(0, 0, 255);
    }
  } else {
    //Advanced Mode
    if(content.substring(1) == card0 || card1 || card2 || card3){
      if(selector == true && i<6){  //choses to either increment or decrease the value of 'i' (starts at value: -1 to then travel between 0 and 5)
        i++;
        if(i == 6) {
          selector = false;
        }
      } else {
        i--;
        if(i == 0){
          selector = true;
        }
      }
      Serial.println(i);
      led(vec[i][0], vec[i][1], vec[i][2]); //sets R, G and B values of the lamp from respectively the 0, 1 and 2 column of the vector
      delay(1500);
    }
  }
}

void led(int r, int g, int b){
  analogWrite(R, r);
  analogWrite(G, g);
  analogWrite(B, b);
}
