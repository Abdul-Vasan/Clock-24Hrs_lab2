#include "stm32f4xx_hal.h"
#include "main.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#define USE_GLCD         

char time[100];
int i=0,m=59,h=23;
char sec[20];
char min[20];
char hour[20];

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;
#ifdef RTE_CMSIS_RTOS_RTX
extern   uint32_t               os_time;
uint32_t HAL_GetTick (void) {
  return os_time;
}
#endif

void SystemClock_Config(void);

static void Error_Handler (void) {
  while (1);
}

void TIM2_IRQHandler(void)
	{
	int correct;
	i++;
	correct=i/38;
	if(correct==60)
	{
		i=0;
		correct=0;
		m++;
		if(m==60)
		{
			m=0;
			h++;
			if(h==24)
			{
				h=0;
			}
		}
	}
	sprintf(sec,"  %.2d",correct);
	sprintf(min,"  %.2d",m);
	sprintf(hour,"  %.2d",h);
	
	GLCD_DrawString(0, 5*24, " Hours");
	GLCD_DrawString(0, 6*24, hour);
	
	GLCD_DrawString(85, 5*24, " Mins");
	GLCD_DrawString(85, 6*24, min);
	
	GLCD_DrawString(170, 5*24, "  Seconds");
	GLCD_DrawString(170, 6*24, sec);
	
	//TIM2->EGR |= 0x01; //Possibly the problem.
	TIM2->SR &=(1<<0);
}
void TIM2_Initialize (void)
	{
	const uint16_t PSC_val =  16000;
  const uint16_t ARR_val= 1000; 
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
  TIM2->PSC=PSC_val-1;	
	TIM2->ARR=ARR_val-1;	
	TIM2->CR1=0x0081;			
	TIM2->DIER =0x0041;
	NVIC_EnableIRQ(TIM2_IRQn); 	
	}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	GLCD_Initialize         ();
  GLCD_SetBackgroundColor (GLCD_COLOR_CYAN);
  GLCD_SetForegroundColor (GLCD_COLOR_RED);
  GLCD_ClearScreen        ();
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_DrawString         (0, 0*24, "    ");
	GLCD_DrawString         (0, 1*24, "    ");
	GLCD_DrawString         (0, 2*24, "     TIME ");

	TIM2_Initialize();
	
	while(1);
	
	HAL_PWR_EnableSleepOnExit();
	
	 
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}