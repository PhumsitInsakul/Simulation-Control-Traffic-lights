#include <Arduino_FreeRTOS.h>

#define LIGHT_1_RED 2
#define LIGHT_1_YELLOW 3
#define LIGHT_1_GREEN 4

#define LIGHT_2_RED 5
#define LIGHT_2_YELLOW 6
#define LIGHT_2_GREEN 7

#define LIGHT_3_RED 8
#define LIGHT_3_YELLOW 9
#define LIGHT_3_GREEN 10

#define LIGHT_4_RED 11
#define LIGHT_4_YELLOW 12
#define LIGHT_4_GREEN 13

#define GREEN_LIGHT_1 0
#define YELLOW_LIGHT_1 1

#define GREEN_LIGHT_2 2
#define YELLOW_LIGHT_2 3

#define GREEN_LIGHT_3 4
#define YELLOW_LIGHT_3 5

#define GREEN_LIGHT_4 6
#define YELLOW_LIGHT_4 7

TaskHandle_t xTaskHandleTrafficController;
TaskHandle_t xTaskHandleSerialCommunication;
TaskHandle_t xTaskHandleSwitchInterrupt;

//switch
#define SW1 A0
#define SW2 A1
#define SW3 A2
#define SW4 A3
int status_SW1 = 0;
int status_SW2 = 0;
int status_SW3 = 0;
int status_SW4 = 0;

int event = 0;


unsigned long GREEN_1_SET_TIME = 1000;
unsigned long GREEN_2_SET_TIME = 1000;
unsigned long GREEN_3_SET_TIME = 1000;
unsigned long GREEN_4_SET_TIME = 1000;
unsigned long YELLOW_SET_TIME = 2000;

unsigned long Timer_1 = 0;

unsigned int Set_Time[8] = {GREEN_1_SET_TIME, YELLOW_SET_TIME,
                              GREEN_2_SET_TIME, YELLOW_SET_TIME, 
                              GREEN_3_SET_TIME, YELLOW_SET_TIME,
                              GREEN_4_SET_TIME, YELLOW_SET_TIME};

short Light_Index = GREEN_LIGHT_1;

String receivedData;
unsigned int uintReceivedData;

//function
void Traffic_GREEN_1();
void Traffic_GREEN_2();
void Traffic_GREEN_3();
void Traffic_GREEN_4();
void Traffic_YELLOW_1();
void Traffic_YELLOW_2();
void Traffic_YELLOW_3();
void Traffic_YELLOW_4();
int Calculate_Time(int time, short index);

void setup() {
  Serial.begin(9600);

  pinMode(LIGHT_1_GREEN, OUTPUT);
  pinMode(LIGHT_1_YELLOW, OUTPUT);
  pinMode(LIGHT_1_RED, OUTPUT);

  pinMode(LIGHT_2_GREEN, OUTPUT);
  pinMode(LIGHT_2_YELLOW, OUTPUT);
  pinMode(LIGHT_2_RED, OUTPUT);

  pinMode(LIGHT_3_GREEN, OUTPUT);
  pinMode(LIGHT_3_YELLOW, OUTPUT);
  pinMode(LIGHT_3_RED, OUTPUT);

  pinMode(LIGHT_4_GREEN, OUTPUT);
  pinMode(LIGHT_4_YELLOW, OUTPUT);
  pinMode(LIGHT_4_RED, OUTPUT);

  pinMode(SW1,INPUT_PULLUP);
  pinMode(SW2,INPUT_PULLUP);
  pinMode(SW3,INPUT_PULLUP);
  pinMode(SW4,INPUT_PULLUP);

  Serial.println("Rx");

  //Create Task
  xTaskCreate(TaskTrafficController, "TrafficController", 128, NULL, 2, &xTaskHandleTrafficController);
  xTaskCreate(TaskSerialCommunication, "SerialCommunication", 128, NULL, 1, &xTaskHandleSerialCommunication);
  xTaskCreate(TaskSwitchInterrupt, "SwitchInterrupt", 128, NULL, 3, &xTaskHandleSwitchInterrupt);
}

void loop() {
  // Empty loop
}

