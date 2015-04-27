#include "..\App\includes.h"
#include "..\App\Globals.h"

extern u16 USART3_RX_STA;							//��������״̬

/*
*********************************************************************************************************


*/

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

////////////////////////////////////////////////////////
void RCC_Configuration(void)
{	
  SystemInit(); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			   //���ù���ʹ��
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE); 			//ʹ�ܴ���2ʱ�� 
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE); 			//ʹ�ܴ���3ʱ�� 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);	 		//ʹ�ܴ���1ʱ��
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����ϵͳ�Ӻ���RCC_Configuration()ʹ��TIM2�ӣ�
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//����ϵͳ�Ӻ���RCC_Configuration()ʹ��TIM2�ӣ�
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//����ϵͳ�Ӻ���RCC_Configuration()ʹ��TIM4�ӣ�
}

//�رյ��Խӿڣ���GPIOʹ��
void UnableJTAG(void)
{
   
   RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;                     // enable clock for Alternate Function
   AFIO->MAPR &= ~(7UL<<24); // clear used bit
   AFIO->MAPR |= (4UL<<24); // set used bits
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : PB5: LED1 (mini and V3)
                   PD6��LED2 (only V3)
				   PD3��LED3 (only V3)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				                 //LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					 
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_3;		 			//LED2, LED3
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  //LCD SPI����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  //���迪����������
  //  |GPRS_SW  PA8|SK_SW  PE2|SMM_SW PE3|QD_SW  PC7|QDDY_SW PC6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //|SK_SW|SMM_SW|KB_H1|KB_H2|KB_H3|KB_H4|					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;				                 
  GPIO_Init(GPIOE, &GPIO_InitStructure);					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;				                 
  GPIO_Init(GPIOC, &GPIO_InitStructure);					 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	//SPEAKER			                 
  GPIO_Init(GPIOD, &GPIO_InitStructure);		

  //��������V1-V4����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  //��������A1-A4����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;				                 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

// PCA9555��I2C����
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);     									 
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  #if defined (VECT_TAB_RAM)
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#elif defined(VECT_TAB_FLASH_IAP)
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif 

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			    //���ô���1�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//�����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ��
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			     	//���ô���2�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//�����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ��
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			     	//���ô���3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	     	//��ռ���ȼ� 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;				//�����ȼ�Ϊ2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//ʹ��
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void BSP_Init(void)
{
  /* System Clocks Configuration --72M*/
  RCC_Configuration();   

  GPIO_Configuration();
  
  /* NVIC configuration */
  /*Ƕ�������жϿ����� */ 
  NVIC_Configuration();
  
  USART1_Config(9600);     	   //��ʼ������1     
  USART2_Config(9600);            //��ʼ������2    
  USART3_Config(9600);            //��ʼ������3    

  tp_Config();					   //SPI1 ������·��ʼ��	    
  
  TIM2_Configeration();	//100ms�ж�
  TIM3_Configeration();
  TIM4_Configeration();
}

/****************************************************************************
* ��    �ƣ�void tp_Config(void)
* ��    �ܣ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void tp_Config(void) 
{ 
  GPIO_InitTypeDef  GPIO_InitStructure; 
  SPI_InitTypeDef   SPI_InitStructure; 

  /* SPI1 ʱ��ʹ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE); 
 
  /* SPI1 SCK(PA5)��MISO(PA6)��MOSI(PA7) ���� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	        //����ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ����SPI1�����Ϲҽ���4�����裬������ʹ�ô�����ʱ����Ҫ��ֹ����3��SPI1 ���裬 ������������ */  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           		//SPI1 SST25VF016BƬѡ 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//�����ٶ�50MHZ 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//�������ģʽ
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA, GPIO_Pin_4);							//SPI1 SST25VF016BƬѡ�ø� 
  
   /* SPI1���� ���� */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //ȫ˫��  
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						   //��ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					   //8λ
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;						   //ʱ�Ӽ��� ����״̬ʱ��SCK���ֵ͵�ƽ
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						   //ʱ����λ ���ݲ����ӵ�һ��ʱ�ӱ��ؿ�ʼ
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							   //�������NSS
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;  //�����ʿ��� SYSCLK/128
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				   //���ݸ�λ��ǰ
  SPI_InitStructure.SPI_CRCPolynomial = 7;							   //CRC����ʽ�Ĵ�����ʼֵΪ7 
  SPI_Init(SPI1, &SPI_InitStructure);
  
  /* SPI1 ʹ�� */  
  SPI_Cmd(SPI1,ENABLE);  
}


