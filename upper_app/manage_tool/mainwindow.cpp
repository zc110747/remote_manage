#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "protocol.h"
#include "uartclient.h"
#include "tcpclient.h"
#include "udpclient.h"
#include "commandinfo.h"
#include "appthread.h"
#include "configfile.h"
#include <QDir>
#include <QFileDialog>

static CUartProtocolInfo *pMainUartProtocolInfo;
static CTcpSocketInfo *pMainTcpSocketThreadInfo;
static CUdpSocketInfo *pMainUdpSocketInfo;
static CAppThreadInfo *pAppThreadInfo;
static PROTOCOL_STATUS protocol_flag;
static struct SSystemConfig *pSystemConfigInfo;

#define FRAM_STYLE  "QFrame{border-radius:10px}"

//函数声明
void init_btn_disable(Ui::MainWindow *ui);
void init_btn_enable(Ui::MainWindow *ui);
void QFrame_Init(Ui::MainWindow *ui);

static void load_image(QLabel *label, QString Path)
{
    QImage image;
    if(!(image.load(Path))){
        qDebug()<<"Image load failed, Path:"<<Path;
        return;
    }
    qDebug()<<"Image load ok";
    label->clear();
    label->setPixmap(QPixmap::fromImage(image));
    label->setScaledContents(true);
}

static void load_image(QLabel *label, QImage *pImage)
{
    label->clear();
    label->setPixmap(QPixmap::fromImage(*pImage));
    label->setScaledContents(true);
}

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
    ui->centralwidget->setMinimumSize(QSize(740, 780));
    ui->frame_uart->setFrameShape(QFrame::Shape::Box);
    ui->frame_uart->setFrameShadow(QFrame::Shadow::Sunken);
    ui->frame_dev->setFrameShape(QFrame::Shape::Box);
    ui->frame_dev->setFrameShadow(QFrame::Shadow::Sunken);
    ui->frame_socket->setFrameShape(QFrame::Shape::Box);
    ui->frame_socket->setFrameShadow(QFrame::Shadow::Sunken);
    //ui->label_image->setStyleSheet("border-image:url(:/image/test.jpg);");
    load_image(ui->label_image, QString(QDir::currentPath()+"/image/test.jpg"));
}

/*!
    界面窗口的初始化
    完成Uart应用, Tcp Socket应用的初始化
*/
void MainWindow::init()
{
    SystemConfigInfoInit();
    pSystemConfigInfo = GetSystemConfigInfo();

    //设置table的分页参数
    ui->tabWidget->setCurrentIndex(1);

    //添加COM口
    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }
    ui->combo_box_com->addItems(comList);
    ui->combo_box_com->setCurrentIndex(ui->combo_box_com->findText(pSystemConfigInfo->m_SCom));

    //波特率选项
    QStringList BaudList;
    BaudList <<"9600"<<"38400"<<"76800"<<"115200"<<"230400";
    ui->combo_box_baud->addItems(BaudList);
    ui->combo_box_baud->setCurrentIndex(ui->combo_box_baud->findText(pSystemConfigInfo->m_SBaud));

    //数据位选项
    QStringList dataBitsList;
    dataBitsList <<"6" << "7" << "8"<<"9";
    ui->combo_box_data->addItems(dataBitsList);
    ui->combo_box_data->setCurrentIndex(ui->combo_box_data->findText(pSystemConfigInfo->m_SDataBits));

    //停止位选项
    QStringList StopBitsList;
    StopBitsList<<"1"<<"2";
    ui->combo_box_stop->addItems(StopBitsList);
    ui->combo_box_stop->setCurrentIndex(ui->combo_box_stop->findText(pSystemConfigInfo->m_SStopBits));

    //校验位
    QStringList ParityList;
    ParityList<<"N"<<"Odd"<<"Even";
    ui->combo_box_parity->addItems(ParityList);
    ui->combo_box_parity->setCurrentIndex(ui->combo_box_parity->findText(pSystemConfigInfo->m_SParity));

    //设置协议类型
    QStringList SocketTypeList;
    SocketTypeList<<"TCP"<<"UDP";
    ui->combo_box_socket_type->addItems(SocketTypeList);
    ui->combo_box_socket_type->setCurrentIndex(ui->combo_box_socket_type->findText(pSystemConfigInfo->m_SProtocol));

    //设置网络相关的配置
    ui->line_edit_ipaddr->setText(pSystemConfigInfo->m_SIpAddr);
    ui->line_edit_local_ipaddr->setText(pSystemConfigInfo->m_SLocalIpAddr);
    ui->line_edit_port->setText(pSystemConfigInfo->m_SPort);