void TaskTrafficController(void *pvParameters) {
  (void)pvParameters;

  Traffic_GREEN_1();

  while (1) {
    if ((millis() - Timer_1) >= Set_Time[Light_Index]) {
      
      Light_Index++;

      if (Light_Index > 7) {
        Light_Index = GREEN_LIGHT_1;
      }
      
      Timer_1 = millis();

      switch (Light_Index) {
        case GREEN_LIGHT_1:
          Traffic_GREEN_1();
          event = 0;
          Serial.println(String(event));
          break;
        case YELLOW_LIGHT_1:
          Traffic_YELLOW_1();
          break;
        case GREEN_LIGHT_2:
          Traffic_GREEN_2();
          break;
        case YELLOW_LIGHT_2:
          Traffic_YELLOW_2();
          break;
        case GREEN_LIGHT_3:
          Traffic_GREEN_3();
          break;
        case YELLOW_LIGHT_3:
          Traffic_YELLOW_3();
          break;
        case GREEN_LIGHT_4:
          Traffic_GREEN_4();
          break;
        case YELLOW_LIGHT_4:
          Traffic_YELLOW_4();
          break;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
  }
}

void TaskSerialCommunication(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    if (Serial.available() > 0) {
      receivedData = Serial.readStringUntil('\n');
      uintReceivedData = receivedData.toInt();
      unsigned int Time_Divider = Calculate_Time(uintReceivedData, Light_Index);
       
      Set_Time[0] = Set_Time[2] = Set_Time[4] = Set_Time[6] = Time_Divider;
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100ms
  }
}

void TaskSwitchInterrupt(void *pvParameters) {
  (void)pvParameters;

  while(1) {

    //switch
    if(digitalRead(SW1) == LOW)
    {
      Traffic_GREEN_1();
    }
    else if(digitalRead(SW2) == LOW)
    {
      Traffic_GREEN_2();
    }
    else if(digitalRead(SW3) == LOW)
    {
      Traffic_GREEN_3();
    }
    else if(digitalRead(SW4) == LOW)
    {
      Traffic_GREEN_4();
    }
    
    if (digitalRead(SW1) == LOW || digitalRead(SW2) == LOW || digitalRead(SW3) == LOW || digitalRead(SW4) == LOW) {
      vTaskSuspend(xTaskHandleTrafficController);
      vTaskSuspend(xTaskHandleSerialCommunication);
      if(digitalRead(SW1) == LOW){
        status_SW1 = 1;
      }else if(digitalRead(SW2) == LOW){
        status_SW2 = 1;
      }else if(digitalRead(SW3) == LOW){
        status_SW3 = 1;
      }else if(digitalRead(SW4) == LOW){
        status_SW4 = 1;
      }
    } else {
      if(status_SW1 == 1){
        Traffic_YELLOW_1();
        delay(1000);
        vTaskResume(xTaskHandleTrafficController);
        vTaskResume(xTaskHandleSerialCommunication);
        status_SW1 = 0;
      }
      else if(status_SW2 == 1){
        Traffic_YELLOW_2();
        delay(1000);
        vTaskResume(xTaskHandleTrafficController);
        vTaskResume(xTaskHandleSerialCommunication);
        status_SW2 = 0;
      }
      else if(status_SW3 == 1){
        Traffic_YELLOW_3();
        delay(1000);
        vTaskResume(xTaskHandleTrafficController);
        vTaskResume(xTaskHandleSerialCommunication);
        status_SW3 = 0;
      }
      else if(status_SW4 == 1){
        Traffic_YELLOW_4();
        delay(1000);
        vTaskResume(xTaskHandleTrafficController);
        vTaskResume(xTaskHandleSerialCommunication);
        status_SW4 = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

int Calculate_Time(unsigned int time, short index){
  Serial.println(index);
  int max = 20000;
  int min = 1000;
  int round = 0;
  int time_distance = 10000;
  int yellow_time = 0;

  switch(index){
    case 0:
      round = 4;
      break;
    case 1:
      yellow_time+=2000;
      round = 3;
      break;
    case 2:
      round = 3;
      break;
    case 3:
      yellow_time+=2000;
      round = 2;
      break;
    case 4:
      round = 2;
      break;
    case 5:
      yellow_time+=2000;
      round = 1;
      break;
    case 6:
      round = 5;
      break;
    case 7:
      yellow_time+=2000;
      round = 4;
      break;
  }

  yellow_time = yellow_time + (round * 2000);

  while(true){
    unsigned int new_time = ((time - yellow_time)/round) + (time_distance/round);
    return new_time;
  }
}

void Traffic_GREEN_1(){
  digitalWrite(LIGHT_1_GREEN, HIGH);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, LOW);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_GREEN_2(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, HIGH);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, LOW);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_GREEN_3(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, HIGH);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, LOW);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_GREEN_4(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, HIGH);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, LOW);
}

void Traffic_YELLOW_1(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, HIGH);
  digitalWrite(LIGHT_1_RED, LOW);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_YELLOW_2(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, HIGH);
  digitalWrite(LIGHT_2_RED, LOW);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_YELLOW_3(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, HIGH);
  digitalWrite(LIGHT_3_RED, LOW);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, HIGH);
}

void Traffic_YELLOW_4(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, HIGH);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, HIGH);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, HIGH);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, HIGH);
  digitalWrite(LIGHT_4_RED, LOW);
}
