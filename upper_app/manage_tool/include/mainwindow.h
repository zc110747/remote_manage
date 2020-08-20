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
    void update_system_config(void);

public slots:
    void append_text_edit_recv(QString s);
    void append_text_edit_test(QString s);
    void process_capture(void);

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

    void on_btn_refresh_clicked();

    void on_btn_filepath_update_clicked();

    void on_btn_filepath_choose_clicked();

    void on_btn_img_choose_clicked();

    void on_btn_img_show_clicked();

    void on_btn_img_base_clicked();

    void on_btn_img_blur_clicked();

    void on_btn_img_erode_clicked();

    void on_btn_img_dilate_clicked();

    void on_btn_img_canny_clicked();

    void on_btn_img_gray_clicked();

    void on_btn_img_save_clicked();

    void on_btn_img_line_scale_clicked();

    void on_btn_img_noline_scale_clicked();

    void on_btn_img_equalizeHist_clicked();

    void on_btn_img_wrap_clicked();

    void on_btn_img_HoughLines_clicked();

    void on_btn_img_backProj_clicked();

    void on_btn_img_capture_clicked();

private:
    Ui::MainWindow *ui;
};

QString byteArrayToHexString(QString head, const uint8_t* str, uint16_t size, QString tail);
#endif // MAINWINDOW_H