/****************************************************************************
* ��    �ƣ�void USART1_Config(u32 baud)
* ��    �ܣ�����1����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART1_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         		 		//USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		 		//A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         	 		//USART1 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);		         	 		//A�˿�

  USART_InitStructure.USART_BaudRate = baud;						//����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
  
  /*����USART1 */
  USART_Init(USART1,&USART_InitStructure);							//���ô��ڲ�������   
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);						//ʹ�ܷ��ͻ�����ж�   
   /* ʹ��USART1 */
  //USART_Cmd(USART1, ENABLE);	
}

/****************************************************************************
* ��    �ƣ�void USART2_Config(u32 baud)
* ��    �ܣ�����2����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART2_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         		 	    //USART2 TX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		        //�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);		    		        //A�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	         	 	        //USART2 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	        //���ÿ�©����
  GPIO_Init(GPIOA, &GPIO_InitStructure);	

  USART_InitStructure.USART_BaudRate = baud;						//����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ

  /* Configure USART2 */
  USART_Init(USART2, &USART_InitStructure);							//���ô��ڲ�������   

  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
   /* ʹ��USART2 */
  USART_Cmd(USART2, ENABLE);	
}
/****************************************************************************
* ��    �ƣ�void USART3_Config(u32 baud)
* ��    �ܣ�����3����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void USART3_Config(u32 baud){
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         		 		//USART3 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    		 		//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);		    		 		//B�˿� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	         	 		//USART3 RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;   	 		//���ÿ�©����
  GPIO_Init(GPIOB, &GPIO_InitStructure);		         	 		//A�˿� 

  USART_InitStructure.USART_BaudRate = baud;						//����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;			//ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;				//��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   //��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�ģʽ
  /*����USART3 */
  USART_Init(USART3,&USART_InitStructure);							//���ô��ڲ�������   
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  //USART_ITConfig(USART3, USART_IT_TXE, ENABLE);						//ʹ�ܷ��ͻ�����ж�   
   /* ʹ��USART1 */
  USART_Cmd(USART3, ENABLE);	
}


//ÿ100ms������ʱ���ж�
//��ʱƵ��=72 000 000/(7200-1)/10000
//8MHz���񣺶�ʱƵ��=72 000 000 / (TIM_TimeBaseStructure.TIM_Prescaler + 1 ) / (TIM_TimeBaseStructure.TIM_Period)
void TIM2_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM2);//��λTIM2����
  TIM_TimeBaseStructure.TIM_Period = 1000;   //�����Զ�װ�ؼĴ���       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //��Ƶ����    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //ѡ�����ϼ���
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM2, DISABLE);   //���ܶ�ʱ��
}
void TIM3_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM3);//��λTIM2����
  TIM_TimeBaseStructure.TIM_Period = 1000;   //�����Զ�װ�ؼĴ���       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //��Ƶ����    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //ѡ�����ϼ���
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  TIM_ClearITPendingBit(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM3, DISABLE);   //���ܶ�ʱ��
}
void TIM4_Configeration()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
  TIM_DeInit(TIM4);//��λTIM2����
  TIM_TimeBaseStructure.TIM_Period = 1000;   //�����Զ�װ�ؼĴ���       
  TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;   //��Ƶ����    
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;   
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //ѡ�����ϼ���
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_ClearITPendingBit(TIM4, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4|TIM_IT_Update); 
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM4, DISABLE);   //���ܶ�ʱ��
}




////SYSTICK ���ã� 10msһ��systick�жϣ�����ucos ���Ƚ��ģ� 1KHz
void  OS_CPU_SysTickInit(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    INT32U         cnts;


    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (INT32U)rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC;

	SysTick_Config(cnts);

  
}


#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

