#include "includes.h"

#define I2C_Speed              100000 
#define I2C1_SLAVE_ADDRESS7    0xA0 
#define I2C_PageSize           256 

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8 
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8  
    
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9 
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9 

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8 
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9 

u8 ack;
/* Private macro -------------------------------------------------------------*/ 
/* Private variables ---------------------------------------------------------*/ 
vu8 FRAM_ADDRESS; 

/* Private function prototypes -----------------------------------------------*/ 

/**/ 
void I2C_delay(void) 
{ 
   u8 i=150; //��������Ż��ٶ� ����������͵�5����д�� 
   while(i)  
   {  
     i--;  
   }  
} 

bool I2C_Start(void) 
{ 
SDA_H; 
SCL_H; 
I2C_delay(); 
if(!SDA_read)return FALSE; //SDA��Ϊ�͵�ƽ������æ,�˳� 
SDA_L; 
I2C_delay(); 
if(SDA_read) return FALSE; //SDA��Ϊ�ߵ�ƽ�����߳���,�˳� 
SDA_L; 
I2C_delay(); 
return TRUE; 
} 

void I2C_Stop(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_L; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
} 

void I2C_Ack(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_L; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SCL_L; 
I2C_delay(); 
} 

void I2C_NoAck(void) 
{ 
SCL_L; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
SCL_L; 
I2C_delay(); 
} 

bool I2C_WaitAck(void)   //����Ϊ:=1��ACK,=0��ACK 
{ 
SCL_L; 
I2C_delay(); 
SDA_H; 
I2C_delay(); 
SCL_H; 
I2C_delay(); 
if(SDA_read) 
{ 
      SCL_L; 
      return 0; 
} 
SCL_L; 
return 1; 
} 

void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ// 
{ 
    u8 i=8; 
    while(i--) 
    { 
        SCL_L; 
        I2C_delay(); 
      if(SendByte&0x80) 
        SDA_H;   
      else  
        SDA_L;    
        SendByte<<=1; 
        I2C_delay(); 
SCL_H; 
        I2C_delay(); 
    } 
    SCL_L; 
} 

u8 I2C_ReceiveByte(void)  //���ݴӸ�λ����λ// 
{  
    u8 i=8; 
    u8 ReceiveByte=0; 

    SDA_H; 
    while(i--) 
    { 
      ReceiveByte<<=1;       
      SCL_L; 
      I2C_delay(); 
  SCL_H; 
      I2C_delay(); 
      if(SDA_read) 
      { 
        ReceiveByte|=0x01; 
      } 
    } 
    SCL_L; 
    return ReceiveByte; 
} 

bool I2C_FRAM_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite) 
{ 
u8 Addr = 0, count = 0; 

Addr = WriteAddr / I2C_PageSize; 

count = WriteAddr % I2C_PageSize; 

Addr = Addr << 1; 

Addr = Addr & 0x0F;   

FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr; 

    if (!I2C_Start()) return FALSE; 
    I2C_SendByte(FRAM_ADDRESS);//����������ַ+�ε�ַ  
    if (!I2C_WaitAck()) 
{ 
I2C_Stop();  
return FALSE; 
} 
    I2C_SendByte(count);   //���ö��ڵ�ַ       
I2C_WaitAck(); 
   
while(NumByteToWrite--) 
{ 
  I2C_SendByte(* pBuffer); 
  I2C_WaitAck(); 
      pBuffer++; 
} 
I2C_Stop(); 
  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms) 
  //Systick_Delay_1ms(10); 
return TRUE; 
} 


//����1������          
bool I2C_FRAM_BufferRead(u8* pBuffer, u16 WriteAddr, u16 NumByteToRead) 
{ 
u8 Addr = 0, count = 0; 

    Addr = WriteAddr / I2C_PageSize; 

count = WriteAddr % I2C_PageSize; 

Addr = Addr << 1; 

Addr = Addr & 0x0F;   

FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr; 

if (!I2C_Start()) return FALSE; 

    I2C_SendByte(FRAM_ADDRESS);//����������ַ+�ε�ַ  

    if (!I2C_WaitAck())  
{ 
I2C_Stop();  
return FALSE; 
} 

    I2C_SendByte(count);   //���õ���ʼ��ַ       
    I2C_WaitAck(); 
    I2C_Start(); 
    I2C_SendByte(FRAM_ADDRESS | 0x01); 
    I2C_WaitAck(); 
    while(NumByteToRead) 
    { 
      *pBuffer = I2C_ReceiveByte(); 
      if(NumByteToRead == 1)I2C_NoAck(); 
      else I2C_Ack();  
      pBuffer++; 
      NumByteToRead--; 
    } 
    I2C_Stop(); 
    return TRUE; 
} 




