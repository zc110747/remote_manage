#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"
#include "uartclient.h"
#include "tcpclient.h"
#include "commandinfo.h"

static CUartProtocolThreadInfo *pMainUartProtocolThreadInfo;
static CTcpSocketThreadInfo *pMainTcpSocketThreadInfo;
static PROTOCOL_STATUS protocol_flag;

#define FRAM_STYLE  "QFrame{border-radius:10px}"

//函数声明
void init_btn_disable(Ui::MainWindow *ui);
void init_btn_enable(Ui::MainWindow *ui);
void QFrame_Init(Ui::MainWindow *ui);

//类的实现
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFrame_Init(ui);
    init();
    CommandInfoInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void QFrame_Init(Ui::MainWindow *ui)
{
    ui->centralwidget->setMinimumSize(QSize(710, 720));
    ui->frame_uart->setFrameShape(QFrame::Shape::Box);
    ui->frame_uart->setFrameShadow(QFrame::Shadow::Sunken);
    ui->frame_dev->setFrameShape(QFrame::Shape::Box);
    ui->frame_dev->setFrameShadow(QFrame::Shadow::Sunken);
    ui->frame_socket->setFrameShape(QFrame::Shape::Box);
    ui->frame_socket->setFrameShadow(QFrame::Shadow::Sunken);
}

void MainWindow::init()
{
    //添加COM口
    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }
    ui->combo_box_com->addItems(comList);

    //波特率选项
    QStringList BaudList;
    BaudList <<"9600"<<"38400"<<"76800"<<"115200"<<"230400";
    ui->combo_box_baud->addItems(BaudList);
    ui->combo_box_baud->setCurrentIndex(3);

    //数据位选项
    QStringList dataBitsList;
    dataBitsList <<"6" << "7" << "8"<<"9";
    ui->combo_box_data->addItems(dataBitsList);
    ui->combo_box_data->setCurrentIndex(2);

    //停止位选项
    QStringList StopBitsList;
    StopBitsList<<"1"<<"2";
    ui->combo_box_stop->addItems(StopBitsList);
    ui->combo_box_stop->setCurrentIndex(0);

    //校验位
    QStringList ParityList;
    ParityList<<"N"<<"Odd"<<"Even";
    ui->combo_box_parity->addItems(ParityList);
    ui->combo_box_parity->setCurrentIndex(0);

    //设置协议类型
    QStringList SocketTypeList;
    SocketTypeList<<"TCP"<<"UDP";
    ui->combo_box_socket_type->addItems(SocketTypeList);
    ui->combo_box_parity->setCurrentIndex(0);

    //正则限制部分输入需要为数据
    QRegExp regx("[0-9]+$");
    QValidator *validator_time = new QRegExpValidator(regx,  ui->line_edit_time);
    ui->line_edit_time->setValidator( validator_time );
    QValidator *validator_id = new QRegExpValidator(regx,  ui->line_edit_dev_id);
    ui->line_edit_dev_id->setValidator( validator_id );

    //默认按键配置不可操作
    init_btn_disable(ui);
    ui->btn_uart_close->setDisabled(true);
    ui->btn_socket_close->setDisabled(true);

    //Uart应用相关线程和数据初始化
    UartThreadInit();
    pMainUartProtocolThreadInfo = GetUartProtocolInfo();
    connect(pMainUartProtocolThreadInfo, SIGNAL(send_edit_recv(QString)), this, SLOT(append_text_edit_recv(QString)));
    connect(pMainUartProtocolThreadInfo, SIGNAL(send_edit_test(QString)), this, SLOT(append_text_edit_test(QString)));
    pMainUartProtocolThreadInfo->start();

    //Socket应用相关线程和数据初始化
    TcpClientSocketInit();
    pMainTcpSocketThreadInfo = GetTcpClientSocket();
    connect(pMainTcpSocketThreadInfo, SIGNAL(send_edit_recv(QString)), this, SLOT(append_text_edit_recv(QString)));
    connect(pMainTcpSocketThreadInfo, SIGNAL(send_edit_test(QString)), this, SLOT(append_text_edit_test(QString)));
    pMainTcpSocketThreadInfo->start();
}

