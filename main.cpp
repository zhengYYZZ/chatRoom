#include "login.h"
#include "charroom.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    login w;
    charRoom c;
    w.show();
    c.show();

    return a.exec();
}
