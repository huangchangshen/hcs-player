#include <QtWidgets/qapplication.h>
#include "MyWidget.h"
#include "GeneratedFiles/ui_MyWidget.h"



#undef main

int main(int argc, char * argv[])
{
    QApplication a(argc, argv);
    MyWidget w;
    w.setFixedWidth(2000);
    w.setFixedHeight(1000);

    w.show();
    return a.exec();
}
