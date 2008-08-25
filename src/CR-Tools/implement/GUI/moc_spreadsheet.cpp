/****************************************************************************
** Meta object code from reading C++ file 'spreadsheet.h'
**
** Created: Fri Jun 20 06:27:37 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <GUI/spreadsheet.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spreadsheet.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Spreadsheet[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      24,   12,   12,   12, 0x0a,
      30,   12,   12,   12, 0x0a,
      37,   12,   12,   12, 0x0a,
      45,   12,   12,   12, 0x0a,
      51,   12,   12,   12, 0x0a,
      70,   12,   12,   12, 0x0a,
      92,   12,   12,   12, 0x0a,
     113,  106,   12,   12, 0x0a,
     145,  138,   12,   12, 0x0a,
     183,  138,   12,   12, 0x0a,
     225,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Spreadsheet[] = {
    "Spreadsheet\0\0modified()\0cut()\0copy()\0paste()\0del()\0"
    "selectCurrentRow()\0selectCurrentColumn()\0recalculate()\0recalc\0"
    "setAutoRecalculate(bool)\0str,cs\0findNext(QString,Qt::CaseSensitivity)\0"
    "findPrevious(QString,Qt::CaseSensitivity)\0somethingChanged()\0"
};

const QMetaObject Spreadsheet::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_Spreadsheet,
      qt_meta_data_Spreadsheet, 0 }
};

const QMetaObject *Spreadsheet::metaObject() const
{
    return &staticMetaObject;
}

void *Spreadsheet::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Spreadsheet))
	return static_cast<void*>(const_cast<Spreadsheet*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int Spreadsheet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: modified(); break;
        case 1: cut(); break;
        case 2: copy(); break;
        case 3: paste(); break;
        case 4: del(); break;
        case 5: selectCurrentRow(); break;
        case 6: selectCurrentColumn(); break;
        case 7: recalculate(); break;
        case 8: setAutoRecalculate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: findNext((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< Qt::CaseSensitivity(*)>(_a[2]))); break;
        case 10: findPrevious((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< Qt::CaseSensitivity(*)>(_a[2]))); break;
        case 11: somethingChanged(); break;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Spreadsheet::modified()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
