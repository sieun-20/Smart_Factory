// 라이브러리
// currentPosition() //현재 위치 값
// move(distance)  //이동할 거리 지정
// stepper.setCurrentPosition(position); //현재 스탭을 설정
// stepper.currentPosition(); //현재 스탭을 반환
// stepper.setAcceleration(2000); //가속량을 설정함
// stepper.moveTo(1000); //목표스탭량을 설정함
// stepper.runToPosition(); //가속도 조절 스탭모터제어

#include <AccelStepper.h> //스텝모터
#include <Wire.h> // I2C통신
#include <SoftwareSerial.h> // UART통신----------------------------------------------!!

#define moterInterfaceType 1 // 스텝모터의 인터페이스 타입을 나타내는 상수
#define SLAVE 2 // 슬레이브
#define sensor_cam 3 // 적외선센서
#define sensor_lobo 4 // 적외선센서
#define enablePin 5 // 스텝모터(전원해제핀)
#define dirxPin 6 // 스텝모터
#define stepxPin 7 // 스텝모터
#define RX_PIN 8 // Rx
#define TX_PIN 9 // Tx
#define swich_R 10 // 마이크로스위치A
#define swich_G 11 // 마이크로스위치B
#define swich_B 12 // 마이크로스위치C
#define swich_Y 13 // 마이크로스위치D


AccelStepper stepperx = AccelStepper(moterInterfaceType, stepxPin, dirxPin);
// AccelStepper stepperx = AcclStepper(연결방식, step핀, dir핀)

int state1; // 센서 상태 저장 변수
int state2; // 센서 상태 저장 변수
int isStop = 0; // 스텝모터 중지 결정 변수

int count_R = 0; // 마이크로스위치
int count_G = 0; // 마이크로스위치
int count_B = 0; // 마이크로스위치
int count_Y = 0; // 마이크로스위치

int cam;
int lobo;

//byte count = 0;

int isAction = 0;
char action_value;


SoftwareSerial mySerial(8, 9); // RX, TX UART통신----------------------------------------------!!
String inputString = "";
boolean stringComplete = false;



void setup() {   
  Serial.begin(9600);
  Serial.println("NSF START");
  pinMode(enablePin, OUTPUT); // 모터는 OUTPUT
  digitalWrite(enablePin, LOW); 
  stepperx.setMaxSpeed(1000); // 최대속도
  stepperx.setSpeed(900);


  pinMode(sensor_cam, INPUT); // 센서는 INPUT
  pinMode(sensor_lobo, INPUT);
  pinMode(swich_R, INPUT);
  pinMode(swich_G, INPUT);
  pinMode(swich_B, INPUT);
  pinMode(swich_Y, INPUT);

  Wire.begin(); // 마스터 


  mySerial.begin(9600);
  // while(mySerial.available() <= 0) {
  // sendStatus();
  // delay(300);
  // }


}