//    //正则限制部分输入需要为数据
//    QRegExp regx("[0-9]+$");
//    QValidator *validator_time = new QRegExpValidator(regx,  ui->line_edit_time);
//    ui->line_edit_time->setValidator( validator_time );
//    QValidator *validator_id = new QRegExpValidator(regx,  ui->line_edit_dev_id);
//    ui->line_edit_dev_id->setValidator( validator_id );

    //默认按键配置不可操作
    init_btn_disable(ui);
    ui->btn_uart_close->setDisabled(true);
    ui->btn_socket_close->setDisabled(true);

    //Uart应用相关线程和数据初始化
    UartThreadInit();
    pMainUartProtocolInfo = GetUartProtocolInfo();
    connect(pMainUartProtocolInfo, SIGNAL(send_edit_recv(QString)), this, SLOT(append_text_edit_recv(QString)));
    connect(pMainUartProtocolInfo, SIGNAL(send_edit_test(QString)), this, SLOT(append_text_edit_test(QString)));

    //Socket应用相关线程和数据初始化
    TcpClientSocketInit();
    pMainTcpSocketThreadInfo = GetTcpClientSocketInfo();
    connect(pMainTcpSocketThreadInfo, SIGNAL(send_edit_recv(QString)), this, SLOT(append_text_edit_recv(QString)));
    connect(pMainTcpSocketThreadInfo, SIGNAL(send_edit_test(QString)), this, SLOT(append_text_edit_test(QString)));
    //pMainTcpSocketThreadInfo->start();

    //Udp应用相关线程和数据初始化
    UdpSocketInfoInit();
    pMainUdpSocketInfo = GetUdpClientSocketInfo();
    connect(pMainUdpSocketInfo, SIGNAL(send_edit_recv(QString)), this, SLOT(append_text_edit_recv(QString)));
    connect(pMainUdpSocketInfo, SIGNAL(send_edit_test(QString)), this, SLOT(append_text_edit_test(QString)));

    //主线程应用执行
    AppThreadInit();
    pAppThreadInfo = GetAppThreadInfo();
    pAppThreadInfo->start();
}

/*!
    将数据显示在接收窗口的槽函数
*/
void MainWindow::append_text_edit_recv(QString s)
{
    ui->text_edit_recv->setText(s);
}

/*!
    将数据显示在测试窗口的槽函数
*/
void MainWindow::append_text_edit_test(QString s)
{
    //对于超长数据，截取后显示
    if(s.size() > 150)
    {
        s = s.mid(0, 40) + "..." + s.mid(s.size()-20, s.size());
    }

    if(ui->text_edit_test->document()->lineCount() > 20)
    {
        ui->text_edit_test->setText(s);
    }
    else
    {
        ui->text_edit_test->append(s);
    }
}

/*!
    关闭按钮(包含串口和网络)执行的界面状态管理函数
*/
void init_btn_disable(Ui::MainWindow *ui)
{
    ui->btn_led_on->setDisabled(true);
    ui->btn_led_off->setDisabled(true);
    ui->btn_reboot->setDisabled(true);
    ui->btn_beep_on->setDisabled(true);
    ui->btn_beep_off->setDisabled(true);
    ui->btn_refresh->setDisabled(true);
    ui->btn_send_cmd->setDisabled(true);
    ui->btn_filepath_update->setDisabled(true);
    ui->line_edit_dev_id->setReadOnly(false);
}

/*!
    开启按钮(包含串口和网络)执行的界面状态管理函数
*/
void init_btn_enable(Ui::MainWindow *ui)
{
    ui->btn_led_on->setEnabled(true);
    ui->btn_led_off->setEnabled(true);
    ui->btn_reboot->setEnabled(true);
    ui->btn_beep_on->setEnabled(true);
    ui->btn_beep_off->setEnabled(true);
    ui->btn_refresh->setEnabled(true);
    ui->btn_send_cmd->setEnabled(true);
    ui->btn_filepath_update->setEnabled(true);
    ui->line_edit_dev_id->setReadOnly(true);
}

/*!
    指令数据的发送和创建，提供给应用线程处理
*/
void CmdSendBuffer(uint8_t *pStart, uint16_t nSize, int nCommand, bool isThrough,
                   std::function<QString(uint8_t *, int)> pfunc, QString pathInfo = nullptr)
{
    QString Strbuf;
    SSendBuffer *pSendbuf = new SSendBuffer(pStart, nSize, nCommand, isThrough, pfunc, protocol_flag, pathInfo);

    if(pAppThreadInfo->QueuePost(pSendbuf) != QUEUE_INFO_OK)
    {
        delete pSendbuf;
    }
}

