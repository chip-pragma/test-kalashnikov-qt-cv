#pragma once

#include <QtWidgets>

namespace gui {

class MainWindow : public QWidget {
public:
    explicit MainWindow(QWidget *parent = nullptr, const Qt::WindowFlags &f = Qt::WindowFlags());

private:
    QImage* mFrameOne = nullptr;

    void _retrieveFrames();
};

}