void loop() {

  state1 = digitalRead(sensor_cam); // 적외선 센서를 읽어와 저장  
  state2 = digitalRead(sensor_lobo);

  if (!isStop) // 센서가 멈추지 않을 경우
  {
    if (state1 == LOW) { //센서가 감지되면 
      isStop = 1; // 동작을 멈춤
      mySerial.println("scam"); // println 개행 꼭

      // Wire.beginTransmission(SLAVE); //s에게 전송
      // Wire.write("scam");
      // Wire.endTransmission();
    }
    else if (state2 == LOW){
      isStop = 1; // 동작을 멈춤
      mySerial.println("srobot");

      // Wire.beginTransmission(SLAVE); //s에게 전송
      // Wire.write("srobot");
      // Wire.endTransmission();
    }
    else { // 센서가 감지되지 않으면
      stepperx.setSpeed(900); // 설정된 속도와 방향
      stepperx.runSpeed(); // 스텝 모터를 움직임
    }  
  } 


  if (stringComplete){
    // if (inputString.startsWith("status")) {
    //   sendStatus();
    // }
    // stringComplete = false;
    // inputString = "";

  // }
    if (inputString.startsWith("sensor")) {
      if (inputString.indexOf("cam") > -1) {
        mySerial.println("sensor1_cam");
      }
      else if (inputString.indexOf("robot") > -1) {
        mySerial.println("sensor2_robot");
      }
      else {
        mySerial.println("invalid set command"); // 잘못된 명령
      }
  
  delay(100);






  //const char arduinoCommands[] = {'r', '1', 'b', '2', 'g', '3', 'y', '4', 'e', 'c', 'i', 'q'}; // 배열test


  if (mySerial.available() > 0)  // 0 이상의 my시리얼통신이 들어올 경우 UART통신----------------------------------------------!!
  {
    serialEvent();

    // char ch = mySerial.read(); 

    // // 13=\r 동일한 줄의 첫번째 자리에 커서를 위치 시킴, 10=\n 현재에서 아래로 한줄 내리는 기능
    // if (!(ch == 13 || ch == 10)) { //13 또는 10이 아닐 경우
    //   Serial.print("rpi-ardu send : ");
    //   Serial.println(int(ch));
    //   action_value = ch; // 숫자 시리얼 값을 action_value 저장
    // }
    // else {
    //   return;
    // }

    if (inputString == 's'){ // 컨베이어 다시 작동 s
      isStop = 0; // 동작을 시작

      int i = 0;
      for(i = 0; i < 10000; i++) 
      {
        stepperx.setSpeed(900);
        stepperx.runSpeed();
      }
    }



    Wire.beginTransmission(SLAVE); // SLAVE에게 전송
    // servo r g b y
    // lobot e c i q

    if (inputString.startsWith("servo")) {

      if (inputString.indexOf("red open") > -1) {
        Wire.write('r');
      }
      else if (inputString.indexOf("red close") > -1) {
        Wire.write('1');
      }
      else if (inputString.indexOf("green open") > -1) {
        Wire.write('g');
      }
      else if (inputString.indexOf("green close") > -1) {
        Wire.write('2');
      }
      else if (inputString.indexOf("blue open") > -1) {
        Wire.write('b');
      }
      else if (inputString.indexOf("blue close") > -1) {
        Wire.write('3');
      }
      else if (inputString.indexOf("yellow open") > -1) {
        Wire.write('y');
      }
      else if (inputString.indexOf("yellow close") > -1) {
        Wire.write('4');
      }


    else if (inputString.startsWith("robot")) {
      if (inputString.indexOf("edge error") > -1) {
        Wire.write('e');
      }
      else if (inputString.indexOf("image error") > -1) {
        Wire.write('i');
      }
      else if (inputString.indexOf("color error") > -1) {
        Wire.write('c');
      }
      else if (inputString.indexOf("qr error") > -1) {
        Wire.write('q');
      }      
    }




    // for (int i = 0; i < sizeof(arduinoCommands); i++) { // 배열test
    //   if (ch == arduinoCommands[i]) {
    //     Wire.write(ch); 
    //     break; 
    //   }
    // }


    Wire.endTransmission();
    isAction = 1;

  }


  if (isAction) {
    Wire.requestFrom(SLAVE,1); // 1바이트씩 받기
    char se=Wire.read();

    Serial.print("ing : ");
    Serial.println(se);
    Serial.print("action_value : ");
    Serial.println(action_value);

    if (se == action_value){
      Serial.print("receive : ");
      Serial.println(se);
      isAction = 0;
    }
    delay(100);
  }

  if (digitalRead(swich_R) == HIGH) {
    count_R++;
    Serial.print("swich_R :");
    Serial.println(count_R);
    delay(1000);
    return;
  }  
  if (digitalRead(swich_G) == HIGH) {
    count_G++;
    Serial.print("swich_G :");
    Serial.println(count_G);
    delay(1000);
    return;
  }
  if (digitalRead(swich_B) == HIGH) {
    count_B++;
    Serial.print("swich_B :");
    Serial.println(count_B);
    delay(1000);
    return;
  }
  if (digitalRead(swich_Y) == HIGH) {
    count_Y++;
    Serial.print("swich_Y :");
    Serial.println(count_Y);
    delay(1000);
    return;
  }

}


void serialEvent() {
  while (mySerial.available()) {
    char inChar = (char)mySerial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}




