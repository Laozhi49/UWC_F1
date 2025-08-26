#include "uwc.h"
#include <stdint.h>

HostReadData_t 		HostReadData;
SlaveReadData_t 	SlaveReadData;
HostWriteData_t		HostWriteData;
SlaveWriteData_t	SlaveWriteData;

SendData_t				SendData;
RecvData_t				RecvData;

UWC_Data_t				UWC_Data;

uint16_t 	crc16_value;
uint8_t 	rx_data;
uint8_t		rx_buff[64];
uint8_t		rx_index = 0;
uint8_t		start_index;
uint8_t		RX_STATUS;


uint8_t tx_data[7];
uint8_t tx_buff[13];

uint8_t send_flag = 0;
uint8_t txdata_len;

uint16_t Modbus_CRC16(uint8_t *buf, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < len; i++)
    {
        crc ^= buf[i];  // 先异或当前字节
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)   // 如果最低位为1
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc; // 返回值: 高字节在高位, 低字节在低位
}


void UWC_Config()
{
	RX_STATUS = STATUS_HEADER;
	uwc_start_receive_data();
	
	UWC_Send_Read(0x01,0x0201,0x0001);			//获取设备的波特率
	//UWC_Send_Write(0x01, 0x0200, 0x0005);
}


void UWC_Test()
{
	if(send_flag == 1)
	{
		memset(tx_data,0,sizeof(tx_data));
		
		tx_data[0] = 0x3F;
		txdata_len = 1;
		
		UWC_Send(0x01, 0x6A, txdata_len, tx_data);
		send_flag = 0;
	}
	else if(send_flag == 2) 
	{
		memset(tx_data,0,sizeof(tx_data));
		
		tx_data[0] = 0x01;
		txdata_len = 1;
		
		UWC_Send(0x01, 0x6C, txdata_len, tx_data);
		send_flag = 0;
	}
	else if(send_flag == 3) 
	{
		memset(tx_data,0,sizeof(tx_data));
		
		tx_data[0] = 0x02;
		tx_data[1] = 0x1E;
		txdata_len = 2;
		
		UWC_Send(0x01, 0x6D, txdata_len, tx_data);
		send_flag = 0;
	}
	else if(send_flag == 4) 
	{
		memset(tx_data,0,sizeof(tx_data));
		
		tx_data[0] = 0x01;
		tx_data[1] = 0x02;
		tx_data[2] = 0x03;
		tx_data[3] = 0x04;
		tx_data[4] = 0x05;
		
		txdata_len = 5;
		
		UWC_Send(0x01, 0x6E, txdata_len, tx_data);
		send_flag = 0;
	}
}
/**************************接收回调函数*******************************/

void uwc_start_receive_data()
{
	HAL_UART_Receive_IT(&huart1,(uint8_t*)&rx_data,1);
}

void receive_callback()
{
	rx_buff[rx_index++] = rx_data;
	switch(RX_STATUS)
	{
		case STATUS_HEADER:
			if(rx_index>=3 && rx_buff[rx_index-3]==Device_Addr && isCode(rx_buff[rx_index-2]))
			{
				if(rx_buff[rx_index-2] == STATUS_DATA_03 || rx_buff[rx_index-2] == STATUS_DATA_06)
					RX_STATUS = rx_buff[rx_index-2];
				else RX_STATUS = STATUS_DATA;
				start_index = rx_index-3;
			}
			break;
		case STATUS_DATA_03:
			DataProcess_03();
			break;
		case STATUS_DATA_06:
			DataProcess_06();
			break;
		case STATUS_DATA:
			DataProcess();
			break;
		default:break;
	}
	
	uwc_start_receive_data();
}

// 检查字节是否为功能码
bool isCode(uint8_t code)
{
	switch(code)
	{
		case 0x03:		//读数据回应
			return true;
			break;
		case 0x06:		//写数据回应
			return true;
			break;
		case 0x8A:		//发送遥控数据成功回应
			return true;
			break;
		case 0x8B:		//接收遥控数据回应
			return true;
			break;
		case 0x8C:		//接收定位数据回应
			return true;
			break;
		case 0x8D:		//接收纠偏数据回应
			return true;
			break;
		case 0x8E:		//接收多字节回应
			return true;
			break;
		default:
			return false;
			break;
	}
}

/*
数据区都是 高位在前低位在后
而校验和是 低位在前高位在后
*/

// 从机读回应数据处理
void DataProcess_03()
{
	SlaveReadData.byte_cnt 	= rx_buff[start_index+2];
	if(rx_index-start_index>=SlaveReadData.byte_cnt+5)
	{
		SlaveReadData.ip_addr 	= rx_buff[start_index];
		SlaveReadData.code 			= rx_buff[start_index+1];
		SlaveReadData.byte_cnt 	= rx_buff[start_index+2];
		for(uint8_t i=0;i<SlaveReadData.byte_cnt;i++)
		{
			SlaveReadData.data[i] = rx_buff[start_index+2+i+1];
		}
		SlaveReadData.crc_16		= rx_buff[rx_index-1]<<8|rx_buff[rx_index-2];
		
		RX_STATUS = STATUS_HEADER;
		rx_index = 0;
		memset(rx_buff,0,sizeof(rx_buff));
	}
}