/*******************************************************************
                     �����ߺ���               
����ԭ��: void  Start_I2c();  
����:       ����I2C����,������I2C��ʼ����.
  
********************************************************************/
void Start_I2c()
{
	SDA_H;   /*������ʼ�����������ź�*/
	delay_us(1);

	SCL_H;
	delay_us(1);    /*��ʼ��������ʱ�����4.7us,��ʱ*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1); 
	   
	SDA_L;   /*������ʼ�ź�*/
	delay_us(1);    /* ��ʼ��������ʱ�����4��s*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1); 
	      
	SCL_L;   /*ǯסI2C���ߣ�׼�����ͻ�������� */
	delay_us(1);
	delay_us(1);
}				  

/*******************************************************************
                      �������ߺ���               
����ԭ��: void  Stop_I2c();  
����:       ����I2C����,������I2C��������.
  
********************************************************************/
void Stop_I2c()
{
	SDA_L;  /*���ͽ��������������ź�*/
	delay_us(1);   /*���ͽ���������ʱ���ź�*/

	SCL_H;  /*������������ʱ�����4��s*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);

	SDA_H;  /*����I2C���߽����ź�*/
	delay_us(1);
	delay_us(1);
	delay_us(1);
	delay_us(1);
}							 

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: void  SendByte(u8 c);
����:  ������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������,�����ȴ�Ӧ��,����
     ��״̬λ���в���.(��Ӧ����Ӧ��ʹack=0 ��)     
     ��������������ack=1; ack=0��ʾ��������Ӧ����𻵡�
********************************************************************/
void  SendByte(u8 c)
{
	u8 BitCnt;
	
	for(BitCnt=0;BitCnt<8;BitCnt++)  /*Ҫ���͵����ݳ���Ϊ8λ*/
	{
		if((c<<BitCnt)&0x80)
			SDA_H;   /*�жϷ���λ*/
		else  
			SDA_L;                
		delay_us(1);

		SCL_H;               /*��ʱ����Ϊ�ߣ�֪ͨ��������ʼ��������λ*/
		delay_us(1); 
		delay_us(1);               /*��֤ʱ�Ӹߵ�ƽ���ڴ���4��s*/
		delay_us(1);
		delay_us(1);
		delay_us(1);         
		SCL_L; 
	}
	
	delay_us(1);
	delay_us(1);
	SDA_H;               /*8λ��������ͷ������ߣ�׼������Ӧ��λ*/
	delay_us(1);
	delay_us(1);   
	SCL_H;
	delay_us(1);
	delay_us(1);
	delay_us(1);
	if(SDA_read)
		ack=0;     
	else 
		ack=1;        /*�ж��Ƿ���յ�Ӧ���ź�*/
	SCL_L;
	delay_us(1);
	delay_us(1);
}					

/*******************************************************************
                 �ֽ����ݴ��ͺ���               
����ԭ��: u8  RcvByte();
����:  �������մ���������������,���ж����ߴ���(����Ӧ���ź�)��
     ���������Ӧ������  
********************************************************************/	
u8  RcvByte()
{
	u8 retc;
	u8 BitCnt;
	
	retc=0; 
	SDA_H;             /*��������Ϊ���뷽ʽ*/
	for(BitCnt=0;BitCnt<8;BitCnt++)
	{
		delay_us(1);           
		SCL_L;       /*��ʱ����Ϊ�ͣ�׼����������λ*/
		delay_us(1);
		delay_us(1);         /*ʱ�ӵ͵�ƽ���ڴ���4.7��s*/
		delay_us(1);
		delay_us(1);
		delay_us(1);
		SCL_H;       /*��ʱ����Ϊ��ʹ��������������Ч*/
		delay_us(1);
		delay_us(1);
		retc=retc<<1;
		if(SDA_read)
			retc=retc+1; /*������λ,���յ�����λ����retc�� */
		delay_us(1);
		delay_us(1); 
	}
	SCL_L;    
	delay_us(1);
	delay_us(1);
	return(retc);
}				   

