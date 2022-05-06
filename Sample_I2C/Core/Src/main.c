
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx.h"

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osThreadId HCSR04_TaskHandle;
osThreadId IR_TaskHandle;
osThreadId I2C_TaskHandle;
osMutexId i2cmutexHandle;
/* USER CODE BEGIN PV */
#define TRBUFSIZE 1
uint8_t toggle=0;
uint8_t rx_buf;
//uint8_t tx_buf = 0xFE;
uint8_t tx_buf=0;

HAL_StatusTypeDef g_ret = 0x0;
void Reset_I2C(I2C_HandleTypeDef* rv_i2c)
{
	HAL_I2C_DeInit(rv_i2c);
	HAL_I2C_Init(rv_i2c);
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
void StartDefaultTask(void const * argument);
void HCSR04_Start(void const * argument);
void IRTask_Start(void const * argument);
void Start_I2CTask(void const * argument);

void delay (uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while (__HAL_TIM_GET_COUNTER(&htim1)<time);

}

uint32_t inputVal1=0;
uint32_t inputVal2=0;
uint32_t difference=0;
uint8_t firstVal=0;
uint8_t distance=0;


void i2c_send_data (uint8_t cmd) {

    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd&0xf0);
    data_l = ((cmd<<4)&0xf0);
    data_t[0] = data_u|0x0C;
    data_t[1] = data_u|0x08;
    data_t[2] = data_l|0x0C;
    data_t[3] = data_l|0x08;
//    HAL_I2C_Master_Transmit (&hi2c1,SLAVE_ADDRESS_ETC,(uint8_t *) data_t, 4,100);
    HAL_I2C_Slave_Transmit(&hi2c1, data_t, 4, 100);

}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
	{
		if(firstVal==0)
		{
			inputVal1=HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); //Read second value
			__HAL_TIM_SET_COUNTER(htim,0); //Reset counter
			firstVal=1; //Set first captured as true

			__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);
		}
		else if(firstVal==1)
		{
			inputVal2=HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // read second value
			__HAL_TIM_SET_COUNTER(htim,0); //Reset counter

			if (inputVal1<=inputVal2)

			{
				difference=inputVal2-inputVal1;
			}
			else if (inputVal2<inputVal1)
			{
				difference=(0xffff-inputVal1)+inputVal2;
//				difference=0.0f;
//				difference = (htim->Instance->ARR-inputVal1)+inputVal2;
			}

			distance=difference * .034/2;
			firstVal=0; //Set it back to false


			//Set polarity to rising edge
			__HAL_TIM_SET_CAPTUREPOLARITY(htim,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
			__HAL_TIM_DISABLE_IT(&htim1,TIM_IT_CC1);
		}

	}

}
/*
In case you want to incorporate a HRS04 sensor
*/

void HCSR04_Read(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	delay(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
//	HAL_TIM_ENABLE_IT(&htim1,TIM_IT_CC1);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_CC1);
}

int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();

  osMutexDef(i2cmutex);
  i2cmutexHandle = osMutexCreate(osMutex(i2cmutex));


  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of HCSR04_Task */
  osThreadDef(HCSR04_Task, HCSR04_Start, osPriorityNormal, 0, 128);
  HCSR04_TaskHandle = osThreadCreate(osThread(HCSR04_Task), NULL);

  /* definition and creation of IR_Task */
  osThreadDef(IR_Task, IRTask_Start, osPriorityNormal, 0, 128);
  IR_TaskHandle = osThreadCreate(osThread(IR_Task), NULL);

  /* definition and creation of I2C_Task */
  osThreadDef(I2C_Task, Start_I2CTask, osPriorityIdle, 0, 128);
  I2C_TaskHandle = osThreadCreate(osThread(I2C_Task), NULL);


  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{


  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;//100000
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0x20;//144
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 84-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }


}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

void HCSR04_Start(void const * argument)
{
  /* USER CODE BEGIN HCSR04_Start */
//	MX_TIM1_Init();
  /* Infinite loop */
  for(;;)
  {




    osDelay(1);
  }
  /* USER CODE END HCSR04_Start */
}


void IRTask_Start(void const * argument)
{
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);

  for(;;)
  {
	  if(!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)) {
		  	  tx_buf=1;
	  		  toggle=1;

	  	  }
	  	  else
	  		  {
	  		  	  toggle=0;
	  		  	  tx_buf=0;
	  		  }
    osDelay(20);

  }
  /* USER CODE END IRTask_Start */
}

/* USER CODE BEGIN Header_Start_I2CTask */
/**
* @brief Function implementing the I2C_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Start_I2CTask */
void Start_I2CTask(void const * argument)
{
  /* USER CODE BEGIN Start_I2CTask */
  /* Infinite loop */
  for(;;)
  {
	  if((g_ret = HAL_I2C_Slave_Receive(&hi2c1, (uint8_t*)&rx_buf,TRBUFSIZE,0xFFFFFFFF))!=HAL_OK)
	  	  {
	  		  Reset_I2C(&hi2c1);
	  	  }
	  	  if (g_ret ==HAL_OK) {
	  		if((g_ret = HAL_I2C_Slave_Transmit(&hi2c1,(uint8_t*)&tx_buf, TRBUFSIZE, 0xFFFFFFFF))!=HAL_OK)
	  		{
	  			Reset_I2C(&hi2c1);

	  	  }//STM_GET SEND SIGNAL
	  		else if (rx_buf==0x02){
	  			rx_buf=0;
	  			HAL_I2C_Slave_Transmit(&hi2c1,(uint8_t*)&tx_buf, TRBUFSIZE, 0xFFFFFFFF);

	  		}
	  	  }


    osDelay(20);
  }
  /* USER CODE END Start_I2CTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

