// STM32 Blue Pill HAL Blink + printf() example

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stm32f1xx_hal.h>

// Set STM32F103 LED RED (PC13)
#define LED_PORT                GPIOC
#define LED_PIN                 GPIO_PIN_13
#define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOC_CLK_ENABLE

static volatile uint32_t ms_ticks = 0;
static volatile char tick_1ms_elapsed = 0;
static volatile char tick_1000ms_elapsed = 0;
unsigned long mycnt1;
float myflt1;

UART_HandleTypeDef huart2;

void Error_Handler(void);
void SystemClock_Config(void);
static void MX_USART2_UART_Init(void);

//! The interrupt handler for the SysTick module
void SysTick_Handler(void)
{
//  HAL_IncTick();

  ms_ticks++;

  tick_1ms_elapsed = 1;

  if (ms_ticks % 1000 == 0)
  {
    tick_1000ms_elapsed = 1;
  }

/***
  // 1 Hz blinking
  if ((HAL_GetTick() % 500) == 0)
  {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

    printf("count= %08ld\n", mycnt1);
    mycnt1++;
//    printf("%5.1f\n", myflt1);
//    myflt1 += 0.1;
  }
***/
}


int main(void)
{
  const char * hello_world = "Hello STM32\r\n";

  HAL_Init();
  SystemClock_Config();

  initGPIO();
  MX_USART2_UART_Init();

  //1kHz ticks
  //HAL_SYSTICK_Config(SystemCoreClock / 1000);
  //HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    while (1) {  /* no error must happen here, otherwise this board is dead */ }
  }
  printf("SystemCoreClock: %ld\n\n", SystemCoreClock);

  HAL_UART_Transmit(&huart2, (uint8_t *)hello_world, strlen(hello_world), HAL_MAX_DELAY);

  printf("Characters: %c %c\n", 'a', 65);
  printf("Decimals: %d %ld\n", 1977, 650000L);
  printf("Preceding with blanks: %10d\n", 1977);
  printf("Preceding with  zeros: %010d\n", 1977);
  printf("Some different radices: %d %x %o %#x %#o\n", 100, 100, 100, 100, 100);
  printf("floats: %4.2f %+.0e %E\n", 3.1416, 3.1416, 3.1416);
  printf("Width trick: %*d\n", 5, 10);
  printf("%s\n", "A string");

  while (1)
  {
    if (tick_1ms_elapsed)
    {
      //... Do something every ms
      tick_1ms_elapsed = 0; // Reset the flag (signal 'handled')
    }

    if (tick_1000ms_elapsed)
    {
      //... Do something every second
      HAL_GPIO_TogglePin(LED_PORT, LED_PIN);

      printf("count= %08ld\n", mycnt1);
      mycnt1++;
//    printf("%5.1f\n", myflt1);
//    myflt1 += 0.1;

      tick_1000ms_elapsed = 0;  // Reset the flag (signal 'handled')
    }
  }

  return 0;
}

/**
    @brief System Clock Configuration
    @retval None
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void initGPIO()
{
  GPIO_InitTypeDef GPIO_Config;

  GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Config.Pull = GPIO_NOPULL;
  GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_Config.Pin = LED_PIN;

  LED_PORT_CLK_ENABLE();
  HAL_GPIO_Init(LED_PORT, &GPIO_Config);

//  __HAL_RCC_GPIOA_CLK_ENABLE();
}

/**
    @brief USART2 Initialization Function
    @param None
    @retval None
*/
static void MX_USART2_UART_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**
    USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA15    ------> USART2_RX
  */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
//  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

  __HAL_RCC_USART2_CLK_ENABLE();
}

#define STDOUT_FILENO   1
#define STDERR_FILENO   2

int _write(int file, char *ptr, int len)
{
  switch (file)
  {
    case STDOUT_FILENO: /*stdout*/
      HAL_UART_Transmit(&huart2, ptr, len, HAL_MAX_DELAY);
      // Send the string somewhere
      break;
    case STDERR_FILENO: /* stderr */
      HAL_UART_Transmit(&huart2, ptr, len, HAL_MAX_DELAY);
      // Send the string somewhere
      break;
    default:
      return -1;
  }
  return len;
}

void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
}

#ifdef USE_FULL_ASSERT
/**
    @brief  Reports the name of the source file and the source line number
            where the assert_param error has occurred.
    @param  file: pointer to the source file name
    @param  line: assert_param error line source number
    @retval None
*/
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
    tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
