
#include "drv_rs485.h"
#include "protocol.h"
#include "dev_logic.h"
#include "dev_time.h"
#include "dev_memory.h"
#include "OTA.h"

extern uint8_t DeviceID;
extern uint8_t DeviceMAC[6];


uint32_t last_valid_connection;


void protocol_parse_packet(uint8_t *buff, uint16_t buff_len);

uint16_t OTA_CalculatedCRC;

uint32_t TxMessageCounter;

#define RESPONSE_TO_MASTER_SIZE 40
uint8_t responseToMasterBuffer[RESPONSE_TO_MASTER_SIZE];

uint8_t protocolBuffer[RS485_BUFFER_SIZE];
uint8_t msgLentgh;
uint8_t debug_inf;


void reciveValidMsg(uint8_t *buff, uint16_t buffer_len){


	protocol_parse_packet((uint8_t *)buff, buffer_len);

	/*
		for (int i = 0; i < buffer_len; i++) {
		protocolBuffer[i] = buff[i];
	}
	msgLentgh = buffer_len;
	*/
}


// CRC16-ANSI calcualtion
// Returns: Calculated value
uint16_t CRC16ANSI(uint8_t *data, uint16_t data_len,uint16_t  Firstval )
{
  static const uint16_t crc_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
  };

  uint8_t ch,ch1;
  uint16_t retval = Firstval;// 0xFFFF;

  while (data_len--)
  {
	  ch1=*data++;
	  ch = ch1 ^ retval;
	  retval >>= 8;
	  retval ^= crc_table[ch];
  }
  return retval;
}


// Test if a packet is valid
// Returns: 1 = valid packet, 0 = invalid packet
uint8_t protocol_test_packet(uint8_t *buff, uint16_t buff_len)
{

  // Test minimal possible packet length
  if (buff_len < 4)
  {

	return 0;
  }

  // Test CRC16
  uint16_t CalculatedCRC = CRC16ANSI(buff, buff_len - 2,0xFFFF);
  uint16_t InBuffCRC = UINT16_FROM_BUFF(buff, buff_len - 2);

  if (CalculatedCRC != InBuffCRC)
  {
	  printf("crc CalculatedCRC %d InBuffCRC %d \n",CalculatedCRC,InBuffCRC);
//	  Serial.print("\nCRC failed");
    return 2;
  }

//	  Serial.print("\nPacket OK");
  return 1;
}

// Sends a SLIP char to the RS-485 driver, escape it if needed
void protocol_send_slip_char(uint8_t ch)
{
	switch(ch){
		case SLIP_START:
			rs485_send_char(SLIP_ESC);
			rs485_send_char(SLIP_START-SLIP_ESC);
			break;
		case SLIP_ESC:
			rs485_send_char(SLIP_ESC);
			rs485_send_char(SLIP_ESC-SLIP_ESC);
			break;
		case SLIP_END:
			rs485_send_char(SLIP_ESC);
			rs485_send_char(SLIP_END-SLIP_ESC);
			break;
		default:
		rs485_send_char(ch);
	}	
}

// Send a packet according to the FoldiMate protocol specification
void protocol_send_packet(uint8_t *packet, uint16_t packet_len)
{



   rs485_send_char(SLIP_START);

  // Sends the Headers + Data
  for (uint16_t i = 0; i < packet_len; i++)
    protocol_send_slip_char(packet[i]);

  // Calculate CRC16-ANSI and send it as well
  uint16_t crc = CRC16ANSI(packet, packet_len,0xFFFF);
  protocol_send_slip_char(crc & 0xFF);
  protocol_send_slip_char(crc >> 0x08);

  // Send the SLIP_END footer
  rs485_send_char(SLIP_END);
  rs485_send_data();
}



