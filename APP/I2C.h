/*
	I2C.h
	��׼80C51��Ƭ��ģ��I2C���ߵ���������ͷ�ļ�
	Copyright (c) 2005��������������Ƭ����չ���޹�˾
	All rights reserved.
	���������ѧϰ�ο������ṩ�κοɿ��Է���ĵ���������������ҵĿ��
*/

#ifndef _I2C_H_
#define _I2C_H_

#include <reg52.h>

#define I2C_SCL_ON GPIO_SetBits(GPIOB, GPIO_Pin_8);
#define I2C_SCL_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_8);

#define I2C_SDA_ON GPIO_SetBits(GPIOB, GPIO_Pin_9);
#define I2C_SDA_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_9);

//ģ��I2C���ߵ����Ŷ���
sbit I2C_SCL = P3^6;
sbit I2C_SDA = P3^7;

//����I2C����ʱ�ӵ���ʱֵ��Ҫ����ʵ������޸ģ�ȡֵ1��255
//SCL�ź�����ԼΪ(I2C_DELAY_VALUE*4+15)����������
#define I2C_DELAY_VALUE		12

//����I2C����ֹͣ������һ�ο�ʼ֮ǰ�ĵȴ�ʱ�䣬ȡֵ1��65535
//�ȴ�ʱ��ԼΪ(I2C_STOP_WAIT_VALUE*8)����������
//���ڶ�������ȡֵΪ1���ɣ�������ĳЩ������˵���ϳ�����ʱ�Ǳ����
#define I2C_STOP_WAIT_VALUE	120

//I2C���߳�ʼ����ʹ���ߴ��ڿ���״̬
void I2C_Init();

//I2C�����ۺϷ��ͺ�������ӻ����Ͷ���ֽڵ�����
bit I2C_Puts
(
	unsigned char SlaveAddr,
	unsigned int SubAddr,
	unsigned char SubMod,
	char *dat,
	unsigned int Size
);

//I2C�����ۺϽ��պ������Ӵӻ����ն���ֽڵ�����
bit I2C_Gets
(
	unsigned char SlaveAddr,
	unsigned int SubAddr,
	unsigned char SubMod,
	char *dat,
	unsigned int Size
);

#endif	//_I2C_H_
