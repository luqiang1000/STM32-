#include "sim900a.h"
#include "../delay/delay.h"
#include "globals.h"

extern u16 USART3_RX_STA;							//��������״̬


//SIM900A����

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}
//unicode gbk ת������
//src:�����ַ���
//dst:���(uni2gbkʱΪgbk����,gbk2uniʱ,Ϊunicode�ַ���)
//mode:0,unicode��gbkת��;
//     1,gbk��unicodeת��; 
/*void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode)
{
	u16 temp; 
	u8 buf[2];
	if(mode)//gbk 2 unicode
	{
		while(*src!=0)
		{
			if(*src<0X81)	//�Ǻ���
			{
				temp=(u16)ff_convert((WCHAR)*src,1);
				src++;
			}else 			//����,ռ2���ֽ�
			{
				buf[1]=*src++;
				buf[0]=*src++; 
				temp=(u16)ff_convert((WCHAR)*(u16*)buf,1); 
			}
			*dst++=sim900a_hex2chr((temp>>12)&0X0F);
			*dst++=sim900a_hex2chr((temp>>8)&0X0F);
			*dst++=sim900a_hex2chr((temp>>4)&0X0F);
			*dst++=sim900a_hex2chr(temp&0X0F);
		}
	}else	//unicode 2 gbk
	{ 
		while(*src!=0)
		{
			buf[1]=sim900a_chr2hex(*src++)*16;
			buf[1]+=sim900a_chr2hex(*src++);
			buf[0]=sim900a_chr2hex(*src++)*16;
			buf[0]+=sim900a_chr2hex(*src++);
 			temp=(u16)ff_convert((WCHAR)*(u16*)buf,0);
			if(temp<0X80){*dst=temp;dst++;}
			else {*(u16*)dst=swap16(temp);dst+=2;}
		} 
	}
	*dst=0;//��ӽ�����
} 
*/
//�յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART2_RX_STA;
//		1,����USART2_RX_STA��
void sim_at_response(u8 mode)
{
	if(USART3_RX_STA & 0x8000)		//���յ�һ������
	{
		USART2_RX_BUF[USART3_RX_STA & 0x7FFF] = 0; //��ӽ�����
		printf("%s", USART2_RX_BUF);  //���͵�����
		if(mode) USART3_RX_STA = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A �������(���Ų��ԡ����Ų��ԡ�GPRS����)���ô���

//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx = 0;
	if(USART3_RX_STA & 0x8000)		//���յ�һ������
	{
		USART2_RX_BUF[USART3_RX_STA & 0x7FFF] = 0;  //��ӽ�����
		printf("%s", USART2_RX_BUF);  //���͵�����
		strx = strstr((const char*)USART2_RX_BUF,(const char*)str);
	}
	return (u8*)strx;
}

//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res = 0;
	USART3_RX_STA = 0;
	if((u32)cmd <= 0xFF)
	{
		while(DMA1_Channel2->CNDTR != 0);	//�ȴ�ͨ��2�������
		USART3->DR = (u32)cmd;
	}
	else
		u2_printf("%s\r\n",cmd);	//��������
	if(ack && waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)		//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART3_RX_STA & 0x8000) //���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack)) break;	//�õ���Ч����
				USART3_RX_STA = 0;
			}
		}
		if(waittime == 0) res = 1;
	}
	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//���Ų��Բ��ִ���

