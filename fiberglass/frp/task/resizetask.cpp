#include "resizetask.h"


ResizeTask::ResizeTask(cv::Mat src, double scaleFactor, AbstractTask *parent):
    AbstractTask(parent),
    m_cvSrc(src),
    m_scaleFactor(scaleFactor)
{

}

QString ResizeTask::getName()
{
    return tr("Resize image");
}

void ResizeTask::run()
{
    cv::resize(m_cvSrc,m_cvDst,cv::Size(),m_scaleFactor,m_scaleFactor);
    m_qtDst = MatToQImage(m_cvDst);
}
