#include "sys.h"
#include "usart.h"

extern u8 g_Usart_Print_Flag;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	if(g_Usart_Print_Flag==1)//指向串口1
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (u8) ch;      
		return ch;
	}
	else if(g_Usart_Print_Flag==3)//指向串口3
	{
		while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
		USART3->DR = (u8) ch;      
		return ch;
	}
	else if(g_Usart_Print_Flag == 4)//指向串口4
	{
		while((UART4->SR&0X40)==0);//循环发送,直到发送完毕   
		UART4->DR = (u8) ch;      
		return ch;
	}
	else if(g_Usart_Print_Flag == 5)//指向串口5
	{
		while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
		UART5->DR = (u8) ch;      
		return ch;
	}
	else //指向串口1
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = (u8) ch;      
		return ch;
	}
}
#endif 
////////////////////////////////////////////////////////////////////


//串口1初始化
void USART1_Config(void)		//初始化 配置USART1
{
	GPIO_InitTypeDef     GPIO_InitStructure;   //串口端口配置结构体变量
	USART_InitTypeDef    USART_InitStructure;  //串口参数配置结构体变量
	NVIC_InitTypeDef    NVIC_InitStructure;

	//第1步：打开GPIO和USART部件的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	 //打开GPIOA时钟和GPIOA复用时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//打开串口复用时钟
	USART_DeInit(USART1);  //复位串口1

	//第2步：将USART1 Tx（发送脚）的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;			   //串口1发送脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //输出速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA
														  
	//第3步：将USART Rx（接收脚）的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;			   //串口1接收脚
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   //初始化GPIOA

	//第4步：配置USART1参数
	USART_InitStructure.USART_BaudRate             = 50000;							 //波特率设置：115200
	USART_InitStructure.USART_WordLength           = USART_WordLength_8b;			 //数据位数设置：8位
	USART_InitStructure.USART_StopBits             = USART_StopBits_1;				 //停止位设置：1位
	USART_InitStructure.USART_Parity               = USART_Parity_No;				 //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl  = USART_HardwareFlowControl_None; //硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode                 = USART_Mode_Rx | USART_Mode_Tx;	 //接收与发送都使能
	USART_Init(USART1, &USART_InitStructure);										 //初始化USART1
	
	//串口1用DMA接收中断优先级设置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;       //通道设置为串口DMA通道4中断  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
  //打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);  // 使能指定的USART1发送中断 ；
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能指定的USART1接收中断 ；

	//第5步：使能 USART1， 配置完毕
	USART_Cmd(USART1, ENABLE);							   //使能 USART1

  //如下语句解决第1个字节无法正确发送出去的问题
  USART_ClearFlag(USART1, USART_FLAG_TC);                //清串口1发送标志
}

//串口2初始化
void USART2_Config(void)   //初始化 配置USART2
{
	GPIO_InitTypeDef GPIO_InitStructure;	   //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;   //串口参数配置结构体变量
	NVIC_InitTypeDef    NVIC_InitStructure;

	//使能 USART2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//打开串口复用时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);   //打开PA端口时钟，在GPIO_Configuration初始化gpioA

	//将USART2 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				  //选定哪个IO口 现选定PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);    				  //初始化GPIOA

	//将USART2 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				  //选定哪个IO口 现选定PA3
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);                    //初始化GPIOA
	  
	//配置USART2参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART2, &USART_InitStructure);                       //初始化USART2
	
		//串口2中断优先级设置(RS485)
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			   //指定中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);	
	
	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);  // 使能指定的USART2发送中断
  	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // 使能指定的USART2接收中断
		USART_ITConfig(USART2, USART_IT_ERR, ENABLE); //使能ERR中断

	//使能 USART2， 配置完毕
	USART_Cmd(USART2, ENABLE);                             // USART2使能

	//如下语句解决第1个字节无法正确发送出去的问题
  USART_ClearFlag(USART2, USART_FLAG_TC);                //清串口2发送标志
}

//串口3初始化
void USART3_Config(void)   //初始化 配置USART3
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;	  //串口参数配置结构体变量

	//使能 USART3 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//打开串口复用时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //打开PB端口时钟
  	
	//将USART3 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);    				  //初始化GPIOB
	
	//将USART3 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);                    //初始化GPIOA
	  
	//配置USART3参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：2400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(USART3, &USART_InitStructure);                       //初始化USART3
	
    //打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(USART3, USART_IT_TXE, ENABLE);  // 使能指定的USART3发送中断
  //USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 使能指定的USART3接收中断

	//使能 USART3， 配置完毕
	USART_Cmd(USART3, ENABLE);                             // USART3使能

	//如下语句解决第1个字节无法正确发送出去的问题
    USART_ClearFlag(USART3, USART_FLAG_TC);                //清串口3发送标志
}

//串口4初始化
void UART4_Config(void)   //初始化 配置USART4
{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//打开串口复用时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //打开PC端口时钟
	
 	//USART_DeInit(UART4);  //复位串口1
	 //将USART4 Tx（发送脚）的GPIO配置为推挽复用模式   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				  //选定哪个IO口 现选定PC10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);    				  //初始化GPIOC

	//将UART4 Rx（接收脚）的GPIO配置为浮空输入模式														  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				  //选定哪个IO口 现选定PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);                    //初始化GPIOC


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = 115200;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART4, &USART_InitStructure); //初始化串口
//#if EN_UART4_RX		  //如果使能了接收  
   //UART4 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
//#endif
  USART_Cmd(UART4, ENABLE);                    //使能串口 
	
	//如下语句解决第1个字节无法正确发送出去的问题 */
    USART_ClearFlag(UART5, USART_FLAG_TC);                //清串口5发送标志

}

//串口5初始化
void UART5_Config(void)   //初始化 配置UART5
{
	GPIO_InitTypeDef GPIO_InitStructure;		//串口端口配置结构体变量
	USART_InitTypeDef USART_InitStructure;		//串口参数配置结构体变量
	NVIC_InitTypeDef    NVIC_InitStructure;

	//使能 UART5 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);	//打开串口复用时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   //打开PC端口时钟
	
	//将UART5 Tx（发送脚）的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				  //选定哪个IO口 现选定PC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		  //设定IO口的输出速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);    				  //初始化GPIOC

	//将UART5 Rx（接收脚）的GPIO配置为浮空输入模式															  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				  //选定哪个IO口 现选定PD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	  //浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);                    //初始化GPIOC
	  
	//配置UART5参数
	USART_InitStructure.USART_BaudRate = 9600;	                    //波特率设置：2400
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	    //数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	        //停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;            //是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //接收与发送都使能
	USART_Init(UART5, &USART_InitStructure);                       //初始化UART5

	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;       //通道设置为串口DMA通道4中断  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	   //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;             //指定响应优先级别1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	               //打开中断
	NVIC_Init(&NVIC_InitStructure);							       //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	//打开发送中断和接收中断(如果需要中断)
	//USART_ITConfig(UART5, USART_IT_TXE, ENABLE);  // 使能指定的UART4发送中断
  	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 使能指定的UART4接收中断

	//使能 UART5， 配置完毕
	USART_Cmd(UART5, ENABLE);                             // UART5使能

	//如下语句解决第1个字节无法正确发送出去的问题 */
    USART_ClearFlag(UART5, USART_FLAG_TC);                //清串口5发送标志
}

void usart3_SendByte(unsigned char SendData)	  //用串口3发送一个字节数据
{	   
	USART_SendData(USART1,SendData);   //用串口3发送一个字节
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  //等待串口3发送完毕
}  
