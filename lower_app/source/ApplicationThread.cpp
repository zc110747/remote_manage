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
#include "../include/Communication.h"
#include <signal.h>
#include <sys/time.h>

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static CApplicationReg *pApplicationReg;
static CCommunicationInfo *pCommunicationInfo;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

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
    uint16_t nIndex, nEndRegIndex, modifySize;

    nEndRegIndex = nRegIndex+nRegSize;
    if(nEndRegIndex>REG_NUM)
    {
        nEndRegIndex = REG_NUM;
    }
    modifySize = nEndRegIndex-nRegIndex;

    pthread_mutex_lock(&m_RegMutex);
    for(nIndex=0; nIndex<modifySize; nIndex++)
    {
        m_RegVal[nRegIndex+nIndex] = *(pDataStart+nIndex);
    }
    pthread_mutex_unlock(&m_RegMutex);
    #if __SYSTEM_DEBUG
    printf("set array:");
    SystemLogArray(&m_RegVal[nRegIndex], nRegSize);
    printf("set array:");
    SystemLogArray(pDataStart, nRegSize);
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
int CApplicationReg::DiffSetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, 
                                        uint8_t *pDataStart, uint8_t *pDataCompare)
{
    uint16_t nIndex, nEndRegIndex, modifySize;

    nEndRegIndex = nRegIndex+nRegSize;
    if(nEndRegIndex>REG_NUM)
        nEndRegIndex = REG_NUM;
    modifySize = nEndRegIndex-nRegIndex;

    pthread_mutex_lock(&m_RegMutex);
    if(memcmp((char *)&m_RegVal[nRegIndex], pDataCompare, nRegSize) != 0)
    {
        pthread_mutex_unlock(&m_RegMutex);
        return RT_FAIL;
    }

    for(nIndex=0; nIndex<modifySize; nIndex++)
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
 * 根据硬件状态更新寄存器
 * 
 * @param NULL
 *  
 * @return NULL
 */
void CApplicationReg::UpdateHardware(void)
{
    static uint8_t nRegInfoArray[REG_INFO_NUM];
    static uint8_t nRegCacheArray[REG_INFO_NUM];
    struct SRegInfoList *pRegInfoList;

    GetMultipleReg(REG_CONFIG_NUM, REG_INFO_NUM, nRegInfoArray);
    memcpy(nRegCacheArray, nRegInfoArray, REG_INFO_NUM);
    pRegInfoList = (struct SRegInfoList *)nRegInfoArray;

    //更新LED的状态
    pRegInfoList->s_base_status.b.led = LedStatusRead()&0x01;
    pRegInfoList->s_base_status.b.beep = BeepStatusRead()&0x01;

    if(memcmp(nRegCacheArray, nRegInfoArray, REG_INFO_NUM) != 0)
    {
        USR_DEBUG("Update HardWare, led:%d, beep:%d!\r\n", pRegInfoList->s_base_status.b.led,
        pRegInfoList->s_base_status.b.beep);
        SetMultipleReg(REG_CONFIG_NUM, REG_INFO_NUM, nRegInfoArray);
    }
}

/**
 * 根据寄存器配置更新硬件状态
 * 
 * @param cmd 执行的读写指令
 * @param pConfig 配置的数据首地址
 * @param size 配置的数据长度
 *  
 * @return NULL
 */
void CApplicationReg::WriteHardware(uint8_t cmd, uint8_t *pConfig, int size)
{
    switch(cmd)
    {
        case DEVICE_LED0:
            LedStatusConvert(pConfig[2]&0x01);
            break;
        case DEVICE_BEEP:
            BeepStatusConvert((pConfig[2]>>1)&0x01);
            break;
        case DEVICE_REBOOT:
            {
                pid_t status;

                status = system("reboot");
                if(WIFEXITED(status) && (WEXITSTATUS(status) == 0))
                {
                    USR_DEBUG("Wait For System Reboot:%d\r\n", status);
                    while(1){
                        sleep(1);
                    }
                }
                else
                {
                    USR_DEBUG("System Reboot failed, %d\r\n", status);
                }
            }
            break;
        default:
            USR_DEBUG("Invalid Cmd!\r\n");
            break;
    }
}

/**
 * 定时器触发的回调
 * 
 * @param NULL
 *  
 * @return NULL
 */
void TimerSignalHandler(int signo)
{
    char buf = 1;
    pCommunicationInfo->SendMqInformation(APP_MQ, &buf, sizeof(buf), 1);
}

/**
 * 用于触发的Timer定时器动作
 * 
 * @param NULL
 *  
 * @return NULL
 */
void CApplicationReg::TimerSingalStart(void)
{
    static struct itimerval tick = {0};

    signal(SIGALRM, TimerSignalHandler);
    
    //初始执行的定时器计数值
    tick.it_value.tv_sec = 1;
    tick.it_value.tv_usec = 0;

    //后续定时器执行的加载值
    tick.it_interval.tv_sec = 1;
    tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
    {
        USR_DEBUG("Set timer failed!\n");
    }
    USR_DEBUG("Set timer Start!\n");
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
    static uint8_t nRegCacheArray[REG_CONFIG_NUM];
    static uint8_t nRegValArray[REG_CONFIG_NUM];
    uint8_t  nRegModifyFlag;        //Reg修改状态
    uint16_t nRegConfigStatus;      //Reg配置状态
    uint8_t *pRegVal;
    uint8_t * pRegCacheVal;
    
    pRegVal = nRegValArray;
    pRegCacheVal = nRegCacheArray;
    nRegModifyFlag = 0;

    /*读取设置寄存器的值到缓存中*/
    GetMultipleReg(0, REG_CONFIG_NUM, pRegVal);
    memcpy(pRegCacheVal, pRegVal, REG_CONFIG_NUM); 

    /*有设置消息*/
    nRegConfigStatus = pRegVal[1] <<8 | pRegVal[0];
    if(nRegConfigStatus&0x01)
    {
        for(uint8_t nIndex = 1; nIndex<16; nIndex++)
        {
            uint16_t device_cmd = nRegConfigStatus>>nIndex;
            if(device_cmd == 0)
            {
                break;
            }
            else if((device_cmd&0x01) != 0)
            {
                WriteHardware(nIndex, pRegVal, REG_CONFIG_NUM);
            }         
        }

        pRegVal[0] = 0;
        pRegVal[1] = 0;
        nRegModifyFlag = 1;
    }

    /*修改后更新寄存器状态, 确定无其它应用修改后才可以真正更新*/
    if(nRegModifyFlag == 1)
    {
        if(DiffSetMultipleReg(0, REG_CONFIG_NUM, pRegVal, pRegCacheVal) == RT_OK)
        { 
            nRegModifyFlag = 0;
        }
        else
        {
            USR_DEBUG("Modify By Other Application\n");
            return RT_FAIL;
        }
    }

    /*更新内部硬件状态到信息寄存器*/
    UpdateHardware();

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
    pCommunicationInfo = GetCommunicationInfo();

    //创建消息队列
    if(pCommunicationInfo->CreateMqInfomation() == RT_INVALID_MQ)
    {
        return;
    }

    //创建应用线程
    nErr = pthread_create(&tid1, NULL, ApplicationLoopTask, NULL);	
    if(nErr != 0){
        USR_DEBUG("App Task Thread Create Err:%d\n", nErr);
    }
}

char buf[8193];

/**
 * app模块初始化
 * 
 * @param arg 线程传递的参数
 *  
 * @return NULL
 */
void *ApplicationLoopTask(void *arg)
{
    int flag;

    USR_DEBUG("App Thread Start\n");
    pApplicationReg->TimerSingalStart();

    for(;;)
    {
        flag = pCommunicationInfo->WaitMqInformation(APP_MQ, buf, sizeof(buf));
        if(flag > 0)
        {
            pApplicationReg->RefreshAllDevice();
        }
        else
        {
            USR_DEBUG("Mq Information Error, Application Tread Stop!\n");
            break;
        }     
    }

    pCommunicationInfo->CloseMqInformation(APP_MQ);
    
    //将线程和进程脱离,释放线程
    pthread_detach(pthread_self());
}