/********************************************************************************
** Form generated from reading ui file 'gotocelldialog.ui'
**
** Created: Thu Jun 19 19:18:58 2008
**      by: Qt User Interface Compiler version 4.2.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_GOTOCELLDIALOG_H
#define UI_GOTOCELLDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_GoToCellDialog
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QWidget *GoToCellDialog)
    {
    GoToCellDialog->setObjectName(QString::fromUtf8("GoToCellDialog"));
    vboxLayout = new QVBoxLayout(GoToCellDialog);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    label = new QLabel(GoToCellDialog);
    label->setObjectName(QString::fromUtf8("label"));

    hboxLayout->addWidget(label);

    lineEdit = new QLineEdit(GoToCellDialog);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

    hboxLayout->addWidget(lineEdit);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    okButton = new QPushButton(GoToCellDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setEnabled(false);
    okButton->setDefault(true);

    hboxLayout1->addWidget(okButton);

    cancelButton = new QPushButton(GoToCellDialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    hboxLayout1->addWidget(cancelButton);


    vboxLayout->addLayout(hboxLayout1);

    label->setBuddy(lineEdit);
    QWidget::setTabOrder(lineEdit, okButton);
    QWidget::setTabOrder(okButton, cancelButton);

    retranslateUi(GoToCellDialog);

    QMetaObject::connectSlotsByName(GoToCellDialog);
    } // setupUi

    void retranslateUi(QWidget *GoToCellDialog)
    {
    GoToCellDialog->setWindowTitle(QApplication::translate("GoToCellDialog", "Go to Cell", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("GoToCellDialog", "&Cell Location:", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("GoToCellDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("GoToCellDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(GoToCellDialog);
    } // retranslateUi

};

namespace Ui {
    class GoToCellDialog: public Ui_GoToCellDialog {};
} // namespace Ui

#endif // UI_GOTOCELLDIALOG_H
