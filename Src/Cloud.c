/*
 * Cloud.c
 *
 *  Created on: 2019年8月19日
 *      Author: Breuning
 */

#include "Cloud.h"
#include "CloudRX.h"
#include "usart.h"
#include "tim.h"
#include "cJSON.h"
#include "MD5.h"
#include "CircularQueue.h"
#include "LED.h"
#include "md5.h"
#include "string.h"

hcloud_t hcloud;

/*云端下行数据接受队列*/
uint8_t cloudRxQueueMem[cloudRxQueueLen] = { 0 };
CQ_handleTypeDef cloudRxQueue;

/*json缓冲区(发送至云端)*/
uint8_t jsonOutPutBuf[jsonOutPutBufLen] = { 0 };

static uint32_t EthernetOffLineCount = 0;
const char MD5_KEY[] = "ABCk4JBQWEL.SNWn2&(M)@kEY";

#define STATE_START_NUM 18

/*风机位数、小窗位数、加热位数、湿帘位数、报警位数、自动位数*/
uint8_t AddrNum[] = {16, 4, 4, 3, 1, 4};


const char ibsDataTypeName[CLOUD_REAL_DATA_NUM][ibsDataTypeNameMaxLen] =
{
 "null","null","null","null","null","null","outTemperature","aveTemperature","inTemperature1","inHumidity","inTemperature2",
 "inTemperature3","water","pressure","co2","NH3","age","desTemperature","fan","window","hot","pad","alarm","auto"
};

/* static函数声明*/
static void UploadData(void);
static void UploadDataToCloud(void);
static void AddUpDataToJsonObj(void);
static void GetTime(TIME* time);
static void AddSendPackage_TempHum(cJSON *js_body);
static void AddSendPackage_Other(cJSON *js_body);
static void EthernetResetJudege(void);
static void CJSON_AddValueToArray_TempHum(cJSON *js_body, uint8_t num, uint8_t order, uint8_t type);
static void CJSON_AddValueToArray_Other(cJSON *js_list, uint8_t num, uint8_t type);
static void AddStateValue(cJSON *root);
static void addList(uint8_t *target, uint32_t len, uint32_t value);
static void ETH_reset(void) ;


void Cloud_Init()
{

	/*初始化云端数据接收队列*/
	memset(cloudRxQueueMem,0,sizeof(cloudRxQueueMem));
	CQ_init(&cloudRxQueue, cloudRxQueueMem, sizeof(cloudRxQueueMem));

	/*获取环控stm32芯片ID*/
	Get_MCUUID();

	/*初始化报警状态*/
	hcloud.ibsAlarmState = 1;

	/*清空实时数据*/
	for (uint8_t i = 0; i < CLOUD_REAL_DATA_NUM; i++)
	{
		CLOUD_BREAK_REAL_DATA(hcloud.ibsRealData[i]);
	}

	/*初始化时开启云端下行数据接收*/
	cloudRxCrt.isRxEnable = TRUE;
}

void Get_MCUUID(void)
{
	uint32_t UID[3];
	HAL_GetUID(UID);   //读取芯片的96位唯一标识ID
	sprintf((char*) hcloud.McuId, "%0*lx", 8, UID[0]);
	sprintf((char*) hcloud.McuId + 8, "%0*lx", 8, UID[1]);
	sprintf((char*) hcloud.McuId + 16, "%0*lx", 8, UID[2]);
//	HAL_UART_Transmit_DMA(&huart2, (uint8_t *)hcloud.McuId, sizeof(hcloud.McuId));
}

/*云端接口主任务函数*/
void CloudServerFunc(void)
{
//	CloudDataRecv();
	UploadData();
}

static void UploadData(void)
{
	if (UploadDataToCloud_TimerFlag == TRUE)           //通过TIM2设置为每10秒进行一次数据的组包上传
	{

		cloudRxCrt.isRxEnable = FALSE;                 //上传数据过程中停止进入中断接收云端下行数据

		UploadDataToCloud();

		cloudRxCrt.isRxEnable = TRUE;
		UploadDataToCloud_TimerFlag = FALSE;
	}
}

