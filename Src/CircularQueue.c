/*
 * CircularQueue.c
 *
 *  Created on: 2019��8��21��
 *      Author: Breuning
 *
 *  �̳�Linux�ں� kfifoԭ��
 *  �ŵ㣺ֻ�мӼ������㣬�����Ƚ��ȳ�ԭ�򣬸�Ч�Ļ���Ͷ�ȡ��
 *
 */


#include "CircularQueue.h"
#include "string.h"
#include "main.h"
#include "screen_com.h"
/*��ʼ���������壺
*CircularQueue��Ϊ���γ����ļ�¼���Ǹ��ṹ��
*memAdd ��Ϊʵ�����ݴ洢����
*len ��¼ʵ�ʴ洢������󳤶�
*/
BOOL CQ_init(CQ_handleTypeDef *CircularQueue ,uint8_t *memAdd, uint16_t len)
{

    CircularQueue->size = len;

    if (!IS_POWER_OF_2(CircularQueue->size))
        return FALSE;

    if(memAdd == NULL)
    {
    	return FALSE;
    }

    CircularQueue->dataBufer = memAdd;

    memset(CircularQueue->dataBufer, 0, len);
    CircularQueue->entrance = CircularQueue->exit = 0;

    return TRUE;
}

/*���λ������ж��Ƿ�Ϊ�գ�
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*��д�������룬��ȡ���ݳ���һ�£���ô������Ϊ��return 1
*/
BOOL CQ_isEmpty(CQ_handleTypeDef *CircularQueue)
{
    if (CircularQueue->entrance == CircularQueue->exit)
        return TRUE;
    else
        return FALSE;
}

/*���λ������ж��Ƿ�Ϊ�����������⣡
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*�� ���ѡ�д�������룬��ȥ ���ѡ���ȡ���ݳ��� = ʣ��ռ� ʣ��ռ�==�ܳ���  �ж���
*/
BOOL CQ_isFull(CQ_handleTypeDef *CircularQueue)
{
    if ((CircularQueue->entrance - CircularQueue->exit) == CircularQueue->size)//MAXSIZE=5,Q.rear=2,Q.front=3��
        return TRUE;//��
    else
        return FALSE;
}

/*���λ�������ȡʣ��ռ䳤�ȣ�
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*�� ���ѡ�д�������룬��ȥ ���ѡ���ȡ���ݳ��� = ʣ��ռ� 
*/
uint32_t CQ_getLength(CQ_handleTypeDef*CircularQueue)
{
    return (CircularQueue->entrance - CircularQueue->exit);
}

/*���λ�������ղ�����
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
* �Ѷ��Ϳɶ����ݳ�������      ʵ�ʴ洢�����
*/
void CQ_emptyData(CQ_handleTypeDef*CircularQueue)
{
    CircularQueue->entrance = CircularQueue->exit = 0;
    memset(CircularQueue->dataBufer, 0, CircularQueue->size);
}