//sim900a���Ų���
//���ڲ���绰�ͽ����绰
//����ֵ:0,����
//    ����,�������
u8 sim900a_call_test(void)
{
	u8 phonenumber[20];
	u8 *temp;
	u16 len,i;
	u8 key;
	u8 *p,*p1,*p2;
	u8 oldmode = 0;
	u8 cmode = 0; //ģʽ
				//0���ȴ����ţ�1��������
				//2��ͨ���У�3�����յ�����
	if(sim900a_send_cmd("AT+CLIP=1","OK",200)) return 1; //����������ʾ 
	if(sim900a_send_cmd("AT+COLP=1","OK",200)) return 2;  //���ñ��к�����ʾ
	printf("ATATK-SIM900A\r\n");

	while(1)
	{
		printf("��KEYLEFT����/��ͨ�绰\r\n");
		printf("��KEYRIGHT�Ҷϵ绰\r\n");
		delay_ms(10);
		if(USART3_RX_STA & 0x8000)		//���յ�����
		{
			sim_at_response(0);
			if(cmode==1 || cmode==2)
			{
				if(cmode==1)
					if(sim900a_check_cmd("+COLP:")) cmode = 2; //���ųɹ�
				if(sim900a_check_cmd("NO CARRIER")) cmode = 0; //����ʧ��
				if(sim900a_check_cmd("NO ANSWER")) cmode = 0;  //����ʧ��
				if(sim900a_check_cmd("ERROR")) cmode = 0;	   //����ʧ��
			} 
			if(sim900a_check_cmd("+CLIP:"))		//���յ�����
			{
				LED0 = 0;
				cmode = 3;
				p = sim900a_check_cmd("+CLIP:");
				p += 8;
				p2 = (u8*)strstr((const char*)p,"\"");
				p2[0]=0;  //��ӽ�����
				strcpy((char*)p1,(char*)p);
			}
			USART3_RX_STA = 0;
		}
	
		key = KEY_Scan(0);
		if(key == 1)
		{
			printf("�벦��\r\n");
			LED1 = 0;
			if(cmode == 0)	 //����ģʽ
			{
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)phonenumber,0,sizeof((char*)phonenumber));
						len = USART_RX_STA & 0x3FFF; //�õ��˴ν��յ����ݵĳ���
						for(i=0;i<len;i++)
						{
							phonenumber[i] = USART_RX_BUF[i];
						}  
						
						cmode = 1;		//������
						USART_RX_STA = 0;
						delay_ms(20);  //һ��Ҫ����ʱ�������ܽ��еڶ��ν���
						printf("%s\r\n",phonenumber);
					}
				//u2_printf("ATD%s;\r\n",phonenumber);  //����	
			
				sprintf((char*)temp,"ATD%s;\r\n",phonenumber);   //ʹ��send_cmd ���ų���
			   	sim900a_send_cmd("ATE1;","OK",200);
				sim900a_send_cmd(temp,"+COLP:",200);  
			  key = 0;
			  while(1)  //�ȴ��Ҷ�
				{
					key = KEY_Scan(0);
					if(key == 3)		 //�Ҷϵ绰
					{
						sim900a_send_cmd("ATH",0,0);
						LED1 = 1;
						break;
					}
				}
			}
				
		}
		if(cmode == 3)
		{
		//	key = KEY_Scan(0);
			if(key == 1)	  //��ͨ�绰
			{
				LED1 = 0;
				  sim900a_send_cmd("ATA","OK",200);		//����Ӧ��ָ��
				  if(USART_RX_STA & 0x8000)
				{
					printf("%s",(char*)USART_RX_BUF);
					USART_RX_STA = 0;
					cmode=2;
				}
			}
		
			if(key == 3)	//�Ҷϵ绰
			{
				sim900a_send_cmd("ATH",0,0);
				LED0 = 1;
				LED1 = 1;
			}
		}
	}
}

/////////////////////////////////////////GPRS////////////////////////////////////////////////////////// 
const u8 *modetbl[2] = {"TCP","UDP"};  	//ģʽ����
//tcp/udp����
//����������,��ά������
//mode:0:TCP����;1,UDP����)
//ipaddr:ip��ַ
//port:�˿� 

