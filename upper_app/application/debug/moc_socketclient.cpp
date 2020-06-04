/****************************************************************************
** Meta object code from reading C++ file 'socketclient.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../manage_tool/socketclient.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'socketclient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CTcpSocketThreadInfo_t {
    QByteArrayData data[7];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CTcpSocketThreadInfo_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CTcpSocketThreadInfo_t qt_meta_stringdata_CTcpSocketThreadInfo = {
    {
QT_MOC_LITERAL(0, 0, 20), // "CTcpSocketThreadInfo"
QT_MOC_LITERAL(1, 21, 14), // "send_edit_recv"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 14), // "send_edit_test"
QT_MOC_LITERAL(4, 52, 13), // "slotConnected"
QT_MOC_LITERAL(5, 66, 16), // "slotDisconnected"
QT_MOC_LITERAL(6, 83, 12) // "dataReceived"

    },
    "CTcpSocketThreadInfo\0send_edit_recv\0"
    "\0send_edit_test\0slotConnected\0"
    "slotDisconnected\0dataReceived"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CTcpSocketThreadInfo[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   45,    2, 0x0a /* Public */,
       5,    0,   46,    2, 0x0a /* Public */,
       6,    0,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void CTcpSocketThreadInfo::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CTcpSocketThreadInfo *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->send_edit_recv((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->send_edit_test((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->slotConnected(); break;
        case 3: _t->slotDisconnected(); break;
        case 4: _t->dataReceived(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CTcpSocketThreadInfo::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTcpSocketThreadInfo::send_edit_recv)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CTcpSocketThreadInfo::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CTcpSocketThreadInfo::send_edit_test)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CTcpSocketThreadInfo::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_CTcpSocketThreadInfo.data,
    qt_meta_data_CTcpSocketThreadInfo,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CTcpSocketThreadInfo::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CTcpSocketThreadInfo::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CTcpSocketThreadInfo.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "CProtocolInfo"))
        return static_cast< CProtocolInfo*>(this);
    return QThread::qt_metacast(_clname);
}

int CTcpSocketThreadInfo::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CTcpSocketThreadInfo::send_edit_recv(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CTcpSocketThreadInfo::send_edit_test(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