/*���λ��������������ݣ��������ݹ��������޸ģ�ÿ������֡��ͷ�ȴ��뱾֡�����ݳ��ȣ�����ÿ����ȡһ���ֽڵõ������ȣ��ٰ�����ȡ��
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*sourceBuf Ϊʵ�ʴ洢����ַ
*len Ϊ�������ݴ��볤�� 
*ʹ���ܳ���-��д��+��ȡ��� == ���ÿռ��С
*��kfifo->sizeȡģ�������ת��Ϊ�����㣬�磺kfifo->in % kfifo->size ����ת��Ϊ kfifo->in & (kfifo->size �C 1)
*/
uint32_t CQ_putData(CQ_handleTypeDef *CircularQueue, uint8_t * sourceBuf, uint32_t len)
{
    uint32_t size = 0;
    uint32_t lenth = 1;
    uint32_t pack_len = len;
    /*�˴δ����ʵ�ʴ�С��ȡ ʣ��ռ� �� Ŀ���������  ����ֵС���Ǹ�*/
    len = GET_MIN(len+lenth, CircularQueue->size - CircularQueue->entrance + CircularQueue->exit);//������ͷ���������ݳ��ȼ�¼
    
    /*&(size-1)����ȡģ���㣬ͬ��ԭ���õ��˴δ��������ڵ�ĩβ�Ĵ�С*/
    size = GET_MIN(len, CircularQueue->size - (CircularQueue->entrance & (CircularQueue->size - 1)));//5
    memcpy(CircularQueue->dataBufer + (CircularQueue->entrance & (CircularQueue->size - 1)), &pack_len, lenth);//�������ݰ����ȣ�ռ1�ֽ�
    memcpy(CircularQueue->dataBufer + (CircularQueue->entrance & (CircularQueue->size - 1))+lenth, sourceBuf, size-lenth);//��ַ�ٴ�ƫ��cmd_len������β������һ��
    memcpy(CircularQueue->dataBufer, sourceBuf + size - lenth, len - size);//�´���Ҫд������ݳ��ȣ����Ȳ���-1����Ϊ��д�룬��ʼ��ַ��Ҫ-1����Ϊʵ�ʰ�û��ô����

    /*�����޷������ݵ��������*/
    CircularQueue->entrance += len; //д�����ݼ�¼

    return len;
}
/*
*���λ������������ݣ�
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*targetBuf Ϊ��ʱ���ݴ���
*len Ϊ�������ݶ�ȡ���� 
*ʹ��д�볤��-��ȡ�ĳ��� == ʣ��ɶ�  ��Ҫ��   ȡСֵ
*
*/
uint32_t CQ_getData(CQ_handleTypeDef *CircularQueue, uint8_t *targetBuf, uint32_t len)
{
    uint32_t size = 0;

    /*�˴ζ�ȡ��ʵ�ʴ�С��ȡ �����¼����ݴ�С �� Ŀ���ȡ����  ����ֵС���Ǹ�*/
    len = GET_MIN(len, CircularQueue->entrance - CircularQueue->exit);// �����ܴ�С10  д����5 - �Ѷ�4  == 1 δ��   Ҫ��5��   ����1
    /*ԭ����ͬ����*/
    size = GET_MIN(len, CircularQueue->size - (CircularQueue->exit & (CircularQueue->size - 1)));//10 - 0 > 1 ����1
    memcpy(targetBuf, CircularQueue->dataBufer + (CircularQueue->exit & (CircularQueue->size - 1)), size);//ƫ��0�� ����һ���ֽ�
    memcpy(targetBuf + size, CircularQueue->dataBufer, len - size);// �洢��ƫ��0���ֽ�
    /*�����޷������ݵ��������*/
    CircularQueue->exit += len;//ȡ�����ݼ� len ��¼

    return len;
}


/*���λ��������������ݣ��������ݹ��������޸ģ�ÿ������֡��ͷ�ȴ��뱾֡�����ݳ��ȣ�����ÿ����ȡһ���ֽڵõ������ȣ��ٰ�����ȡ��
*CircularQueue��Ϊ���γ����ļ�¼�����Ǹ��ṹ��
*sourceBuf Ϊʵ�ʴ洢����ַ
*len Ϊ�������ݴ��볤�� 
*ʹ���ܳ���-��д��+��ȡ��� == ���ÿռ��С
*��kfifo->sizeȡģ�������ת��Ϊ�����㣬�磺kfifo->in % kfifo->size ����ת��Ϊ kfifo->in & (kfifo->size �C 1)
*/
uint32_t DQ_putData(CQ_handleTypeDef *CircularQueue, uint8_t * sourceBuf, uint32_t len)
{
    uint32_t size = 0;
    /*�˴δ����ʵ�ʴ�С��ȡ ʣ��ռ� �� Ŀ���������  ����ֵС���Ǹ�*/
    len = GET_MIN(len, CircularQueue->size - CircularQueue->entrance + CircularQueue->exit);//������ͷ���������ݳ��ȼ�¼
    
    /*&(size-1)����ȡģ���㣬ͬ��ԭ���õ��˴δ��������ڵ�ĩβ�Ĵ�С*/
    size = GET_MIN(len, CircularQueue->size - (CircularQueue->entrance & (CircularQueue->size - 1)));
    memcpy(CircularQueue->dataBufer + (CircularQueue->entrance & (CircularQueue->size - 1)), sourceBuf, size);
    memcpy(CircularQueue->dataBufer, sourceBuf + size, len - size);//�´���Ҫд������ݳ���

    /*�����޷������ݵ��������*/
    CircularQueue->entrance += len; //д�����ݼ�¼

    return len;
}