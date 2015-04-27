#ifndef _GLOBALVAR_H_
#define _GLOBALVAR_H_

///////////////
#define MAIN_KEY      0x0F//key�źŵ�
#define MAIN_CODE_OUTTIME 0x0E//���볬ʱ
#define MAIN_PAGE_OUTTIME 0x0D//����ҳ�泬ʱ

#define USART3_MAX_RECV_LEN		1024		//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		1024		//����ͻ����ֽ���
#define USART3_RX_EN			1			//0,������;1,����

////////PCA9555��ַ����
#define	PCA9555_01W	0x40// 0100 0000
#define	PCA9555_02W	0x42// 0100 0010
#define	PCA9555_03W	0x44// 0100 0100
#define	PCA9555_01R	0x41// 0100 0001
#define	PCA9555_02R	0x43// 0100 0011
#define	PCA9555_03R	0x45// 0100 0101
///////////////////


#define GPRS_SW_ON()   GPIO_SetBits(GPIOA, GPIO_Pin_8 );  	       //GPRSģ�鿪
#define GPRS_SW_OFF()   GPIO_ResetBits(GPIOA, GPIO_Pin_8 );  	       //GPRSģ����

#define QD_SW_ON()   GPIO_SetBits(GPIOC, GPIO_Pin_7 );  	       //QD_SWģ�鿪
#define QD_SW_OFF()   GPIO_ResetBits(GPIOC, GPIO_Pin_7 );  	       //QD_SWģ���

#define QDDY_SW_OFF()   GPIO_SetBits(GPIOC, GPIO_Pin_6 );  	       //QDDY_SWģ�鿪
#define QDDY_SW_ON()   GPIO_ResetBits(GPIOC, GPIO_Pin_6 );  	       //QDDY_SWģ���

#define SK_SW_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_2 );  	       //SKģ�鿪
#define SK_SW_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_2 );  	       //SKģ���

#define SMM_SW_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_3 );  	       //QDDY_SWģ�鿪
#define SMM_SW_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_3 );  	       //QDDY_SWģ���

#define SPEAKER_ON()   GPIO_SetBits(GPIOD, GPIO_Pin_13 );  	       //SPEAKER��
#define SPEAKER_OFF()   GPIO_ResetBits(GPIOD, GPIO_Pin_13 );  	       //SPEAKER��

/////����
#define KB_H1_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_7 );  	       //KB_H1��
#define KB_H1_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_7 );  	       //KB_H1��
#define KB_H2_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_8 );  	       //KB_H2��
#define KB_H2_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_8 );  	       //KB_H2��
#define KB_H3_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_9 );  	       //KB_H3��
#define KB_H3_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_9 );  	       //KB_H3��
#define KB_H4_ON()   GPIO_SetBits(GPIOE, GPIO_Pin_10 );  	       //KB_H4��
#define KB_H4_OFF()   GPIO_ResetBits(GPIOE, GPIO_Pin_10 );  	       //KB_H4��
#define KB_V1_read GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)
#define KB_V2_read GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)
#define KB_V3_read GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)
#define KB_V4_read GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)
#define KB_A1_ON()   GPIO_SetBits(GPIOD, GPIO_Pin_8 );  	   
#define KB_A1_OFF()   GPIO_ResetBits(GPIOD, GPIO_Pin_8 );  	   
#define KB_A2_ON()   GPIO_SetBits(GPIOD, GPIO_Pin_9 );  	   
#define KB_A2_OFF()   GPIO_ResetBits(GPIOD, GPIO_Pin_9 );  	   
#define KB_A3_ON()   GPIO_SetBits(GPIOD, GPIO_Pin_10 );  	   
#define KB_A3_OFF()   GPIO_ResetBits(GPIOD, GPIO_Pin_10 );  	   
#define KB_A4_ON()   GPIO_SetBits(GPIOD, GPIO_Pin_11 );  	   
#define KB_A4_OFF()   GPIO_ResetBits(GPIOD, GPIO_Pin_11 );  	   





#ifdef GLOBALS 
#define EXT
#else
#define EXT extern 
#endif


EXT volatile unsigned short Tx_Size,Rst_Buf, Rst1_Buf;
EXT unsigned char rx_buf[1500],rx_buf_t[1500],tx_buf[1500];                 
EXT unsigned char rx1_buf[1500],rx1_buf_t[1500];                    
EXT unsigned short RxCount,RxCount1;                   

EXT unsigned char TxBuffer1[400]; 
 
EXT unsigned char TxBuffer2[]; 
EXT unsigned char RxBuffer1[400];
EXT unsigned char RxBuffer2[];
EXT unsigned char TxCounter1;
EXT unsigned int TxCounter2;
EXT volatile unsigned int RxCounter1; 
EXT volatile unsigned int RxCounter2;

EXT volatile unsigned char rec_f,tx_flag;
EXT volatile unsigned long Rec_Len;	
EXT volatile unsigned  int  milsec1,milsec2,milsec3;


#endif /* _GLOBALVAR_H_ */

