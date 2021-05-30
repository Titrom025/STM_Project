#include "main.h"

/**
  * Initializes the Global MSP.
  */
//void HAL_MspInit(void)
//{
//  __HAL_RCC_SYSCFG_CLK_ENABLE();
//  __HAL_RCC_PWR_CLK_ENABLE();
//}

/**
* @brief I2C MSP Initialization
* This function configures the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
//void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
//{
//  GPIO_InitTypeDef GPIO_InitStruct = {0};
//  if(hi2c->Instance==I2C2)
//  {
//  /* USER CODE BEGIN I2C2_MspInit 0 */
//
//  /* USER CODE END I2C2_MspInit 0 */
//
//    __HAL_RCC_GPIOB_CLK_ENABLE();
//    /**I2C2 GPIO Configuration
//    PB10     ------> I2C2_SCL
//    PB11     ------> I2C2_SDA
//    */
//    GPIO_InitStruct.Pin = I2C2_SCL_Pin|I2C2_SDA_Pin;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF1_I2C2;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    /* Peripheral clock enable */
//    __HAL_RCC_I2C2_CLK_ENABLE();
//  /* USER CODE BEGIN I2C2_MspInit 1 */
//
//  /* USER CODE END I2C2_MspInit 1 */
//  }
//
//}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
//*/
//void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
//{
//  if(hi2c->Instance==I2C2)
//  {
//  /* USER CODE BEGIN I2C2_MspDeInit 0 */
//
//  /* USER CODE END I2C2_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_I2C2_CLK_DISABLE();
//
//    /**I2C2 GPIO Configuration
//    PB10     ------> I2C2_SCL
//    PB11     ------> I2C2_SDA
//    */
//    HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);
//
//    HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);
//
//  /* USER CODE BEGIN I2C2_MspDeInit 1 */
//
//  /* USER CODE END I2C2_MspDeInit 1 */
//  }
//
//}

/**
* @brief RTC MSP Initialization
* This function configures the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    __HAL_RCC_RTC_ENABLE();
    HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  }
}

/**
* @brief RTC MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hrtc: RTC handle pointer
* @retval None
*/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc)
{
  if(hrtc->Instance==RTC)
  {
    __HAL_RCC_RTC_DISABLE();
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  }
}