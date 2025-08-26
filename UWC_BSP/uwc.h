#ifndef __UWC_H__
#define __UWC_H__

#include "main.h"
#include "usart.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define STATUS_HEADER 	0
#define STATUS_DATA_03	0x03
#define STATUS_DATA_06	0x06
#define STATUS_DATA			1

#define Device_Addr	 	0x01

#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t 	ip_addr;		//�豸��ַ
	uint8_t 	code;				//������
	uint16_t 	reg_addr;		//�Ĵ�����ַ
	uint16_t 	reg_cnt;		//�Ĵ�������
	uint16_t 	crc_16;			//CRC16У��
}HostReadData_t;
#pragma pack()

#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t 	ip_addr;		//�豸��ַ
	uint8_t 	code;				//������
	uint8_t 	byte_cnt;		//�����ֽ���
	uint16_t 	data[8];		//������
	uint16_t 	crc_16;			//CRC16У��
}SlaveReadData_t;
#pragma pack()


#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t 	ip_addr;		//�豸��ַ
	uint8_t 	code;				//������
	uint16_t 	reg_addr;		//�Ĵ�����ַ
	uint16_t 	data;				//������
	uint16_t 	crc_16;			//CRC16У��
}HostWriteData_t;
#pragma pack()

#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t 	ip_addr;		//�豸��ַ
	uint8_t 	code;				//������
	uint16_t 	reg_addr;		//�Ĵ�����ַ
	uint16_t 	data;				//������
	uint16_t 	crc_16;			//CRC16У��
}SlaveWriteData_t;
#pragma pack()

#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t slave_addr;	//�ӻ���ַ
	uint8_t code;				//������
	uint8_t frame_len;	//֡����
	uint8_t status;			//״̬
	uint8_t recv_addr;	//���յ�ַ
	uint8_t data[7];			//������
	uint8_t crc_8;			//У��� = ( �� ַ + �� �� �� + ֡ �� �� + ״ ̬ + Ԥ �� + ����)&0X0F
}SendData_t;
#pragma pack()

#pragma pack(1)		 // ��ÿ��Ԫ��ֱ��ǿ�ƶ��룬��Ҫ���κοհ�ռλ
typedef struct {
	uint8_t ip_addr;				//�豸��ַ
	uint8_t code;				//������
	uint8_t frame_len;	//֡����
	uint8_t status;			//״̬
	uint8_t data[16];		//������
	uint8_t crc_8;			//У��� = ( �� ַ + �� �� �� + ֡ �� �� + ״ ̬ + Ԥ �� + ����)&0X0F
}RecvData_t;
#pragma pack()

// ң�ع�������
struct RemoteData_t
{
  uint8_t 	data;						// ң������
};

// ��λ��������
struct LocationData_t
{
  uint16_t 	distance;				// ��ǰ��λ����
	uint16_t 	time;						// ����ʱ��
	uint16_t	temperature;		// �¶�
};

// ��ƫ��������
struct YawCorrectionData_t
{
  uint16_t 	distance_H_to_B;// ��ģ�鵽��վ�ľ���
	uint16_t 	distance_S_to_B;// ��ģ�鵽��վ�ľ���
	uint16_t 	yaw_angle;			// ƫת�Ƕ�
	uint16_t 	temperature;		// �¶�
};

// ���ֽڹ�������
struct MultiByteData_t
{
  uint8_t data[7];					// ���ֽ�����
};

typedef struct{
	struct RemoteData_t 	RemoteData;
	
	struct LocationData_t	LocationData;
	
	struct YawCorrectionData_t YawCorrectionData;
	
	struct MultiByteData_t MultiByteData;
}UWC_Data_t;

void UWC_Config();

void UWC_Test();

void uwc_start_receive_data();
bool isCode(uint8_t code);
void receive_callback();

void DataProcess_03();
void DataProcess_06();
void DataProcess();
void RecvData_Process();

void UWC_Send(uint8_t ip_addr, uint8_t code, uint8_t frame_len, uint8_t* data);
void UWC_Send_Read(uint8_t ip_addr, uint16_t reg_addr, uint16_t reg_cnt);
void UWC_Send_Write(uint8_t ip_addr, uint16_t reg_addr, uint16_t data);

#endif