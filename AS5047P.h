/*************************************
 * @file           : AS5047P.h
 * @brief          : magnetic position sensor
 * @author         : Ouyang Junyuan
 *************************************
 * @attention      :
 *
 *************************************/
#ifndef AS5047P_H_
#define AS5047P_H_

#include "spi.h"


typedef struct
{
	SPI_HandleTypeDef *hspin;

	GPIO_TypeDef *CSNport;
	uint16_t CSNpin;
}AS5047_TypeDef;
extern AS5047_TypeDef  has5047;

















//spi command frame 格式
#define AS5047_COMMAND_READ 0x4000




//Volatil Registers 的 addrees mapping
#define	NOP_AS5047P_VOL_REG_ADD 0x0000
#define ERRFL_AS5047P_VOL_REG_ADD 0x0001
#define PROG_AS5047P_VOL_REG_ADD 0x0003
#define DIAAGC_AS5047P_VOL_REG_ADD 0x3ffc
#define MAG_AS5047P_VOL_REG_ADD 0x3ffd
#define ANGLEUNC_AS5047P_VOL_REG_ADD 0x3ffe
#define ANGLECOM_AS5047P_VOL_REG_ADD 0x3fff

//non-volatile-registers 的 addrees mapping
#define ZPOSM_AS5047P_nVOL_REG_ADD 0x0016
#define ZPOSL_AS5047P_nVOL_REG_ADD 0x0017
#define SETTINGS1_AS5047P_nVOL_REG_ADD 0x0018
#define SETTINGS2_AS5047P_nVOL_REG_ADD 0x0019
#define RED_AS5047P_VOL_nREG_ADD 0x001a



//------------------------------------------------
#define SPI_CSN_1  HAL_GPIO_WritePin(has5047.CSNport,has5047.CSNpin,GPIO_PIN_SET)
#define SPI_CSN_0  HAL_GPIO_WritePin(has5047.CSNport,has5047.CSNpin,GPIO_PIN_RESET)







void AS5047_Init(void);
void AS5047_SetZeroPosition(void);
uint16_t AS5047_Get_ZeroPosition(void);
uint16_t AS5047_Get_ERRFL(void);
uint16_t AS5047_WriteData(uint16_t addr,uint16_t data);
uint16_t AS5047_ReadData(uint16_t addr);
uint16_t AS5047_ReadSPI(void);


#endif

