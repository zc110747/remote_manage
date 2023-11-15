
#include "configfile.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>


static struct SSystemConfig SystemConfigInfo;

/*!
   ��ȡ����������ļ���Ϣ
*/
struct SSystemConfig *GetSystemConfigInfo(void)
{
    return &SystemConfigInfo;
}

/*!
   ����������ļ���Ϣ��ʼ��
*/
void SystemConfigInfoInit(void)
{
    QFile file(DEFAULT_CONFIG_FILE);

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(file.isOpen())
    {
        QString JsonConfigString = file.readAll();
        file.close();

        //����Json��������Ϣ
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(JsonConfigString.toUtf8(),&parseJsonErr);

        if(parseJsonErr.error == QJsonParseError::NoError)
        {
            QJsonObject jsonObject = document.object();
            qDebug()<<jsonObject;
            if(jsonObject.contains(QStringLiteral("Serial")))
            {
                QJsonValue jsonValueList = jsonObject.value(QStringLiteral("Serial"));
                SystemConfigInfo.m_SCom = jsonValueList.toObject()["Com"].toString();
                SystemConfigInfo.m_SBaud = jsonValueList.toObject()["Baud"].toString();
                SystemConfigInfo.m_SDataBits = jsonValueList.toObject()["DataBits"].toString();
                SystemConfigInfo.m_SStopBits = jsonValueList.toObject()["StopBits"].toString();
                SystemConfigInfo.m_SParity = jsonValueList.toObject()["Parity"].toString();
            }

            if(jsonObject.contains(QStringLiteral("Device")))
            {
                QJsonValue jsonValueList = jsonObject.value(QStringLiteral("ID"));
                SystemConfigInfo.m_SDeviceID = jsonValueList.toObject()["ID"].toString();
            }

            if(jsonObject.contains(QStringLiteral("Socket")))
            {
                QJsonValue jsonValueList = jsonObject.value(QStringLiteral("Socket"));
                SystemConfigInfo.m_SProtocol = jsonValueList.toObject()["Protocol"].toString();
                SystemConfigInfo.m_SIpAddr = jsonValueList.toObject()["IpAddr"].toString();
                SystemConfigInfo.m_SPort = jsonValueList.toObject()["Port"].toString();
                SystemConfigInfo.m_SLocalIpAddr = jsonValueList.toObject()["LocalIpAddr"].toString();
                SystemConfigInfo.m_SLocalPort = jsonValueList.toObject()["LocalPort"].toString();
                qDebug()<<SystemConfigInfo.m_SProtocol;
            }
        }
        else
        {
            qDebug()<<QString("Config Json file Parse Error, Use Default");
        }
    }
    else
    {
        SystemConfigUpdate();
    }
}

/*!
   ����Json���ݣ���д���ļ�
*/
void SystemConfigUpdate(void)
{
    QFile file(DEFAULT_CONFIG_FILE);

    file.open(QIODevice::WriteOnly | QIODevice::Text);
    if(file.isOpen())
    {
        QJsonObject MainObject;
        QJsonObject rootSerialObj;
        rootSerialObj.insert("Com", SystemConfigInfo.m_SCom);
        rootSerialObj.insert("Baud", SystemConfigInfo.m_SBaud);
        rootSerialObj.insert("DataBits", SystemConfigInfo.m_SDataBits);
        rootSerialObj.insert("StopBits", SystemConfigInfo.m_SStopBits);
        rootSerialObj.insert("Parity", SystemConfigInfo.m_SParity);
        MainObject["Serial"] = rootSerialObj;

        QJsonObject rootDeviceObj;
        rootDeviceObj.insert("ID", SystemConfigInfo.m_SDeviceID);
        MainObject["Device"] = rootDeviceObj;

        QJsonObject rootPSocketObj;
        rootPSocketObj.insert("Protocol", SystemConfigInfo.m_SProtocol);
        rootPSocketObj.insert("IpAddr", SystemConfigInfo.m_SIpAddr);
        rootPSocketObj.insert("Port", SystemConfigInfo.m_SPort);
        rootPSocketObj.insert("LocalIpAddr", SystemConfigInfo.m_SLocalIpAddr);
        rootPSocketObj.insert("LocalPort", SystemConfigInfo.m_SLocalPort);
        MainObject["Socket"] = rootPSocketObj;

        QJsonDocument document;
        document.setObject(MainObject);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        QString strJson(byteArray);

        file.write(byteArray, byteArray.size());
        qDebug()<<strJson;

        file.close();
    }
}
