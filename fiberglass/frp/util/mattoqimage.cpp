#include "mattoqimage.h"
#include <QDebug>

/**
 * @brief MatToQImage
 * @param mat
 * @return
 * cv::mat -> QImage
 */
QImage MatToQImage(const cv::Mat& mat)
{
    if(mat.type() == CV_8UC1)
    {
        //
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
        {
            colorTable.push_back(qRgb(i, i, i));
        }

        //
        const unsigned char *qImageBuffer = (const unsigned char*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, (int)mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    else if(mat.type() == CV_8UC3)
    {
        //
        const unsigned char *qImageBuffer = (const unsigned char*)mat.data;
        return QImage(qImageBuffer, mat.cols, mat.rows, (int)mat.step, QImage::Format_RGB888).rgbSwapped();
    }
    else
    {
        qCritical() << QString("Cannot convert cv::Mat to QImage. Unrecognized input image type: %1").arg(mat.type());
        return QImage();
    }
}