/* 设备数据上传    网关 -> mqtt -> 服务器 报文处理并发送 */
static void UploadDataToCloud(void)
{
	AddUpDataToJsonObj();   //添加实时数据

	EthernetResetJudege();
}

/*把设备上传需要的数据添加到JSON中 */
static void AddUpDataToJsonObj(void)
{
	uint8_t tempBuf[92] = { '\0' };
	TIME time;

	uint8_t MD5[17];
	uint8_t temp,i;
	MD5_CTX ctx;
	const char MD5_KEY[] = "ABCk4JBQWEL.SNWn2&(M)@kEY";

	/*创建JSON对象*/
	cJSON *root, *rootPtr, *js_body;
	root = cJSON_CreateObject();
	rootPtr = cJSON_CreateObject();
	if (root == NULL || rootPtr == NULL)
	{
		cJSON_Delete(root); //释放内存
		cJSON_Delete(rootPtr);
		root = NULL;
		rootPtr = NULL;
		return;
	}


	/*添加"mqttType"*/
	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%d", ibs_dataTypeUpData);
	cJSON_AddStringToObject(root, "mqttType", (const char*) tempBuf);

	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%d", ibs_dataTypeGetHomePageData);
	cJSON_AddStringToObject(rootPtr, "mqttType", (const char*) tempBuf);


	/*添加上传时间到"collectDt"*/
	GetTime(&time);  //得到时间值
	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%d-%02d-%02d %02d:%02d:%02d", time.year, time.month, time.day, time.hour, time.minute, time.second);
	cJSON_AddStringToObject(root, "collectDt", (const char*) tempBuf);
	cJSON_AddStringToObject(rootPtr, "collectDt", (const char*) tempBuf);


	/*添加上传间隔到"upInterval"*/
	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%ld", project_MQTT_SAMPLE_IVT_S);
	cJSON_AddStringToObject(root, "upInterval", (const char*) tempBuf);
	cJSON_AddStringToObject(rootPtr, "upInterval", (const char*) tempBuf);


	/*添加MD5到"apptoken"*/
	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%d%02d%02d", time.year, time.month, time.day);
	MD5Init(&ctx);
	strcat((char*) tempBuf, (char*) MD5_KEY);
	MD5Update(&ctx, (uint8_t*) tempBuf, strlen((const char*) tempBuf));
	MD5Final(&ctx, MD5);

	for (i = 0; i < 16; i++)         //MD5值转成字符
	{
		temp = MD5[i];
		sprintf((char*) (tempBuf + 2 * i), "%02x", temp);
	}
	cJSON_AddStringToObject(root, "apptoken", (const char*) tempBuf);
	cJSON_AddStringToObject(rootPtr, "apptoken", (const char*) tempBuf);


	/*添加"dataTarget"*/
	memset(tempBuf, 0, sizeof(tempBuf));
	sprintf((char *) tempBuf, "%3.1f", hcloud.ibsRealData[TempTarget_Addr]);
//	cJSON_AddStringToObject(root, "dataTarget", (const char*) tempBuf);         //由于长度不够第一个包不添加dataTarget，添加到第二个包
	cJSON_AddStringToObject(rootPtr, "dataTarget", (const char*) tempBuf);


	/*第一个包添加"data"数组*/
	memset(tempBuf, 0, sizeof(tempBuf));
	cJSON_AddItemToObject(root, "data", js_body = cJSON_CreateArray());
	AddSendPackage_TempHum(js_body);
	AddSendPackage_Other(js_body);

	/*第二个包添加网关编号"netGateNum"*/
	Get_MCUUID();
	cJSON_AddStringToObject(rootPtr, "netGateNum", (const char *) (hcloud.McuId));

	/*第二个包添加状态值"fan","window","hot","pad","alarm","auto"*/
	AddStateValue(rootPtr);


	/*将两个完整的数据包通过串口发出去*/
	memset(jsonOutPutBuf, 0, sizeof(jsonOutPutBuf));  											//建议使用前清零
	if (cJSON_PrintPreallocated(root, (char*) jsonOutPutBuf, sizeof(jsonOutPutBuf), FALSE))
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)jsonOutPutBuf, sizeof(jsonOutPutBuf), 1000);  	//将数组内的数据通过USART3由DMA发送至上云模块
		LED1_RJ45_Blink();																		//发一包数据闪一次灯
	}

	HAL_Delay(1000);

	memset(jsonOutPutBuf, 0, sizeof(jsonOutPutBuf));
	if (cJSON_PrintPreallocated(rootPtr, (char*) jsonOutPutBuf, sizeof(jsonOutPutBuf), FALSE))
	{
		HAL_UART_Transmit(&huart3, (uint8_t *)jsonOutPutBuf, sizeof(jsonOutPutBuf), 1000);  	//将数组内的数据通过USART3由DMA发送至上云模块
		LED1_RJ45_Blink();																		//发一包数据闪一次灯
	}


	cJSON_Delete(root);																			//指针用完了以后，需要手工调用 cJSON_Delete(root)进行内存回收，释放内存空间
	cJSON_Delete(rootPtr);

	root = NULL;
	rootPtr = NULL;

	memset(jsonOutPutBuf, 0, sizeof(jsonOutPutBuf));  											//建议使用后清零
}

