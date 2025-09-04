#include <QApplication>
#include "chatwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ChatWindow w;
    w.setWindowTitle("局域网牌桌");
    w.resize(600, 400);
    w.show();
    return app.exec();
}