void SendStatus(uint8_t* _dataArrived)
{

	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	//Header->DeviceId = DeviceID;
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_GetStatusRespons;//return function
	Header->DataLength = sizeof(DeviceStatus_STD);//data length without CRC

	//return device status to master
	uint8_t  statusData[sizeof(DeviceStatus_STD)];


	DeviceStatus_STD * deviceStatus = (DeviceStatus_STD *)statusData;
	deviceStatus->Position = position_PV ;
	deviceStatus->TargetPosition = position_SP;
	deviceStatus->Speed = dev_speed;
	deviceStatus->IO_BitMap = io_val ; // IO bitmap;
	//deviceStatus->DeviceState = motor_status;
	//printf("SendStatus motor_status %d  \n",motor_status);
	//deviceStatus->DeviceStatus = motion_step;
	deviceStatus->debug_inf = debug_inf;
	deviceStatus->sunAzimuth  = (int32_t)(sunAzimuth * 100.0);
	deviceStatus->sunZenith = (int32_t)(sunZenith* 100.0);
	deviceStatus->Device_Volt = Device_Volt;

	deviceStatus->Device_version = DEVICE_VERSION;
	deviceStatus->SUB_version = SUB_VERSION;
	//deviceStatus->debug_inf = driver_error_status;
	deviceStatus->Shutter_Angale = 0;
	//deviceStatus->driver_error_status = driver_error_status;
	//copy data to array
	memset(deviceStatus->RemoteControl,0,6);


	uint8_t  dataToSend[sizeof(DeviceStatus_STD)+ sizeof(CONNECTION_STRACT_STD)];


	//copy Header data to send
	memcpy(dataToSend, headerData, sizeof(CONNECTION_STRACT_STD));

	//copy data data to send
	memcpy(dataToSend+sizeof(CONNECTION_STRACT_STD), statusData, sizeof(DeviceStatus_STD));


	//printf("SendStatus deviceStatus->debug_inf %d motor_current %d dataToSend %d \n",deviceStatus->debug_inf,motor_current,dataToSend[sizeof(CONNECTION_STRACT_STD)+7]);

	int slen = sizeof(DeviceStatus_STD)+ sizeof(CONNECTION_STRACT_STD);

	protocol_send_packet(dataToSend,  slen);

}


void SendParms()
{

	//return device status to master
	//uint8_t  parmData[sizeof(MOTOR_PARM_STD)];
	//motor_memory = (MOTOR_PARM_STD *)&parmData[0];

	//memcpy(parmData,&motor_memory,sizeof(MOTOR_PARM_STD));

	//uint8_t  dataToSend[sizeof(MOTOR_PARM_STD)+ sizeof(CONNECTION_STRACT_STD)];

	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	//Header->DeviceId = DeviceID;
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_SendingParm;//return function
	//Header->DataLength = sizeof(MOTOR_PARM_STD);//data length without CRC

	//copy Header data to send
//	memcpy(dataToSend, headerData, sizeof(CONNECTION_STRACT_STD));

	//copy data data to send
	//memcpy(dataToSend+sizeof(CONNECTION_STRACT_STD), parmData, sizeof(MOTOR_PARM_STD));

//	int slen = sizeof(MOTOR_PARM_STD)+ sizeof(CONNECTION_STRACT_STD);

	//protocol_send_packet(dataToSend,  slen);
	printf("SendParms  \n");

}


void ReadParms(uint8_t* _dataArrived)
{

	//uint8_t  parmData[sizeof(MOTOR_PARM_STD)];
	//motor_memory = (MOTOR_PARM_STD *)parmData;

//	memcpy(&motor_memory,_dataArrived+sizeof(CONNECTION_STRACT_STD),sizeof(MOTOR_PARM_STD));


	//in flash
	//SaveMotorParamToMemory();

	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;

	//Header->DeviceId = DeviceID;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_ReadParamRespons;//return function
	Header->DataLength = 0;//data length without CRC

	int slen = sizeof(CONNECTION_STRACT_STD);

	protocol_send_packet(headerData,  slen);

	//if(motor_memory.motor_mode == E_S_WAKEUP  ) motor_memory.motor_mode = E_S_ONLINE;

/*
	if (motor_status == E_S_WAKEUP)
	{
		//set location
		position_PV = motor_memory.Position;


		//motion_step = E_HOMING;
	}
*/
	//motor_status = motor_memory.motor_mode ;//E_S_ONLINE;

}



void ReadHours(uint8_t* _dataArrived)
{

	uint8_t  parmData[sizeof(HOUR_TIMES_STD)*7];
	//motor_memory = (MOTOR_PARM_STD *)parmData;

//	memcpy(&hour_parm,_dataArrived+sizeof(CONNECTION_STRACT_STD),sizeof(HOUR_TIMES_STD)*7);
	//SaveMotorParamToMemory();
	SaveHoursToMemory();



}





