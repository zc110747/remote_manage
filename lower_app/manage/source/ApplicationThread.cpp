/*
 * File      : ApplicationThread.cpp
 * 硬件相关的状态配置和读取的线程处理
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-12      zc           the first version
 * 2020-5-20      zc           Code standardization 
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include "ApplicationThread.hpp"
#include "GroupApp/FifoManage.hpp"
#include "GroupApp/MqManage.hpp"
#include "../driver/driver.hpp"

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
static CBaseMessageInfo *pBaseMessageInfo;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/*硬件状态相关应用处理接口*/
void *ApplicationLoopThread(void *arg);

/**************************************************************************
* Function
***************************************************************************/
/**
 * 构造函数
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
 * 析构函数
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
 * 从内部共享数据寄存器中读取数据
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

    assert(pDataStart != nullptr);

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
 * 将数据写入内部共享的数据寄存器
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

    assert(pDataStart != nullptr);

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
 * 带判断是否修改的写入寄存器实现，变化了表明了其它线程修改了指令
 * 
 * @param nRegIndex 寄存器的起始地址
 * @param nRegSize 读取的寄存器的数量
 * @param pDataStart 设置数据的地址
 * @param psrc   缓存的原始寄存器数据
 *  
 * @return 寄存器的比较写入处理结果
 */
int CApplicationReg::DiffSetMultipleReg(uint16_t nRegIndex, uint16_t nRegSize, 
                                        uint8_t *pDataStart, uint8_t *pDataCompare)
{
    uint16_t nIndex, nEndRegIndex, modifySize;

    assert(pDataStart != nullptr && pDataCompare != nullptr);

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

/**\n
 * 读取硬件状态并更新到寄存器中
 * 
 * @param NULL
 *  
 * @return NULL
 */
bool CApplicationReg::ReadDeviceStatus(void)
{
    static uint8_t nRegInfoArray[REG_INFO_NUM];
    static uint8_t nRegCacheArray[REG_INFO_NUM];
    struct SRegInfoList *pRegInfoList;
    bool isError = false;

    GetMultipleReg(REG_CONFIG_NUM, REG_INFO_NUM, nRegInfoArray);
    memcpy(nRegCacheArray, nRegInfoArray, REG_INFO_NUM);
    pRegInfoList = (struct SRegInfoList *)nRegInfoArray;

    //更新led的状态
    if(ledTheOne::getInstance()->readIoStatus())
    {
        pRegInfoList->s_base_status.b.led = ledTheOne::getInstance()->getIoStatus();
    }
    else
    {
        isError = true;
    }

    //更新beep的状态
    if(beepTheOne::getInstance()->readIoStatus())
    {
        pRegInfoList->s_base_status.b.beep = beepTheOne::getInstance()->getIoStatus();
    }
    else
        isError = true;

    if(ICMDevice::getInstance()->readInfo())
    {
        ICM_INFO *pIcmInfo;
        pIcmInfo = ICMDevice::getInstance()->getInfo();
        
        pRegInfoList->sensor_gyro_x = pIcmInfo->gyro_x_adc;
        pRegInfoList->sensor_gyro_y = pIcmInfo->gyro_y_adc;
        pRegInfoList->sensor_gyro_z = pIcmInfo->gyro_z_adc;
        pRegInfoList->sensor_accel_x = pIcmInfo->accel_x_adc;
        pRegInfoList->sensor_accel_y = pIcmInfo->accel_y_adc;
        pRegInfoList->sensor_accel_z = pIcmInfo->accel_z_adc;
        pRegInfoList->sensor_temp = pIcmInfo->temp_adc;
    }
    else
        isError = true;

    if(APDevice::getInstance()->readInfo())
    {
        //读取I2c设备状态
        AP_INFO *pApInfo;
        pApInfo = APDevice::getInstance()->getInfo();
        pRegInfoList->sensor_ir = pApInfo->ir;
        pRegInfoList->sensor_ps = pApInfo->ps;
        pRegInfoList->sensor_als = pApInfo->als;
    }
    else
        isError = true;

    if(isError)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "[%s]ReadDeviceStatus error!", __FILE__);
        return false;
    }

    //if read not error and convert, write.
    if(memcmp(nRegCacheArray, nRegInfoArray, REG_INFO_NUM) != 0)
    {
        SetMultipleReg(REG_CONFIG_NUM, REG_INFO_NUM, nRegInfoArray);
    }

    return true;
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
void CApplicationReg::WriteDeviceConfig(uint8_t cmd, uint8_t *pConfig, int size)
{
    assert(pConfig != nullptr);

    switch(cmd)
    {
        case DEVICE_LED0:
            ledTheOne::getInstance()->writeIoStatus(pConfig[2]&0x01);
            break;
        case DEVICE_BEEP:
            beepTheOne::getInstance()->writeIoStatus((pConfig[2]>>1)&0x01);
            break;
        case DEVICE_REBOOT:
            {
                pid_t status;

                status = system("reboot");
                if(WIFEXITED(status) && (WEXITSTATUS(status) == 0))
                {
                    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "wait for system reboot!");
                    while(1){
                        sleep(1);
                    }
                }
                else
                {
                    PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "system reboot failed!");
                }
            }
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Invalid Command!");
            break;
    }
}

