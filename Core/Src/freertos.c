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
#include "bsp_can.h"
#include "bsp_led.h"
#include "can_protocol.h"
#include <math.h>
#include <stdio.h>
#ifdef GIMBAL
#include "servo_app.h"
#include "joystick_app.h"
#include "bsp_joystick.h"
#include "bsp_servo.h"
#endif
#ifdef CHASSIS
#include "OLED_app.h"
#include "motor_control.h"
#include "bsp_oled.h"
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
static uint8_t g_self_test_active = 0U;
/* USER CODE END Variables */
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
/* Definitions for CAN_HB */
osThreadId_t CAN_HBHandle;
const osThreadAttr_t CAN_HB_attributes = {
  .name = "CAN_HB",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef GIMBAL
static void SelfTest_Execute(void);
#endif
#ifdef CHASSIS
static float encoder_to_rpm(int32_t delta, float dt_sec);
#endif
/* USER CODE END FunctionPrototypes */

void StartCAN_SendTask(void *argument);
void StartCAN_RecvTask(void *argument);
void StartCAN_HBTask(void *argument);
void StartLEDTask(void *argument);
void StartJoystickTask(void *argument);
void StartServoTask(void *argument);
void StartMotorTask(void *argument);
void StartOLEDTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

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
  CAN_HBHandle = osThreadNew(StartCAN_HBTask, NULL, &CAN_HB_attributes);
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);

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
  (void)argument;
  uint8_t tx_data[8];
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;)
  {
    uint32_t period = g_can_state.can_comm_ok
                    ? CAN_SEND_PERIOD_MS : CAN_SLOW_SEND_PERIOD_MS;
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(period));
#ifdef GIMBAL
    g_can_state.gimbal_ctrl.gimbal_heartbeat = g_can_state.gimbal_heartbeat;
    GimbalCtrlMsg_t ctrl = g_can_state.gimbal_ctrl;
    Protocol_EncodeGimbalCtrl(&ctrl, tx_data);
    HAL_StatusTypeDef status = BSP_CAN_SendMessage(CAN_GIMBAL_TO_CHASSIS_ID, tx_data, CAN_TX_TIMEOUT);
    g_can_state.gimbal_heartbeat = (g_can_state.gimbal_heartbeat + 1U) & CAN_HEARTBEAT_MASK;
#endif
#ifdef CHASSIS
    g_can_state.chassis_feedback.chassis_heartbeat = g_can_state.chassis_heartbeat;
    ChassisFeedbackMsg_t fb = g_can_state.chassis_feedback;
    Protocol_EncodeChassisFeedback(&fb, tx_data);
    HAL_StatusTypeDef status = BSP_CAN_SendMessage(CAN_CHASSIS_TO_GIMBAL_ID, tx_data, CAN_TX_TIMEOUT);
    g_can_state.chassis_heartbeat = (g_can_state.chassis_heartbeat + 1U) & CAN_HEARTBEAT_MASK;
#endif
    if (status == HAL_OK) g_can_state.can_tx_cnt++;
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
  (void)argument;
  BSP_CAN_RxMsg_t rx_msg;
  for(;;)
  {
    if (BSP_CAN_GetRxMessage(&rx_msg) == 0U) {
      vTaskDelay(pdMS_TO_TICKS(1));
      continue;
    }
#ifdef GIMBAL
    if (rx_msg.header.StdId == CAN_CHASSIS_TO_GIMBAL_ID) {
      Protocol_DecodeChassisFeedback(rx_msg.data, &g_can_state.chassis_feedback_rx);
      g_can_state.chassis_fb_updated = 1U;
      g_can_state.last_chassis_rx_time = HAL_GetTick();
      g_can_state.can_rx_cnt++;
      if (g_can_state.chassis_feedback_rx.chassis_heartbeat != g_can_state.last_chassis_hb) {
        g_can_state.last_chassis_hb = g_can_state.chassis_feedback_rx.chassis_heartbeat;
        g_can_state.chassis_online = 1U;
      }
    }
#endif
#ifdef CHASSIS
    if (rx_msg.header.StdId == CAN_GIMBAL_TO_CHASSIS_ID) {
      Protocol_DecodeGimbalCtrl(rx_msg.data, &g_can_state.gimbal_ctrl_rx);
      g_can_state.gimbal_ctrl_updated = 1U;
      g_can_state.last_gimbal_rx_time = HAL_GetTick();
      g_can_state.can_rx_cnt++;
      if (g_can_state.gimbal_ctrl_rx.gimbal_heartbeat != g_can_state.last_gimbal_hb) {
        g_can_state.last_gimbal_hb = g_can_state.gimbal_ctrl_rx.gimbal_heartbeat;
        g_can_state.gimbal_online  = 1U;
      }
    }
#endif
    osDelay(1);
  }
  /* USER CODE END StartCAN_RecvTask */
}

