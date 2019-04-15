#include "io/opentask.h"
#include "util/mattoqimage.h"

OpenTask::OpenTask(const QString &fileName, AbstractTask *parent):
    AbstractTask(parent),
    m_filename(fileName)
{

}

QString OpenTask::getName()
{
    return tr("Open image");
}

void OpenTask::run()
{
    std::string stdFileName = m_filename.toStdString();
    m_cvImg = cv::imread(stdFileName);
}