void sim900a_tcpudp_test(u8 mode,u8 *ipaddr,u8 *port)
{
	u8 p[100],p1[100],*p2,*p3;
	u8 key;
	u16 timex = 0;
	u8 count = 0;
	const u8* cnttbl[3] = {"��������","���ӳɹ�","���ӹر�"};
	u8 connectsta = 0;  	//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������
	u8 oldsta=0XFF;
	printf("-----------------GPRS ����------------------\r\n");
	if(mode) printf("---ATK-SIM900A UDP���Ӳ���-----\r\n");
	else printf("---ATK-SIM900A TCP���Ӳ���-----\r\n");
	printf("KEY_UP:�˳�����  KEY_DONW:��������\r\n");
	sprintf((char*)p,"IP��ַ:%s,�˿�:%s",ipaddr,port);		
	USART3_RX_STA = 0;
	sprintf((char*)p,"AT+CIPSTART=\"%s\",\"%s\",\"%s\"",modetbl[mode],ipaddr,port);
	if(sim900a_send_cmd(p,"OK",50)) return;
	while(1)
	{
		key = KEY_Scan(0);
		if(key == KEY_UP)  //�˳�����
		{
			sim900a_send_cmd("AT+CIPCLOSE","CLOSE OK",500);
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);
			break;
		}
		else if(key == KEY_RIGHT &&(hbeaterrcnt==0))
		{
			printf("���ݷ�����...");
			if(sim900a_send_cmd("AT+CIPSEND",">",500) == 0)
			{
				printf("CIPSEND DATA:%s\r\n",p1);
				u2_printf("%s\r\n",p1);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0x1A,"SEND OK",1000) == 0)  //��ȴ�10s
					printf("���ݷ��ͳɹ�\r\n");
				else
					printf("���ݷ���ʧ��\r\n");
				delay_ms(1000);
			}
		else
			sim900a_send_cmd((u8*)0x1B,0,0);  //ESCȡ������
		oldsta = 0xFF;
	}
	if((timex%20) == 0)
	{
		LED0 = !LED0;
		count++;
		if(connectsta==2 || hbeaterrcnt>8)  //�����жϣ���������8������û����ȷ���ͣ�����������
		{
			sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500); 	//�ر�����
			sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);	 	//�ر��ƶ����� 
			sim900a_send_cmd(p,"OK",500);						//������������
			connectsta = 0;
			hbeaterrcnt = 0;
		}
		sprintf((char*)p1,"ATK-SIM900A %s���� %d  ",modetbl[mode],count);
		printf("\r\n%s\r\n",(char*)p1);
	}
	if(connectsta==0 && (timex%200)==0)   //���ӻ�û������ʱ��,ÿ2���ѯһ��CIPSTATUS.
	{
		sim900a_send_cmd("AT+CIPSTATUS","OK",500);	//��ѯ����״̬
		if(strstr((const char*)USART2_RX_BUF,"CLOSED"))
			connectsta = 2;
		if(strstr((const char*)USART2_RX_BUF,"CONNECT OK"))
			connectsta = 1;
	}
	if(connectsta==1 && timex>=600)  //����������ʱ��,ÿ6�뷢��һ������
	{
		timex = 0;
		if(sim900a_send_cmd("AT+CIPSEND",">",200) == 0)  //��������
		{
			sim900a_send_cmd((u8*)0x00,0,0);	//��������0x00
			delay_ms(20);
			sim900a_send_cmd((u8*)0x1A,0,0);	//CTRL+Z�������������ݣ�����һ�δ���
		}
		else
			sim900a_send_cmd((u8*)0x1B,0,0);	//ESC��ȡ������
		hbeaterrcnt++;
		printf("hbeaterrcn:%d\r\n",hbeaterrcnt); //������
	}
	delay_ms(10);
	if(USART3_RX_STA & 0x8000) 		//���յ�һ������
	{
		USART2_RX_BUF[USART3_RX_STA&0x7FFF] = 0;	//��ӽ�����
		if(hbeaterrcnt)
		{
			if(strstr((const char*)USART2_RX_BUF,"SEND OK"))
				hbeaterrcnt = 0;  //��������
		}
		p2 = (u8*)strstr((const char*)USART2_RX_BUF,"+IPD");
		printf("�յ�������: %s\r\n",p2);

		USART3_RX_STA = 0;
	}
	if(oldsta != connectsta)
	{
		oldsta = connectsta;
		printf("����״̬:%s\r\n",cnttbl[connectsta]);
	}
	timex++;
	}
}



