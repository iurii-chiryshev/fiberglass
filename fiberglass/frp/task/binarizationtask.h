#ifndef BINARIZATIONTASK_H
#define BINARIZATIONTASK_H

#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "async/abstracttask.h"

/**
 * @brief The BinarizationTask class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Задача бинаризации картинки
 */
class BinarizationTask: public AbstractTask
{
    Q_OBJECT
public:
    explicit BinarizationTask(  int blockSize,
                                double c,
                                int gaussKSize,
                                double maxValue,
                                int adaptMethod,
                                int threshType,
                                cv::Mat src,
                                uchar pix,
                                AbstractTask* parent = 0);


     // AbstractTask interface
 public:
     QString getName();
     void run();

public:
    // входные данные
    int m_blockSize;
    double m_c;
    int m_gaussKSize;
    double m_maxValue;
    int m_adaptMethod;
    int m_threshType;
    cv::Mat m_src;
    uchar m_pix;
    // выходные данные
    cv::Mat m_dst;
    QImage m_qtDst;
    double m_percent;

private:
    /**
     * @brief operator ()
     * @param src
     * @param dst
     * @param gaussKernelSize
     * @param maxValue
     * @param adaptiveMethod
     * @param thresholdType
     * @param blockSize
     * @param C
     * @param pix - какой пиксель нужен
     * @return - процент percentPix пикселей от площади картинки
     * Бинаризует и подчитает процент брака
     */
    double calc(cv::InputArray src,
                   cv::OutputArray dst,
                   const cv::Size& gaussKernelSize = cv::Size(1,1),
                   double maxValue = 255.0,
                   int adaptiveMethod = cv::ADAPTIVE_THRESH_MEAN_C,
                   int thresholdType = cv::THRESH_BINARY,
                   int blockSize = 7,
                   double C = 1.0,
                   uchar pix = 0 /*default - black*/);
    /**
     * @brief calcWhitePercent
     * @param src
     * @param pix
     * @return
     * Подсчет процента пикселей на изображении
     */
    double calcPixPercent(cv::InputArray src, const uchar pix = 255);
};

#endif // BINARIZATIONTASK_H
