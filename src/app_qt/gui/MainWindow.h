#pragma once

#include <QtWidgets>

#include <chip/core.h>
#include <chip/api.h>

namespace gui {

class MainWindow : public QWidget {
public:
    explicit MainWindow(QWidget *parent = nullptr, const Qt::WindowFlags &f = Qt::WindowFlags());

private:
    QLabel* mImageLbl = nullptr;
    QLabel* mInfoLlb = nullptr;
    QLineEdit* mShmNameEdln = nullptr;

    chip::SharedMemory mInfoShm;
    chip::MappedData<chip::FrameInfo> mInfoMap;
    chip::SharedMemory mPairShm;
    chip::MappedData<uint8_t> mPairMap;

    QImage mFrameOne;

private slots:
    void onShmOpenClicked();

    void onShmCloseClicked();

};

}



