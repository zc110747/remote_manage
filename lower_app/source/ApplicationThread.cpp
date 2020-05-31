/*
 * File      : app_task.cpp
 * appliction task
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-12      zc           the first version
 * 2020-5-20     zc           Code standardization 
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include "../driver/Led.h"
#include "../driver/Beep.h"
#include "../include/ApplicationThread.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/
CApplicationReg *pApplicationReg;

/**************************************************************************
* Function
***************************************************************************/
void *ApplicationLoopTask(void *arg);

/**
 * 内部数据构造函数
 * 
 * @param NULL
 *  
 * @return NULL
 */
CApplicationReg::CApplicationReg(void)
{
    /*清除内部寄存状态*/
    memset((char *)m_RegVal, 0, REG_NUM);
    if(pthread_mutex_init(&m_RegMutex, NULL) != 0)
    {
        printf("mutex init failed\n");
    }
}

/**
 * 内部数据析构函数
 * 
 * @param NULL
 *  
 * @return NULL
 */
CApplicationReg::~CApplicationReg()
{
    pthread_mutex_destroy(&m_RegMutex);
}

/**
 * 获取内部寄存器变量的值
 * 
 * @param nRegIndex  待读取寄存器的起始地址
 * @param nRegSize   读取的寄存器的数量
 * @param pDataStart 放置读取数据的首地址
 *  
 * @return 读取寄存器的数量
 */
uint16_t CApplicationReg::GetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart)
{
    uint16_t nIndex;

    if(nRegSize > REG_NUM)
        nRegSize = REG_NUM;

    pthread_mutex_lock(&m_RegMutex);
    for(nIndex=0; nIndex<nRegSize; nIndex++)
    {
        *(pDataStart+nIndex) = m_RegVal[nRegIndex+nIndex];
    }
    pthread_mutex_unlock(&m_RegMutex);
    #if __SYSTEM_DEBUG
    printf("get array:");
    SystemLogArray(m_RegVal, nRegSize);
    #endif
    return nRegSize;
}

/**
 * 设置数据到内部寄存器
 * 
 * @param nRegIndex 设置寄存器的起始地址
 * @param nRegSize  设置的寄存器的数量
 * @param pDataStart 放置设置数据的首地址
 *  
 * @return NULL
 */
void CApplicationReg::SetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart)
{
    uint16_t nIndex, nEndIndex;

    nEndIndex = nRegIndex+nRegSize;
    if(nEndIndex>REG_NUM)
        nEndIndex = REG_NUM;

    pthread_mutex_lock(&m_RegMutex);
    for(nIndex=nRegIndex; nIndex<nEndIndex; nIndex++)
    {
        m_RegVal[nIndex] = *(pDataStart+nIndex);
    }
    pthread_mutex_unlock(&m_RegMutex);
    #if __SYSTEM_DEBUG
    printf("set array:");
    SystemLogArray(m_RegVal, nRegSize);
    #endif
}

/**
 * 判断数据是否变化后修改数据，变化了表明了其它线程修改了指令
 * 
 * @param nRegIndex 寄存器的起始地址
 * @param nRegSize 读取的寄存器的数量
 * @param pDataStart 设置数据的地址
 * @param psrc   缓存的原始寄存器数据
 *  
 * @return NULL
 */
int CApplicationReg::DiffSetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, uint8_t *pDataStart, uint8_t *pDataCompare)
{
    uint16_t nIndex, nEndRegIndex;

    nEndRegIndex = nRegIndex+nRegSize;
    if(nEndRegIndex>REG_NUM)
        nEndRegIndex = REG_NUM;

    pthread_mutex_lock(&m_RegMutex);
    if(memcmp((char *)&m_RegVal[nRegIndex], pDataCompare, nRegSize) != 0)
    {
        pthread_mutex_unlock(&m_RegMutex);
        return RT_FAIL;
    }

    for(nIndex=nRegIndex; nIndex<nEndRegIndex; nIndex++)
    {
        m_RegVal[nIndex] = *(pDataStart+nIndex);
    }
    pthread_mutex_unlock(&m_RegMutex);
    #if __SYSTEM_DEBUG
    printf("diff array:");
    SystemLogArray(m_RegVal, nRegSize);
    #endif
    return RT_OK;
}

