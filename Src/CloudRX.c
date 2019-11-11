/*
 * CloudRX.c
 *
 *  Created on: 2019��8��29��
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

/*json������(�����ƶ���������)*/
uint8_t jsonInPutBuf[jsonInPutBufLen] = { 0 };

/* static��������*/
BOOL static responseJsonFromCloud(uint16_t jLen);
static void responseHomepageData(void);
static void addHomeDataToJsonObj(cJSON *root,uint8_t Num);
static void addList(uint8_t *target, uint32_t len, uint32_t value);

/* �ƶ������������ */
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
	//����   ����һ��JSON���ݰ�������cJSON�ṹ��Ľṹ���л��������ݰ������ڶ��п���һ���ڴ�洢cJSON�ṹ��
	//����ֵ���ɹ�����һ��ָ���ڴ���е�cJSON��ָ�룬ʧ�ܷ���NULL
	json = cJSON_Parse((char*) jsonInPutBuf);
	memset(jsonInPutBuf,0, sizeof(jsonInPutBuf));

	if (json == NULL)
	{
		return FALSE;
	}


	jsonPtr = json;

	/**�ж�mcuID*/
	if (strcmp((const char*) hcloud.McuId, (const char*) jsonPtr->child->valuestring) == 0)
	{
		CloudIdResultFlag = 1;
	}

	if(CloudIdResultFlag != 1)
	{
		//void  cJSON_Delete(cJSON *c);
		//���ã��ͷ�λ�ڶ���cJSON�ṹ���ڴ�
		cJSON_Delete(json);
		return FALSE;
	}

	jsonPtr = jsonPtr->child->next;
	/*�ж��Ƿ���mqttType�ֶ�*/
	if (strcmp((const char*) "mqttType", (const char*) jsonPtr->string) != 0)
	{
		/*�ͷ�*/
		cJSON_Delete(json);
		return FALSE;
	}

	switch (jsonPtr->valueint)
	{
			/*Զ������õ���ҳ����*/
		case ibs_dataTypeGetHomePageData:
			responseHomepageData();
			break;
			/*Զ������Զ�̿���*/
		case ibs_dataTypeRemoteCtr:
			//responseRomoteCtr(jsonPtr);
			break;
		default:
			break;
	}

	/*�ͷ�*/
	cJSON_Delete(json);
	return TRUE;
}

static void responseHomepageData(void)
{
	uint8_t tempBuf[3];

	/*����һ����JSON����*/
	cJSON *root = cJSON_CreateObject();
	if (root == NULL)
	{
		cJSON_Delete(root);
		root = NULL;
		return;
	}

	//cJSON_AddStringToObject(): ����һ��ֵ���ַ����ļ�ֵ��JSON����
	/*������ر�� TODO*/
	cJSON_AddStringToObject(root, "netGateNum", (const char *) (hcloud.McuId));

	/*�����mqttType*/
	sprintf((char *) tempBuf, "%d", ibs_dataTypeGetHomePageData);
	cJSON_AddStringToObject(root, "mqttType", (const char*) tempBuf);


	addHomeDataToJsonObj(root,0);


	/*���������*/
	if (cJSON_PrintPreallocated(root, (char*) jsonInPutBuf, sizeof(jsonInPutBuf), FALSE))
	{
//		HAL_UART_Transmit_DMA(&huart3, (uint8_t *)jsonInPutBuf, sizeof(jsonInPutBuf));  //�������ڵ�����ͨ��USART3��DMA����������ģ��
		HAL_UART_Transmit(&huart3, (uint8_t *)jsonInPutBuf, sizeof(jsonInPutBuf), 4000);
	}

	cJSON_Delete(root);
	memset(jsonInPutBuf, 0, sizeof(jsonInPutBuf)); //����ʹ�ú�����

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

		/* List���͵����� ex:[1,1,1,0,0] */
		else
		{
			addList(str, AddrNum[i - 17], (int16_t)hcloud.ibsRealData[i]);
		}

		/*��ӵ�JSONʵ����*/
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



