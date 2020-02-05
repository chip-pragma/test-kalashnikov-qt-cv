#include "MainWindow.h"

namespace gui {

MainWindow::MainWindow(QWidget *parent, const Qt::WindowFlags &f)
        : QWidget(parent, f) {

    // widgets
    auto imageLbl = new QLabel;

    // layout
    auto hBox = new QHBoxLayout;
    hBox->addWidget(imageLbl, 1);

    // setup
    setLayout(hBox);
    _retrieveFrames();
}

void MainWindow::_retrieveFrames() {
    return;
}
}