static void GetTime(TIME* time)
{
	time->year   =  hcloud.ibsRealData[0];
	time->month  =  hcloud.ibsRealData[1];
	time->day    =  hcloud.ibsRealData[2];
	time->hour   =  hcloud.ibsRealData[3];
	time->minute =  hcloud.ibsRealData[4];
	time->second =  hcloud.ibsRealData[5];
}

static void AddSendPackage_TempHum(cJSON *js_body)
{
	uint8_t i;
	for(i=6;i<12;i++)    //添加温湿度数据包
	{
		switch (i)
		{
			case TempHumOut_Addr:
				CJSON_AddValueToArray_TempHum(js_body, TempHumOut_Addr, 4, 1);
				break;
			case TempHumAve_Addr:
				CJSON_AddValueToArray_TempHum(js_body, TempHumAve_Addr, 8, 1);
				break;
			case TempHum1_Addr:
				CJSON_AddValueToArray_TempHum(js_body, TempHum1_Addr, 1, 1);
				break;
			case Humidity_Addr:   //湿度数据添加到上一个平均温湿度的数据包里了

				break;
			case TempHum2_Addr:
				CJSON_AddValueToArray_TempHum(js_body, TempHum2_Addr, 2, 1);
				break;
			case TempHum3_Addr:
				CJSON_AddValueToArray_TempHum(js_body, TempHum3_Addr, 3, 1);
				break;
			default:
				break;
		}
	}
}

static void AddSendPackage_Other(cJSON *js_body)
{
	uint8_t i;
	for(i=12;i<16;i++)
	{
		cJSON *js_list = NULL;
		cJSON_AddItemToArray(js_body, js_list = cJSON_CreateObject());
		switch (i)
		{
			case TempWater_Addr:
				CJSON_AddValueToArray_Other(js_list, TempWater_Addr, 4);
				break;
			case Pressure_Addr:
				CJSON_AddValueToArray_Other(js_list, Pressure_Addr, 7);
				break;
			case CO2_Addr:
				CJSON_AddValueToArray_Other(js_list, CO2_Addr, 3);
				break;
			case NH3_Addr :
				CJSON_AddValueToArray_Other(js_list, NH3_Addr, 2);
				break;
//			case Age_Addr:
//				CJSON_AddValueToArray_Other(js_list, Age_Addr, 1);
//				break;
//			case TempTarget_Addr:
//				CJSON_AddValueToArray_Other(js_list, TempTarget_Addr, 1);
//				break;
			default:
				break;
		}
	}
}


