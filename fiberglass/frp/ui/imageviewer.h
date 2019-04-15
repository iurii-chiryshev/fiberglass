/****************************************************************************

****************************************************************************/
#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMdiSubWindow>
#include <QImage>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

/**
 * @brief The ImageViewer class
 * Упроценный класс вывода изображений в qt
 * @see http://doc.qt.io/qt-5/qtwidgets-widgets-imageviewer-example.html
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 */
class ImageViewer : public QMdiSubWindow
{
    Q_OBJECT

public:
    ImageViewer();
    void setImage(const QImage &newImage, bool normalizeSize = true);
    QImage getImage();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();

public slots:



private:
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    QImage m_image;
    QLabel *m_imageLabel;
    QScrollArea *m_scrollArea;
    double m_scaleFactor;

};

#endif
