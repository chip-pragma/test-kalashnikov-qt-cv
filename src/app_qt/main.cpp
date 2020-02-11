#include <QApplication>

#include "gui/MainWindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    gui::MainWindow mainWin;
    mainWin.show();

    return app.exec();
}