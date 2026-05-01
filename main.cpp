// ==================== main.cpp ====================
#include <QApplication>
#include "GameWindow.h"
#include <QTextCodec>
int main(int argc, char* argv[]) {
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication app(argc, argv);
    GameWindow window;
    window.show();
    return app.exec();
}