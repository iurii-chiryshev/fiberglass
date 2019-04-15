#include "taskwrap.h"

TaskWrap::TaskWrap(QObject *parent) : QObject(parent)
{

}

void TaskWrap::setTask(const QSharedPointer<AbstractTask> &task)
{
    m_task = task;
}

void TaskWrap::run()
{
    emit started(m_task->getName());
    m_task->run();
    emit finished(m_task);
}
