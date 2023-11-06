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

#define GREEN_1_SET_TIME 2000
#define GREEN_2_SET_TIME 2000
#define GREEN_3_SET_TIME 2000
#define GREEN_4_SET_TIME 2000
#define YELLOW_SET_TIME 1000

//switch
#define SW5 A0
#define SW6 A1
#define SW7 A2
#define SW8 A3
//
#define SW9 A4


int status_SW5 = 0;
int status_SW6 = 0;
int status_SW7 = 0;
int status_SW8 = 0;
int state_LED = 0;

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

  pinMode(SW5,INPUT_PULLUP);
  pinMode(SW6,INPUT_PULLUP);
  pinMode(SW7,INPUT_PULLUP);
  pinMode(SW8,INPUT_PULLUP);

  pinMode(SW9,INPUT_PULLUP);

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
        int TIME_GREEN_NEXT = GREEN_1_SET_TIME + GREEN_2_SET_TIME + GREEN_3_SET_TIME + GREEN_4_SET_TIME + (YELLOW_SET_TIME * 5);
        Serial.println(String(TIME_GREEN_NEXT));
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
    if(digitalRead(SW5) == LOW)
    {
      Traffic_GREEN_1();
    }
    else if(digitalRead(SW6) == LOW)
    {
      Traffic_GREEN_2();
    }
    else if(digitalRead(SW7) == LOW)
    {
      Traffic_GREEN_3();
    }
    else if(digitalRead(SW8) == LOW)
    {
      Traffic_GREEN_4();
    }
    
    if (digitalRead(SW5) == LOW || digitalRead(SW6) == LOW || digitalRead(SW7) == LOW || digitalRead(SW8) == LOW) {
      vTaskSuspend(TrafficControlTaskHandle);
      vTaskSuspend(LightControlTaskHandle);
      if(digitalRead(SW5) == LOW){
        status_SW5 = 1;
      }else if(digitalRead(SW6) == LOW){
        status_SW6 = 1;
      }else if(digitalRead(SW7) == LOW){
        status_SW7 = 1;
      }else if(digitalRead(SW8) == LOW){
        status_SW8 = 1;
      }
    } else {
      if(status_SW5 == 1){
        Traffic_YELLOW_1();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW5 = 0;
      }
      else if(status_SW6 == 1){
        Traffic_YELLOW_2();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW6 = 0;
      }
      else if(status_SW7 == 1){
        Traffic_YELLOW_3();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW7 = 0;
      }
      else if(status_SW8 == 1){
        Traffic_YELLOW_4();
        delay(1000);
        vTaskResume(TrafficControlTaskHandle);
        vTaskResume(LightControlTaskHandle);
        status_SW8 = 0;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//
void TaskSwitchLEDControl(void *pvParameters) {
  (void)pvParameters;
  while (1){
    if(digitalRead(SW9) == LOW ){
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
      vTaskResume(TrafficControlTaskHandle);
      vTaskResume(LightControlTaskHandle);
      vTaskResume(SwitchInterruptTaskHandle);
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
