#include <stdio.h>
#include <gpio.h>
#include <ADS1256.h>
#include "spi.h"


uint8_t ADS1256_CMD_WAKEUP  = 0x00 ;
uint8_t ADS1256_CMD_RDATA  =  0x01 ;
uint8_t ADS1256_CMD_WREG   =  0x50 ;
uint8_t ADS1256_CMD_SYNC  =   0xfc ;
//-----------------------------------------------------------------//
//	功    能：ADS1256 写数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 向ADS1256中地址为regaddr的寄存器写入一个字节databyte
//-----------------------------------------------------------------//
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
	uint8_t temp = ADS1256_CMD_WREG | (regaddr & 0x0F);
	//printf("%d\n",! HAL_GPIO_ReadPin(GPIOB,GPIO_ADS1256DRDY));
    HAL_GPIO_WritePin(GPIOB, GPIO_ADS1256CS,GPIO_PIN_RESET);
		while(! HAL_GPIO_ReadPin(GPIOB,GPIO_ADS1256DRDY))
		{
		}
    HAL_SPI_Transmit(&hspi2,&temp,1,10);
    HAL_SPI_Transmit(&hspi2,0x00,1,10);
     HAL_SPI_Transmit(&hspi2,&databyte,1,10);
	HAL_GPIO_WritePin(GPIOB, GPIO_ADS1256CS,GPIO_PIN_SET);
}



//-----------------------------------------------------------------//
//	功    能：初始化ADS1256
//	入口参数: /
//	出口参数: /
//	全局变量: /
//-----------------------------------------------------------------//
void ADS1256_Init(void)
{
	ADS1256WREG(ADS1256_STATUS,0x06);               // 高位在前、校准、使用缓冲
//	ADS1256WREG(ADS1256_MUX,0x08);                  // 初始化端口A0为‘+’，AINCOM位‘-’
	ADS1256WREG(ADS1256_ADCON,0x00);                // 放大倍数1
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_10SPS);  // 数据5sps
	ADS1256WREG(ADS1256_IO,0x00);               
}



//-----------------------------------------------------------------//
//	功    能：读取AD值
//	入口参数: /
//	出口参数: /
//	全局变量: /
//-----------------------------------------------------------------//
unsigned int ADS1256ReadData()  
{
	uint8_t temp = 0xff;
	
    unsigned char i=0;
    unsigned int sum=0;
	unsigned int r=0;
	HAL_GPIO_WritePin(GPIOB,GPIO_ADS1256CS,GPIO_PIN_RESET);

	while(!HAL_GPIO_ReadPin(GPIO_ADS1256DRDY_PORT,GPIO_ADS1256DRDY))
	{
	}	
	//当ADS1256_DRDY为低时才能写寄存器 
//	ADS1256WREG(ADS1256_MUX,channel);		//设置通道
	 HAL_SPI_Transmit(&hspi2,&ADS1256_CMD_WREG,1,10);
	 HAL_SPI_Transmit(&hspi2,&ADS1256_CMD_SYNC,1,10);
	 HAL_SPI_Transmit(&hspi2,&ADS1256_CMD_WAKEUP,1,10);
	 HAL_SPI_Transmit(&hspi2,&ADS1256_CMD_RDATA,1,10);

   	sum |= (HAL_SPI_Transmit(&hspi2,&temp,1,10) << 16);
	sum |= (HAL_SPI_Transmit(&hspi2,&temp,1,10) << 8);
	sum |= HAL_SPI_Transmit(&hspi2,&temp,1,10);

	HAL_GPIO_WritePin(GPIOB, GPIO_ADS1256CS,GPIO_PIN_SET);
    return sum;
}



//-----------------------------------------------------------------//
//	功    能：读取ADS1256单路数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: /
//-----------------------------------------------------------------//
unsigned int ADS_sum(unsigned char channel)
{
	ADS1256WREG(ADS1256_MUX,channel);		//设置通道
	return ADS1256ReadData();//读取AD值，返回24位数据。
}
