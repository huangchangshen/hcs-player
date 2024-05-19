/********************************************************************************
** Form generated from reading UI file 'MyWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYWIDGET_H
#define UI_MYWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyWidgetClass
{
public:
    QHBoxLayout *horizontalLayout;

    void setupUi(QWidget *MyWidgetClass)
    {
        if (MyWidgetClass->objectName().isEmpty())
            MyWidgetClass->setObjectName(QString::fromUtf8("MyWidgetClass"));
        MyWidgetClass->resize(600, 400);
        horizontalLayout = new QHBoxLayout(MyWidgetClass);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

        retranslateUi(MyWidgetClass);

        QMetaObject::connectSlotsByName(MyWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *MyWidgetClass)
    {
        MyWidgetClass->setWindowTitle(QCoreApplication::translate("MyWidgetClass", "MyWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyWidgetClass: public Ui_MyWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYWIDGET_H