/********************************************************************
                     Ӧ���Ӻ���
ԭ��:  void Ack_I2c(bit a);
 
����:����������Ӧ���ź�,(������Ӧ����Ӧ���ź�)
********************************************************************/
void Ack_I2c(u8 a)
{
	if(a==0)
		SDA_L;     /*�ڴ˷���Ӧ����Ӧ���ź� */
	else 
		SDA_H;
	delay_us(1);
	delay_us(1);
	delay_us(1);      
	SCL_H;
	delay_us(1);
	delay_us(1);              /*ʱ�ӵ͵�ƽ���ڴ���4��s*/
	delay_us(1);
	delay_us(1);
	delay_us(1);  
	SCL_L;                /*��ʱ���ߣ�ǯסI2C�����Ա��������*/
	delay_us(1);
	delay_us(1);    
}			  

/*******************************************************************
                    �����ӵ�ַ���������ֽ����ݺ���               
����ԭ��: bit  ISendByte(u8 sla,ucahr c);  
����:     ���������ߵ����͵�ַ�����ݣ��������ߵ�ȫ����,��������ַsla.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
u8 ISendByte(u8 sla,u8 c)
{
	Start_I2c();               /*��������*/
	SendByte(sla);            /*����������ַ*/
	if(ack==0)
		return(0);
	SendByte(c);               /*��������*/
	if(ack==0)
		return(0);
	Stop_I2c();                 /*��������*/ 
	return(1);
}				  

/*******************************************************************
                    �����ӵ�ַ�������Ͷ��ֽ����ݺ���               
����ԭ��: bit  ISendStr(u8 sla,u8 suba,ucahr *s,u8 no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,���ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba������������sָ������ݣ�����no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
u8 ISendStr(u8 sla,u8 suba,u8 *s,u8 no)
{
	u8 i;
	
	Start_I2c();               /*��������*/

	SendByte(sla);            /*����������ַ*/
	if(ack==0)
		return(0);

	SendByte(suba);            /*���������ӵ�ַ*/
	if(ack==0)
		return(0);
	
	for(i=0;i<no;i++)
	{   
		SendByte(*s);               /*��������*/
		if(ack==0)return(0);
		s++;
	}
	 
	Stop_I2c();                 /*��������*/
	 
	return(1);
}					  

/*******************************************************************
                    �����ӵ�ַ�������ֽ����ݺ���               
����ԭ��: bit  IRcvByte(u8 sla,ucahr *c);  
����:     ���������ߵ����͵�ַ�������ݣ��������ߵ�ȫ����,��������
          ַsla������ֵ��c.
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
u8 IRcvByte(u8 sla,u8 *c)
{
	Start_I2c();                /*��������*/

	SendByte(sla+1);           /*����������ַ*/
	if(ack==0)
		return(0);

	*c=RcvByte();               /*��ȡ����*/
	Ack_I2c(1);               /*���ͷǾʹ�λ*/
	Stop_I2c();                  /*��������*/ 
	return(1);
}				   

/*******************************************************************
                    �����ӵ�ַ������ȡ���ֽ����ݺ���               
����ԭ��: bit  ISendStr(u8 sla,u8 suba,ucahr *s,u8 no);  
����:     ���������ߵ����͵�ַ���ӵ�ַ,�����ݣ��������ߵ�ȫ����,������
          ��ַsla���ӵ�ַsuba�����������ݷ���sָ��Ĵ洢������no���ֽڡ�
           �������1��ʾ�����ɹ��������������
ע�⣺    ʹ��ǰ�����ѽ������ߡ�
********************************************************************/
u8 IRcvStr(u8 sla,u8 suba,u8 *s,u8 no)
{
	u8 i;
	
	Start_I2c();               /*��������*/

	SendByte(sla);            /*����������ַ*/
	if(ack==0)
		return(0);

	SendByte(suba);            /*���������ӵ�ַ*/
	if(ack==0)
		return(0);
	
	Start_I2c();
	SendByte(sla+1);
	if(ack==0)
		return(0);
	
	for(i=0;i<no-1;i++)
	{   
		*s=RcvByte();               /*��������*/
		Ack_I2c(0);                /*���;ʹ�λ*/  
		s++;
	} 
	*s=RcvByte();
	Ack_I2c(1);  			/*���ͷ�Ӧλ*/
	Stop_I2c();             /*��������*/ 
	return(1);
}



