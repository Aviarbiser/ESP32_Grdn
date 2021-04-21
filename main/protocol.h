#include <string.h>

#ifndef _DRV_PROTOCOL_H_
#define _DRV_PROTOCOL_H_


#define DEVICE_VERSION 1
#define SUB_VERSION 5

// Helper macros
#define INT16_FROM_BUFF(buff, index) ((int16_t)(((uint16_t)buff[index]) | ((uint16_t)buff[index+1] << 0x08)))
#define INT32_FROM_BUFF(buff, index) ((int32_t)(((uint32_t)buff[index]) | ((uint32_t)buff[index+1] << 0x08) | ((uint32_t)buff[index+2] << 0x10) | ((uint32_t)buff[index+3] << 0x18)))
#define UINT16_FROM_BUFF(buff, index) (((uint16_t)buff[index]) | ((uint16_t)buff[index+1] << 0x08))
#define UINT32_FROM_BUFF(buff, index) (((uint32_t)buff[index]) | ((uint32_t)buff[index+1] << 0x08) | ((uint32_t)buff[index+2] << 0x10) | ((uint32_t)buff[index+3] << 0x18))

#define INT16_TO_BUFF(value, buff, index) {buff[index] = (uint16_t)value & 0xFF; buff[index+1] = (uint16_t)value >> 0x08;}
#define INT32_TO_BUFF(value, buff, index) {buff[index] = (uint32_t)value & 0xFF; buff[index+1] = (uint32_t)value >> 0x08; buff[index+2] = (uint32_t)value >> 0x10; buff[index+3] = (uint32_t)value >> 0x18;}
#define UINT16_TO_BUFF(value, buff, index) {buff[index] = (uint16_t)value & 0xFF; buff[index+1] = (uint16_t)value >> 0x08;}
#define UINT32_TO_BUFF(value, buff, index) {buff[index] = (uint32_t)value & 0xFF; buff[index+1] = (uint32_t)value >> 0x08; buff[index+2] = (uint32_t)value >> 0x10; buff[index+3] = (uint32_t)value >> 0x18;}


#define DEVICE_TYPE 6 //blind =2 , shutter = 3


typedef enum
{
	E_GetStatus,
	E_GetStatusRespons,//return by devce
	E_MOTOR_GO=2,
	E_MotorStop,
	E_MotorGoToPosition,
	E_MotorHoming,
	E_ReadParam,
	E_SendParm,
	E_SendingParm,//return by device
	E_DeviceMode,
	E_SerchDevice,
	E_SerchResponse,
	E_SetID,
	E_Sync_CMD,//command to sync function between devices
	E_SetTimes,
	E_SET_IO,
	E_REBOOT,
	E_MotorStopRespons,
	E_MotorGoToPositionRespons,
	E_MotorHomingRespons =19,
	E_ReadParamRespons,
	E_MOTOR_GORespons,
	E_DeviceModeRespons,
	E_SET_IORespons,
	E_OTA=100,
	E_OTA_ACK,

}ENUM_PROTCOL_FUNCTION;


typedef struct
{
	uint8_t Position;
	uint8_t TargetPosition;
	uint16_t Speed;
	uint8_t IO_BitMap;
	uint8_t DeviceState;//offline ,online ,error ...
	uint8_t DeviceStatus;//stop,moving by speed,moving to position ,homing
	uint16_t debug_inf;
	uint16_t Device_version;
	uint8_t RemoteControl[6];	
    int32_t sunZenith;//2 dig after the point
    int32_t sunAzimuth;//2 dig after the point
    uint16_t Device_Volt;
    uint8_t driver_error_status;
    uint8_t Shutter_Angale;
    uint16_t SUB_version;
}__attribute__((packed)) DeviceStatus_STD;


typedef struct
{
	uint32_t PositionIndex;//in the array
	uint16_t Length;//array length
	uint32_t ProgramLength;// total program length
	uint16_t ProgramCRC;
	char OS_Name[50];
	uint8_t Data[400];
 }__attribute__((packed)) DeviceOTA_Recive_STD;



 typedef struct
 {
 	uint8_t Staus_response;//1 = Ok 2 = version error 3=CRC error 4=error in esp 5=error hole in data,6= done
 	uint32_t PositionIndex;//in the array
	char OS_Name[50];
  }__attribute__((packed)) DeviceOTA_Response_STD;



typedef struct
{
	uint8_t Direction;
	uint16_t Speed;
}__attribute__((packed)) MotorGo_STD;

typedef struct
{
	uint8_t mode;//mode = 0 io from logic mode = 1 io from server
	uint8_t IO_NUM;
	uint8_t Value;
}__attribute__((packed)) SET_IO_STD;

typedef struct
{
	uint8_t Angle;
	uint8_t Position;
}__attribute__((packed)) MotorGoPosition_STD;

typedef struct
{
	uint8_t MAC[6];
	uint8_t ID;
}__attribute__((packed)) DEVICE_ID_STD;


typedef struct
{
	uint64_t MAC_val;
	uint16_t index;
}__attribute__((packed)) SerchDevice_STD;

typedef struct
{
	uint16_t DeviceType; //blind =2 , shutter = 3
	uint8_t DeviceId[6];//(1-32)
	uint16_t FunctionID;
	uint16_t DataLength;
	uint8_t msgCount;
}__attribute__((packed)) CONNECTION_STRACT_STD;





typedef struct
{
	uint8_t Mode;
	uint32_t UNIX_time;
}__attribute__((packed)) MotorUpdate_STD;



//uint8_t DeviceID = 1;

extern uint8_t debug_inf;

extern uint8_t (*logicRunProcedureFunctionPtr)(uint8_t procedure_id, uint32_t param1);

extern uint32_t last_valid_connection;

extern void protocol_loop(void);
extern void reciveValidMsg(uint8_t *buff,uint16_t buffer_len);
void SendParms();




#endif // _DRV_PROTOCOL_H_