/**
 * 根据寄存器更新内部硬件参数
 * 
 * @param NULL
 *  
 * @return NULL
 */
int CApplicationReg::RefreshAllDevice(void)
{
    uint8_t *pRegVal;           
    uint8_t *pRegCacheVal;
    uint8_t  nRegModifyFlag;        //Reg修改状态
    uint16_t nRegConfigStatus;      //Reg配置状态

    pRegVal = (uint8_t *)malloc(REG_CONFIG_NUM);
    pRegCacheVal = (uint8_t *)malloc(REG_CONFIG_NUM);
    nRegModifyFlag = 0;

    if(pRegVal != NULL && pRegCacheVal != NULL)
    {
        /*读取所有的寄存值并复制到缓存中*/
        GetMultipleReg(0, REG_CONFIG_NUM, pRegVal);
        memcpy(pRegCacheVal, pRegVal, REG_CONFIG_NUM); 

        /*有设置消息*/
        nRegConfigStatus = pRegVal[1] <<8 | pRegVal[0];
        if(nRegConfigStatus&0x01)
        {
            /*LED设置处理*/
            for(uint8_t nIndex = 1; nIndex<16; nIndex++)
            {
                uint16_t device_cmd = nRegConfigStatus>>nIndex;
                if(device_cmd == 0)
                {
                    break;
                }
                else if((device_cmd&0x01) != 0)
                {
                    switch(nIndex)
                    {
                        case DEVICE_LED0:
                            LedStatusConvert(pRegVal[2]&0x01);
                        break;
                        case DEVICE_BEEP:
                            BeepStatusConvert((pRegVal[2]>>1)&0x01);
                        break;
                        case DEVICE_REBOOT:
                            system("reboot");
                            while(1){
                                USR_DEBUG("wait for reboot\r\n");
                                sleep(1);
                            }
                        break;
                        default:
                        break;
                    }
                }         
            }

            pRegVal[0] = 0;
            pRegVal[1] = 0;
            nRegModifyFlag = 1;
        }
  
        /*更新寄存器状态*/
        if(nRegModifyFlag == 1){
            if(DiffSetMultipleReg(0, REG_CONFIG_NUM, pRegVal, pRegCacheVal) == RT_OK){
                nRegModifyFlag = 0;
            }
            else
            {
                free(pRegVal);
                free(pRegCacheVal);
                USR_DEBUG("modify by other interface\n");
                return RT_FAIL;
            }
        }

        free(pRegVal);
        free(pRegCacheVal);
    }
    else{
        USR_DEBUG("malloc error\n");
    }

    return RT_OK;
}

/**
 * 获取寄存器数据结构体指针
 * 
 * @param NULL
 *  
 * @return 返回寄存器的信息
 */
CApplicationReg *GetApplicationReg(void)
{
    return pApplicationReg;
}

/**
 * 设置寄存器结构体指针
 * 
 * @param NULL
 *  
 * @return 返回寄存器的信息
 */
void SetApplicationReg(CApplicationReg *pAppReg)
{
   pApplicationReg = pAppReg;
}

/**
 * app模块任务初始化
 * 
 * @param NULL
 *  
 * @return NULL
 */
void ApplicationThreadInit(void)
{
    pthread_t tid1;
    int nErr;
    pApplicationReg = new CApplicationReg();

    nErr = pthread_create(&tid1, NULL, ApplicationLoopTask, NULL);	
    if(nErr != 0){
        USR_DEBUG("app task thread create nErr, %d\n", nErr);
    }
}

/**
 * app模块初始化
 * 
 * @param arg 线程传递的参数
 *  
 * @return NULL
 */
void *ApplicationLoopTask(void *arg)
{
    USR_DEBUG("app Task Start\n");
    for(;;){
        if(pApplicationReg->RefreshAllDevice() == RT_OK){
            usleep(800);      //指令处理完休眠，非RT_OK表示仍有指令需要处理
        }
    }
}