//接收窗口的显示执行
void MainWindow::append_text_edit_recv(QString s)
{
    ui->text_edit_recv->append(s);
}

//测试窗口的显示执行
void MainWindow::append_text_edit_test(QString s)
{
    ui->text_edit_test->append(s);
}

//关闭应用后能操作按键配置
void init_btn_disable(Ui::MainWindow *ui)
{
    ui->btn_led_on->setDisabled(true);
    ui->btn_led_off->setDisabled(true);
    ui->btn_reboot->setDisabled(true);
    ui->btn_beep_on->setDisabled(true);
    ui->btn_beep_off->setDisabled(true);
    ui->btn_refresh->setDisabled(true);
    ui->btn_send_cmd->setDisabled(true);
    ui->line_edit_dev_id->setReadOnly(false);
}

//开启应用后能操作按键配置
void init_btn_enable(Ui::MainWindow *ui)
{
    ui->btn_led_on->setEnabled(true);
    ui->btn_led_off->setEnabled(true);
    ui->btn_reboot->setEnabled(true);
    ui->btn_beep_on->setEnabled(true);
    ui->btn_beep_off->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
    ui->btn_send_cmd->setEnabled(true);
    ui->line_edit_dev_id->setReadOnly(true);
}

void CmdSendBuffer(uint8_t *pStart, uint16_t nSize, int nCommand, bool isThrough)
{
    QString Strbuf;
    SSendBuffer *pSendbuf = new SSendBuffer(pStart, nSize, nCommand, isThrough);
    if(protocol_flag == PROTOCOL_UART)
    {
        pMainUartProtocolThreadInfo->PostQueue(pSendbuf);
    }
    else if(protocol_flag == PROTOCOL_TCP)
    {
        pMainTcpSocketThreadInfo->PostQueue(pSendbuf);
    }
    else
    {
        delete pSendbuf;
    }
}

//打开LED
void MainWindow::on_btn_led_on_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(LED_ON_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false);
}

//关闭LED
void MainWindow::on_btn_led_off_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(LED_OFF_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false);
}

//打开蜂鸣器
void MainWindow::on_btn_beep_on_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(BEEP_ON_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false);
}

//关闭蜂鸣器
void MainWindow::on_btn_beep_off_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(BEEP_OFF_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false);
}

//复位设备
void MainWindow::on_btn_reboot_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(DEV_REBOOT_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false);
}

//指令直接发送的命令
void MainWindow::on_btn_send_cmd_clicked()
{
    static uint8_t nCacheBuf[256];
    bool bStatus;
    uint8_t nSize;
    QStringList QStrArr = ui->line_edit_cmd->text().split(" ");

    nSize = QStrArr.size();
    for(int index=0; index<nSize;index++){
        nCacheBuf[index] = QStrArr[index].toInt(&bStatus, 16);
    }

    CmdSendBuffer(nCacheBuf, nSize, DEV_WRITE_THROUGH_CMD, true);
}

//关闭串口
void MainWindow::on_btn_uart_close_clicked()
{
    init_btn_disable(ui);
    pMainUartProtocolThreadInfo->m_pQueue->clear();      //清空队列
    pMainUartProtocolThreadInfo->m_bComStatus = false;
    pMainUartProtocolThreadInfo->m_pSerialPortCom->close();
    pMainUartProtocolThreadInfo->m_pSerialPortCom->deleteLater();
    ui->btn_uart_close->setDisabled(true);
    ui->btn_uart_open->setEnabled(true);
    ui->btn_socket_open->setEnabled(true);
    ui->btn_socket_close->setEnabled(true);
    ui->combo_box_com->setEnabled(true);
    ui->combo_box_baud->setEnabled(true);
    ui->combo_box_data->setEnabled(true);
    ui->combo_box_stop->setEnabled(true);
    ui->combo_box_parity->setEnabled(true);
    append_text_edit_test(QString::fromLocal8Bit("serial close!"));
}

