/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../manage_tool/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[16];
    char stringdata0[323];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 21), // "append_text_edit_recv"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 1), // "s"
QT_MOC_LITERAL(4, 36, 21), // "append_text_edit_test"
QT_MOC_LITERAL(5, 58, 20), // "on_btn_clear_clicked"
QT_MOC_LITERAL(6, 79, 22), // "on_btn_led_off_clicked"
QT_MOC_LITERAL(7, 102, 21), // "on_btn_led_on_clicked"
QT_MOC_LITERAL(8, 124, 22), // "on_btn_beep_on_clicked"
QT_MOC_LITERAL(9, 147, 23), // "on_btn_beep_off_clicked"
QT_MOC_LITERAL(10, 171, 25), // "on_btn_uart_close_clicked"
QT_MOC_LITERAL(11, 197, 24), // "on_btn_uart_open_clicked"
QT_MOC_LITERAL(12, 222, 23), // "on_btn_send_cmd_clicked"
QT_MOC_LITERAL(13, 246, 26), // "on_btn_socket_open_clicked"
QT_MOC_LITERAL(14, 273, 27), // "on_btn_socket_close_clicked"
QT_MOC_LITERAL(15, 301, 21) // "on_btn_reboot_clicked"

    },
    "MainWindow\0append_text_edit_recv\0\0s\0"
    "append_text_edit_test\0on_btn_clear_clicked\0"
    "on_btn_led_off_clicked\0on_btn_led_on_clicked\0"
    "on_btn_beep_on_clicked\0on_btn_beep_off_clicked\0"
    "on_btn_uart_close_clicked\0"
    "on_btn_uart_open_clicked\0"
    "on_btn_send_cmd_clicked\0"
    "on_btn_socket_open_clicked\0"
    "on_btn_socket_close_clicked\0"
    "on_btn_reboot_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x0a /* Public */,
       4,    1,   82,    2, 0x0a /* Public */,
       5,    0,   85,    2, 0x08 /* Private */,
       6,    0,   86,    2, 0x08 /* Private */,
       7,    0,   87,    2, 0x08 /* Private */,
       8,    0,   88,    2, 0x08 /* Private */,
       9,    0,   89,    2, 0x08 /* Private */,
      10,    0,   90,    2, 0x08 /* Private */,
      11,    0,   91,    2, 0x08 /* Private */,
      12,    0,   92,    2, 0x08 /* Private */,
      13,    0,   93,    2, 0x08 /* Private */,
      14,    0,   94,    2, 0x08 /* Private */,
      15,    0,   95,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->append_text_edit_recv((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->append_text_edit_test((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->on_btn_clear_clicked(); break;
        case 3: _t->on_btn_led_off_clicked(); break;
        case 4: _t->on_btn_led_on_clicked(); break;
        case 5: _t->on_btn_beep_on_clicked(); break;
        case 6: _t->on_btn_beep_off_clicked(); break;
        case 7: _t->on_btn_uart_close_clicked(); break;
        case 8: _t->on_btn_uart_open_clicked(); break;
        case 9: _t->on_btn_send_cmd_clicked(); break;
        case 10: _t->on_btn_socket_open_clicked(); break;
        case 11: _t->on_btn_socket_close_clicked(); break;
        case 12: _t->on_btn_reboot_clicked(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