void SET_IO_VAL(uint8_t* _dataArrived)
{

	uint8_t parmdata[sizeof(SET_IO_STD)];
	SET_IO_STD * parm = (SET_IO_STD *)parmdata;

	memcpy(parmdata,_dataArrived+sizeof(CONNECTION_STRACT_STD),sizeof(SET_IO_STD));
	printf(" SET_IO_VAL IO Num %d , Value %d \n", parm->IO_NUM, parm->Value);

	io_dig_mode = parm->mode;


	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;

	//Header->DeviceId = DeviceID;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_SET_IORespons;//return function
	Header->DataLength = 0;//data length without CRC

	int slen = sizeof(CONNECTION_STRACT_STD);

	protocol_send_packet(headerData,  slen);

	if (io_dig_mode == 0) return;
	ESPWriteToDigital(parm->IO_NUM ,parm->Value);

}





char * uintToStr( const uint64_t num, char *str )
{
  uint8_t i = 0;
  uint64_t n = num;

  do
    i++;
  while ( n /= 10 );

  str[i] = '\0';
  n = num;

  do
    str[--i] = ( n % 10 ) + '0';
  while ( n /= 10 );

  return str;
}


void RetuernMAC(uint8_t* _dataArrived)
{
	//this function return mac address if DeviceID is zero (not set)



	uint8_t buff_data[sizeof(SerchDevice_STD)];
	SerchDevice_STD * buff = (SerchDevice_STD *)buff_data;

	memcpy(buff_data,_dataArrived+sizeof(CONNECTION_STRACT_STD),sizeof(SerchDevice_STD));

	//buff->MAC_val =
	//copy mac id to uint64
	uint64_t mymac;
	memcpy(&mymac, DeviceMAC, sizeof(DeviceMAC));

	double b = pow(2, (double)buff->index+1)-1;
	uint64_t shiftX = mymac & (uint64_t)b;


	char str[21];
	//char str2[21];uintToStr( mymac, str ), uintToStr( shiftX, str2 )

	//int z = memcmp(&buff->MAC_val, &macval, sizeof(uint64_t));
	ets_printf("RetuernMAC index %d MAC_val  %d mymac %lld shiftX %lld \n",buff->index,buff->MAC_val,mymac,shiftX);


	if(shiftX!=buff->MAC_val)
	{
		ets_printf("shiftX!=buff->MAC_val \n");
		return;
	}

	//return device mac to master



	uint8_t  dataToSend[ 2 + sizeof(CONNECTION_STRACT_STD)];

	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;

	//Header->DeviceId = DeviceID;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_SerchResponse;//return function
	Header->DataLength = 4;//data length without CRC

	//copy Header data to send
	memcpy(dataToSend, headerData, sizeof(CONNECTION_STRACT_STD));

	//copy data data to send

	uint16_t ver = DEVICE_VERSION;
	memcpy(dataToSend+sizeof(CONNECTION_STRACT_STD), &ver, 2);
	ver = SUB_VERSION;
	memcpy(dataToSend+sizeof(CONNECTION_STRACT_STD)+2, &ver, 2);

	int slen = 4 + sizeof(CONNECTION_STRACT_STD);

	protocol_send_packet(dataToSend,  slen);


}