//开启串口
void MainWindow::on_btn_uart_open_clicked()
{
    pMainUartProtocolThreadInfo->m_pSerialPortCom = new QextSerialPort(ui->combo_box_com->currentText(), QextSerialPort::Polling);
    pMainUartProtocolThreadInfo->m_bComStatus = pMainUartProtocolThreadInfo->m_pSerialPortCom->open(QIODevice::ReadWrite);

    if(pMainUartProtocolThreadInfo->m_bComStatus)
    {
        //清除缓存区
        pMainUartProtocolThreadInfo->m_pSerialPortCom ->flush();
        //设置波特率
        pMainUartProtocolThreadInfo->m_pSerialPortCom ->setBaudRate((BaudRateType)ui->combo_box_baud->currentText().toInt());
        //设置数据位
        pMainUartProtocolThreadInfo->m_pSerialPortCom->setDataBits((DataBitsType)ui->combo_box_data->currentText().toInt());
        //设置校验位
        pMainUartProtocolThreadInfo->m_pSerialPortCom->setParity((ParityType)ui->combo_box_parity->currentText().toInt());
        //设置停止位
        pMainUartProtocolThreadInfo->m_pSerialPortCom->setStopBits((StopBitsType)ui->combo_box_stop->currentText().toInt());
        pMainUartProtocolThreadInfo->m_pSerialPortCom->setFlowControl(FLOW_OFF);
        pMainUartProtocolThreadInfo->m_pSerialPortCom ->setTimeout(10);
        init_btn_enable(ui);
        pMainUartProtocolThreadInfo->SetId(ui->line_edit_dev_id->text().toShort());
        ui->btn_uart_close->setEnabled(true);
        ui->btn_uart_open->setDisabled(true);
        ui->btn_socket_open->setDisabled(true);
        ui->btn_socket_close->setDisabled(true);
        ui->combo_box_com->setDisabled(true);
        ui->combo_box_baud->setDisabled(true);
        ui->combo_box_data->setDisabled(true);
        ui->combo_box_stop->setDisabled(true);
        ui->combo_box_parity->setDisabled(true);
        append_text_edit_test(QString::fromLocal8Bit("serial open success!"));
        protocol_flag = PROTOCOL_UART;
    }
    else
    {
        pMainUartProtocolThreadInfo->m_pSerialPortCom->deleteLater();
        pMainUartProtocolThreadInfo->m_bComStatus = false;
        append_text_edit_test(QString::fromLocal8Bit("serial open failed!"));
    }
}

//清理接收数据框
void MainWindow::on_btn_clear_clicked()
{
    ui->text_edit_test->clear();
    ui->text_edit_recv->clear();
}

//数组转换成指针发送
QString byteArrayToHexString(QString head, const uint8_t* str, uint16_t size, QString tail)
{
    QString result = head;
    QString s;

    for(int i = 0; i < size; ++i)
    {
        s = QString("%1").arg(str[i], 0, 16);
        if(s.length() == 1)
            result.append("0x");
        result.append(s.toUpper());
        result.append(' ');
    }
    result += tail;
    result.chop(1);
    return result;
}

//开启网络连接
void MainWindow::on_btn_socket_open_clicked()
{
    init_btn_enable(ui);
    pMainTcpSocketThreadInfo->SetId(ui->line_edit_dev_id->text().toShort());
    pMainTcpSocketThreadInfo->SetSocketInfo(ui->line_edit_ipaddr->text(), ui->line_edit_port->text().toInt());
    ui->btn_uart_close->setDisabled(true);
    ui->btn_uart_open->setDisabled(true);
    ui->btn_socket_open->setDisabled(true);
    ui->btn_socket_close->setEnabled(true);
    append_text_edit_test(QString::fromLocal8Bit("socket appliaction open success!"));
    protocol_flag = PROTOCOL_TCP;
}

//断开网络连接
void MainWindow::on_btn_socket_close_clicked()
{
    init_btn_disable(ui);
    pMainTcpSocketThreadInfo->m_pQueue->clear();
    ui->btn_uart_close->setEnabled(true);
    ui->btn_uart_open->setEnabled(true);
    ui->btn_socket_open->setEnabled(true);
    ui->btn_socket_close->setDisabled(true);
    append_text_edit_test(QString::fromLocal8Bit("socket close!"));
    protocol_flag = PROTOCOL_NULL;
}

