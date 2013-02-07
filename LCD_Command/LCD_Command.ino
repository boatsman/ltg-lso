#include <LiquidCrystal.h>

#define DEVICE_ADDRESS 10
#define CMD_PREFIX 255
#define MAX_COMMAND_LENGTH 4

/*
FF префикс
0A адрес
xx длина команды
..
..  команда (xx байт)
..

*/

#define CMD_STATE_WAIT 0
#define CMD_STATE_PREFIX 1
#define CMD_STATE_ADDRESS 2
#define CMD_STATE_BUFFERING 3

LiquidCrystal lcd(A3, A2, 13, 10, 11, 12);
long lastMillis;

int serialByte = 0;
int bytesToRead = 0;
int bytesInBuffer = 0;
int commandBuffer[MAX_COMMAND_LENGTH];
int commandState = CMD_STATE_WAIT;

void setup() {
  lcd.begin(20, 4);
  pinMode(2, OUTPUT);
  pinMode(9, OUTPUT);
  analogWrite(9, 255);
  Serial.begin(57600);
  lcd.print("Ready...");
  //lastMillis = millis();
}

void loop() {
  if (Serial.available()) {
    serialByte = Serial.read();
    
    if (serialByte == CMD_PREFIX)
          commandState = CMD_STATE_PREFIX;
          
    switch (commandState) {
      
      case CMD_STATE_WAIT:
        if (serialByte == CMD_PREFIX)
          commandState = CMD_STATE_PREFIX;
        break;
        
      case CMD_STATE_PREFIX:
        if (serialByte == DEVICE_ADDRESS)
          commandState = CMD_STATE_ADDRESS;
        break;
        
      case CMD_STATE_ADDRESS:
        if (serialByte > 0 && serialByte <= MAX_COMMAND_LENGTH) {
          bytesToRead = serialByte;
          commandState = CMD_STATE_BUFFERING;
          clearCommandBuffer();
        } else {
          commandState = CMD_STATE_WAIT;
        }
        break;
        
      case CMD_STATE_BUFFERING:
        commandBuffer[bytesInBuffer++] = serialByte;
        if (--bytesToRead <= 0) {
          commandState = CMD_STATE_WAIT;
          processCommand();
        }
        break;
        

    }
  }
  
  lcd.setCursor(10, 0);
  lcd.print(millis()/1000);
}

void clearCommandBuffer() {
  for (int i = 0; i < MAX_COMMAND_LENGTH; i++)
    commandBuffer[i] = 0;
  bytesInBuffer = 0;
  
  for (int i = 0; i < MAX_COMMAND_LENGTH; i++) {
    lcd.setCursor(0+i*2, 1);
    lcd.print("  ");
  }
}

void processCommand() {
  digitalWrite(2, HIGH);
  
  for (int i = 0; i < bytesInBuffer; i++) {
    lcd.setCursor(0+i*2, 1);
    lcd.print(commandBuffer[i], HEX);
    Serial.write(commandBuffer[i]);
  }
    
  digitalWrite(2, LOW);
}

