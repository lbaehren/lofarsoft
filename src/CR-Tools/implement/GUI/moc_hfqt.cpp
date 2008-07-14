/****************************************************************************
** Meta object code from reading C++ file 'hfqt.h'
**
** Created: Thu Jul 10 22:01:55 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "hfqt.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hfqt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HQLabel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HQLabel[] = {
    "HQLabel\0\0print(QString)\0"
};

const QMetaObject HQLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_HQLabel,
      qt_meta_data_HQLabel, 0 }
};

const QMetaObject *HQLabel::metaObject() const
{
    return &staticMetaObject;
}

void *HQLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HQLabel))
	return static_cast<void*>(const_cast<HQLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int HQLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: print((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FindDialog1[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      20,   13,   12,   12, 0x05,
      58,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     100,   12,   12,   12, 0x08,
     119,  114,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FindDialog1[] = {
    "FindDialog1\0\0str,cs\0findNext(QString,Qt::CaseSensitivity)\0"
    "findPrevious(QString,Qt::CaseSensitivity)\0findClicked()\0text\0"
    "enableFindButton(QString)\0"
};

const QMetaObject FindDialog1::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FindDialog1,
      qt_meta_data_FindDialog1, 0 }
};

const QMetaObject *FindDialog1::metaObject() const
{
    return &staticMetaObject;
}

void *FindDialog1::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FindDialog1))
	return static_cast<void*>(const_cast<FindDialog1*>(this));
    return QDialog::qt_metacast(_clname);
}

int FindDialog1::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: findNext((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< Qt::CaseSensitivity(*)>(_a[2]))); break;
        case 1: findPrevious((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< Qt::CaseSensitivity(*)>(_a[2]))); break;
        case 2: findClicked(); break;
        case 3: enableFindButton((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FindDialog1::findNext(const QString & _t1, Qt::CaseSensitivity _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void FindDialog1::findPrevious(const QString & _t1, Qt::CaseSensitivity _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