//sim900a GPRS����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 sim900a_gprs_test(void)
{
	u8 port[5];//�˿ڹ̶�Ϊ8086,����ĵ���8086�˿ڱ���������ռ�õ�ʱ��,���޸�Ϊ�������ж˿�			
	u8 mode=0;				//0,TCP����;1,UDP����
	u8 key;
	u16 len;
	u16 i;
	u8 timex=0; 
	u8 ipbuf[16]; 		//IP����
	u8 iplen=0;			//IP���� 
	printf("���ӷ�ʽ:%s,�˿�:%s\r\n",(char*)modetbl[mode],(char*)port);
   	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",100);	//�ر�����
	sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);		//�ر��ƶ����� 
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",1000))return 1;				//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 2;//����PDP������,��������Э��,��������Ϣ
	if(sim900a_send_cmd("AT+CGATT=1","OK",500))return 3;					//����GPRSҵ��
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",500))return 4;	 	//����ΪGPRS����ģʽ
	if(sim900a_send_cmd("AT+CIPHEAD=1","OK",500))return 5;	 				//���ý���������ʾIPͷ(�����ж�������Դ)
	ipbuf[0]=0;
	printf("��KEY_UP�����л�����ģʽ\r\n"); 
	while(1)
	{
		
		key = KEY_Scan(0);
		if(key == KEY_UP)
		{
			mode = !mode;
			printf("�л���ģʽ:%s\r\n",(char*)modetbl[mode]);
		}
		printf("���������ӷ�����IP\r\n");	
		while(1)
		{  
		
			if(USART_RX_STA & 0x8000)
			{
				memset((char*)ipbuf,0,sizeof((char*)ipbuf)); //����ַ���
				len=USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
				for(i=0;i<len;i++)	   //��BUF�е����ݿ����ַ����� ����strcpy�����
				{
					ipbuf[i] = USART_RX_BUF[i];
				}
				USART_RX_STA = 0;
				printf("��������ַ:%s\r\n",(char*)ipbuf);
				printf("\r\n");
				delay_ms(20);  //һ��Ҫ����ʱ�������ܽ��еڶ��ν���
				break;
			}
		}
		printf("���������ӷ������˿ں�\r\n");	
		while(1)
		{  
		
			if(USART_RX_STA & 0x8000)
			{
				memset((char*)port,0,sizeof((char*)port)); //����ַ���
				len=USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
				for(i=0;i<len;i++)	   //��BUF�е����ݿ����ַ�����
				{
					port[i] = USART_RX_BUF[i];
				}
				USART_RX_STA = 0;
				printf("�������˿ں�:%s\r\n",(char*)port);
				printf("\r\n");
				delay_ms(20);
				break;
			}	 
		}	 
		sim900a_tcpudp_test(mode,ipbuf,(u8*)port);	
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);//���GSMģ�鷢�͹���������,��ʱ�ϴ�������
	}
	return 0;
}			

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//���Ų��Բ��ִ���

