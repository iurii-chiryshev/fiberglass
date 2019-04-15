#include <QtWidgets>
#include "imageviewer.h"
/**
 * @brief ImageViewer::ImageViewer
 */
ImageViewer::ImageViewer()
   : m_imageLabel(new QLabel)
   , m_scrollArea(new QScrollArea)
   , m_scaleFactor(1.0)
{
    setMinimumSize(400,300);

    m_imageLabel->setBackgroundRole(QPalette::Base);
    m_imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_imageLabel->setScaledContents(true);

    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidget(m_imageLabel);
    m_scrollArea->setVisible(false);
    this->setWidget(m_scrollArea);

}

/**
 * @brief ImageViewer::setImage
 * @param newImage
 * @param normalizeSize
 */
void ImageViewer::setImage(const QImage &newImage,bool normalizeSize)
{
    m_image = newImage;
    m_imageLabel->setPixmap(QPixmap::fromImage(m_image));
    if(normalizeSize){
        m_scaleFactor = 1.0;
    }
    m_scrollArea->setVisible(true);
    m_imageLabel->adjustSize();
    scaleImage(1.0);

}

/**
 * @brief ImageViewer::getImage
 * @return
 */
QImage ImageViewer::getImage()
{
    return m_image;
}

/**
 * @brief ImageViewer::zoomIn
 */
void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}
/**
 * @brief ImageViewer::zoomOut
 */
void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}
/**
 * @brief ImageViewer::normalSize
 */
void ImageViewer::normalSize()
{
    m_imageLabel->adjustSize();
    m_scaleFactor = 1.0;
}
/**
 * @brief ImageViewer::fitToWindow
 */
void ImageViewer::fitToWindow()
{
    // todo implement
    normalSize();
}
/**
 * @brief ImageViewer::scaleImage
 * @param factor
 */
void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(m_imageLabel->pixmap());
    m_scaleFactor *= factor;
    m_imageLabel->resize(m_scaleFactor * m_imageLabel->pixmap()->size());

    adjustScrollBar(m_scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(m_scrollArea->verticalScrollBar(), factor);

}
/**
 * @brief ImageViewer::adjustScrollBar
 * @param scrollBar
 * @param factor
 */
void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}
