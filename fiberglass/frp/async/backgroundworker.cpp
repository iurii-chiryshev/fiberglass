#include "backgroundworker.h"
#include "taskwrap.h"

BackgroundWorker::BackgroundWorker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<QSharedPointer<AbstractTask>>();
    // запускаем по одной задаче за раз
    m_threadPool.setMaxThreadCount(1);
}

BackgroundWorker::~BackgroundWorker()
{
    // ждем заверщения задач в пуле
    m_threadPool.waitForDone();
}


void BackgroundWorker::start(const QSharedPointer<AbstractTask> &task)
{
    m_threadPool.start(createRunnable(task));
}

bool BackgroundWorker::tryStart(const QSharedPointer<AbstractTask> &task)
{
    return m_threadPool.tryStart(createRunnable(task));
}

void BackgroundWorker::on_started(const QString &name)
{
    emit started(name);
}

void BackgroundWorker::on_finished(const QSharedPointer<AbstractTask> &task)
{
    emit finished(task);
}

QRunnable *BackgroundWorker::createRunnable(const QSharedPointer<AbstractTask> &task)
{
    TaskWrap* proxy = new TaskWrap();
    proxy->setAutoDelete(true);
    proxy->setTask(task);

    // started
    QObject::connect(proxy,
                     SIGNAL(started(const QString &)),
                     this,
                     SLOT(on_started(const QString &)));
    // finished
    QObject::connect(proxy,
                     SIGNAL(finished(const QSharedPointer<AbstractTask> &)),
                     this,
                     SLOT(on_finished(const QSharedPointer<AbstractTask> &)));

    return proxy;
}








