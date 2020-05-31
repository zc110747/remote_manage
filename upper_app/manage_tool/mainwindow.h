#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();

public slots:
    void append_text_edit_recv(QString s);
    void append_text_edit_test(QString s);

private slots:
    void on_btn_clear_clicked();
    void on_btn_led_off_clicked();
    void on_btn_led_on_clicked();
    void on_btn_beep_on_clicked();
    void on_btn_beep_off_clicked();
    void on_btn_uart_close_clicked();
    void on_btn_uart_open_clicked();

    void on_btn_send_cmd_clicked();

    void on_btn_socket_open_clicked();

    void on_btn_socket_close_clicked();

    void on_btn_reboot_clicked();

private:
    Ui::MainWindow *ui;
};

QString byteArrayToHexString(QString head, uint8_t* str, uint16_t size, QString tail);
#endif // MAINWINDOW_H
