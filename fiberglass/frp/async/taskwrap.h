#ifndef TASKWRAP_H
#define TASKWRAP_H

#include <QObject>
#include <QSharedPointer>
#include <QRunnable>
#include "abstracttask.h"

/**
 * @brief The TaskWrap class
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Обертка над задачей - он выполняет задачу т.к. QRunnable
 */
class TaskWrap : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit TaskWrap(QObject *parent = 0);

    /**
     * @brief setTask
     * @param task - задача для выполнения
     * Установить задачу
     */
    void setTask(const QSharedPointer<AbstractTask> &task);
    /**
     * @brief run
     * implementation QRunnable
     */
    void run();

signals:
    /**
     * @brief started
     * @param name - имя задачи
     * Задача началась
     */
    void started(const QString &name);
    /**
     * @brief finished
     * @param task - задача
     * Задача завершилась
     */
    void finished(const QSharedPointer<AbstractTask>& task);

private slots:

private:
    QSharedPointer<AbstractTask> m_task;


public:
};

#endif // TASKWRAP_H
