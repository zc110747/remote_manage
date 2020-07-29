/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFrame *frame_uart;
    QLabel *label_comid;
    QComboBox *combo_box_com;
    QLabel *label_baud;
    QComboBox *combo_box_baud;
    QLabel *label_data;
    QComboBox *combo_box_data;
    QLabel *label_stop;
    QComboBox *combo_box_stop;
    QLabel *label_parity;
    QComboBox *combo_box_parity;
    QPushButton *btn_uart_open;
    QPushButton *btn_uart_close;
    QFrame *frame_dev;
    QLabel *label_dev_id;
    QLineEdit *line_edit_dev_id;
    QFrame *frame_socket;
    QLabel *label_server_ipaddr;
    QLineEdit *line_edit_ipaddr;
    QLabel *label_server_port;
    QLineEdit *line_edit_port;
    QPushButton *btn_socket_open;
    QPushButton *btn_socket_close;
    QLabel *label_socket_type;
    QComboBox *combo_box_socket_type;
    QLabel *label_local_ipaddr;
    QLineEdit *line_edit_local_ipaddr;
    QTabWidget *tabWidget;
    QWidget *deviceTab;
    QFrame *frame_info;
    QLabel *label_led;
    QPushButton *btn_led_on;
    QPushButton *btn_led_off;
    QLabel *label_beep;
    QPushButton *btn_beep_on;
    QPushButton *btn_beep_off;
    QTextEdit *text_edit_recv;
    QLabel *label_stauts;
    QPushButton *btn_refresh;
    QLabel *label_reboot;
    QPushButton *btn_reboot;
    QPushButton *btn_filepath_choose;
    QLabel *label_file_path;
    QComboBox *combo_box_filepath;
    QPushButton *btn_filepath_update;
    QFrame *frame_test;
    QLabel *label_cmd;
    QLineEdit *line_edit_cmd;
    QPushButton *btn_send_cmd;
    QLabel *label_test;
    QTextEdit *text_edit_test;
    QPushButton *btn_clear;
    QWidget *ImageTable;
    QFrame *frame_image;
    QLabel *label_image;
    QLabel *label_img_path;
    QComboBox *combox_img_path;
    QPushButton *btn_img_choose;
    QPushButton *btn_img_show;
    QPushButton *btn_img_base;
    QPushButton *btn_img_blur;
    QPushButton *btn_img_erode;
    QLabel *label_img_cmd;
    QPushButton *btn_img_dilate;
    QPushButton *btn_img_canny;
    QLabel *label_img_log;
    QPushButton *btn_img_gray;
    QPushButton *btn_img_save;
    QLabel *label_img_cmd_2;
    QPushButton *btn_img_sync;
    QPushButton *btn_img_trans;
    QPushButton *btn_img_line_scale;
    QPushButton *btn_img_noline_scale;
    QPushButton *btn_img_equalizeHist;
    QPushButton *btn_img_wrap;
    QPushButton *btn_img_HoughLines;
    QPushButton *btn_img_backProj;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(730, 830);
        MainWindow->setMinimumSize(QSize(730, 830));
        MainWindow->setMaximumSize(QSize(730, 830));
        MainWindow->setAutoFillBackground(false);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        frame_uart = new QFrame(centralwidget);
        frame_uart->setObjectName(QString::fromUtf8("frame_uart"));
        frame_uart->setGeometry(QRect(500, 10, 221, 201));
        frame_uart->setMinimumSize(QSize(100, 100));
        frame_uart->setAutoFillBackground(false);
        frame_uart->setFrameShape(QFrame::StyledPanel);
        frame_uart->setFrameShadow(QFrame::Raised);
        label_comid = new QLabel(frame_uart);
        label_comid->setObjectName(QString::fromUtf8("label_comid"));
        label_comid->setGeometry(QRect(10, 10, 61, 20));
        combo_box_com = new QComboBox(frame_uart);
        combo_box_com->setObjectName(QString::fromUtf8("combo_box_com"));
        combo_box_com->setGeometry(QRect(100, 10, 111, 21));
        label_baud = new QLabel(frame_uart);
        label_baud->setObjectName(QString::fromUtf8("label_baud"));
        label_baud->setGeometry(QRect(10, 40, 61, 20));
        combo_box_baud = new QComboBox(frame_uart);
        combo_box_baud->setObjectName(QString::fromUtf8("combo_box_baud"));
        combo_box_baud->setGeometry(QRect(100, 40, 111, 21));
        label_data = new QLabel(frame_uart);
        label_data->setObjectName(QString::fromUtf8("label_data"));
        label_data->setGeometry(QRect(10, 70, 61, 20));
        combo_box_data = new QComboBox(frame_uart);
        combo_box_data->setObjectName(QString::fromUtf8("combo_box_data"));
        combo_box_data->setGeometry(QRect(100, 70, 111, 21));
        label_stop = new QLabel(frame_uart);
        label_stop->setObjectName(QString::fromUtf8("label_stop"));
        label_stop->setGeometry(QRect(10, 100, 61, 20));
        combo_box_stop = new QComboBox(frame_uart);
        combo_box_stop->setObjectName(QString::fromUtf8("combo_box_stop"));
        combo_box_stop->setGeometry(QRect(100, 100, 111, 21));
        label_parity = new QLabel(frame_uart);
        label_parity->setObjectName(QString::fromUtf8("label_parity"));
        label_parity->setGeometry(QRect(10, 130, 61, 20));
        combo_box_parity = new QComboBox(frame_uart);
        combo_box_parity->setObjectName(QString::fromUtf8("combo_box_parity"));
        combo_box_parity->setGeometry(QRect(100, 130, 111, 21));
        btn_uart_open = new QPushButton(frame_uart);
        btn_uart_open->setObjectName(QString::fromUtf8("btn_uart_open"));
        btn_uart_open->setGeometry(QRect(10, 170, 81, 23));
        btn_uart_close = new QPushButton(frame_uart);
        btn_uart_close->setObjectName(QString::fromUtf8("btn_uart_close"));
        btn_uart_close->setGeometry(QRect(120, 170, 81, 23));
        frame_dev = new QFrame(centralwidget);
        frame_dev->setObjectName(QString::fromUtf8("frame_dev"));
        frame_dev->setGeometry(QRect(500, 230, 221, 51));
        frame_dev->setFrameShape(QFrame::StyledPanel);
        frame_dev->setFrameShadow(QFrame::Raised);
        label_dev_id = new QLabel(frame_dev);
        label_dev_id->setObjectName(QString::fromUtf8("label_dev_id"));
        label_dev_id->setGeometry(QRect(10, 10, 71, 21));
        line_edit_dev_id = new QLineEdit(frame_dev);
        line_edit_dev_id->setObjectName(QString::fromUtf8("line_edit_dev_id"));
        line_edit_dev_id->setGeometry(QRect(100, 10, 111, 21));
        frame_socket = new QFrame(centralwidget);
        frame_socket->setObjectName(QString::fromUtf8("frame_socket"));
        frame_socket->setGeometry(QRect(500, 300, 221, 211));
        frame_socket->setFrameShape(QFrame::StyledPanel);
        frame_socket->setFrameShadow(QFrame::Raised);
        label_server_ipaddr = new QLabel(frame_socket);
        label_server_ipaddr->setObjectName(QString::fromUtf8("label_server_ipaddr"));
        label_server_ipaddr->setGeometry(QRect(10, 60, 81, 20));
        line_edit_ipaddr = new QLineEdit(frame_socket);
        line_edit_ipaddr->setObjectName(QString::fromUtf8("line_edit_ipaddr"));
        line_edit_ipaddr->setGeometry(QRect(100, 60, 111, 20));
        label_server_port = new QLabel(frame_socket);
        label_server_port->setObjectName(QString::fromUtf8("label_server_port"));
        label_server_port->setGeometry(QRect(10, 100, 81, 20));
        line_edit_port = new QLineEdit(frame_socket);
        line_edit_port->setObjectName(QString::fromUtf8("line_edit_port"));
        line_edit_port->setGeometry(QRect(100, 100, 111, 21));
        btn_socket_open = new QPushButton(frame_socket);
        btn_socket_open->setObjectName(QString::fromUtf8("btn_socket_open"));
        btn_socket_open->setGeometry(QRect(10, 180, 81, 23));
        btn_socket_close = new QPushButton(frame_socket);
        btn_socket_close->setObjectName(QString::fromUtf8("btn_socket_close"));
        btn_socket_close->setGeometry(QRect(120, 180, 81, 23));
        label_socket_type = new QLabel(frame_socket);
        label_socket_type->setObjectName(QString::fromUtf8("label_socket_type"));
        label_socket_type->setGeometry(QRect(10, 20, 51, 20));
        combo_box_socket_type = new QComboBox(frame_socket);
        combo_box_socket_type->setObjectName(QString::fromUtf8("combo_box_socket_type"));
        combo_box_socket_type->setGeometry(QRect(100, 20, 111, 21));
        label_local_ipaddr = new QLabel(frame_socket);
        label_local_ipaddr->setObjectName(QString::fromUtf8("label_local_ipaddr"));
        label_local_ipaddr->setGeometry(QRect(10, 140, 81, 20));
        line_edit_local_ipaddr = new QLineEdit(frame_socket);
        line_edit_local_ipaddr->setObjectName(QString::fromUtf8("line_edit_local_ipaddr"));
        line_edit_local_ipaddr->setGeometry(QRect(100, 140, 111, 20));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 481, 771));
        deviceTab = new QWidget();
        deviceTab->setObjectName(QString::fromUtf8("deviceTab"));
        frame_info = new QFrame(deviceTab);
        frame_info->setObjectName(QString::fromUtf8("frame_info"));
        frame_info->setGeometry(QRect(0, 0, 481, 431));
        frame_info->setAutoFillBackground(true);
        frame_info->setFrameShape(QFrame::StyledPanel);
        frame_info->setFrameShadow(QFrame::Raised);
        label_led = new QLabel(frame_info);
        label_led->setObjectName(QString::fromUtf8("label_led"));
        label_led->setGeometry(QRect(10, 10, 71, 16));
        btn_led_on = new QPushButton(frame_info);
        btn_led_on->setObjectName(QString::fromUtf8("btn_led_on"));
        btn_led_on->setGeometry(QRect(90, 10, 91, 23));
        btn_led_off = new QPushButton(frame_info);
        btn_led_off->setObjectName(QString::fromUtf8("btn_led_off"));
        btn_led_off->setGeometry(QRect(200, 10, 91, 23));
        label_beep = new QLabel(frame_info);
        label_beep->setObjectName(QString::fromUtf8("label_beep"));
        label_beep->setGeometry(QRect(10, 40, 71, 21));
        btn_beep_on = new QPushButton(frame_info);
        btn_beep_on->setObjectName(QString::fromUtf8("btn_beep_on"));
        btn_beep_on->setGeometry(QRect(90, 40, 91, 23));
        btn_beep_off = new QPushButton(frame_info);
        btn_beep_off->setObjectName(QString::fromUtf8("btn_beep_off"));
        btn_beep_off->setGeometry(QRect(200, 40, 91, 23));
        text_edit_recv = new QTextEdit(frame_info);
        text_edit_recv->setObjectName(QString::fromUtf8("text_edit_recv"));
        text_edit_recv->setGeometry(QRect(10, 130, 461, 231));
        text_edit_recv->setAcceptDrops(false);
        text_edit_recv->setReadOnly(true);
        label_stauts = new QLabel(frame_info);
        label_stauts->setObjectName(QString::fromUtf8("label_stauts"));
        label_stauts->setGeometry(QRect(10, 100, 81, 21));
        btn_refresh = new QPushButton(frame_info);
        btn_refresh->setObjectName(QString::fromUtf8("btn_refresh"));
        btn_refresh->setGeometry(QRect(90, 100, 91, 21));
        label_reboot = new QLabel(frame_info);
        label_reboot->setObjectName(QString::fromUtf8("label_reboot"));
        label_reboot->setGeometry(QRect(10, 70, 71, 21));
        btn_reboot = new QPushButton(frame_info);
        btn_reboot->setObjectName(QString::fromUtf8("btn_reboot"));
        btn_reboot->setGeometry(QRect(90, 70, 91, 23));
        btn_filepath_choose = new QPushButton(frame_info);
        btn_filepath_choose->setObjectName(QString::fromUtf8("btn_filepath_choose"));
        btn_filepath_choose->setGeometry(QRect(430, 370, 41, 21));
        label_file_path = new QLabel(frame_info);
        label_file_path->setObjectName(QString::fromUtf8("label_file_path"));
        label_file_path->setGeometry(QRect(10, 370, 71, 21));
        combo_box_filepath = new QComboBox(frame_info);
        combo_box_filepath->setObjectName(QString::fromUtf8("combo_box_filepath"));
        combo_box_filepath->setGeometry(QRect(90, 370, 331, 21));
        btn_filepath_update = new QPushButton(frame_info);
        btn_filepath_update->setObjectName(QString::fromUtf8("btn_filepath_update"));
        btn_filepath_update->setGeometry(QRect(400, 400, 71, 21));
        frame_test = new QFrame(deviceTab);
        frame_test->setObjectName(QString::fromUtf8("frame_test"));
        frame_test->setGeometry(QRect(0, 430, 481, 331));
        frame_test->setAutoFillBackground(true);
        frame_test->setFrameShape(QFrame::StyledPanel);
        frame_test->setFrameShadow(QFrame::Raised);
        label_cmd = new QLabel(frame_test);
        label_cmd->setObjectName(QString::fromUtf8("label_cmd"));
        label_cmd->setGeometry(QRect(10, 10, 81, 20));
        line_edit_cmd = new QLineEdit(frame_test);
        line_edit_cmd->setObjectName(QString::fromUtf8("line_edit_cmd"));
        line_edit_cmd->setGeometry(QRect(10, 30, 381, 31));
        btn_send_cmd = new QPushButton(frame_test);
        btn_send_cmd->setObjectName(QString::fromUtf8("btn_send_cmd"));
        btn_send_cmd->setGeometry(QRect(400, 30, 71, 31));
        label_test = new QLabel(frame_test);
        label_test->setObjectName(QString::fromUtf8("label_test"));
        label_test->setGeometry(QRect(10, 70, 71, 20));
        text_edit_test = new QTextEdit(frame_test);
        text_edit_test->setObjectName(QString::fromUtf8("text_edit_test"));
        text_edit_test->setGeometry(QRect(10, 100, 461, 171));
        text_edit_test->setReadOnly(true);
        btn_clear = new QPushButton(frame_test);
        btn_clear->setObjectName(QString::fromUtf8("btn_clear"));
        btn_clear->setGeometry(QRect(10, 280, 92, 28));
        tabWidget->addTab(deviceTab, QString());
        tabWidget->setTabText(tabWidget->indexOf(deviceTab), QString::fromUtf8("\350\256\276\345\244\207\347\256\241\347\220\206"));
        ImageTable = new QWidget();
        ImageTable->setObjectName(QString::fromUtf8("ImageTable"));
        frame_image = new QFrame(ImageTable);
        frame_image->setObjectName(QString::fromUtf8("frame_image"));
        frame_image->setGeometry(QRect(10, -20, 491, 771));
        frame_image->setAutoFillBackground(true);
        frame_image->setFrameShape(QFrame::StyledPanel);
        frame_image->setFrameShadow(QFrame::Raised);
        label_image = new QLabel(frame_image);
        label_image->setObjectName(QString::fromUtf8("label_image"));
        label_image->setGeometry(QRect(0, 10, 481, 381));
        label_img_path = new QLabel(frame_image);
        label_img_path->setObjectName(QString::fromUtf8("label_img_path"));
        label_img_path->setGeometry(QRect(0, 400, 72, 15));
        combox_img_path = new QComboBox(frame_image);
        combox_img_path->setObjectName(QString::fromUtf8("combox_img_path"));
        combox_img_path->setGeometry(QRect(80, 400, 331, 21));
        btn_img_choose = new QPushButton(frame_image);
        btn_img_choose->setObjectName(QString::fromUtf8("btn_img_choose"));
        btn_img_choose->setGeometry(QRect(420, 400, 41, 21));
        btn_img_show = new QPushButton(frame_image);
        btn_img_show->setObjectName(QString::fromUtf8("btn_img_show"));
        btn_img_show->setGeometry(QRect(391, 430, 71, 31));
        btn_img_base = new QPushButton(frame_image);
        btn_img_base->setObjectName(QString::fromUtf8("btn_img_base"));
        btn_img_base->setGeometry(QRect(10, 500, 71, 31));
        btn_img_blur = new QPushButton(frame_image);
        btn_img_blur->setObjectName(QString::fromUtf8("btn_img_blur"));
        btn_img_blur->setGeometry(QRect(110, 500, 71, 31));
        btn_img_erode = new QPushButton(frame_image);
        btn_img_erode->setObjectName(QString::fromUtf8("btn_img_erode"));
        btn_img_erode->setGeometry(QRect(210, 500, 71, 31));
        label_img_cmd = new QLabel(frame_image);
        label_img_cmd->setObjectName(QString::fromUtf8("label_img_cmd"));
        label_img_cmd->setGeometry(QRect(0, 470, 101, 16));
        btn_img_dilate = new QPushButton(frame_image);
        btn_img_dilate->setObjectName(QString::fromUtf8("btn_img_dilate"));
        btn_img_dilate->setGeometry(QRect(320, 500, 71, 31));
        btn_img_canny = new QPushButton(frame_image);
        btn_img_canny->setObjectName(QString::fromUtf8("btn_img_canny"));
        btn_img_canny->setGeometry(QRect(10, 540, 71, 31));
        label_img_log = new QLabel(frame_image);
        label_img_log->setObjectName(QString::fromUtf8("label_img_log"));
        label_img_log->setGeometry(QRect(0, 430, 341, 31));
        btn_img_gray = new QPushButton(frame_image);
        btn_img_gray->setObjectName(QString::fromUtf8("btn_img_gray"));
        btn_img_gray->setGeometry(QRect(110, 540, 71, 31));
        btn_img_save = new QPushButton(frame_image);
        btn_img_save->setObjectName(QString::fromUtf8("btn_img_save"));
        btn_img_save->setGeometry(QRect(310, 430, 71, 31));
        label_img_cmd_2 = new QLabel(frame_image);
        label_img_cmd_2->setObjectName(QString::fromUtf8("label_img_cmd_2"));
        label_img_cmd_2->setGeometry(QRect(0, 620, 101, 16));
        btn_img_sync = new QPushButton(frame_image);
        btn_img_sync->setObjectName(QString::fromUtf8("btn_img_sync"));
        btn_img_sync->setGeometry(QRect(110, 640, 71, 31));
        btn_img_trans = new QPushButton(frame_image);
        btn_img_trans->setObjectName(QString::fromUtf8("btn_img_trans"));
        btn_img_trans->setGeometry(QRect(10, 640, 71, 31));
        btn_img_line_scale = new QPushButton(frame_image);
        btn_img_line_scale->setObjectName(QString::fromUtf8("btn_img_line_scale"));
        btn_img_line_scale->setGeometry(QRect(210, 540, 71, 31));
        btn_img_noline_scale = new QPushButton(frame_image);
        btn_img_noline_scale->setObjectName(QString::fromUtf8("btn_img_noline_scale"));
        btn_img_noline_scale->setGeometry(QRect(320, 540, 91, 31));
        btn_img_equalizeHist = new QPushButton(frame_image);
        btn_img_equalizeHist->setObjectName(QString::fromUtf8("btn_img_equalizeHist"));
        btn_img_equalizeHist->setGeometry(QRect(320, 580, 91, 31));
        btn_img_wrap = new QPushButton(frame_image);
        btn_img_wrap->setObjectName(QString::fromUtf8("btn_img_wrap"));
        btn_img_wrap->setGeometry(QRect(10, 580, 71, 31));
        btn_img_HoughLines = new QPushButton(frame_image);
        btn_img_HoughLines->setObjectName(QString::fromUtf8("btn_img_HoughLines"));
        btn_img_HoughLines->setGeometry(QRect(210, 580, 81, 31));
        btn_img_backProj = new QPushButton(frame_image);
        btn_img_backProj->setObjectName(QString::fromUtf8("btn_img_backProj"));
        btn_img_backProj->setGeometry(QRect(110, 580, 81, 28));
        tabWidget->addTab(ImageTable, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 730, 26));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        label_comid->setText(QApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267", nullptr));
        label_baud->setText(QApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207", nullptr));
        label_data->setText(QApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215", nullptr));
        label_stop->setText(QApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215", nullptr));
        label_parity->setText(QApplication::translate("MainWindow", "\346\240\241\351\252\214\344\275\215", nullptr));
        btn_uart_open->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        btn_uart_close->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\344\270\262\345\217\243", nullptr));
        label_dev_id->setText(QApplication::translate("MainWindow", "\350\256\276\345\244\207ID", nullptr));
        line_edit_dev_id->setText(QApplication::translate("MainWindow", "1", nullptr));
        label_server_ipaddr->setText(QApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250IP", nullptr));
        line_edit_ipaddr->setText(QApplication::translate("MainWindow", "192.168.1.251", nullptr));
        label_server_port->setText(QApplication::translate("MainWindow", "\346\234\215\345\212\241\345\231\250Port", nullptr));
        line_edit_port->setText(QApplication::translate("MainWindow", "8000", nullptr));
        btn_socket_open->setText(QApplication::translate("MainWindow", "\350\277\236\346\216\245\347\275\221\347\273\234", nullptr));
        btn_socket_close->setText(QApplication::translate("MainWindow", "\346\226\255\345\274\200\347\275\221\347\273\234", nullptr));
        label_socket_type->setText(QApplication::translate("MainWindow", "\345\215\217\350\256\256", nullptr));
        label_local_ipaddr->setText(QApplication::translate("MainWindow", "\346\234\254\345\234\260IP", nullptr));
        line_edit_local_ipaddr->setText(QApplication::translate("MainWindow", "192.168.1.105", nullptr));
        label_led->setText(QApplication::translate("MainWindow", "LED\346\216\247\345\210\266", nullptr));
        btn_led_on->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200LED", nullptr));
        btn_led_off->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255LED", nullptr));
        label_beep->setText(QApplication::translate("MainWindow", "BEEP\346\216\247\345\210\266", nullptr));
        btn_beep_on->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\350\234\202\351\270\243\345\231\250", nullptr));
        btn_beep_off->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255\350\234\202\351\270\243\345\231\250", nullptr));
        label_stauts->setText(QApplication::translate("MainWindow", "\345\206\205\351\203\250\347\212\266\346\200\201\345\214\272", nullptr));
        btn_refresh->setText(QApplication::translate("MainWindow", "\345\210\267\346\226\260\347\212\266\346\200\201", nullptr));
        label_reboot->setText(QApplication::translate("MainWindow", "\345\244\215\344\275\215", nullptr));
        btn_reboot->setText(QApplication::translate("MainWindow", "\345\244\215\344\275\215", nullptr));
        btn_filepath_choose->setText(QApplication::translate("MainWindow", "...", nullptr));
        label_file_path->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        btn_filepath_update->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\346\233\264\346\226\260", nullptr));
        label_cmd->setText(QApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211\346\214\207\344\273\244", nullptr));
        btn_send_cmd->setText(QApplication::translate("MainWindow", "\346\214\207\344\273\244\345\217\221\351\200\201", nullptr));
        label_test->setText(QApplication::translate("MainWindow", "\350\260\203\350\257\225\347\252\227\345\217\243", nullptr));
        btn_clear->setText(QApplication::translate("MainWindow", "\346\270\205\347\251\272\346\225\260\346\215\256", nullptr));
        label_image->setText(QString());
        label_img_path->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\350\267\257\345\276\204", nullptr));
        btn_img_choose->setText(QApplication::translate("MainWindow", "...", nullptr));
        btn_img_show->setText(QApplication::translate("MainWindow", "\345\212\240\350\275\275\345\233\276\345\203\217", nullptr));
        btn_img_base->setText(QApplication::translate("MainWindow", "\345\216\237\345\247\213\345\233\276\345\203\217", nullptr));
        btn_img_blur->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\346\273\244\346\263\242", nullptr));
        btn_img_erode->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\350\205\220\350\232\200", nullptr));
        label_img_cmd->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\345\244\204\347\220\206\346\214\207\344\273\244", nullptr));
        btn_img_dilate->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\350\206\250\350\203\200", nullptr));
        btn_img_canny->setText(QApplication::translate("MainWindow", "\350\276\271\347\274\230\346\243\200\346\265\213", nullptr));
        label_img_log->setText(QApplication::translate("MainWindow", "log:\345\233\276\345\203\217\346\234\252\345\212\240\350\275\275", nullptr));
        btn_img_gray->setText(QApplication::translate("MainWindow", "\347\201\260\345\272\246\350\275\254\346\215\242", nullptr));
        btn_img_save->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230\345\233\276\345\203\217", nullptr));
        label_img_cmd_2->setText(QApplication::translate("MainWindow", "\344\272\244\344\272\222\346\214\207\344\273\244", nullptr));
        btn_img_sync->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\345\220\214\346\255\245", nullptr));
        btn_img_trans->setText(QApplication::translate("MainWindow", "\345\233\276\345\203\217\344\274\240\350\276\223", nullptr));
        btn_img_line_scale->setText(QApplication::translate("MainWindow", "\347\272\277\346\200\247\346\211\251\345\261\225", nullptr));
        btn_img_noline_scale->setText(QApplication::translate("MainWindow", "\351\235\236\347\272\277\346\200\247\346\211\251\345\261\225", nullptr));
        btn_img_equalizeHist->setText(QApplication::translate("MainWindow", "\347\233\264\346\226\271\345\233\276\345\235\207\350\241\241", nullptr));
        btn_img_wrap->setText(QApplication::translate("MainWindow", "\344\273\277\345\260\204\345\217\230\346\215\242", nullptr));
        btn_img_HoughLines->setText(QApplication::translate("MainWindow", "\351\234\215\345\244\253\347\272\277\345\217\230\346\215\242", nullptr));
        btn_img_backProj->setText(QApplication::translate("MainWindow", "\347\233\264\346\226\271\345\233\276\346\230\276\347\244\272", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(ImageTable), QApplication::translate("MainWindow", "\345\233\276\345\203\217\345\244\204\347\220\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
