/*
 * CloudRX.c
 *
 *  Created on: 2019年8月29日
 *      Author: Breuning
 */

#include "main.h"
#include "CloudRX.h"
#include "Cloud.h"
#include "cJSON.h"
#include <string.h>
#include "usart.h"

uint8_t RJ45_RX[RJ45_RX_LEN] = { 0 };
cloudRxCrt_t cloudRxCrt;

/*json缓存区(接收云端下行数据)*/
uint8_t jsonInPutBuf[jsonInPutBufLen] = { 0 };

/* static函数声明*/
BOOL static responseJsonFromCloud(uint16_t jLen);
static void responseHomepageData(void);
static void addHomeDataToJsonObj(cJSON *root,uint8_t Num);
static void addList(uint8_t *target, uint32_t len, uint32_t value);

/* 云端下行数据入口 */
inline void CLUOD_SaveRemoteData(void)
{
	if (cloudRxCrt.isRxEnable == TRUE)
	{
		static uint8_t CloudRecvByets=0;
		cloudRxQueueMem[CloudRecvByets] = RJ45_RX[0];


		if(cloudRxQueueMem[0] == '{')
		{
			if (cloudRxQueueMem[CloudRecvByets] == '}')
			{
				cloudRxCrt.isRxJsonMsgComplete = TRUE;
				cloudRxCrt.RxJsonMsgCount = CloudRecvByets + 1;
			}
			CloudRecvByets++;
		}
		else
		{
			CloudRecvByets=0;
		}
	}
}

void CloudDataRecv(void)
{
	uint16_t rxJsonLen = 0;
	if (cloudRxCrt.isRxJsonMsgComplete)
	{
		rxJsonLen = cloudRxCrt.RxJsonMsgCount;
		if (rxJsonLen != 0)
		{
			memcpy(jsonInPutBuf,cloudRxQueueMem, sizeof(cloudRxQueueMem));
			memset(cloudRxQueueMem,0, sizeof(cloudRxQueueMem));

			responseJsonFromCloud(rxJsonLen);
		}
		cloudRxCrt.isRxJsonMsgComplete = FALSE;
	}
}

BOOL static responseJsonFromCloud(uint16_t jLen)
{
	cJSON *json = NULL;
	cJSON *jsonPtr = NULL;
	uint8_t CloudIdResultFlag = 0;

	if (jLen == 0)
	{
		return FALSE;
	}

	//cJSON *cJSON_Parse(const char *value);
	//作用   ：将一个JSON数据包，按照cJSON结构体的结构序列化整个数据包，并在堆中开辟一块内存存储cJSON结构体
	//返回值：成功返回一个指向内存块中的cJSON的指针，失败返回NULL
	json = cJSON_Parse((char*) jsonInPutBuf);
	memset(jsonInPutBuf,0, sizeof(jsonInPutBuf));

	if (json == NULL)
	{
		return FALSE;
	}


	jsonPtr = json;

	/**判断mcuID*/
	if (strcmp((const char*) hcloud.McuId, (const char*) jsonPtr->child->valuestring) == 0)
	{
		CloudIdResultFlag = 1;
	}

	if(CloudIdResultFlag != 1)
	{
		//void  cJSON_Delete(cJSON *c);
		//作用：释放位于堆中cJSON结构体内存
		cJSON_Delete(json);
		return FALSE;
	}

	jsonPtr = jsonPtr->child->next;
	/*判断是否有mqttType字段*/
	if (strcmp((const char*) "mqttType", (const char*) jsonPtr->string) != 0)
	{
		/*释放*/
		cJSON_Delete(json);
		return FALSE;
	}

	switch (jsonPtr->valueint)
	{
			/*远程请求得到首页数据*/
		case ibs_dataTypeGetHomePageData:
			responseHomepageData();
			break;
			/*远程请求远程控制*/
		case ibs_dataTypeRemoteCtr:
			//responseRomoteCtr(jsonPtr);
			break;
		default:
			break;
	}

	/*释放*/
	cJSON_Delete(json);
	return TRUE;
}

static void responseHomepageData(void)
{
	uint8_t tempBuf[3];

	/*创建一个空JSON对象*/
	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
	{
		cJSON_Delete(root);
		root = NULL;
		return;
	}

	//cJSON_AddStringToObject(): 增加一个值是字符串的键值到JSON对象
	/*添加网关编号 TODO*/
	cJSON_AddStringToObject(root, "netGateNum", (const char *) (hcloud.McuId));

	/*添加设mqttType*/
	sprintf((char *) tempBuf, "%d", ibs_dataTypeGetHomePageData);
	cJSON_AddStringToObject(root, "mqttType", (const char*) tempBuf);


	addHomeDataToJsonObj(root,0);


	/*输出并发送*/
	if (cJSON_PrintPreallocated(root, (char*) jsonInPutBuf, sizeof(jsonInPutBuf), FALSE))
	{
//		HAL_UART_Transmit_DMA(&huart3, (uint8_t *)jsonInPutBuf, sizeof(jsonInPutBuf));  //将数组内的数据通过USART3由DMA发送至上云模块
		HAL_UART_Transmit(&huart3, (uint8_t *)jsonInPutBuf, sizeof(jsonInPutBuf), 4000);
	}

	cJSON_Delete(root);
	memset(jsonInPutBuf, 0, sizeof(jsonInPutBuf)); //建议使用后清零

}

static void addHomeDataToJsonObj(cJSON *root,uint8_t Num)
{

	uint8_t str[32];
	uint8_t i;

	for (i = 6; i < 23; i++)
	{
		memset(str, 0, sizeof(str));

		if (i < 17)
		{
			sprintf((char*) str, "%2.1f", (float)hcloud.ibsRealData[i]);
		}

		/* List类型的数据 ex:[1,1,1,0,0] */
		else
		{
			addList(str, AddrNum[i - 17], (int16_t)hcloud.ibsRealData[i]);
		}

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