/* USER CODE BEGIN Header_StartCAN_HBTask */
/**
* @brief Function implementing the CAN_HB thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCAN_HBTask */
void StartCAN_HBTask(void *argument)
{
  /* USER CODE BEGIN StartCAN_HBTask */
  (void)argument;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  vTaskDelay(pdMS_TO_TICKS(500));
  /* Infinite loop */
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
    uint32_t now = HAL_GetTick();
    uint8_t prev_comm_ok = g_can_state.can_comm_ok;
    uint8_t comm_ok = 1U;
#ifdef GIMBAL
    if ((now - g_can_state.last_chassis_rx_time) > CAN_HEARTBEAT_TIMEOUT) {
      g_can_state.chassis_online = 0U;
      comm_ok = 0U;
    }
    g_can_state.can_comm_ok = comm_ok;
    if (prev_comm_ok != comm_ok) {
      if (comm_ok)
        g_can_state.gimbal_ctrl.status_flags &= ~STATUS_CAN_ERROR;
      else
        g_can_state.gimbal_ctrl.status_flags |= STATUS_CAN_ERROR;
    }
#endif
#ifdef CHASSIS
    if ((now - g_can_state.last_gimbal_rx_time) > CAN_HEARTBEAT_TIMEOUT) {
      g_can_state.gimbal_online = 0U;
      comm_ok = 0U;
    }
    g_can_state.can_comm_ok = comm_ok;
    if (prev_comm_ok != comm_ok) {
      if (comm_ok)
        g_can_state.chassis_feedback.status_flags &= ~STATUS_CAN_ERROR;
      else
        g_can_state.chassis_feedback.status_flags |= STATUS_CAN_ERROR;
    }
#endif
    osDelay(1);
  }
  /* USER CODE END StartCAN_HBTask */
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
  (void)argument;
  LED_StartBlink();
  uint8_t should_breath = 0U;
  for(;;)
  {
    if (g_self_test_active) {
      LED_On();
      vTaskDelay(pdMS_TO_TICKS(50));
      continue;
    }
    uint8_t steady_on = 0U;
#ifdef GIMBAL
    if (g_can_state.chassis_online && g_can_state.chassis_feedback_rx.motor_online) {
      steady_on = 1U;
    }
    should_breath = g_can_state.can_comm_ok ? 0U : 1U;
#endif
#ifdef CHASSIS
    if (MotorControl_IsError() == 0) {
      steady_on = 1U;
    }
    should_breath = g_motor.motor_error ? 1U : 0U;
#endif
    if (steady_on) {
      LED_On();
      vTaskDelay(pdMS_TO_TICKS(200));
    } else {
      LED_Breath(should_breath);
    }
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
  (void)argument;
  BSP_Joystick_Calibrate();
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(JOYSTICK_SAMPLE_PERIOD_MS));
    if (!g_self_test_active && Joystick_APP_IsSWPressed()) {
      g_self_test_active = 1U;
      SelfTest_Execute();
      g_self_test_active = 0U;
      continue;
    }
    if (g_self_test_active) {
      continue;
    }
    int16_t servo_rpm = 0;
    int16_t wheel_rpm = 0;
    Joystick_APP_Update(&servo_rpm, &wheel_rpm);
    CAN_App_SetGimbalCtrl(servo_rpm, wheel_rpm);
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
  (void)argument;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SERVO_CTRL_PERIOD_MS));
    if (g_self_test_active) {
      osDelay(20);
      continue;
    }
    int8_t speed = g_servo.target_speed;
    BSP_Servo_SetSpeed(speed);
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
  (void)argument;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MOTOR_CTRL_PERIOD_MS));
    int32_t encoder_delta = BSP_Motor_GetEncoderAndClear();
    float actual_rpm = encoder_to_rpm(encoder_delta, MOTOR_CTRL_DT);
    g_motor.actual_rpm = actual_rpm;
    g_motor.raw_encoder = encoder_delta;
    g_motor.motor_online = 1U;
    float target_rpm = g_motor.target_rpm;
    if (CAN_App_IsGimbalCtrlUpdated()) {
      GimbalCtrlMsg_t ctrl = CAN_App_GetGimbalCtrl();
      target_rpm = (float)ctrl.wheel_target_speed;
      g_motor.target_rpm = target_rpm;
      g_motor.target_updated = 1U;
    }
    if (fabsf(target_rpm) < MOTOR_ZERO_SPEED_THRESHOLD) {
      PID_Reset(&g_motor.speed_pid);
    }
    float pid_out = PID_Compute(&g_motor.speed_pid, target_rpm, actual_rpm, MOTOR_CTRL_DT);
    g_motor.pid_output = pid_out;
    uint16_t duty = (uint16_t)fabsf(pid_out);
    if (duty > MOTOR_PWM_MAX) duty = MOTOR_PWM_MAX;
    if (duty < 50 && fabsf(target_rpm) < MOTOR_ZERO_SPEED_THRESHOLD) {
      BSP_Motor_Stop();
    }
    else if (pid_out > 0.0f) {
      BSP_Motor_SetSpeed(duty, MOTOR_DIR_CW);
    }
    else if (pid_out < 0.0f) {
      BSP_Motor_SetSpeed(duty, MOTOR_DIR_CCW);
    }
    else {
      BSP_Motor_Stop();
    }
    CAN_App_SetChassisFeedback((int16_t)actual_rpm, (int16_t)encoder_delta);
    if (fabsf(actual_rpm) > 500.0f) {
      g_motor.motor_error = 1U;
      BSP_Motor_Stop();
    }
    else if (fabsf(target_rpm) > 50.0f && fabsf(actual_rpm) < 5.0f
               && duty > MOTOR_PWM_MAX * 0.3f) {
      g_motor.motor_error = 1U;
               }
    else {
      g_motor.motor_error = 0U;
    }
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
  (void)argument;
  vTaskDelay(pdMS_TO_TICKS(100));
  TickType_t xLastWakeTime = xTaskGetTickCount();
  for(;;)
  {
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(OLED_REFRESH_PERIOD_MS));
    OLED_app_Update();
    osDelay(1);
  }
  /* USER CODE END StartOLEDTask */
}
#endif

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#ifdef GIMBAL
static void SelfTest_Execute(void)
{
  CAN_App_SetStatusFlag(STATUS_SELF_TEST, 1U);
  for (int i = 0; i < 2; i++) {
    LED_Off();
    vTaskDelay(pdMS_TO_TICKS(150));
    LED_On();
    vTaskDelay(pdMS_TO_TICKS(150));
  }
  CAN_App_SetGimbalCtrl(60, 0);
  BSP_Servo_SetSpeed(100);
  vTaskDelay(pdMS_TO_TICKS(SELF_TEST_SERVO_DURATION_MS));
  BSP_Servo_SetSpeed(-100);
  vTaskDelay(pdMS_TO_TICKS(SELF_TEST_SERVO_DURATION_MS));
  BSP_Servo_SetSpeed(0);
  CAN_App_SetGimbalCtrl(0, 0);
  vTaskDelay(pdMS_TO_TICKS(500));
  CAN_App_SetGimbalCtrl(0, 100);
  vTaskDelay(pdMS_TO_TICKS(SELF_TEST_PHASE_DURATION_MS));
  CAN_App_SetGimbalCtrl(0, -100);
  vTaskDelay(pdMS_TO_TICKS(SELF_TEST_PHASE_DURATION_MS));
  CAN_App_SetGimbalCtrl(0, 0);
  CAN_App_SetStatusFlag(STATUS_SELF_TEST, 0U);
  LED_On();
}
#endif

#ifdef CHASSIS
static float encoder_to_rpm(int32_t delta, float dt_sec)
{
  if (dt_sec <= 0.0f) return 0.0f;
  float revolutions = (float)delta / MOTOR_ENCODER_CPR;
  return revolutions * 60.0f / dt_sec;
}
#endif
/* USER CODE END Application */
