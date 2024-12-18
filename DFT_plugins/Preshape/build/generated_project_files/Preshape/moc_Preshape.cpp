/****************************************************************************
** Meta object code from reading C++ file 'Preshape.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Preshape/Preshape.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Preshape.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Preshape_t {
    QByteArrayData data[11];
    char stringdata0[149];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Preshape_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Preshape_t qt_meta_stringdata_Preshape = {
    {
QT_MOC_LITERAL(0, 0, 8), // "Preshape"
QT_MOC_LITERAL(1, 9, 21), // "dimensionalityChanged"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 20), // "dimensionSizeChanged"
QT_MOC_LITERAL(4, 53, 11), // "resetMemory"
QT_MOC_LITERAL(5, 65, 8), // "reChoice"
QT_MOC_LITERAL(6, 74, 16), // "reNameSubscriber"
QT_MOC_LITERAL(7, 91, 15), // "reNamePublisher"
QT_MOC_LITERAL(8, 107, 10), // "mtCallback"
QT_MOC_LITERAL(9, 118, 26), // "sensor_msgs::ImageConstPtr"
QT_MOC_LITERAL(10, 145, 3) // "msg"

    },
    "Preshape\0dimensionalityChanged\0\0"
    "dimensionSizeChanged\0resetMemory\0"
    "reChoice\0reNameSubscriber\0reNamePublisher\0"
    "mtCallback\0sensor_msgs::ImageConstPtr\0"
    "msg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Preshape[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    0,   54,    2, 0x0a /* Public */,
       8,    1,   55,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,

       0        // eod
};

void Preshape::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Preshape *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->dimensionalityChanged(); break;
        case 1: _t->dimensionSizeChanged(); break;
        case 2: _t->resetMemory(); break;
        case 3: _t->reChoice(); break;
        case 4: _t->reNameSubscriber(); break;
        case 5: _t->reNamePublisher(); break;
        case 6: _t->mtCallback((*reinterpret_cast< const sensor_msgs::ImageConstPtr(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Preshape::staticMetaObject = { {
    &cedar::dyn::Dynamics::staticMetaObject,
    qt_meta_stringdata_Preshape.data,
    qt_meta_data_Preshape,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Preshape::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Preshape::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Preshape.stringdata0))
        return static_cast<void*>(this);
    return cedar::dyn::Dynamics::qt_metacast(_clname);
}

int Preshape::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = cedar::dyn::Dynamics::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
