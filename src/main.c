/**
 ******************************************************************************
 * @file    Template/main.c 
 * @author  MCD Application Team
 * @version V1.0.0
 * @date    20-September-2013
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "RTC.h"
#include "main.h"
#include "stm32f4xx_conf.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static void LED_task(void *pvParameters);
static void LCD_display_task(void *pvParameters);
char * _8bitToStr(uint8_t time);
void prvInit();
char timeStr[];
xTaskHandle *pvLEDTask;

int main(void)
{
	RTC_setting();
	prvInit();
	/* Turn OFF all LEDs */
	STM_EVAL_LEDOff(LED4);
	STM_EVAL_LEDOff(LED3);

	/* Create a task to display something in the LCD. */
	xTaskCreate(LCD_display_task,
			(signed portCHAR *) "Liquid Crystal Display",
			128 , NULL,
			tskIDLE_PRIORITY + 1, NULL);
	/* Create a task to flash the LED. */
	xTaskCreate(LED_task,
			(signed portCHAR *) "LED Flash",
			128 /* stack size */, NULL,
			tskIDLE_PRIORITY + 1, pvLEDTask );
	/* Start running the tasks. */
	vTaskStartScheduler();
	return 0;
}
static void LCD_display_task(void *pvParameters)
{
	char *hello = "hello world";	
	char sec[3], min[3], hour[10], time[15];
	LCD_SetColors(LCD_COLOR_RED, LCD_COLOR_GREY);
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	while(1){
		RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
		RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
		strcpy(sec, _8bitToStr(RTC_TimeStruct.RTC_Seconds));
		strcpy(min, _8bitToStr(RTC_TimeStruct.RTC_Minutes));
		strcpy(hour, _8bitToStr(RTC_TimeStruct.RTC_Hours));
		strcpy(time,hour);
		strcat(time, ":");
		strcat(time, min);
		strcat(time, ":");
		strcat(time, sec);
		LCD_DisplayStringLine(LCD_LINE_8, time);
			
		/*showCalendar_time((uint8_t)RTC_TimeStruct.RTC_Hours,
		(uint8_t)RTC_TimeStruct.RTC_Minutes,
		(uint8_t)RTC_TimeStruct.RTC_Seconds);
		showCalendar_date((uint8_t)RTC_DateStruct.RTC_Year,
		(uint8_t)RTC_DateStruct.RTC_Month,
		(uint8_t)RTC_DateStruct.RTC_Date);
	*/}
}
static void LED_task(void *pvParameters)
{
	RCC_ClocksTypeDef RCC_Clocks;
	uint8_t togglecounter = 0x00;
	while(1)
	{
		/* Toggle LED3 */
		STM_EVAL_LEDToggle(LED3);
		vTaskDelay(200);
		/* Toggle LED4 */
		STM_EVAL_LEDToggle(LED4);
		vTaskDelay(200);
	}
}

void prvInit()
{
	//LCD init
	LCD_Init();
	IOE_Config();
	LTDC_Cmd( ENABLE );
	LCD_LayerInit();
	//Two LCD Layers setting initial
	LCD_SetLayer( LCD_BACKGROUND_LAYER );
	LCD_Clear( LCD_COLOR_BLACK );
	LCD_SetTransparency(0x00);
	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	LCD_SetTransparency(0xFF);
	LCD_Clear( LCD_COLOR_BLACK );
	LCD_SetTextColor( LCD_COLOR_WHITE );

	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED3);
}

char * _8bitToStr(uint8_t time)	// Hexadecimal to Decimal'string 
{
	uint8_t cnt = 0;
	cnt += ((time & 0xF0) >> 4) * 16;
	cnt += (time & 0x0F);
	*timeStr = cnt/10 + '0';
	*(timeStr+1) = cnt%10 + '0';
	*(timeStr+2) = '\0';
	return timeStr;
}
