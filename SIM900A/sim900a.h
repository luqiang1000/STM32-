#include "includes.h"
#include "globals.h"
 #ifndef __SIM900A_H
#define __SIM900A_H

//SIM900A����

#define swap16(x)	(x&0xFF)<<8 | (0xFF00&x)>>8		//�ߵ��ֽڽ����궨��

void sim900a_test(void);		//�����Գ���
//void sim_sen_sms( u8 *phonenumber, u8 *msg);
void sim_at_response(u8 mode);
u8* sim900a_check_cmd(u8 *str);
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
//u8 sim900a_chr2hex(u8 chr);
//u8 sim900a_hex2chr(u8 hex);
//void sim900a_unigbk_exchange(u8 *src,u8 *dst,u8 mode);
//void sim900a_load_keyboard(u16 x,u16 y,u8 **kbtbl);
//void sim900a_key_keynumber(u16 x,u16 y);
//u8 sim900a_get_keynum(u16 x,u16 y);
u8 sim900a_call_test(void);		//���Ų���
//void sim900a_sms_read_test(void);		//�����Ų���
//void sim900a_sms_send_test(void);		//�����Ų���
//void sim900a_sms_ui(u16 x,u16 y);	  	//���Ų���UI
//u8 sim900a_sms_test(void);				//���Ų���
//void sim900a_mtest_ui(u16 x,u16 y);		//SIM900A������UI
u8 sim900a_gsminfo_show(u16 x,u16 y);	//��ʾGSMģ����Ϣ
void sim900a_test(void);				//SIM900A�����Ժ���
#endif