/*!
    开启led执行的槽函数
*/
void MainWindow::on_btn_led_on_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(LED_ON_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    关闭led执行的槽函数
*/
void MainWindow::on_btn_led_off_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(LED_OFF_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    开启beep执行的槽函数
*/
void MainWindow::on_btn_beep_on_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(BEEP_ON_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    关闭beep执行的槽函数
*/
void MainWindow::on_btn_beep_off_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(BEEP_OFF_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    复位设备执行的槽函数
*/
void MainWindow::on_btn_reboot_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(DEV_REBOOT_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    更新界面状态，获取设备内信息的槽函数
*/
void MainWindow::on_btn_refresh_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(GET_INFO_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
}

/*!
    通用发送指令的槽函数，需要为十六进制
*/
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

    CmdSendBuffer(nCacheBuf, nSize, DEV_WRITE_THROUGH_CMD, true, nullptr);
}

/*!
    关闭串口执行的槽函数
*/
void MainWindow::on_btn_uart_close_clicked()
{
    init_btn_disable(ui);
    pMainUartProtocolInfo->m_pQueue->clear();      //清空队列
    pMainUartProtocolInfo->m_bComStatus = false;
    pMainUartProtocolInfo->m_pSerialPortCom->close();
    pMainUartProtocolInfo->m_pSerialPortCom->deleteLater();
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

/*!
    更新内部数据
*/
void MainWindow::update_system_config(void)
{
    //串口相关配置更新
    pSystemConfigInfo->m_SCom = ui->combo_box_com->currentText();
    pSystemConfigInfo->m_SBaud = ui->combo_box_baud->currentText();
    pSystemConfigInfo->m_SDataBits = ui->combo_box_data->currentText();
    pSystemConfigInfo->m_SParity = ui->combo_box_parity->currentText();
    pSystemConfigInfo->m_SStopBits = ui->combo_box_stop->currentText();

    //设备ID
    pSystemConfigInfo->m_SDeviceID = ui->line_edit_dev_id->text();

    //协议类型
    pSystemConfigInfo->m_SProtocol = ui->combo_box_socket_type->currentText();;
    pSystemConfigInfo->m_SIpAddr = ui->line_edit_ipaddr->text();
    pSystemConfigInfo->m_SPort = ui->line_edit_port->text();
    pSystemConfigInfo->m_SLocalIpAddr = ui->line_edit_local_ipaddr->text();
}

/*!
    开启串口执行的槽函数
*/
void MainWindow::on_btn_uart_open_clicked()
{
    update_system_config();
    pMainUartProtocolInfo->m_pSerialPortCom = new QextSerialPort(ui->combo_box_com->currentText(), QextSerialPort::Polling);
    pMainUartProtocolInfo->m_bComStatus = pMainUartProtocolInfo->m_pSerialPortCom->open(QIODevice::ReadWrite);

    if(pMainUartProtocolInfo->m_bComStatus)
    {
        //清除缓存区
        pMainUartProtocolInfo->m_pSerialPortCom ->flush();
        //设置波特率
        pMainUartProtocolInfo->m_pSerialPortCom ->setBaudRate((BaudRateType)ui->combo_box_baud->currentText().toInt());
        //设置数据位
        pMainUartProtocolInfo->m_pSerialPortCom->setDataBits((DataBitsType)ui->combo_box_data->currentText().toInt());
        //设置校验位
        pMainUartProtocolInfo->m_pSerialPortCom->setParity((ParityType)ui->combo_box_parity->currentText().toInt());
        //设置停止位
        pMainUartProtocolInfo->m_pSerialPortCom->setStopBits((StopBitsType)ui->combo_box_stop->currentText().toInt());
        pMainUartProtocolInfo->m_pSerialPortCom->setFlowControl(FLOW_OFF);
        pMainUartProtocolInfo->m_pSerialPortCom ->setTimeout(10);
        init_btn_enable(ui);
        pMainUartProtocolInfo->SetId(ui->line_edit_dev_id->text().toShort());
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
        SystemConfigUpdate();
    }
    else
    {
        pMainUartProtocolInfo->m_pSerialPortCom->deleteLater();
        pMainUartProtocolInfo->m_bComStatus = false;
        append_text_edit_test(QString::fromLocal8Bit("serial open failed!"));
    }
}

/*!
    清理显示窗口执行的槽函数
*/
void MainWindow::on_btn_clear_clicked()
{
    ui->text_edit_test->clear();
    ui->text_edit_recv->clear();
}

/*!
    将数组转成字符串数据的实现
*/
QString byteArrayToHexString(QString head, const uint8_t* str, uint16_t size, QString tail)
{
    QString result = head;
    QString s;

    for(int i = 0; i < size; ++i)
    {
        s = QString("%1").arg(str[i], 0, 16);
        if(s.length() == 1)
            result.append("0x0");
        else
            result.append("0x");
        result.append(s.toUpper());
        result.append(' ');
    }
    result += tail;
    result.chop(1);
    return result;
}

/*!
    开启网络通讯接口的槽函数
*/
void MainWindow::on_btn_socket_open_clicked()
{
    init_btn_enable(ui);
    ui->btn_uart_close->setDisabled(true);
    ui->btn_uart_open->setDisabled(true);
    ui->btn_socket_open->setDisabled(true);
    ui->btn_socket_close->setEnabled(true);
    ui->line_edit_port->setDisabled(true);
    ui->line_edit_ipaddr->setDisabled(true);
    ui->line_edit_local_ipaddr->setDisabled(true);
    ui->combo_box_socket_type->setDisabled(true);
    if(ui->combo_box_socket_type->currentText() == QString("TCP"))
    {
        pMainTcpSocketThreadInfo->SetId(ui->line_edit_dev_id->text().toShort());
        pMainTcpSocketThreadInfo->SetSocketInfo(ui->line_edit_ipaddr->text(), ui->line_edit_port->text().toInt());
        protocol_flag = PROTOCOL_TCP;
    }
    else
    {
        pMainUdpSocketInfo->SetId(ui->line_edit_dev_id->text().toShort());
        pMainUdpSocketInfo->SetSocketInfo(ui->line_edit_ipaddr->text(), ui->line_edit_local_ipaddr->text(), ui->line_edit_port->text().toInt());
        protocol_flag = PROTOCOL_UDP;
    }
    update_system_config();
    SystemConfigUpdate();
    append_text_edit_test(QString::fromLocal8Bit("Socket App Open Success"));
}

/*!
    关闭网络通讯接口的槽函数
*/
void MainWindow::on_btn_socket_close_clicked()
{
    init_btn_disable(ui);
    pMainTcpSocketThreadInfo->m_pQueue->clear();
    ui->btn_uart_close->setEnabled(true);
    ui->btn_uart_open->setEnabled(true);
    ui->btn_socket_open->setEnabled(true);
    ui->btn_socket_close->setDisabled(true);
    ui->combo_box_socket_type->setEnabled(true);
    ui->line_edit_port->setEnabled(true);
    ui->line_edit_ipaddr->setEnabled(true);
    ui->line_edit_local_ipaddr->setEnabled(true);
    if(protocol_flag == PROTOCOL_UDP)
    {
        SCommandInfo *pCmdInfo = GetCommandPtr(ABORT_CMD);
        if(pCmdInfo != nullptr)
            CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc);
    }
    else
    {
        append_text_edit_test(QString::fromLocal8Bit("Tcp Socket Close!"));
    }
    protocol_flag = PROTOCOL_NULL;
}

void MainWindow::on_btn_filepath_update_clicked()
{
    SCommandInfo *pCmdInfo = GetCommandPtr(SYSTEM_UPDATE_CMD);
    if(pCmdInfo != nullptr)
        CmdSendBuffer(pCmdInfo->m_pbuffer, pCmdInfo->m_nSize, pCmdInfo->m_nCommand, false, pCmdInfo->m_pFunc,
                      ui->combo_box_filepath->itemText( ui->combo_box_filepath->currentIndex()));
}

/*!
    选择文件路径
*/
void MainWindow::on_btn_filepath_choose_clicked()
{
    /*!选择文件目录路径*/
    QString directory;
    QFileDialog *pFileDialog = new QFileDialog(this);

    //directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("choose path"), QDir::currentPath()));
    pFileDialog->setWindowTitle(tr("Choose Update file"));
    pFileDialog->setDirectory(QDir::currentPath());
    pFileDialog->setNameFilter(tr("all file (*)"));

    if(pFileDialog->exec() == QDialog::Accepted)
    {
        directory = pFileDialog->selectedFiles()[0];
    }

    if(!directory.isEmpty())
    {
        if(ui->combo_box_filepath->findText(directory) == -1)
        {
            ui->combo_box_filepath->addItem(directory);
        }
        ui->combo_box_filepath->setCurrentIndex(ui->combo_box_filepath->findText(directory));
    }
}

void MainWindow::on_btn_img_choose_clicked()
{
    /*!选择文件目录路径*/
    QString directory;
    QFileDialog *pFileDialog = new QFileDialog(this);

    //directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("choose path"), QDir::currentPath()));
    pFileDialog->setWindowTitle(tr("Choose Update file"));
    pFileDialog->setDirectory(QDir::currentPath());
    pFileDialog->setNameFilter(tr("images file (*.jpg *.png *.bmp)"));

    if(pFileDialog->exec() == QDialog::Accepted)
    {
        directory = pFileDialog->selectedFiles()[0];
    }

    if(!directory.isEmpty())
    {
        if(ui->combox_img_path->findText(directory) == -1)
        {
            ui->combox_img_path->addItem(directory);
        }
        ui->combox_img_path->setCurrentIndex(ui->combox_img_path->findText(directory));
    }
}

void MainWindow::on_btn_img_show_clicked()
{
    QString path = ui->combox_img_path->currentText();
    load_image(ui->label_image, path);
}
