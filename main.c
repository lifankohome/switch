#include "stm32f10x.h"

void RCC_Configuration(void);
void LED_Initial(void);
void USART_Initial(void);
void NVIC_Configuration(void);
void Delay(vu32 nCount);

void USART1SendData(char *Buffer);
void USART2SendData(char *Buffer);

int main()
{
	RCC_Configuration();	//复位后外设时钟处于DISABLE状态，先开启时钟
    LED_Initial();
	USART_Initial();

	USART1SendData("USART1 Initial Complete!\n");
	USART2SendData("USART2 Initial Complete!\n");

    while(1)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        Delay(0x8ffff);
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        Delay(0x8ffff);
    }
}

void USART1_IRQHandler(void){
	unsigned char ch;
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET){
		ch = USART_ReceiveData(USART1);
	}
}
void USART2_IRQHandler(void){
	unsigned char ch;
	if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET){
		ch = USART_ReceiveData(USART2);
	}
}
void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//A口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//串口1
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//串口2
}
void LED_Initial(void){	//初始化LED引脚
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
void USART_Initial(void){	//初始化串口1引脚，PA9为复用推挽输出，PA10为浮空输入
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 4800;					//速率4800bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式
	USART_Init(USART1, &USART_InitStructure);					//配置串口参数函数
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				//使能接收中断
	USART_Cmd(USART1, ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;					//速率9600bps
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//停止位1位
	USART_InitStructure.USART_Parity = USART_Parity_No;			//无校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//收发模式
	USART_Init(USART2, &USART_InitStructure);					//配置串口参数函数
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);				//使能接收中断
	USART_Cmd(USART2, ENABLE);

	NVIC_Configuration();
}
void NVIC_Configuration(void){	//中断源配置
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		//优先级组，说明了抢占优先级所用的位数，和响应优先级所用的位数

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
void Delay(vu32 nCount){
     for(; nCount != 0; nCount--);
}
void USART1SendData(char *Buffer)
{
	u8 Counter = 0;
	while(Counter==0||Buffer[Counter] != 0){
		USART_SendData(USART1, Buffer[Counter++]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}
void USART2SendData(char *Buffer)
{
	u8 Counter = 0;
	while(Counter==0||Buffer[Counter] != 0){
		USART_SendData(USART2, Buffer[Counter++]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	}
}
