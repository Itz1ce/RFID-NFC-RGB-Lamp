//Code by Botan Denis of the class 3ATT-2022/2023 IIS Levi-Ponti(Levi)
#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
#define R 3
#define G 5
#define B 6
#define SWITCH 2
#define STOP 4

int vec[6][3] = {/*RED*/{255, 0, 0},/*ORANGE*/ {255, 64, 0},/*YELLOW*/ {255, 255, 0},/*GREEN*/ {0, 255, 0},/*BLU*/ {0, 0, 255}, /*Purple*/ {255, 0, 255}}, i = -1, sw, pastState = LOW;

unsigned long previusMillis = 0;
const long debounce = 200;

bool manualMode = false, selector = true;

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
 
void setup(){
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  Serial.println("Approximate your  card to the reader");
  
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(SWITCH, INPUT);
  pinMode(STOP, INPUT);
  
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);

}
void  loop(){
  sw = digitalRead(SWITCH); 
  unsigned long currentMillis = millis();
  if(currentMillis - previusMillis >= debounce){ //sets a time delay between the reception of inputs
    previusMillis = currentMillis;
   
    if(sw == HIGH && pastState == LOW){
      Serial.println("Manual mode engaged");
      manualMode = true;
      pastState = HIGH;
      i = -1;
    } else if(sw == HIGH && pastState == HIGH){
      Serial.println("manual mode unengaged");
      manualMode = false;
      pastState = LOW;
    }
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
  
  if(manualMode == false){ //checks if the manual mode is enabled
    if(content.substring(1) == "04 5A C1 EA 5E 70 81"){
      Serial.println("RED");
      led(255, 0, 0);
    } else if(content.substring(1) == "04 42 EF EA 5E 70 80"){
      Serial.println("GREEN");
      led(0, 255, 0);
    } else if(content.substring(1) == "04 1C 86 EA 5E 70 80"){
      Serial.println("BLUE");
      led(0, 0, 255);
    }
  } else {
    if(content.substring(1) == "04 5A C1 EA 5E 70 81" || "04 42 EF EA 5E 70 80" || "04 1C 86 EA 5E 70 80"){

      if(selector == true && i<5){  //choses to either increment or decrease the value of 'i' (starts at value: -1 to then travel between 0 and 5)
        i++;
        if(i == 5) {
          selector = false;
        }
      } else {
        i--;
        if(i == 0){
          selector = true;
        }
      }
      Serial.println(i);
      led(vec[i][0], vec[i][1], vec[i][2]);
      delay(1500);
    }
  }
}

void led(int r, int g, int b){
  analogWrite(R, r);
  analogWrite(G, g);
  analogWrite(B, b);
}