void SendOTA_Response(uint8_t status_response,uint32_t PositionIndex,char * OS_Name)
{

	uint8_t  parmData[sizeof(DeviceOTA_Response_STD)];

	DeviceOTA_Response_STD * parm = (DeviceOTA_Response_STD *)parmData;
	parm->PositionIndex = PositionIndex;
	parm->Staus_response = status_response;
	memcpy(parm->OS_Name, OS_Name , sizeof(parm->OS_Name));


	uint8_t  dataToSend[sizeof(DeviceOTA_Response_STD)+ sizeof(CONNECTION_STRACT_STD)];

	uint8_t headerData[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Header = (CONNECTION_STRACT_STD *)headerData;
	memcpy(Header->DeviceId, DeviceMAC , sizeof(DeviceMAC));
	//Header->DeviceId = DeviceID;
	Header->DeviceType = DEVICE_TYPE;
	Header->FunctionID = E_OTA_ACK;//return function
	Header->DataLength = sizeof(DeviceOTA_Response_STD);//data length without CRC

	//copy Header data to send
	memcpy(dataToSend, headerData, sizeof(CONNECTION_STRACT_STD));

	//copy data data to send
	memcpy(dataToSend+sizeof(CONNECTION_STRACT_STD), parmData, sizeof(DeviceOTA_Response_STD));

	int slen = sizeof(DeviceOTA_Response_STD)+ sizeof(CONNECTION_STRACT_STD);

	protocol_send_packet(dataToSend,  slen);




}


uint32_t LastPositionIndex;
uint32_t LastOTA_length;




void ReciveOTA(uint8_t* _dataArrived)
{


	uint8_t buff_data[sizeof(DeviceOTA_Recive_STD)];
	DeviceOTA_Recive_STD * buff = (DeviceOTA_Recive_STD *)buff_data;
	int ret;
	memcpy(buff_data,_dataArrived+sizeof(CONNECTION_STRACT_STD),sizeof(DeviceOTA_Recive_STD));




	if((buff->Length == 0))
	{
		printf(" ================ ReciveOTA data buff->Length == 0 =============== \n ");
		printf("write buff_data \n");
			for(int i = sizeof(CONNECTION_STRACT_STD);i<=200;i++)
			{
				printf("%d,",_dataArrived[i]);
			}
			printf("\n end obuff_data \n");
		//send ACK to TBox
		//SendOTA_Response(1,buff->PositionIndex,buff->OS_Name);
		return;
	}

	if((buff->PositionIndex <= LastPositionIndex) && (buff->PositionIndex != 0))
	{
		if (LastPositionIndex == 0)
		{
			printf(" ================ ReciveOTA after restart =============== \n ");
			SendOTA_Response(6,buff->PositionIndex,buff->OS_Name);
			return;
		}

		printf(" ================ ReciveOTA data all ready writing =============== \n ");
		//send ACK to TBox
		SendOTA_Response(1,buff->PositionIndex,buff->OS_Name);
		return;
	}

	if((buff->PositionIndex != (LastPositionIndex + LastOTA_length)) && (buff->PositionIndex != 0))
	{
		printf(" +++++++++++++++++ ReciveOTA data find hole in position %d  last position %d +++++++++++++++++++ \n ",buff->PositionIndex,LastPositionIndex);
		//send ACK to TBox
		SendOTA_Response(5,LastPositionIndex,buff->OS_Name);
		return;
	}
	/*
	//check if new program version is GT (>) from current version
	if(buff->Version <= DEVICE_VERSION)
	{
		//return wrong version
		SendOTA_Response(2,buff->PositionIndex);
		return;
	}
	*/

	printf("ReciveOTA PositionIndex %d Length %d \n ",buff->PositionIndex,buff->Length);
	//if first time
	if(buff->PositionIndex == 0)
	{
		//first array data
		// check CRC for all OTA data (total data)
		//OTA_CalculatedCRC = CRC16ANSI(buff->Data, buff->Length ,0xFFFF);
		ret =  starting_ota();
		if (ret != 0)
		{
			SendOTA_Response(4,buff->PositionIndex,buff->OS_Name);
			printf("ReciveOTA starting_ota %d \n ",ret);
			return;
		}
		LastPositionIndex = 0;
		LastOTA_length = 0;
/*
		printf("write partition \n");
		for(int i = 0;i<=buff->Length;i++)
		{
			printf("%d,",buff->Data[i]);
		}
		printf("\n end write partition \n");
*/
	}
	else if((buff->PositionIndex + buff->Length) >= buff->ProgramLength )
	{
		printf(" ReciveOTA last update data \n ");
		// last array data
		// check CRC for all OTA data (total data)
		//OTA_CalculatedCRC = CRC16ANSI(buff->Data, buff->Length ,OTA_CalculatedCRC);
		/*check if calc CRC is EQ (=) to Program crc (from server)
		if (OTA_CalculatedCRC != buff->ProgramCRC)
		{
			//return wrong CRC
			SendOTA_Response(3,buff->PositionIndex,buff->OS_Name);
			//To do cancel update
			printf(" ReciveOTA last update data CRC error %d original CRC %d \n ",OTA_CalculatedCRC,buff->ProgramCRC);
			return;
		}
		*/
		//write data to partition
		ret =  ota_task(buff->Data ,buff->Length);
		if (ret != 0)
		{
			SendOTA_Response(4,buff->PositionIndex,buff->OS_Name);
			printf(" ReciveOTA write data to partition %d \n ",ret);
			return;
		}

		LastPositionIndex = buff->PositionIndex;
		LastOTA_length = buff->Length;
		SendOTA_Response(6,buff->PositionIndex,buff->OS_Name);

		//update done
		ret = OTA_End();
		if (ret != 0)
		{
			SendOTA_Response(4,buff->PositionIndex,buff->OS_Name);
			printf(" ReciveOTA update done %d \n ",ret);
			return;
		}
		return;
	}
	//else
	//{
		//if(buff->PositionIndex >  LastPositionIndex)
		//{
		//in the middle of transfer data
	//	OTA_CalculatedCRC = CRC16ANSI(buff->Data, buff->Length ,OTA_CalculatedCRC);
	//	printf(" ReciveOTA calculate CRC  %d  \n ",OTA_CalculatedCRC);
		//}
	//}

	//write data to partition

	ret =  ota_task(buff->Data ,buff->Length);

	if (ret != 0)
	{
		SendOTA_Response(4,buff->PositionIndex,buff->OS_Name);
		printf("ReciveOTA write data to partition %d \n ",ret);
		return;
	}
	LastPositionIndex = buff->PositionIndex;
	LastOTA_length = buff->Length;

	//send ACK to TBox
	SendOTA_Response(1,buff->PositionIndex,buff->OS_Name);







}





void  ParseMessageResponse(uint8_t* _dataArrived, uint8_t* _dataToBeSent, uint16_t* _sizeOfDataToBeSent)
{


	//check if msg is for this device same mac addrass
	uint8_t parmHader[sizeof(CONNECTION_STRACT_STD)];
	CONNECTION_STRACT_STD * Hader = (CONNECTION_STRACT_STD *)parmHader;

	memcpy(parmHader,_dataArrived,sizeof(CONNECTION_STRACT_STD));

	//check if data is for this device (compare mac address)
	int res;
	uint8_t xx[6] = { 0 };
	if (memcmp(Hader->DeviceId, xx, sizeof(DeviceMAC))!=0)
	{
		res = memcmp(Hader->DeviceId, DeviceMAC, sizeof(DeviceMAC));

		if (res != 0)
		{
		//	printf("ParseMessageResponse memcmp res %d function %d \n",res,Hader->FunctionID);
			return;
		}

	}

	//get the CPU time of the last good connection
	last_valid_connection = xTaskGetTickCount();


	/*
	int res1 = (1UL << (DeviceID-1)) & ArriveDeviceID;
	if (res1 == 0 && ArriveDeviceID != 0)
	{
		printf("ParseMessageResponse return deviceid %d \n",ArriveDeviceID);
		return;
	}
	*/

	//printf("function %d \n",Hader->FunctionID);
	//msg is for this device
	//checking function
	switch (Hader->FunctionID)
	{
	case E_GetStatus:
		//check MAC address
		SendStatus(_dataArrived);
		break;
	case E_MOTOR_GO:
	//	MotorGo(_dataArrived);
		break;
	case E_MotorStop:
	//	MotorStop();
		break;
	case E_MotorGoToPosition:
	//	MotorGoToPosition(_dataArrived);
		break;
	case E_MotorHoming:
	//	MotorHome(_dataArrived);
		break;
	case E_ReadParam:
		ReadParms(_dataArrived);
		break;
	case E_SendParm:
		SendParms();
		break;
	case E_DeviceMode:
	//	ModeUpdate(_dataArrived);
		break;
	case E_SerchDevice:
		RetuernMAC(_dataArrived);
		break;
	case E_SetID:
		res = memcmp(_dataArrived+6, DeviceMAC, sizeof(DeviceMAC));
		if (res != 0) return;
		//set DeviceID
		DeviceID = _dataArrived[12];
		printf("Set device id new id is: %d \n",DeviceID);
		break;
	case E_Sync_CMD:
		//MotorSenc_move();
		break;
	case E_SetTimes:
		ReadHours(_dataArrived);
		break;
	case E_OTA:
		ReciveOTA(_dataArrived);
		break;
	case E_OTA_ACK:
		break;
	case E_SET_IO:
		SET_IO_VAL(_dataArrived);
		break;
	case E_REBOOT:
		printf("E_REBOOT ............. \n");
		vTaskDelay(100);
		esp_restart();
		break;

	default:
		break;
	}



	return ;


}


// Parse a received packet
//#define SEMI_DEBUG3 1

void protocol_parse_packet(uint8_t *buff, uint16_t buff_len)
{



  uint8_t xx =  protocol_test_packet(buff, buff_len);

  if (xx != 1)
  {

   // logicDebugging = buff[2];
	//set_error_status(E_CRC_ERROR);
	int i;
	for(i=0;i<buff_len;i++)
	{
		printf("data Failed -  %d \n",buff[i]);
	}


	printf("Failed!!!! %d \n",xx);
    return ; // Quit if packet is invalid
  }

  uint16_t sendResponse = 0;
  
  ParseMessageResponse(buff, responseToMasterBuffer, &sendResponse);
  
}
	





void protocol_loop(void)
{

	if (!msgLentgh)
		return ;
	protocol_parse_packet((uint8_t *)protocolBuffer, msgLentgh);
	msgLentgh = 0;
}