// 从机写回应数据处理
void DataProcess_06()
{
	if(rx_index-start_index>=8)
	{
		SlaveWriteData.ip_addr 	= rx_buff[start_index];
		SlaveWriteData.code 		= rx_buff[start_index+1];
		SlaveWriteData.reg_addr = rx_buff[start_index+2]<<8|rx_buff[start_index+3];
		SlaveWriteData.data 		= rx_buff[start_index+4]<<8|rx_buff[start_index+5];
		SlaveWriteData.crc_16		= rx_buff[start_index+7]<<8|rx_buff[start_index+6];
		
		RX_STATUS = STATUS_HEADER;
		rx_index = 0;
		memset(rx_buff,0,sizeof(rx_buff));
	}
}

// 从机回应数据处理
void DataProcess()
{
	RecvData.frame_len 	= rx_buff[start_index+2];
	if(rx_index-start_index>=RecvData.frame_len)
	{
		RecvData.ip_addr 		= rx_buff[start_index];
		RecvData.code 			= rx_buff[start_index+1];
		RecvData.frame_len 	= rx_buff[start_index+2];
		RecvData.status 		= rx_buff[start_index+3];
		
		for(uint8_t i=0;i<RecvData.frame_len-5;i++)
		{
			RecvData.data[i] = rx_buff[start_index+3+i+1];
		}
		RecvData.crc_8		= rx_buff[rx_index-1];
		
		RecvData_Process();
		
		RX_STATUS = STATUS_HEADER;
		rx_index = 0;
		memset(rx_buff,0,sizeof(rx_buff));
	}
}

// 从机回应数据解析函数
void RecvData_Process()
{
	switch(RecvData.code)
	{
		case 0x8A:
			UWC_Data.RemoteData.data = RecvData.data[0];
			break;
		case 0x8B:
			UWC_Data.RemoteData.data = RecvData.data[0];
			break;
		case 0x8C:
			UWC_Data.LocationData.distance 		= RecvData.data[0]<<8|RecvData.data[1];
			UWC_Data.LocationData.time 				= RecvData.data[2]<<8|RecvData.data[3];
			UWC_Data.LocationData.temperature = RecvData.data[4]<<8|RecvData.data[5];
			break;
		case 0x8D:
			UWC_Data.YawCorrectionData.distance_H_to_B 	= RecvData.data[0]<<8|RecvData.data[1];
			UWC_Data.YawCorrectionData.distance_S_to_B 	= RecvData.data[2]<<8|RecvData.data[3];
			UWC_Data.YawCorrectionData.yaw_angle 				= RecvData.data[4]<<8|RecvData.data[5];
			UWC_Data.YawCorrectionData.temperature 			= RecvData.data[6]<<8|RecvData.data[7];
			break;
		case 0x8E:
			memset(UWC_Data.MultiByteData.data,0,sizeof(UWC_Data.MultiByteData.data));
			for(uint8_t i = 0;i<RecvData.frame_len-5;i++)
			{
				UWC_Data.MultiByteData.data[i] = RecvData.data[i];
			}
			break;
		default:break;
	}
}


/***********************主机发送函数********************************/

// 主机发送函数
void UWC_Send(uint8_t ip_addr, uint8_t code, uint8_t buff_len, uint8_t* data)
{
	SendData.slave_addr = ip_addr;			// 从机地址
	SendData.code				= code;					// 功能码
	SendData.frame_len	= 0x06+buff_len;// 帧长度
	SendData.status			= 0x01;					// 状态
	SendData.recv_addr	= 0xff;					// 接收地址/预留
	
	uint16_t data_sum = 0;
	for(uint8_t i = 0;i<buff_len;i++)
	{
		SendData.data[i] = data[i];
		data_sum += SendData.data[i];
	}
	
	SendData.crc_8			= (uint16_t)(SendData.slave_addr + SendData.code + SendData.frame_len + 
												 SendData.status + SendData.recv_addr + data_sum);// & 0x00ff;
	
	memcpy((uint8_t*)tx_buff, (uint8_t*)&SendData,SendData. frame_len-1);
	tx_buff[SendData.frame_len-1] = SendData.crc_8;
	
	HAL_UART_Transmit(&huart1, (uint8_t*)tx_buff, SendData.frame_len, HAL_MAX_DELAY);
}

// 主机发送（读）函数
void UWC_Send_Read(uint8_t ip_addr, uint16_t reg_addr, uint16_t reg_cnt)
{
	HostReadData.ip_addr 	= ip_addr;
	HostReadData.code			= 0x03;
	HostReadData.reg_addr	= ((reg_addr>>8)&0x00ff)|((reg_addr<<8)&0xff00);
	HostReadData.reg_cnt	= ((reg_cnt>>8)&0x00ff)|((reg_cnt<<8)&0xff00);
	HostReadData.crc_16		= Modbus_CRC16((uint8_t*)&HostReadData,sizeof(HostReadData)-2);
	
	HAL_UART_Transmit(&huart1, (uint8_t*)&HostReadData, sizeof(HostReadData), HAL_MAX_DELAY);
}

// 主机发送（写）函数
void UWC_Send_Write(uint8_t ip_addr, uint16_t reg_addr, uint16_t data)
{
	HostWriteData.ip_addr 	= ip_addr;
	HostWriteData.code			= 0x06;
	HostWriteData.reg_addr	= ((reg_addr>>8)&0x00ff)|((reg_addr<<8)&0xff00);
	HostWriteData.data			= ((data>>8)&0x00ff)|((data<<8)&0xff00);
	HostWriteData.crc_16		= Modbus_CRC16((uint8_t*)&HostWriteData,sizeof(HostWriteData)-2);
	
	HAL_UART_Transmit(&huart1, (uint8_t*)&HostWriteData, sizeof(HostWriteData), HAL_MAX_DELAY);
}


