#include <stdio.h>
#include <gpio.h>
#include <ADS1256.h>
#include "spi.h"


uint8_t ADS1256_CMD_WAKEUP  = 0x00 ;
uint8_t ADS1256_CMD_RDATA  =  0x01 ;
uint8_t ADS1256_CMD_WREG   =  0x50 ;
uint8_t ADS1256_CMD_SYNC  =   0xfc ;
//-----------------------------------------------------------------//
//	��    �ܣ�ADS1256 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1256�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
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
//	��    �ܣ���ʼ��ADS1256
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//-----------------------------------------------------------------//
void ADS1256_Init(void)
{
	ADS1256WREG(ADS1256_STATUS,0x06);               // ��λ��ǰ��У׼��ʹ�û���
//	ADS1256WREG(ADS1256_MUX,0x08);                  // ��ʼ���˿�A0Ϊ��+����AINCOMλ��-��
	ADS1256WREG(ADS1256_ADCON,0x00);                // �Ŵ���1
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_10SPS);  // ����5sps
	ADS1256WREG(ADS1256_IO,0x00);               
}



//-----------------------------------------------------------------//
//	��    �ܣ���ȡADֵ
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
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
	//��ADS1256_DRDYΪ��ʱ����д�Ĵ��� 
//	ADS1256WREG(ADS1256_MUX,channel);		//����ͨ��
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
//	��    �ܣ���ȡADS1256��·����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
unsigned int ADS_sum(unsigned char channel)
{
	ADS1256WREG(ADS1256_MUX,channel);		//����ͨ��
	return ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
}