/**
 * 状态更新定时触发的执行函数
 * 
 * @param signo 触发传入的数据
 *  
 * @return NULL
 */
static void TimerSignalHandler(int signo)
{
    char buf = 1;

    pBaseMessageInfo->SendInformation(APP_BASE_MESSAGE, &buf, sizeof(buf), 1);
}

/**
 * 状态更新定时触发源启动
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
    tick.it_value.tv_sec = 2;
    tick.it_value.tv_usec = 0;

    //后续定时器执行的加载值
    tick.it_interval.tv_sec = 2;
    tick.it_interval.tv_usec = 0;

    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Timer Signal failed!");
    }
    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "Timer Signal Successed!");
}

/**
 * 进行所有硬件的处理, 包含硬件配置和状态读取
 * 
 * @param NULL
 *  
 * @return 硬件处理的执行结果
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
                WriteDeviceConfig(nIndex, pRegVal, REG_CONFIG_NUM);
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
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "No write, Changed by others!");
            return RT_FAIL;
        }
    }

    /*更新内部硬件状态到信息寄存器*/
    ReadDeviceStatus();

    return RT_OK;
}

/**
 * 获取共享寄存器数据结构体指针
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
 * 设置共享寄存器结构体指针
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
 * 硬件和状态相关应用处理线程初始化执行
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
    #if __WORK_IN_WSL == 1
    pBaseMessageInfo = static_cast<CBaseMessageInfo *>(GetFifoMessageInfo());
    #else
    pBaseMessageInfo = static_cast<CBaseMessageInfo *>(GetMqMessageInfo());
    #endif

    //创建消息队列
    if(pBaseMessageInfo->CreateInfomation() == RT_INVALID_MQ)
    {
        return;
    }

    //创建应用线程
    nErr = pthread_create(&tid1, NULL, ApplicationLoopThread, NULL);	
    if(nErr != 0){
        USR_DEBUG("App Task Thread Create Err:%d\n", nErr);
    }
}

/**
 * 硬件和状态相关应用处理执行函数
 * 
 * @param arg 线程传递的参数
 *  
 * @return NULL
 */
void *ApplicationLoopThread(void *arg)
{
    int Flag;
    char InfoData;  
    
    USR_DEBUG("App Thread Start\n");
    pApplicationReg->TimerSingalStart();

    for(;;)
    {
        Flag = pBaseMessageInfo->WaitInformation(APP_BASE_MESSAGE, &InfoData, sizeof(InfoData));
        if(Flag > 0)
        {
            pApplicationReg->RefreshAllDevice();
        }
        else
        {
            USR_DEBUG("App Information Error, Application Tread Stop!\n");
            break;
        }     
    }

    pBaseMessageInfo->CloseInformation(APP_BASE_MESSAGE);
    
    //将线程和进程脱离,释放线程
    pthread_detach(pthread_self());

    return (void *)0;
}