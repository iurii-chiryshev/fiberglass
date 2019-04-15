#ifndef BACKGROUNDWORKER_H
#define BACKGROUNDWORKER_H

#include <QObject>
#include <QThreadPool>
#include <QSharedPointer>
#include "abstracttask.h"


/**
 * @brief The BackgroundWorker class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Позволяет выполнять операции в отдельном, выделенном потоке.
 */
class BackgroundWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackgroundWorker(QObject *parent = 0);
    ~BackgroundWorker();

    /**
     * @brief start
     * @param task
     * Положить задачу в очередь на исполнение.
     * Притом неважно есть свободный поток или нет
     */
    void start(const QSharedPointer<AbstractTask>& task);
    /**
     * @brief tryStart
     * @param task
     * @return
     * Попытаться положить задачу в очередь.
     * В случае успеха вернет true
     */
    bool tryStart(const QSharedPointer<AbstractTask>& task);

signals:
    /**
     * @brief started
     * @param name - имя задачи
     * Сигнал о начале выполнения задачи
     */
    void started(const QString &name);
    /**
     * @brief finished
     * @param task - умный указатель на задачу, которая была выполнена
     * Сигнал о завершении задачи
     */
    void finished(const QSharedPointer<AbstractTask>& task);

private slots:
    void on_started(const QString &name);
    void on_finished(const QSharedPointer<AbstractTask>& task);

private:
    /**
     * @brief m_threadPool
     * пул потоков
     */
    QThreadPool m_threadPool;
    /**
     * @brief createRunnable
     * @param task
     * @return
     * Обернуть задачу в QRunnable интерфейс, т.к. QThreadPool работает только с ним.
     */
    QRunnable* createRunnable(const QSharedPointer<AbstractTask> &task);
};

#endif // BACKGROUNDWORKER_H
