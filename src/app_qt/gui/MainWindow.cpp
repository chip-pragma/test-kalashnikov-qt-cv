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
        auto msg = QString::fromStdString(infoMap.lastError().toStr());
        QMessageBox::critical(this, "Open error", "FrameInfo.MappedData.Map: " + msg);
        return;
    }
    // init
    const auto infoPtr = infoMap.data();

    /*
     * MATS PAIR
     */
    // shared mem
    SharedMemory pairShm(pairShmName, O_RDONLY, 0777);
    if (not pairShm.isOpen()) {
        auto msg = QString::fromStdString(pairShm.lastError().toStr());
        QMessageBox::critical(this, "Open error", "MatsPair.SharedMemory.Open: " + msg);
        return;
    }
    // map
    MappedData<uint8_t> pairMap(infoPtr->total() * 2, PROT_READ, pairShm.descriptor());
    if (not pairMap.isMapped()) {
        auto msg = QString::fromStdString(pairMap.lastError().toStr());
        QMessageBox::critical(this, "Open error", "MatsPair.MappedData.Map: " + msg);
        return;
    }
    // init
    auto pairPtr = pairMap.data();
    QImage image(
            pairPtr,
            infoPtr->width, infoPtr->height,
            infoPtr->step(),
            QImage::Format_RGB888);

    /*
     * SUCCESS
     */
    mInfoShm = std::move(infoShm);
    mInfoMap = std::move(infoMap);
    mPairShm = std::move(pairShm);
    mPairMap = std::move(pairMap);

    mInfoLlb->setText(
            QString("[ %4 | %5 ] -- %1x%2 (%3 bit)")
                    .arg(infoPtr->width)
                    .arg(infoPtr->height)
                    .arg(infoPtr->channels * 8)
                    .arg(QString::fromStdString(infoShmName))
                    .arg(QString::fromStdString(pairShmName))
    );
    mFrameOne = image;
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
    // mats pair
    if (not mPairMap.unmap()) {
        auto msg = QString::fromStdString(mPairMap.lastError().toStr());
        QMessageBox::critical(this, "Close error", "MatsPair.MappedData.Unmap: " + msg);
    }
    if (not mPairShm.close()) {
        auto msg = QString::fromStdString(mPairShm.lastError().toStr());
        QMessageBox::critical(this, "Close error", "MatsPair.SharedMemory.Close: " + msg);
    }

    mInfoLlb->clear();
    mImageLbl->setText(CHIP_QT_IMAGE_LBL_DEFAULT_TEXT);
}
}