//SIM900A�����Ų���
void sim900a_sms_read_test(void)
{
	u8 p[200],*p1,*p2;
	u8 timex = 0;
	u16 len,i;
	u8 msgindex[3];
	u8 msgchoose[3];
	u8 msgmaxnum = 0;     //�����������
	u8 key = 0;
	u8 smsreadsta = 0;    //�Ƿ��ڶ�����ʾ״̬
	memset((char*)msgindex,0,3);  //�������
	if(sim900a_send_cmd("AT+CNMI=2,1","OK",200)==0)  //������Ϣ����
		printf("����Ϣ�������óɹ�\r\n");
	else
		printf("����Ϣ��������ʧ��\r\n");	
	printf("---------�����Ų���-----------\r\n");
	printf("���󰴼�������\r\n");
	while(1)
	{
		
		if(sim900a_check_cmd("+CMTI:"))
		{	 
			printf("�յ��¶���\r\n");
			p1 = (u8*)strstr((const char*)(USART2_RX_BUF),"\",");
			strcpy((char*)msgindex,(char*)p1);
			USART3_RX_STA = 0;
		} 
		key = KEY_Scan(0);
		if(key)
		{
			if(smsreadsta)
			{
				smsreadsta = 0;
			}
			if(key == KEY_LEFT)   //���󰴼���ȡ����
			{
				printf("������Ҫ�Ķ��Ķ��ţ�С��%s��)\r\n",msgindex);	
				while(1)
				{  
				
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)msgchoose,0,3);
						len=USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
						for(i=0;i<len;i++)	   //��BUF�е����ݿ����ַ�����
						{
							msgchoose[i] = USART_RX_BUF[i];
						}
						printf("msgchoose=%s",(char*)msgchoose);
						USART_RX_STA = 0;
						delay_ms(20);
						break;
					}	 
				}	 
				sprintf((char*)p,"AT+CMGR=%s",msgchoose);
				if(sim900a_send_cmd(p,"+CMGR:",200)==0)    //��ָ���ȡ����
				{
					len=USART3_RX_STA&0x7FFF;//�õ��˴ν��յ������ݳ��� 
					if(strstr((const char*)(USART2_RX_BUF),"UNREAD"))
						printf("δ�����ţ�\r\n");
					printf("unicode�ַ���%s\r\n",USART2_RX_BUF);
					p1 = (u8*)strstr((const char*)(USART2_RX_BUF),"\",");	 //��ȡ�����˺���
					p2 = (u8*)strstr((const char*)(p1+2),"\",");
					p2[0] = 0;  //�ӽ�����
					printf("����:%s\r\n",p1+3);
						p1 = (u8*)strstr((const char*)(p1+1),"/");   //���ֱ�ﲻ��ȡp1���ܵ�p1
					p1 = (u8*)strstr((const char*)(p2+1),"/");//Ѱ�ҽ���ʱ���ַ�λ��
					p2 = (u8*)strstr((const char*)p1,"+");
					p2[0] = 0;   //��ӽ�����
					printf("����ʱ��:%s\r\n",p1-2);
				/*	p1 = (u8*)strstr((const char*)p2,"\r");
					sim900a_unigbk_exchange(p1+2,p,0);			//��unicode�ַ�ת��Ϊgbk��
					printf("��������:%s\r\n",p);*/			
					smsreadsta = 1;
				}
				else
				{
				 	printf("�޶������ݣ�����!\r\n");
					delay_ms(100);
				}
				USART3_RX_STA = 0;
			}
		}
			if(timex == 0)
			{
				if(sim900a_send_cmd("AT+CPMS?","+CPMS:",200)==0)  //��ѯ��ѡ��Ϣ�洢��
				{
					p1 = (u8*)strstr((const char*)(USART2_RX_BUF),","); 
					p2 = (u8*)strstr((const char*)(p1+1),",");
					p2[0] = 0;
					sprintf((char*)msgindex,"%s",p1+1);
					printf("���Ķ�������%s��\r\n",msgindex);
					p2[0] = '/'; 
					if(p2[3]==',')	//С��64K SIM�������洢��ʮ������
					{
						msgmaxnum = (p2[1]-'0')*10+p2[2]-'0';		//��ȡ���洢��������
						p2[3] = 0;
					}else //�����64K SIM�������ܴ洢100�����ϵ���Ϣ
					{
						msgmaxnum = (p2[1]-'0')*100+(p2[2]-'0')*10+p2[3]-'0';//��ȡ���洢��������
						p2[4] = 0;
					}
					sprintf((char*)p,"%s",p1+1);
					printf("ʹ�ÿռ�%s",p);
					USART3_RX_STA=0;
				}
			}	
			if((timex%20)==0)LED0=!LED0;//200ms��˸ 
			timex++;
			delay_ms(10);
			if(USART3_RX_STA&0X8000)sim_at_response(1);//����GSMģ����յ������� 
		}
}
	

//���Զ��ŷ�������(70����[UCS2��ʱ��,1���ַ�/���ֶ���1����])
const u8* sim900a_test_msg = "ATK_SIM900A TEST messeage";
//SIM900A�����Ų���

void sim900a_sms_send_test(void)
{
	u8 p[100],p1[300],p2[100];
			//p:���ڴ�ŵ绰�����unicode�ַ���
			//p1:���ڴ�Ŷ��ŵ�unicode�ַ���
			//p2: ��ţ�AT+CMGS=p1 
	u8 phonebuf[20];	  //���뻺��
	u8 phonenumlen = 0;	  //���볤�ȣ����15����
	u8 timex = 0;
	u8 key = 0;
	u16 len,i;
	u8 smssendsta = 0; 	  //���ŷ���״̬,0,�ȴ�����;1,����ʧ��;2,���ͳɹ�
	printf("-----�����Ų���------\r\n");
	printf("��������: %s",sim900a_test_msg);
	printf("���󰴼����뷢����ģʽ,���Ҽ��˳�����\r\n");
	while(1)
	{
		key = KEY_Scan(0);
		if(key)
		{
			if(smssendsta)
			{
				smssendsta = 0;
				printf("�ȴ�����\r\n");
			}
			if(key ==KEY_LEFT)
			{
				printf("���������\r\n");
				while(1)	//��ȡ����
				{  
			
					if(USART_RX_STA & 0x8000)
					{
						memset((char*)phonebuf,0,sizeof((char*)phonebuf)); //����ַ���
						len=USART_RX_STA&0x3FFF;//�õ��˴ν��յ������ݳ���
						for(i=0;i<len;i++)	   //��BUF�е����ݿ����ַ����� ����strcpy�����
						{
							phonebuf[i] = USART_RX_BUF[i];
						}
						USART_RX_STA = 0;
						delay_ms(20);  //һ��Ҫ����ʱ�������ܽ��еڶ��ν���
						break;
					}
				}
			   	printf("���󰴼�ִ�з��ͣ����Ұ�����������\r\n");
				while(1)
				{
					key = KEY_Scan(0);
					if(key == KEY_LEFT)
					{
						printf("���ڷ���...\r\n");
						smssendsta = 1;
						sprintf((char*)p2,"AT+CMGS=\"%s\"",phonebuf);
						if(sim900a_send_cmd(p2,">",200) == 0)  //���뷢�ͳɹ���
						{
							u2_printf("%s",sim900a_test_msg);
							if(sim900a_send_cmd((u8*)0x1A,"+CMGS:",1000)==0)
								smssendsta = 2;
						}
						if(smssendsta ==1)
							printf("����ʧ�ܣ�\r\n");
						else
							printf("���ͳɹ���\r\n");
						USART3_RX_STA = 0;
					}
					if(key == KEY_RIGHT)
						break;
				}
			}
		}
		if((timex%20)==0)LED0=!LED0;//200ms��˸ 
		timex++;
		delay_ms(10);
		if(USART3_RX_STA&0X8000)sim_at_response(1);//����GSMģ����յ�������
		if(key == KEY_RIGHT)
			break;  //�˳�����
	}
}

//sim900a���Ų���
//���ڶ����Ż��߷�����
//����ֵ:0,����
//    ����,�������
u8 sim900a_sms_test(void)
{
	u8 key;
	u8 timex=0;
	//if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 1;			//�����ı�ģʽ 
//	if(sim900a_send_cmd("AT+CSCS=\"UCS2\"","OK",200))return 2;	//����TE�ַ���ΪUCS2 
//	if(sim900a_send_cmd("AT+CSMP=17,0,2,25","OK",200))return 3;	//���ö���Ϣ�ı�ģʽ���� 
   	//��Ӣ�Ķ��ŷ���
   	if(sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200))return 2;	//����TE�ַ���ΪUCS2 
	if(sim900a_send_cmd("AT+CMGF=1","OK",200))return 1;			//�����ı�ģʽ
	
	printf("���Ҽ������Ų��ԣ�����������Ų��ԣ����ϼ��˳�����\r\n");
	while(1)
	{
		key=KEY_Scan(0);
		if(key==KEY_RIGHT)
		{ 
			sim900a_sms_read_test();
			timex=0;
		}else if(key==KEY_DOWN)
		{ 
			sim900a_sms_send_test();
			timex=0;			
		}else if(key==KEY_UP)break;
		timex++;
		if(timex==20)
		{
			timex=0;
			LED0=!LED0;
		}
		delay_ms(10);
		sim_at_response(1);										//���GSMģ�鷢�͹���������,��ʱ�ϴ�������
	} 
	//sim900a_send_cmd("AT+CSCS=\"GSM\"","OK",200);				//����Ĭ�ϵ�GSM 7λȱʡ�ַ���
	return 0;
}

