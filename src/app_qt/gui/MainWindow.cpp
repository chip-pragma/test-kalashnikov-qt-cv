#include "MainWindow.h"

#define CHIP_QT_IMAGE_LBL_DEFAULT_TEXT "Open shared memory."

namespace gui {

MainWindow::MainWindow(QWidget *parent, const Qt::WindowFlags &f)
        : QWidget(parent, f) {

    // widgets
    auto shmNameEdln = new QLineEdit;
    shmNameEdln->setPlaceholderText("shared memory name");
    auto shmOpenBtn = new QPushButton("Open SHM");
    auto shmCloseBtn = new QPushButton("Close SHM");

    auto infoLbl = new QLabel;

    auto imageLbl = new QLabel(CHIP_QT_IMAGE_LBL_DEFAULT_TEXT);
    imageLbl->setAlignment(Qt::AlignCenter);

    // connections
    connect(shmOpenBtn, &QPushButton::clicked, this, &MainWindow::onShmOpenClicked);
    connect(shmCloseBtn, &QPushButton::clicked, this, &MainWindow::onShmCloseClicked);

    // layout
    auto shmHBox = new QHBoxLayout;
    shmHBox->addWidget(shmNameEdln, 1);
    shmHBox->addWidget(shmOpenBtn);
    shmHBox->addWidget(shmCloseBtn);


    auto vBox = new QVBoxLayout;
    vBox->addLayout(shmHBox);
    vBox->addWidget(imageLbl, 1);
    vBox->addWidget(infoLbl);

    // setup
    setLayout(vBox);
    setMinimumSize({700, 500});

    mImageLbl = imageLbl;
    mInfoLlb = infoLbl;
    mShmNameEdln = shmNameEdln;
}

void MainWindow::onShmOpenClicked() {
    using namespace chip;

    /*
     * PREPARE
     */
    onShmCloseClicked();

    auto shmName = mShmNameEdln->text().toStdString();
    auto infoShmName = shmName + CHIP_SHM_INFO_NAME_POSTFIX;
    auto pairShmName = shmName + CHIP_SHM_PAIR_NAME_POSTFIX;

    /*
     * FRAME INFO
     */
    SharedMemory infoShm(infoShmName, O_RDONLY, 0777);
    if (not infoShm.isOpen()) {
        auto msg = QString::fromStdString(infoShm.lastError().toStr());
        QMessageBox::critical(this, "Open error", "FrameInfo.SharedMemory.Open: " + msg);
        return;
    }
    // map
    MappedData<chip::FrameInfo> infoMap(1, PROT_READ, infoShm.descriptor());
    if (not infoMap.isMapped()) {
        auto msg = QString::fromStdString(infoShm.lastError().toStr());
        QMessageBox::critical(this, "Open error", "FrameInfo.MappedData.Map: " + msg);
        return;
    }
    // init
    const auto infoPtr = infoMap.data();

    /*
     * SUCCESS
     */
    mInfoShm = std::move(infoShm);
    mInfoMap = std::move(infoMap);

    mInfoLlb->setText(
            QString("%1x%2 (%3 bit)")
                    .arg(infoPtr->width)
                    .arg(infoPtr->height)
                    .arg(infoPtr->channels * 8)
    );
    mImageLbl->setPixmap(QPixmap::fromImage(mFrameOne));
}

void MainWindow::onShmCloseClicked() {
    // frame info
    if (not mInfoMap.unmap()) {
        auto msg = QString::fromStdString(mInfoMap.lastError().toStr());
        QMessageBox::critical(this, "Close error", "FrameInfo.MappedData.Unmap: " + msg);
    }
    if (not mInfoShm.close()) {
        auto msg = QString::fromStdString(mInfoShm.lastError().toStr());
        QMessageBox::critical(this, "Close error", "FrameInfo.SharedMemory.Close: " + msg);
    }

    mInfoLlb->clear();
    mImageLbl->setText(CHIP_QT_IMAGE_LBL_DEFAULT_TEXT);
}
}