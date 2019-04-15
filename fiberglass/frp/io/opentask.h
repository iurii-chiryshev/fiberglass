#ifndef OPENTASK_H
#define OPENTASK_H

#include <QObject>
#include "async/abstracttask.h"
#include "opencv2/opencv.hpp"

/**
 * @brief The OpenTask class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Задача по загрузке изображения
 */
class OpenTask : public AbstractTask
{
    Q_OBJECT
public:
    /**
     * @brief OpenTask
     * @param fileName - имя файла
     * @param parent
     */
    explicit OpenTask(const QString &fileName, AbstractTask *parent = 0);

public:
    // AbstractTask interface
    QString getName();
    void run();
public:
    // input
    QString m_filename;
    // output
    cv::Mat m_cvImg;

signals:

public slots:

};

#endif // OPENTASK_H
