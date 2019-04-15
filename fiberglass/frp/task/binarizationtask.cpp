#include "binarizationtask.h"
#include "util/mattoqimage.h"


/**
 * @brief BinarizationTask::BinarizationTask
 * @param blockSize
 * @param c
 * @param gaussKSize
 * @param maxValue
 * @param adaptMethod
 * @param threshType
 * @param src
 * @param pix
 * @param parent
 * ctor
 */
BinarizationTask::BinarizationTask(int blockSize,
                                   double c,
                                   int gaussKSize,
                                   double maxValue,
                                   int adaptMethod,
                                   int threshType,
                                   cv::Mat src,
                                   uchar pix,
                                   AbstractTask *parent):
    AbstractTask(parent),
    m_blockSize(blockSize),
    m_c(c),
    m_gaussKSize(gaussKSize),
    m_maxValue(maxValue),
    m_adaptMethod(adaptMethod),
    m_threshType(threshType),
    m_src(src),
    m_pix(pix),
    m_percent(0)
{

}

QString BinarizationTask::getName()
{
    return tr("Binarization image");
}

/**
 * @brief BinarizationTask::run
 * AbstractTask implementaton
 */
void BinarizationTask::run()
{
    m_percent = calc(m_src,m_dst,cv::Size(m_gaussKSize,m_gaussKSize),m_maxValue,m_adaptMethod,m_threshType,m_blockSize,m_c,m_pix);
}


/**
 * @brief ImageExecutor::operator ()
 * @param _src
 * @param _dst
 * @param _gaussKernelSize
 * @param _maxValue
 * @param _adaptiveMethod
 * @param _thresholdType
 * @param _blockSize
 * @param _C
 * @param _pix
 * @return - процент пикселей
 * Основная запускалка класса
 */
double BinarizationTask::calc(cv::InputArray _src,
                              cv::OutputArray _dst,
                              const cv::Size& _gaussKernelSize /*= cv::Size(1,1)*/,
                              double _maxValue /*= 255.0*/,
                              int _adaptiveMethod /*= cv::ADAPTIVE_THRESH_MEAN_C*/,
                              int _thresholdType /*= cv::THRESH_BINARY*/,
                              int _blockSize /*= 7*/,
                              double _C /*= 1.0*/,
                              uchar _pix /*= 0*/)
{

    cv::Mat src = _src.getMat(), gray, gauss;
    cv::Size ssize = src.size();
    CV_Assert(ssize.area() > 0);
    const int stype = src.type();
    // работаем только с gray или rgb
    CV_Assert(stype == CV_8UC1  || stype == CV_8UC3);
    // приводим к серому
    if(stype == CV_8UC3){
        cv::cvtColor(src,gray,CV_BGR2GRAY);
    }else{
        gray = src;
    }
    // сглаживание по гауссу
    cv::GaussianBlur(gray,gauss,_gaussKernelSize,0);
    // адаптивная бинаризация
    cv::adaptiveThreshold(gauss,_dst,_maxValue,_adaptiveMethod,_thresholdType,_blockSize,_C);
    //cv::threshold(gauss,_dst,_maxValue,255,cv::THRESH_BINARY);
    // процент
    double ret = calcPixPercent(_dst,_pix);
    // в QImage
    m_qtDst = MatToQImage(_dst.getMat());
    return ret;
}

/**
 * @brief ImageExecutor::calcWhitePercent
 * @param _src
 * @param _pix
 * @return
 * Подсчет пиксселей на изображении в процентном отношении
 */
double BinarizationTask::calcPixPercent(cv::InputArray _src,const uchar _pix)
{
    cv::Mat src = _src.getMat();
    const int stype = src.type();
    cv::Size ssize = src.size();
    CV_Assert(ssize.area() > 0 && stype == CV_8UC1);
    const int nrows = src.rows, ncols = src.cols;
    int count = 0;
    for(int y = 0; y < nrows; y++){
        const uchar* ptr = src.ptr(y);
        for(int x = 0; x < ncols; x++){
            if(ptr[x] == _pix) count++;
        }
    }
    return count * 100.0 / ( nrows * ncols );
}
