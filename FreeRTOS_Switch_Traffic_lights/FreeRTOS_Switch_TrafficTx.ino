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

#define GREEN_1_SET_TIME 10000
#define GREEN_2_SET_TIME 10000
#define GREEN_3_SET_TIME 10000
#define GREEN_4_SET_TIME 10000
#define YELLOW_SET_TIME 2000

//switch
#define SW1 A0
#define SW2 A1
#define SW3 A2
#define SW4 A3
//
#define SW5 A4


int status_SW1 = 0;
int status_SW2 = 0;
int status_SW3 = 0;
int status_SW4 = 0;
int status_SW5 = 0;
int state_LED = 0;

int event = 0;
String receivedData;
int intReceivedData;

// Define task handles
TaskHandle_t TrafficControlTaskHandle;
TaskHandle_t LightControlTaskHandle;
TaskHandle_t SwitchInterruptTaskHandle;
TaskHandle_t SwitchLEDControlTaskHandle;

unsigned long Timer_1 = 0;
unsigned long Timer_2 = 0;

short Light_Index = 0;
unsigned long Set_Time[] = {GREEN_1_SET_TIME, YELLOW_SET_TIME, GREEN_2_SET_TIME, YELLOW_SET_TIME, 
                            GREEN_3_SET_TIME, YELLOW_SET_TIME, GREEN_4_SET_TIME, YELLOW_SET_TIME};

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

  pinMode(SW5,INPUT_PULLUP);

  Serial.println("Tx");

  //Create Task
  xTaskCreate(TrafficControlTask, "TrafficControl", 128, NULL, 1, &TrafficControlTaskHandle);
  xTaskCreate(LightControlTask, "LightControl", 128, NULL, 2, &LightControlTaskHandle);
  xTaskCreate(TaskSwitchInterrupt, "SwitchInterrupt", 128, NULL, 3, &SwitchInterruptTaskHandle);
  xTaskCreate(TaskSwitchLEDControl, "SwitchLEDControl", 128, NULL, 3, &SwitchLEDControlTaskHandle);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();
}

void loop() {

}

void TrafficControlTask(void *pvParameters) {
  while (1) {
    // Update traffic control logic here
    if ((millis() - Timer_1) >= Set_Time[Light_Index]) {
      Light_Index++;

      if (Light_Index > 7) {
        Light_Index = 0;
      }

      Timer_1 = millis();
    }
    //ใส่เพื่อให้ทำงานพร้อมกับ function LightControlTask
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
  }
}

void LightControlTask(void *pvParameters) {
  while (1) {
    switch (Light_Index) {
      case 0:
        Traffic_GREEN_1();
        break;
      case 1:
        Traffic_YELLOW_1();
        break;
      case 2:
        Traffic_GREEN_2();
        break;
      case 3:
        Traffic_YELLOW_2();
        break;
      case 4:
        Traffic_GREEN_3();
        break;
      case 5:
        Traffic_YELLOW_3();
        break;
      case 6:
        Traffic_GREEN_4();
        break;
      case 7:
        Traffic_YELLOW_4();
        if (Serial.available() > 0) {
          receivedData = Serial.readStringUntil('\n');
          intReceivedData = receivedData.toInt();
          
        }

        if(intReceivedData == 0 || event == 0){
            unsigned int TIME_GREEN_NEXT = GREEN_1_SET_TIME + GREEN_2_SET_TIME + GREEN_3_SET_TIME + GREEN_4_SET_TIME + (YELLOW_SET_TIME * 5);
            Serial.println(String(TIME_GREEN_NEXT));
            intReceivedData = 1;
            event = 1;
          }else{
          
          }
        
        break;
    }
    //ต้องใส่ vTaskDelay ใน function นี้ไม่งั้น task ไม่ทำงาน
    vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 100 milliseconds
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
      vTaskSuspend(TrafficControlTaskHandle);
      vTaskSuspend(LightControlTaskHandle);
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
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW1 = 0;
      }
      else if(status_SW2 == 1){
        Traffic_YELLOW_2();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW2 = 0;
      }
      else if(status_SW3 == 1){
        Traffic_YELLOW_3();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW3 = 0;
      }
      else if(status_SW4 == 1){
        Traffic_YELLOW_4();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW4 = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//
void TaskSwitchLEDControl(void *pvParameters) {
  (void)pvParameters;
  while (1){
    if(digitalRead(SW5) == LOW ){
      status_SW5 = 1;
      vTaskSuspend(TrafficControlTaskHandle);
      vTaskSuspend(LightControlTaskHandle);
      vTaskSuspend(SwitchInterruptTaskHandle);
    
      if((millis() - Timer_2) >= 1000) {
        Timer_2 = millis();

        if(state_LED == 0 ){
          Traffic_YELLOW_ALL();
        } else {
          Traffic_LOW();
        }
        state_LED = !state_LED;
      }
    } else {
      if(status_SW5 == 1){
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        vTaskResume(SwitchInterruptTaskHandle);
        status_SW5 = 0;
      }
      
    }
    vTaskDelay(pdMS_TO_TICKS(100));
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

void Traffic_YELLOW_ALL() {
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, HIGH);
  digitalWrite(LIGHT_1_RED, LOW);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, HIGH);
  digitalWrite(LIGHT_2_RED, LOW);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, HIGH);
  digitalWrite(LIGHT_3_RED, LOW);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, HIGH);
  digitalWrite(LIGHT_4_RED, LOW);
}

void Traffic_LOW(){
  digitalWrite(LIGHT_1_GREEN, LOW);
  digitalWrite(LIGHT_1_YELLOW, LOW);
  digitalWrite(LIGHT_1_RED, LOW);   

  digitalWrite(LIGHT_2_GREEN, LOW);
  digitalWrite(LIGHT_2_YELLOW, LOW);
  digitalWrite(LIGHT_2_RED, LOW);

  digitalWrite(LIGHT_3_GREEN, LOW);
  digitalWrite(LIGHT_3_YELLOW, LOW);
  digitalWrite(LIGHT_3_RED, LOW);

  digitalWrite(LIGHT_4_GREEN, LOW);
  digitalWrite(LIGHT_4_YELLOW, LOW);
  digitalWrite(LIGHT_4_RED, LOW);

}
