/*
 * 0.3 대구 시범사업 6/17
 * 0.2 대구 시연 6/7
 * 0.1 교사연구회 시연 5/14
 */
// for codestar mobile

#include <SoftwareSerial.h>

// protocol
#define GET 1
#define RUN 2
#define RESET 4

#define COBLO_DEVICE 0xde
#define FUNCTION_BLOCK 0x7a
#define ADD     0
#define START   1
#define FUNCTION 2
#define STOP  3

const int pinButton = 12; // 푸시버튼 연결 핀 번호
const int pinRGB_Red = 9;    // RGB LED의 Red 연결 핀 번호
const int pinRGB_Green = 10; // RGB LED의 Green 연결 핀 번호
const int pinRGB_Blue = 11;  // RGB LED의 Blue 연결 핀 번호
const int pinWhite = 3;
const int pinMic = A2;  // 마이크 연결 핀 번호
// create bluetooth instnace
SoftwareSerial bt(1, 0);

// packet
double  currentTime = 0.0; // on request TIMER
double  lastTime = 0.0;    // on request TIMER
boolean isAvailable = false;  // serial
boolean isStart = false;  // fine package
String  mVersion = "1.0.0";
byte    dataLen = 0;      // package data length;
char    serialRead;       // real data
unsigned char prevc = 0;  // real data
byte    index = 0;        // buffer index
char    buffer[52];       // real buffer
// start
const int pinBuzzer = A3;  // 부저(스피커) 연결 핀 번호
int tones[] = { 261, 294, 330, 349, 392, 440, 494, 523 };

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int modeCount = 1; 
/*
ff 55 len x  GET  sensor  port  slot  data a
0  1  2   3   4      5      6     7     8
*/
void runModule(int device){
  // for 3 device
  int lpos = 0;
  int ledr = 0;
  int ledg = 0;
  int ledb = 0;
  switch(device){
    case 1: // move
      break;
    case 2: // stop
      break;
    case 3: // led
      lpos = readBuffer(6);  // led position(don't care)
      ledr = readBuffer(7);  // led red color
      ledg = readBuffer(8);  // led green color
      ledb = readBuffer(9);  // led blue color
      color(ledr, ledg, ledb);
      break;
    case 4: // led off
      ledoff();
      break;
    case 5: // healing mode
      break;
    case 6: // healing mode off
      ledoff();
    default:
    break;
  }
}

void readSerial(){
  isAvailable = false;
  if(bt.available() > 0){
    isAvailable = true;
    serialRead = bt.read();
  }
}

void writeBuffer(int idx, unsigned char c){
  buffer[idx] = c;
}
unsigned char readBuffer(int idx){
  return buffer[idx];
}
void writeHead(){
  writeSerial(0xff);
  writeSerial(0x55);
}
void writeEnd(){
  bt.println();
}
void writeSerial(unsigned char c){
  bt.write(c);
}
void callOK(){
  writeHead();
  writeEnd();
}

void color(int r, int g, int b){
  digitalWrite(pinRGB_Red, r*28);
  digitalWrite(pinRGB_Green, g*28);
  digitalWrite(pinRGB_Blue, b*28);

}
void ledoff(){
  digitalWrite(pinRGB_Red, LOW);   // 빨간색 켜기
  digitalWrite(pinRGB_Green, LOW); // 초록색 켜기
  digitalWrite(pinRGB_Blue, LOW);   // 파란색 끄기
}

/*
ff 55 len idx action device port  slot  data a
0  1  2   3   4      5      6     7     8
*/
void parseData(){
  int idx = readBuffer(3);
  int action = readBuffer(4);
  int device = readBuffer(5);

  switch(action){
    case GET:
     
    break;
    case RUN:
      runModule(device);
      //callOK(); // ack signal
    break;
    case RESET:
   
    break;
  }
}

