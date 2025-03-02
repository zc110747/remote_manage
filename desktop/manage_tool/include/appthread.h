#ifndef APPTHREAD_H
#define APPTHREAD_H

#include <QThread>
#include "protocol.h"
#include "udpclient.h"

class CAppThreadInfo:public QThread
{
    Q_OBJECT

public:
    CAppThreadInfo()
    {
        m_pQueue = new CProtocolQueue();
    }
    ~CAppThreadInfo()
    {
        delete  m_pQueue;
        m_pQueue = nullptr;
    }
    CProtocolQueue *m_pQueue;

    int QueuePost(SSendBuffer *pSendBuffer)
    {
        if (m_pQueue != nullptr)
        {
            return m_pQueue->QueuePost(pSendBuffer);
        }
        return QUEUE_INFO_INVALID;
    }

protected:
    virtual void run();

    void CloseThread(){
        m_nIsStop = 0;
    }

private:
    volatile bool m_nIsStop{false};
};

void AppThreadInit(void);
CAppThreadInfo *GetAppThreadInfo();
#endif // APPTHREAD_H
