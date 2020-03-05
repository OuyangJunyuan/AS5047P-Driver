#include "AS5047P.h"


AS5047_TypeDef has5047;

uint16_t SPI_ReadWrite_OneByte(uint16_t _txdata)
{
	uint16_t rxdata;
	//waring不用管,传进去的是地址数据不会被截断，HAL库根据配置，按16字节的数据格式将16字节的发送出去
	if(HAL_SPI_TransmitReceive(has5047.hspin,(uint8_t *)&_txdata,(uint8_t *)&rxdata,1,100) !=HAL_OK)
		rxdata=0;
	return rxdata;
}



uint16_t AS5047_ReadSPI(void)
{
	uint16_t data;
	SPI_CSN_0;
	data=SPI_ReadWrite_OneByte(0xffff);
	SPI_CSN_1;
	data &=0x3fff;
	return data;
}


uint16_t Parity_bit_Calculate(uint16_t data_2_cal)
{
	uint16_t parity_bit_value=0;
	//自低位开始统计奇偶
	while(data_2_cal != 0)
	{
		//(((⑤^④)^③)^②)^①——表达式1假设⑤和④不同，则⑤^④运算结果为1，表示有一个1。表达式1可以化成：((1^③)^②)^①——表达式2
		parity_bit_value ^= data_2_cal; 
		data_2_cal >>=1;
	}
	return (parity_bit_value & 0x1);
}





uint16_t AS5047_WriteData(uint16_t addr,uint16_t data)
{
	//发送地址指令
	// & 0x3fff 得到 13:0位 data数据 进行奇偶校验位计算
	if(Parity_bit_Calculate(addr & 0x3fff ) == 1 ) 
		addr |= 0x8000;  //将15bit 置1  偶校验
	SPI_CSN_0;  
	SPI_ReadWrite_OneByte(addr);
	SPI_CSN_1;
	
	//发送数据指令
	if(Parity_bit_Calculate(data &0x3fff) ==1)
		data |=0x8000;
	
	uint16_t feedback;
	SPI_CSN_0;
	feedback = SPI_ReadWrite_OneByte(data);
	SPI_CSN_1;
	
	return feedback;
}

uint16_t AS5047_ReadData(uint16_t addr)
{
	uint16_t data;
	if(Parity_bit_Calculate(addr) ==1 ) 
		addr |=0x8000;
	addr |= AS5047_COMMAND_READ;
	SPI_CSN_0;
	SPI_ReadWrite_OneByte(addr);
	SPI_CSN_1;
	
	SPI_CSN_0;
	data = SPI_ReadWrite_OneByte(addr);
	SPI_CSN_1;
	
	//此处可以做奇偶校验判断是否接收到正确数据，但是也可以不做，直接去掉15，14bit
	data &= 0x3fff; 
	
	return data;
}








void AS5047_Init(void)
{
	//按datasheet中的说到的步骤进行，配置SPI为双全工主机，2字节模式，时钟极性低，相位2，软件使能
	has5047.hspin=&hspi1;
	has5047.CSNport = GPIOA ;     //CSN为SPI片选使能信号，低电平使能，配置为输出模式，可以按自己需要修改引脚号。
	has5047.CSNpin =GPIO_PIN_4;
	
	AS5047_WriteData(SETTINGS1_AS5047P_nVOL_REG_ADD,5); //0000 0101
	AS5047_WriteData(SETTINGS2_AS5047P_nVOL_REG_ADD,0);
}


void AS5047_SetZeroPosition(void)
{
	uint16_t DIAAGC=AS5047_ReadData(DIAAGC_AS5047P_VOL_REG_ADD);
	//获取当前角度
	uint16_t ANGLEUNC=AS5047_ReadData(ANGLEUNC_AS5047P_VOL_REG_ADD);
	//												ANGLEUNC是13:0 14个有效数字，右移6获取高8位。
	AS5047_WriteData(ZPOSM_AS5047P_nVOL_REG_ADD,(ANGLEUNC >>6) & 0x00ff);
	//																							得到低6位 & 11 1111
	AS5047_WriteData(ZPOSL_AS5047P_nVOL_REG_ADD, ANGLEUNC  & 0x003f);
}

void AS5047_Diagnostics(void)
{
	;
}
uint16_t AS5047_Get_ANGLE_COM_Value(void)
{
	uint16_t ANGLECOM = AS5047_ReadSPI();
	return ANGLECOM;
}


uint16_t AS5047D_Get_MAGCORDIC_Value(void)
{
	unsigned int CORDICMAG = AS5047_ReadData(MAG_AS5047P_VOL_REG_ADD);
	return CORDICMAG;
}

unsigned int AS5047P_Get_AGC_Value(void)
{
	unsigned int DIAAGC = AS5047_ReadData(DIAAGC_AS5047P_VOL_REG_ADD);
	return (unsigned char)((DIAAGC >> 8) & 0x00FF);
}


uint16_t AS5047_Get_ZeroPosition(void)
{
	uint16_t ZPOSM=AS5047_ReadData(ZPOSM_AS5047P_nVOL_REG_ADD);
	uint16_t ZPOSL=AS5047_ReadData(ZPOSL_AS5047P_nVOL_REG_ADD);
	//将高八位和低6位拼起来
	return ( ZPOSM<<6 ) & (ZPOSL & 0x003f ) ;
}

uint16_t AS5047_Get_ERRFL(void)
{
	return AS5047_ReadData(ERRFL_AS5047P_VOL_REG_ADD);
}