void setup() {

  // 3 color led
  pinMode(pinRGB_Red, OUTPUT);   // RGB LED의 Red 핀을 출력용 핀으로 설정
  pinMode(pinRGB_Green, OUTPUT); // RGB LED의 Green 핀을 출력용 핀으로 설정
  pinMode(pinRGB_Blue, OUTPUT);  // RGB LED의 Blue 핀을 출력용 핀으로 설정
  pinMode(pinWhite, OUTPUT); 

  pinMode(pinButton, INPUT); // 푸시버튼 핀을 입력용 핀으로 설정
  pinMode(pinBuzzer, OUTPUT); // 부저(스피커) 핀을 출력용 핀으로 설정
  
  // put your setup code here, to run once:
  bt.begin( 9600 );  // 블루투스 통신 초기화 (속도= 9600 bps)
  //Serial.begin(9600);
  // Red : 빨간색 불빛 켜기
  digitalWrite(pinRGB_Red, HIGH);   // 빨간색 켜기
  digitalWrite(pinRGB_Green, LOW);  // 초록색 끄기
  digitalWrite(pinRGB_Blue, LOW);   // 파란색 끄기
  
  delay(500);

  // Yellow : 노란색(빨간색+초록색) 불빛 켜기
  digitalWrite(pinRGB_Red, HIGH);   // 빨간색 켜기
  digitalWrite(pinRGB_Green, HIGH); // 초록색 켜기
  digitalWrite(pinRGB_Blue, LOW);   // 파란색 끄기
  
  delay(500);

  // Green : 초록색 불빛 켜기
  digitalWrite(pinRGB_Red, LOW);    // 빨간색 끄기
  digitalWrite(pinRGB_Green, HIGH); // 초록색 켜기
  digitalWrite(pinRGB_Blue, LOW);   // 파란색 끄기
  delay(500); 
  
  noTone(pinBuzzer);  

  ledoff();

  pinMode(pinMic, INPUT); // 마이크 핀을 입력용 핀으로 설정  
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis()/1000.0-lastTime;
  readSerial();
  if(isAvailable){
    unsigned char c = serialRead & 0xff;
    if(c == 0x55 && isStart == false){
      if(prevc == 0xff){
        index = 1;
        isStart = true;
      }
    }else{  // isn't 0x55
      prevc = c;
      if(isStart){  // finded header code
        if(index == 2){ // is index length?
          dataLen = c;
        }else if(index > 2){
          dataLen--;
        }
        writeBuffer(index, c);    // real value
      }
    }

    index++;
    if(index > 51){ // checking max buffer
      index = 0;
      isStart = false;
    }

    if(isStart && dataLen == 0 && index > 3){
      isStart = false;    // reset
      parseData();
      index = 0;
    }
  }
  // read the state of the switch into a local variable:
  int reading = digitalRead(pinButton);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }else{

        modeCount = 0;
        // STOP
        writeHead(); 
        writeSerial(COBLO_DEVICE); 
        writeSerial(STOP); 
        writeSerial(0); 
        writeEnd();
        delay(200);
        for(int i = 1; i < 10; i++){
          writeHead();
          writeSerial(COBLO_DEVICE); 
          writeSerial(ADD);  
          writeSerial(i); 
          writeEnd();  
          delay(5);
        }

        writeHead();
        writeSerial(COBLO_DEVICE); 
        writeSerial(ADD);  
        writeSerial(FUNCTION_BLOCK);
        writeEnd();  
        delay(5); 

        for(int i = 1; i < 10; i++){

          writeHead(); 
          writeSerial(COBLO_DEVICE); 
          writeSerial(FUNCTION);  
          writeSerial(i); // 1
          writeEnd(); 
          delay(5);
        }

        for(int i = 1; i < 10; i++){
          writeHead();
          writeSerial(COBLO_DEVICE); 
          writeSerial(ADD);  
          writeSerial(i+9); 
          writeEnd();  
          delay(5);
        }

        delay(200);
        writeHead(); 
        writeSerial(COBLO_DEVICE); 
        writeSerial(START);  
        writeSerial(0); 
        writeEnd();
        delay(5);
      }
    }
  }
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;  
}