//GSM��Ϣ��ʾ(�ź�����,��ص���,����ʱ��)
//����ֵ:0,����
//    ����,�������
u8 sim900a_gsminfo_show(u16 x,u16 y)
{
	u8 p[50],*p1,*p2;
	u8 res=0;	
	USART3_RX_STA=0;
	if(sim900a_send_cmd("AT+CPIN?","OK",200))res|=1<<0;	//��ѯSIM���Ƿ���λ 
	USART3_RX_STA=0;  
	if(sim900a_send_cmd("AT+COPS?","OK",200)==0)		//��ѯ��Ӫ������
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\""); 
		if(p1)//����Ч����
		{
			p2=(u8*)strstr((const char*)(p1+1),"\"");
			p2[0]=0;//���������			
			sprintf((char*)p,"��Ӫ��:%s",p1+1);
			printf("%s",p);
		} 
		USART3_RX_STA=0;		
	}else res|=1<<1;
	if(sim900a_send_cmd("AT+CSQ","+CSQ:",200)==0)		//��ѯ�ź�����
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),":");
		p2=(u8*)strstr((const char*)(p1),",");
		p2[0]=0;//���������
		sprintf((char*)p,"�ź�����:%s",p1+2);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<2;
	if(sim900a_send_cmd("AT+CBC","+CBC:",200)==0)		//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),",");
		p2=(u8*)strstr((const char*)(p1+1),",");
		p2[0]=0;p2[5]=0;//���������
		sprintf((char*)p,"��ص���:%s%%  %smV",p1+1,p2+1);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<3; 
	if(sim900a_send_cmd("AT+CCLK?","+CCLK:",200)==0)		//��ѯ��ص���
	{ 
		p1=(u8*)strstr((const char*)(USART2_RX_BUF),"\"");
		p2=(u8*)strstr((const char*)(p1+1),":");
		p2[3]=0;//���������
		sprintf((char*)p,"����ʱ��:%s",p1+1);
		printf("%s",p);
		USART3_RX_STA=0;		
	}else res|=1<<4; 
	return res;
} 

//sim900a�����Գ���
void sim900a_test(void)
{
	u8 key=0; 
	u8 timex=0;
	u8 sim_ready=0;
	printf("%s\r\n","ATK-SIM900A ���Գ���"); 
	while(sim900a_send_cmd("AT","OK",100))//����Ƿ�Ӧ��ATָ�� 
	{
		printf("%s\r\n","δ��⵽ģ��!!!");
		delay_ms(800);
	
		printf("%s\r\n","��������ģ��...");
		delay_ms(400);  
	} 
	key+=sim900a_send_cmd("ATE0","OK",200);//������	
	while(1)
	{
		delay_ms(10); 
		sim_at_response(1);//���GSMģ�鷢�͹���������,��ʱ�ϴ�������
		printf("���Ҽ������Ų��ԣ����¼����Ų��ԣ����ϼ�GPRS����\r\n");
		if(sim_ready)//SIM������.
		{
			key=KEY_Scan(0); 
			if(key)
			{
				switch(key)
				{
					case KEY_RIGHT:
						sim900a_call_test();//���Ų���
						break;
					case KEY_DOWN:
						sim900a_sms_test();	//���Ų���
						break;
					case KEY_UP:
						sim900a_gprs_test();//GPRS����
						break;
				}
				timex=0;
			} 			
		}
		if(timex==0)		//2.5�����Ҹ���һ��
		{
			if(sim900a_gsminfo_show(40,225)==0)sim_ready=1;
			else sim_ready=0;
		}	
		if((timex%20)==0)LED0=!LED0;//200ms��˸ 
		timex++;	 
	} 	
}
		

