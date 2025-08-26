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

#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t 	ip_addr;		//设备地址
	uint8_t 	code;				//功能码
	uint16_t 	reg_addr;		//寄存器地址
	uint16_t 	reg_cnt;		//寄存器数量
	uint16_t 	crc_16;			//CRC16校验
}HostReadData_t;
#pragma pack()

#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t 	ip_addr;		//设备地址
	uint8_t 	code;				//功能码
	uint8_t 	byte_cnt;		//返回字节数
	uint16_t 	data[8];		//数据区
	uint16_t 	crc_16;			//CRC16校验
}SlaveReadData_t;
#pragma pack()


#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t 	ip_addr;		//设备地址
	uint8_t 	code;				//功能码
	uint16_t 	reg_addr;		//寄存器地址
	uint16_t 	data;				//数据区
	uint16_t 	crc_16;			//CRC16校验
}HostWriteData_t;
#pragma pack()

#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t 	ip_addr;		//设备地址
	uint8_t 	code;				//功能码
	uint16_t 	reg_addr;		//寄存器地址
	uint16_t 	data;				//数据区
	uint16_t 	crc_16;			//CRC16校验
}SlaveWriteData_t;
#pragma pack()

#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t slave_addr;	//从机地址
	uint8_t code;				//功能码
	uint8_t frame_len;	//帧长度
	uint8_t status;			//状态
	uint8_t recv_addr;	//接收地址
	uint8_t data[7];			//数据区
	uint8_t crc_8;			//校验和 = ( 地 址 + 功 能 码 + 帧 长 度 + 状 态 + 预 留 + 数据)&0X0F
}SendData_t;
#pragma pack()

#pragma pack(1)		 // 让每个元素直接强制对齐，不要有任何空白占位
typedef struct {
	uint8_t ip_addr;				//设备地址
	uint8_t code;				//功能码
	uint8_t frame_len;	//帧长度
	uint8_t status;			//状态
	uint8_t data[16];		//数据区
	uint8_t crc_8;			//校验和 = ( 地 址 + 功 能 码 + 帧 长 度 + 状 态 + 预 留 + 数据)&0X0F
}RecvData_t;
#pragma pack()

// 遥控功能数据
struct RemoteData_t
{
  uint8_t 	data;						// 遥控数据
};

// 定位功能数据
struct LocationData_t
{
  uint16_t 	distance;				// 当前定位距离
	uint16_t 	time;						// 飞行时间
	uint16_t	temperature;		// 温度
};

// 纠偏功能数据
struct YawCorrectionData_t
{
  uint16_t 	distance_H_to_B;// 主模组到基站的距离
	uint16_t 	distance_S_to_B;// 副模组到基站的距离
	uint16_t 	yaw_angle;			// 偏转角度
	uint16_t 	temperature;		// 温度
};

// 多字节功能数据
struct MultiByteData_t
{
  uint8_t data[7];					// 多字节数据
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