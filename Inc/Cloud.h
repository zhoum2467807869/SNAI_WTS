/*
 * Cloud.h
 *
 *  Created on: 2019年8月19日
 *      Author: Breuning
 */

#ifndef CLOUD_H_
#define CLOUD_H_

#include "main.h"
#include "cJSON.h"

#define CLOUD_REAL_DATA_NUM 24
#define CLOUD_BREAK_REAL_DATA(x) (x = 0)

#define cloudRxQueueLen 256
#define project_MQTT_SAMPLE_IVT_S  (uint32_t)10
#define jsonOutPutBufLen  2048

#define CLOUD_RESET_TIME_STEP 360

typedef enum
{
	mc_AlarmStateNormal = 1,
	mc_AlarmStateOff = 2,
	mc_AlarmStateTrouble = 3,
	mc_AlarmStateOffLine = 4,
}ibsAlarmState_t;

typedef struct
{
	BOOL cloudIsReadingHome;
	char McuId[40];
	float ibsRealData[CLOUD_REAL_DATA_NUM];
	ibsAlarmState_t ibsAlarmState;
}hcloud_t;

/*云下行命令类型*/
typedef enum
{
	ibs_dataTypeUpData = 0,             //定时上传数据命令
	ibs_dataTypeGetHomePageData = 1,    //网页向环控设备请求数据命令
	ibs_dataTypeRemoteCtr = 21,         //网页控制环控命令
}ibsDataType_t;

typedef enum
{
	TempHumOut_Addr = 6,
	TempHumAve_Addr = 7,
	TempHum1_Addr   = 8,
	Humidity_Addr   = 9,
	TempHum2_Addr   = 10,
	TempHum3_Addr   = 11,
	TempWater_Addr  = 12,
	Pressure_Addr   = 13,
	CO2_Addr        = 14,
	NH3_Addr        = 15,
	Age_Addr        = 16,
	TempTarget_Addr = 17,
	FanState_Addr   = 18,
	WindowState_Addr= 19,
	HotState_Addr   = 20,
	PadState_Addr   = 21,
	AlamState_Addr  = 22,
	AutoState_Addr  = 23,
}REG_ADDR;

typedef struct
{
	uint16_t  year;
	uint16_t  month;
	uint16_t  day;
	uint16_t  hour;
	uint16_t  minute;
	uint16_t  second;
	uint16_t  date;
}TIME;

extern hcloud_t hcloud;
extern uint8_t cloudRxQueueMem[cloudRxQueueLen];


#define ibsDataTypeNameMaxLen 16
extern uint8_t AddrNum[];
extern const char ibsDataTypeName[CLOUD_REAL_DATA_NUM][ibsDataTypeNameMaxLen];

void Cloud_Init(void);
void Get_MCUUID(void);
void CloudServerFunc(void);
void user_delay_ms(void);

#endif /* CLOUD_H_ */
