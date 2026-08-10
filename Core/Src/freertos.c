/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Summer.h"
#include "can_app.h"
#include "self_test.h"
#include "bsp_can.h"
#include "bsp_led.h"
#include "can_protocol.h"
#ifdef GIMBAL
#include "joystick_app.h"
#include "servo_app.h"
#include "bsp_joystick.h"
#include "bsp_servo.h"
#endif
#ifdef CHASSIS
#include "OLED_app.h"
#include "motor_control.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for CAN_SendTask */
osThreadId_t CAN_SendTaskHandle;
const osThreadAttr_t CAN_SendTask_attributes = {
  .name = "CAN_SendTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for CAN_RecvTask */
osThreadId_t CAN_RecvTaskHandle;
const osThreadAttr_t CAN_RecvTask_attributes = {
  .name = "CAN_RecvTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for JoystickTask */
osThreadId_t JoystickTaskHandle;
const osThreadAttr_t JoystickTask_attributes = {
  .name = "JoystickTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ServoTask */
osThreadId_t ServoTaskHandle;
const osThreadAttr_t ServoTask_attributes = {
  .name = "ServoTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
  .name = "MotorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for OLEDTask */
osThreadId_t OLEDTaskHandle;
const osThreadAttr_t OLEDTask_attributes = {
  .name = "OLEDTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SelfTestTask */
osThreadId_t SelfTestTaskHandle;
const osThreadAttr_t SelfTestTask_attributes = {
  .name = "SelfTestTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void CAN_RxCallback(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data);
/* USER CODE END FunctionPrototypes */

void StartCAN_SendTask(void *argument);
void StartCAN_RecvTask(void *argument);
void StartLEDTask(void *argument);
void StartJoystickTask(void *argument);
void StartServoTask(void *argument);
void StartMotorTask(void *argument);
void StartOLEDTask(void *argument);
void StartSelfTestTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  HAL_CAN_Start(&hcan);
  BSP_CAN_ConfigFilter(&hcan, 0, 0, CAN_RX_FIFO0);
  BSP_CAN_InstallRxCallback(&hcan, CAN_RxCallback);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  CAN_SendTaskHandle = osThreadNew(StartCAN_SendTask, NULL, &CAN_SendTask_attributes);
  CAN_RecvTaskHandle = osThreadNew(StartCAN_RecvTask, NULL, &CAN_RecvTask_attributes);
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);
  SelfTestTaskHandle = osThreadNew(StartSelfTestTask, NULL, &SelfTestTask_attributes);

#ifdef GIMBAL
  JoystickTaskHandle = osThreadNew(StartJoystickTask, NULL, &JoystickTask_attributes);
  ServoTaskHandle = osThreadNew(StartServoTask, NULL, &ServoTask_attributes);
#endif

#ifdef CHASSIS
  MotorTaskHandle = osThreadNew(StartMotorTask, NULL, &MotorTask_attributes);
  OLEDTaskHandle = osThreadNew(StartOLEDTask, NULL, &OLEDTask_attributes);
#endif

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartCAN_SendTask */
/**
  * @brief  Function implementing the CAN_SendTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartCAN_SendTask */
void StartCAN_SendTask(void *argument)
{
  /* USER CODE BEGIN StartCAN_SendTask */
  uint8_t tx[8];
  for(;;)
  {
#ifdef GIMBAL
    GimbalCtrlMsg_t *gc = &g_can_state.gimbal_ctrl;
    gc->gimbal_heartbeat++;
    Protocol_EncodeGimbalCtrl(gc, tx);
    BSP_CAN_Send(&hcan, CAN_GIMBAL_TO_CHASSIS_ID, tx, 8);
#endif
#ifdef CHASSIS
    ChassisFeedbackMsg_t *cf = &g_can_state.chassis_feedback;
    cf->chassis_heartbeat++;
    Protocol_EncodeChassisFeedback(cf, tx);
    if (BSP_CAN_Send(&hcan, CAN_CHASSIS_TO_GIMBAL_ID, tx, 8) == HAL_OK)
      CAN_App_SendDone();
#endif
    osDelay(20);
  }
  /* USER CODE END StartCAN_SendTask */
}

/* USER CODE BEGIN Header_StartCAN_RecvTask */
/**
* @brief Function implementing the CAN_RecvTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN_RecvTask */
void StartCAN_RecvTask(void *argument)
{
  /* USER CODE BEGIN StartCAN_RecvTask */
  for(;;)
  {
#ifdef GIMBAL
    uint8_t hb = g_can_state.chassis_feedback.chassis_heartbeat;
    osDelay(200);
    if (g_can_state.chassis_feedback.chassis_heartbeat == hb)
      g_can_state.chassis_feedback.motor_online = 0;
#endif
#ifdef CHASSIS
    uint8_t hb = g_can_state.gimbal_ctrl.gimbal_heartbeat;
    osDelay(200);
    if (g_can_state.gimbal_ctrl.gimbal_heartbeat == hb)
      g_can_state.gimbal_ctrl.servo_online = 0;
#endif
  }
  /* USER CODE END StartCAN_RecvTask */
}

/* USER CODE BEGIN Header_StartLEDTask */
/**
* @brief Function implementing the LEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLEDTask */
void StartLEDTask(void *argument)
{
  /* USER CODE BEGIN StartLEDTask */
  LED_SelfTest(3);
  for(;;)
  {
    uint8_t blink = SelfTest_ConsumeBlink();
    if (blink > 0) {
      LED_SelfTest(blink);
    }
    if (SelfTest_IsActive()) {
      LED_On();
    }
#ifdef GIMBAL
    else if (g_can_state.chassis_feedback.motor_online) {
      LED_On();
    }
#endif
#ifdef CHASSIS
    else if (motor_error == 0) {
      LED_On();
    }
#endif
    else {
      LED_Breathe();
    }
    osDelay(200);
  }
  /* USER CODE END StartLEDTask */
}

/* USER CODE BEGIN Header_StartJoystickTask */
/**
* @brief Function implementing the JoystickTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartJoystickTask */
#ifdef GIMBAL
void StartJoystickTask(void *argument)
{
  /* USER CODE BEGIN StartJoystickTask */
  for(;;)
  {
    Joystick_App_Update();
    osDelay(10);
  }
  /* USER CODE END StartJoystickTask */
}
#endif

/* USER CODE BEGIN Header_StartServoTask */
/**
* @brief Function implementing the ServoTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartServoTask */
#ifdef GIMBAL
void StartServoTask(void *argument)
{
  /* USER CODE BEGIN StartServoTask */
  for(;;)
  {
    Servo_App_Update();
    osDelay(20);
  }
  /* USER CODE END StartServoTask */
}
#endif

/* USER CODE BEGIN Header_StartMotorTask */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMotorTask */
#ifdef CHASSIS
void StartMotorTask(void *argument)
{
  /* USER CODE BEGIN StartMotorTask */
  for(;;)
  {
    MotorControl_Update();
    CAN_App_UpdateComm();
    osDelay(10);
  }
  /* USER CODE END StartMotorTask */
}
#endif

/* USER CODE BEGIN Header_StartOLEDTask */
/**
* @brief Function implementing the OLEDTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartOLEDTask */
#ifdef CHASSIS
void StartOLEDTask(void *argument)
{
  /* USER CODE BEGIN StartOLEDTask */
  for(;;)
  {
    OLED_app_Update();
    osDelay(100);
  }
  /* USER CODE END StartOLEDTask */
}
#endif

void StartSelfTestTask(void *argument)
{
  /* USER CODE BEGIN StartSelfTestTask */
  for(;;)
  {
    SelfTest_Update();
    osDelay(10);
  }
  /* USER CODE END StartSelfTestTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void CAN_RxCallback(CAN_HandleTypeDef *hcan, uint32_t id, uint8_t *data)
{
#ifdef GIMBAL
  if (id == CAN_CHASSIS_TO_GIMBAL_ID) {
    Protocol_DecodeChassisFeedback(data, &g_can_state.chassis_feedback);
    g_can_state.chassis_feedback.motor_online = 1;
  }
#endif
#ifdef CHASSIS
  if (id == CAN_GIMBAL_TO_CHASSIS_ID) {
    Protocol_DecodeGimbalCtrl(data, &g_can_state.gimbal_ctrl);
    g_can_state.gimbal_ctrl.servo_online = 1;
    CAN_App_RecvPeer();
  }
#endif
}
/* USER CODE END Application */
