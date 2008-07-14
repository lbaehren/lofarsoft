/********************************************************************************
** Form generated from reading ui file 'sortdialog.ui'
**
** Created: Thu Jun 19 19:18:58 2008
**      by: Qt User Interface Compiler version 4.2.1
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SORTDIALOG_H
#define UI_SORTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_SortDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QSpacerItem *spacerItem;
    QPushButton *moreButton;
    QGroupBox *tertiaryGroupBox;
    QGridLayout *gridLayout1;
    QComboBox *tertiaryOrderCombo;
    QLabel *tertiaryOrderLabel;
    QLabel *tertiaryColumnLabel;
    QSpacerItem *spacerItem1;
    QComboBox *tertiaryColumnCombo;
    QGroupBox *secondaryGroupBox;
    QGridLayout *gridLayout2;
    QComboBox *secondaryOrderCombo;
    QLabel *secondaryOrderLabel;
    QLabel *secondaryColumnLabel;
    QSpacerItem *spacerItem2;
    QComboBox *secondaryColumnCombo;
    QSpacerItem *spacerItem3;
    QGroupBox *primaryGroupBox;
    QGridLayout *gridLayout3;
    QComboBox *primaryOrderCombo;
    QLabel *primaryOrderLabel;
    QLabel *primaryColumnLabel;
    QSpacerItem *spacerItem4;
    QComboBox *primaryColumnCombo;

    void setupUi(QDialog *SortDialog)
    {
    SortDialog->setObjectName(QString::fromUtf8("SortDialog"));
    gridLayout = new QGridLayout(SortDialog);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    okButton = new QPushButton(SortDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setDefault(true);

    vboxLayout->addWidget(okButton);

    cancelButton = new QPushButton(SortDialog);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

    vboxLayout->addWidget(cancelButton);

    spacerItem = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    moreButton = new QPushButton(SortDialog);
    moreButton->setObjectName(QString::fromUtf8("moreButton"));
    moreButton->setCheckable(true);

    vboxLayout->addWidget(moreButton);


    gridLayout->addLayout(vboxLayout, 0, 1, 2, 1);

    tertiaryGroupBox = new QGroupBox(SortDialog);
    tertiaryGroupBox->setObjectName(QString::fromUtf8("tertiaryGroupBox"));
    gridLayout1 = new QGridLayout(tertiaryGroupBox);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    tertiaryOrderCombo = new QComboBox(tertiaryGroupBox);
    tertiaryOrderCombo->setObjectName(QString::fromUtf8("tertiaryOrderCombo"));

    gridLayout1->addWidget(tertiaryOrderCombo, 1, 1, 1, 2);

    tertiaryOrderLabel = new QLabel(tertiaryGroupBox);
    tertiaryOrderLabel->setObjectName(QString::fromUtf8("tertiaryOrderLabel"));

    gridLayout1->addWidget(tertiaryOrderLabel, 1, 0, 1, 1);

    tertiaryColumnLabel = new QLabel(tertiaryGroupBox);
    tertiaryColumnLabel->setObjectName(QString::fromUtf8("tertiaryColumnLabel"));

    gridLayout1->addWidget(tertiaryColumnLabel, 0, 0, 1, 1);

    spacerItem1 = new QSpacerItem(31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout1->addItem(spacerItem1, 0, 2, 1, 1);

    tertiaryColumnCombo = new QComboBox(tertiaryGroupBox);
    tertiaryColumnCombo->setObjectName(QString::fromUtf8("tertiaryColumnCombo"));

    gridLayout1->addWidget(tertiaryColumnCombo, 0, 1, 1, 1);


    gridLayout->addWidget(tertiaryGroupBox, 3, 0, 1, 1);

    secondaryGroupBox = new QGroupBox(SortDialog);
    secondaryGroupBox->setObjectName(QString::fromUtf8("secondaryGroupBox"));
    gridLayout2 = new QGridLayout(secondaryGroupBox);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    secondaryOrderCombo = new QComboBox(secondaryGroupBox);
    secondaryOrderCombo->setObjectName(QString::fromUtf8("secondaryOrderCombo"));

    gridLayout2->addWidget(secondaryOrderCombo, 1, 1, 1, 2);

    secondaryOrderLabel = new QLabel(secondaryGroupBox);
    secondaryOrderLabel->setObjectName(QString::fromUtf8("secondaryOrderLabel"));

    gridLayout2->addWidget(secondaryOrderLabel, 1, 0, 1, 1);

    secondaryColumnLabel = new QLabel(secondaryGroupBox);
    secondaryColumnLabel->setObjectName(QString::fromUtf8("secondaryColumnLabel"));

    gridLayout2->addWidget(secondaryColumnLabel, 0, 0, 1, 1);

    spacerItem2 = new QSpacerItem(31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout2->addItem(spacerItem2, 0, 2, 1, 1);

    secondaryColumnCombo = new QComboBox(secondaryGroupBox);
    secondaryColumnCombo->setObjectName(QString::fromUtf8("secondaryColumnCombo"));

    gridLayout2->addWidget(secondaryColumnCombo, 0, 1, 1, 1);


    gridLayout->addWidget(secondaryGroupBox, 2, 0, 1, 1);

    spacerItem3 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem3, 1, 0, 1, 1);

    primaryGroupBox = new QGroupBox(SortDialog);
    primaryGroupBox->setObjectName(QString::fromUtf8("primaryGroupBox"));
    gridLayout3 = new QGridLayout(primaryGroupBox);
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    primaryOrderCombo = new QComboBox(primaryGroupBox);
    primaryOrderCombo->setObjectName(QString::fromUtf8("primaryOrderCombo"));

    gridLayout3->addWidget(primaryOrderCombo, 1, 1, 1, 2);

    primaryOrderLabel = new QLabel(primaryGroupBox);
    primaryOrderLabel->setObjectName(QString::fromUtf8("primaryOrderLabel"));

    gridLayout3->addWidget(primaryOrderLabel, 1, 0, 1, 1);

    primaryColumnLabel = new QLabel(primaryGroupBox);
    primaryColumnLabel->setObjectName(QString::fromUtf8("primaryColumnLabel"));

    gridLayout3->addWidget(primaryColumnLabel, 0, 0, 1, 1);

    spacerItem4 = new QSpacerItem(31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout3->addItem(spacerItem4, 0, 2, 1, 1);

    primaryColumnCombo = new QComboBox(primaryGroupBox);
    primaryColumnCombo->setObjectName(QString::fromUtf8("primaryColumnCombo"));

    gridLayout3->addWidget(primaryColumnCombo, 0, 1, 1, 1);


    gridLayout->addWidget(primaryGroupBox, 0, 0, 1, 1);

    QWidget::setTabOrder(primaryColumnCombo, primaryOrderCombo);
    QWidget::setTabOrder(primaryOrderCombo, secondaryColumnCombo);
    QWidget::setTabOrder(secondaryColumnCombo, secondaryOrderCombo);
    QWidget::setTabOrder(secondaryOrderCombo, tertiaryColumnCombo);
    QWidget::setTabOrder(tertiaryColumnCombo, tertiaryOrderCombo);
    QWidget::setTabOrder(tertiaryOrderCombo, okButton);
    QWidget::setTabOrder(okButton, cancelButton);
    QWidget::setTabOrder(cancelButton, moreButton);

    retranslateUi(SortDialog);
    QObject::connect(moreButton, SIGNAL(toggled(bool)), secondaryGroupBox, SLOT(setVisible(bool)));
    QObject::connect(moreButton, SIGNAL(toggled(bool)), tertiaryGroupBox, SLOT(setVisible(bool)));
    QObject::connect(okButton, SIGNAL(clicked()), SortDialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), SortDialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(SortDialog);
    } // setupUi

    void retranslateUi(QDialog *SortDialog)
    {
    SortDialog->setWindowTitle(QApplication::translate("SortDialog", "Sort", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("SortDialog", "OK", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("SortDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    moreButton->setText(QApplication::translate("SortDialog", "&More", 0, QApplication::UnicodeUTF8));
    tertiaryGroupBox->setTitle(QApplication::translate("SortDialog", "&Tertiary Key", 0, QApplication::UnicodeUTF8));
    tertiaryOrderCombo->clear();
    tertiaryOrderCombo->addItem(QApplication::translate("SortDialog", "Ascending", 0, QApplication::UnicodeUTF8));
    tertiaryOrderCombo->addItem(QApplication::translate("SortDialog", "Descending", 0, QApplication::UnicodeUTF8));
    tertiaryOrderLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Order:</p></body></html>", 0, QApplication::UnicodeUTF8));
    tertiaryColumnLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Column:</p></body></html>", 0, QApplication::UnicodeUTF8));
    tertiaryColumnCombo->clear();
    tertiaryColumnCombo->addItem(QApplication::translate("SortDialog", "None", 0, QApplication::UnicodeUTF8));
    secondaryGroupBox->setTitle(QApplication::translate("SortDialog", "&Secondary Key", 0, QApplication::UnicodeUTF8));
    secondaryOrderCombo->clear();
    secondaryOrderCombo->addItem(QApplication::translate("SortDialog", "Ascending", 0, QApplication::UnicodeUTF8));
    secondaryOrderCombo->addItem(QApplication::translate("SortDialog", "Descending", 0, QApplication::UnicodeUTF8));
    secondaryOrderLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Order:</p></body></html>", 0, QApplication::UnicodeUTF8));
    secondaryColumnLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Column:</p></body></html>", 0, QApplication::UnicodeUTF8));
    secondaryColumnCombo->clear();
    secondaryColumnCombo->addItem(QApplication::translate("SortDialog", "None", 0, QApplication::UnicodeUTF8));
    primaryGroupBox->setTitle(QApplication::translate("SortDialog", "&Primary Key", 0, QApplication::UnicodeUTF8));
    primaryOrderCombo->clear();
    primaryOrderCombo->addItem(QApplication::translate("SortDialog", "Ascending", 0, QApplication::UnicodeUTF8));
    primaryOrderCombo->addItem(QApplication::translate("SortDialog", "Descending", 0, QApplication::UnicodeUTF8));
    primaryOrderLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Order:</p></body></html>", 0, QApplication::UnicodeUTF8));
    primaryColumnLabel->setText(QApplication::translate("SortDialog", "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Column:</p></body></html>", 0, QApplication::UnicodeUTF8));
    primaryColumnCombo->clear();
    primaryColumnCombo->addItem(QApplication::translate("SortDialog", "None", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SortDialog);
    } // retranslateUi

};

namespace Ui {
    class SortDialog: public Ui_SortDialog {};
} // namespace Ui

#endif // UI_SORTDIALOG_H
