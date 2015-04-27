//========================================================================
// �ļ���:  LCD_Dis.c
//========================================================================
#include "LCD_Config.h"
//#include "stm32f10x_lib.h"


//========================================================================
// ����: void LCD_Init(void)
// ����: LCD��ʼ��������Ҫ��������ɶ˿ڳ�ʼ���Լ�LCDģ��ĸ�λ
//========================================================================

void TimeDelay(unsigned int Timers)
{
	unsigned int i;
	while(Timers)
	{
		Timers--;
		for(i=0;i<500;i++);
	}
}

void LCD_Init(void)
{
	//SCKԤ������Ϊ�ߵ�ƽ
	SPI_SCK_SET();
	//CSԤ������Ϊ�ߵ�ƽ
	SPI_SS_SET();
	
	TimeDelay(80);				    	//��ʱ���10ms����
}
//========================================================================
// ����: void delay2(unsigned char i)
// ����: ��ʱ����
//		delay2(0):��ʱ518us 518-2*256=6
//����delay2(1):��ʱ7us
//����delay2(10):��ʱ25us 25-20=5
//����delay2(20):��ʱ45us 45-40=5
//����delay2(100):��ʱ205us 205-200=5
//����delay2(200):��ʱ405us 405-400=5
//========================================================================
void delay2(unsigned char i)
{
	while(--i);
} 

unsigned char GetKey(void)
{
	/*
	if(key1==0)
	{
			TimeDelay(2);	  //������
			if(key1==0)	  //ȷ�ϰ���������
			{
				while(!key1);//���ּ��
				return 1;
			}
	}
*/
	return 0;
}
//========================================================================
// ����: void SPI_SSSet(unsigned char Status)
// ����: ��SS��״̬
//========================================================================
void SPI_SSSet(unsigned char Status)
{
	if(Status)				//�ж���Ҫ��SSΪ�ͻ��Ǹߵ�ƽ��
		SPI_SS_SET();			//SS�øߵ�ƽ
	else   						
		SPI_SS_CLR();			//SS�õ͵�ƽ
}
//========================================================================
// ����: void SPI_Send(unsigned char Data)
// ����: ͨ������SPI������һ��byte��������ģ��
//========================================================================
void SPI_Send(unsigned int Data)
{
	unsigned char i=0;
	for(i=0;i<9;i++)
	{
		if(Data&0x100)
			SPI_SDA_SET();							//SDA�ø�
		else 
			SPI_SDA_CLR();						//SDA�õ�
		SPI_SCK_CLR();								//SCK�õ�
		delay2(30);//2us
		SPI_SCK_SET();								//SCK�ø�
		delay2(30);//2us
		Data = Data<<1;							//��������һλ
	}
}

