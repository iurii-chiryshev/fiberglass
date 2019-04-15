#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H

#include <QObject>


/**
 * @brief The BackgroundWorkerTask class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Абстрактный класс задачи для выполнения в background-е
 */
class AbstractTask : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTask(QObject *parent = 0);
    /**
     * @brief run
     * main executor
     */
    virtual void run() = 0;
    /**
     * @brief getName
     * @return
     * Получить имя задачи
     */
    virtual QString getName() = 0;

signals:

public slots:

};

#endif // ABSTRACT_TASK_H
