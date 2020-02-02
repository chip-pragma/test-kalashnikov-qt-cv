#include "CameraWorker.h"

namespace chip {

CameraWorker::CameraWorker(int deviceId, const char *shmName) {
    init(deviceId, shmName);
}

bool CameraWorker::init(int deviceId, const char *shmName) {
    // init cam
    mCamera.open(deviceId);
    if (not mCamera.isOpened()) {
        mError = {"OpenCV.VideoCapture", 101};
        return false;
    }

    // cam info
    auto camSize = cv::Size(mCamera.get(cv::CAP_PROP_FRAME_WIDTH), mCamera.get(cv::CAP_PROP_FRAME_HEIGHT));
    auto camType = static_cast<int>(mCamera.get(cv::CAP_PROP_FORMAT));
    std::cout
            << "Size:" << camSize.width << "x" << camSize.height
            << "; "
            << "Type:" << camType
            << std::endl;

    // share mem
    mFrameProvider.init(shmName, {camSize, camType});
    if (mFrameProvider.lastError()) {
        mError = {"FrameProvider", 102};
        return false;
    }

    mMat1 = mFrameProvider.mat1();
    mMat2 = mFrameProvider.mat2();
}

bool CameraWorker::final() {
    mMat1 = cv::Mat();
    mMat2 = cv::Mat();
    mFrameProvider.final();
    mCamera.release();
}

bool CameraWorker::isInit() const {
    return mFrameProvider.isInit();
}

bool CameraWorker::step(const int &sig) {
    if (mCamera.read(mMat1)) {
        _processFrame();
    } else if (not sig) {
        mError = {"read", -2};
        return false;
    }

    if (sig) {
        std::cout << "Stopping..." << std::endl;
        return false;
    }

    return true;
}

void CameraWorker::_processFrame() {
    //
    // general frame processing
    //
    cv::rotate(mMat1, mMat2, cv::ROTATE_180);
    cv::imshow("TEST", mMat2);
    cv::waitKey(1);
}
}