//========================================================================
// ����: void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type) 
// ����: д��һ����׼ASCII�ַ�
//========================================================================
void PutChar(unsigned int x,unsigned int y,unsigned char a,unsigned char type) 
{
	//��ʾASCII�ַ�
	SPI_SS_CLR();														//SS�õ͵�ƽ	
	SPI_Send(ASC_DISPLAY&CMD);										//����ָ��ASC_DISPLAY
	SPI_Send((x>>8)|DAT);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	SPI_Send(y|DAT);
	SPI_Send(type|DAT);
	SPI_Send(a|DAT);															//Ҫ��ʾ���ַ�����
	SPI_SS_SET();															//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutString(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
// ����: ��x��yΪ��ʼ���괦д��һ����׼ASCII�ַ���
//========================================================================
void PutString(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
{
    unsigned char width,i;
    if(type==LCD_ASC8_FONT)
        width=8;
    else if(type==LCD_ASC12_FONT)
        width=12;
    else if(type==LCD_ASC16_FONT)
        width=16;
    for(i=0;i<strlen((char *)p);i++){
        PutChar(x+i*width,y,p[i],type);
        //delay2(200);
    }
}
//========================================================================
// ����: void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type) 
// ����: д��һ���������ֿ⺺��
//========================================================================
void PutChar_cn(unsigned int x,unsigned int y,unsigned char * GB,unsigned char type) 
{
	//��ʾ�����ַ�
	SPI_SS_CLR();															//SS�õ͵�ƽ	
	SPI_Send(HZ_DISPLAY&CMD);											//����ָ��HZ_DISPLAY
	SPI_Send((x>>8)|DAT);					//Ҫ��ʾ�ַ������Ͻǵ�X��λ��
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//Ҫ��ʾ�ַ������Ͻǵ�Y��λ��
	SPI_Send(y|DAT);
	SPI_Send(type|DAT);
	SPI_Send((*(GB++))|DAT);												//���Ͷ����ֿ��к���GB��ĸ߰�λֵ
	SPI_Send((*GB)|DAT);														//���Ͷ����ֿ��к���GB��ĵͰ�λֵ
	SPI_SS_SET();															//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutString_cn(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
// ����: ��x��yΪ��ʼ���괦д��һ�������ַ���
//========================================================================
void PutString_cn(unsigned int x,unsigned int y,unsigned char *p,unsigned char type)
{
    unsigned char width,i;
    if(type==LCD_HZK16_FONT)
        width=16;
    else if(type==LCD_HZK24_FONT)
        width=24;
    else if(type==LCD_HZK32_FONT)
        width=32;
    for(i=0;i<strlen((char *)p)-1;i+=2){
        PutChar_cn(x+(i/2)*width,y,&p[i],type);
        //delay2(200);
    }
}

//========================================================================
// ����: void PutPixel(unsigned int x,unsigned int y)
// ����: ��x��y���ϻ���һ��ǰ��ɫ�ĵ�
//========================================================================
void PutPixel(unsigned int x,unsigned int y)
{
	//������	
	SPI_SS_CLR();															//SS�õ͵�ƽ
	SPI_Send(DRAW_POINT&CMD);											//��ָ��DRAW_POINT
	SPI_Send((x>>8)|DAT);					//�͵�һ������,�����õ��X��λ��
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);					//���Y��λ��
	SPI_Send(y|DAT);
	SPI_SS_SET();															//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y)
// ����: ��s_x��s_yΪ��ʼ���꣬e_x��e_yΪ�����������һ��ֱ��
//========================================================================
void Line(unsigned int s_x,unsigned int  s_y,unsigned int  e_x,unsigned int  e_y)
{  
	//����ֱ��
	SPI_SS_CLR();															//SS�õ͵�ƽ
	SPI_Send(DRAW_LINE&CMD);												//��ָ��DRAW_LINE
	SPI_Send((s_x>>8)|DAT);					//���X������
	SPI_Send(s_x|DAT);
	SPI_Send((s_y>>8)|DAT);					//���Y������
	SPI_Send(s_y|DAT);
	SPI_Send((e_x>>8)|DAT);					//�յ�X������
	SPI_Send(e_x|DAT);
	SPI_Send((e_y>>8)|DAT);					//�յ�Y������
	SPI_Send(e_y|DAT);
	SPI_SS_SET();																//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode)
// ����: ��x,yΪԲ��RΪ�뾶��һ��Բ(mode = 0) or Բ��(mode = 1)
//========================================================================
void Circle(unsigned int x,unsigned int y,unsigned int r,unsigned char mode)
{
	SPI_SS_CLR();													//SS�õ͵�ƽ
	if(mode)
		SPI_Send(DRAW_SOLID_CIRCLE&CMD);				//��ָ��DRAW_SOLID_CIRCLE
	else
		SPI_Send(DRAW_CIRCLE&CMD);							//��ָ��DRAW_CIRCLE
	SPI_Send((x>>8)|DAT);			//���X������
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);			//�յ�Y������
	SPI_Send(y|DAT);
	SPI_Send((r>>8)|DAT);			//Բ�뾶r
	SPI_Send(r|DAT);
	SPI_SS_SET();													//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void Rectangle(unsigned int left, unsigned int top, unsigned int right,
//				 unsigned int bottom, unsigned char mode)
// ����: �������յ㻭���Σ�����(mode = 0) or ʵ�ķ���(mode = 1)
//========================================================================
void Rectangle(unsigned int left, unsigned int top, unsigned int right,
				 unsigned int bottom, unsigned char mode)
{
	SPI_SS_CLR();														//SS�õ͵�ƽ
	if(mode)
		SPI_Send(DRAW_SOLID_RECT&CMD);						//��ָ��DRAW_SOLID_RECT
	else
		SPI_Send(DRAW_RECT&CMD);									//��ָ��DRAW_RECT
	SPI_Send((left>>8)|DAT);
	SPI_Send(left|DAT);
	SPI_Send((top>>8)|DAT);
	SPI_Send(top|DAT);
	SPI_Send((right>>8)|DAT);
	SPI_Send(right|DAT);
	SPI_Send((bottom>>8)|DAT);
	SPI_Send(bottom|DAT);
	SPI_SS_SET();														//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void ClrScreen(void)
// ����: ����������ִ��ȫ��Ļ���
//========================================================================
void ClrScreen(void)
{
	//��������
	SPI_SS_CLR();										//SS�õ͵�ƽ
	SPI_Send(CLEAR_SCREEN&CMD);					//��ָ��CLEAR_SCREEN
	SPI_SS_SET();										//��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: void SetBG_Color(unsigned int BGColor)
// ����: ���ñ���ɫ
//========================================================================
void SetBG_Color(unsigned int BGColor)
{
	
	SPI_SS_CLR();														//SS�õ͵�ƽ
	SPI_Send(SET_BG_COLOR&CMD);									//��ָ��SET_BG_COLOR
	SPI_Send((BGColor>>8)|DAT);  //����ɫ��16bit RGB565
	SPI_Send(BGColor|DAT);
	SPI_SS_SET();														//��ɲ�����SS�ߵ�ƽ
}

//========================================================================
// ����: SetFG_Color(unsigned int FGColor)
// ����: ����ǰ��ɫ
//========================================================================
void SetFG_Color(unsigned int FGColor)
{
	SPI_SS_CLR();														//SS�õ͵�ƽ
	SPI_Send(SET_FG_COLOR&CMD);									//��ָ��SET_FG_COLOR
	SPI_Send((FGColor>>8)|DAT);	//ǰ��ɫ��16bit RGB565
	SPI_Send(FGColor|DAT);
	SPI_SS_SET();														//��ɲ�����SS�ߵ�ƽ
}
//========================================================================
// ����: void PutBitmap(unsigned int x,unsigned int y,unsigned long p)
// ����: ��x��yΪ��ʼ������ʾһ����ɫλͼ
//========================================================================
void PutBitmap(unsigned int x,unsigned int y,unsigned long p)
{

	SPI_SS_CLR();												//SS�õ͵�ƽ
	SPI_Send(ICON_DISPLAY&CMD);							//��ָ��ICON_DISPLAY
	SPI_Send((x>>8)|DAT);		//���X������
	SPI_Send(x|DAT);
	SPI_Send((y>>8)|DAT);		//���Y������
	SPI_Send(y|DAT);
	SPI_Send(((unsigned int)(p>>24))|DAT);		//bmp �����flash��ַ��DWORD ����
	SPI_Send(((unsigned int)(p>>16))|DAT);
	SPI_Send(((unsigned int)(p>>8))|DAT);
	SPI_Send(((unsigned int)p)|DAT);
	SPI_SS_SET();												//��ɲ�����SS�ߵ�ƽ
}


//========================================================================
// ����: void SetBackLight(unsigned char Deg)
// ����: ���ñ������ȵȼ�
//========================================================================
void SetBackLight(unsigned char brightness) 
{
	//��ʾASCII�ַ�
	SPI_SS_CLR();													//SS�õ͵�ƽ	
	SPI_Send(SET_LCD_BRIGHTNESS&CMD);					//����ָ��SET_LCD_BRIGHTNESS
	SPI_Send(brightness|DAT);												//�������� 0���ر� 1~9 ���ȵ���
	SPI_SS_SET();													//��ɲ�����SS�ߵ�ƽ
}


