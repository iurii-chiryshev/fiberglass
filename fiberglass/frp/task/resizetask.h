#ifndef RESIZETASK_H
#define RESIZETASK_H

#include <QObject>
#include <QImage>
#include "async/abstracttask.h"
#include "opencv2/opencv.hpp"
#include "util/mattoqimage.h"

/**
 * @brief The ResizeTask class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Задача по изменению размеров изображения
 */
class ResizeTask : public AbstractTask
{
    Q_OBJECT
public:
    /**
     * @brief ResizeTask
     * @param src - входное изображение
     * @param scaleFactor - масштабный коэффициент
     * @param parent
     */
    explicit ResizeTask(cv::Mat src,double scaleFactor, AbstractTask *parent = 0);

public:
    // AbstractTask interface
    QString getName();
    void run();
    //input
    cv::Mat m_cvSrc;
    double m_scaleFactor;
    //output
    cv::Mat m_cvDst;
    QImage m_qtDst;

};

#endif // RESIZETASK_H