static void CJSON_AddValueToArray_TempHum(cJSON *js_body, uint8_t num, uint8_t order, uint8_t type)
{
	uint8_t CloudBuf[40] = {'\0'};
	uint8_t str[40] = {'\0'};

	cJSON *js_list = NULL;
	cJSON_AddItemToArray(js_body, js_list = cJSON_CreateObject());

	memset((char*) CloudBuf, 0, sizeof(CloudBuf));
	sprintf((char*) CloudBuf, "-%02d-%d", type, order);
	Get_MCUUID();
	strcat((char*) hcloud.McuId, (const char *) CloudBuf);    						//将mcuID与传感器类型、传感器编号放在一起
	cJSON_AddStringToObject(js_list, "devNum", (const char*) hcloud.McuId);

	memset(str, 0, sizeof(str));
	sprintf((char*) str, "%02d", type);
	cJSON_AddStringToObject(js_list, "dataType", (const char*) str);

	memset(str, 0, sizeof(str));
	sprintf((char*) str, "%3.1f",  hcloud.ibsRealData[num]);
	cJSON_AddStringToObject(js_list, "dataValue1", (const char*) str);      		//温度数据

	if(num == TempHum1_Addr)   													//温湿度1数据包里要加湿度数据，其他温湿度数据包里不需要加
	{
		memset(str, 0, sizeof(str));
		sprintf((char*) str, "%3.1f", (float) ((float) hcloud.ibsRealData[num + 1]));
		cJSON_AddStringToObject(js_list, "dataValue2", (const char*) str);			//湿度数据
	}

	memset(CloudBuf, 0, sizeof(CloudBuf));
	sprintf((char *) CloudBuf, "%d", hcloud.ibsAlarmState);
	cJSON_AddStringToObject(js_list, "state", (const char*) CloudBuf);
}

static void CJSON_AddValueToArray_Other(cJSON *js_list, uint8_t num, uint8_t type)
{
	uint8_t CloudBuf[40] = {'\0'};
	uint8_t str[40] = {'\0'};

	memset((char*) CloudBuf, 0, sizeof(CloudBuf));
	sprintf((char*) CloudBuf, "-%02d", type);
	Get_MCUUID();
	strcat((char*) hcloud.McuId, (const char *) CloudBuf);    						//将mcuID与传感器类型放在一起
	cJSON_AddStringToObject(js_list, "devNum", (const char*) hcloud.McuId);

	memset(str, 0, sizeof(str));
	sprintf((char*) str, "%02d", type);
	cJSON_AddStringToObject(js_list, "dataType", (const char*) str);

	memset(str, 0, sizeof(str));
	sprintf((char*) str, "%3.1f", (float) ((float) hcloud.ibsRealData[num]));
	cJSON_AddStringToObject(js_list, "dataValue1", (const char*) str);

	memset(CloudBuf, 0, sizeof(CloudBuf));
	sprintf((char *) CloudBuf, "%d", hcloud.ibsAlarmState);
	cJSON_AddStringToObject(js_list, "state", (const char*) CloudBuf);
}


static void AddStateValue(cJSON *root)
{

	uint8_t str[32];
	uint8_t i;

	for (i = STATE_START_NUM; i < 24; i++)
	{
		memset(str, 0, sizeof(str));

		/* List类型的数据 ex:[1,1,1,0,0] */
		addList(str, AddrNum[i - STATE_START_NUM], (int16_t)hcloud.ibsRealData[i]);

		/*添加到JSON实体中*/
		cJSON_AddStringToObject(root, ibsDataTypeName[i], (const char*) str);
	}
}

static void addList(uint8_t *target, uint32_t len, uint32_t value)
{
	for (uint8_t i = 0; i < len; i++)
	{
		if (i == len - 1)
		{
			sprintf((char*) (target + i * 2), "%01d", (value & (0x00000001 << i) ? TRUE : FALSE));
		}
		else
		{
			sprintf((char*) (target + i * 2), "%01d,", (value & (0x00000001 << i) ? TRUE : FALSE));
		}
	}
}



static void EthernetResetJudege(void)
{
	EthernetOffLineCount++;
	if (EthernetOffLineCount >= CLOUD_RESET_TIME_STEP)   							//10秒x360次 = 1小时  上云模块复位一次
	{
		ETH_reset();
		EthernetOffLineCount = 0;
	}
}

/*拉低RJ45模块RST引脚200ms后复位模块*/
static void ETH_reset(void)
{
	HAL_GPIO_WritePin(RJ45_RST_GPIO_Port, RJ45_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(RJ45_RST_GPIO_Port, RJ45_RST_Pin, GPIO_PIN_SET);
}


