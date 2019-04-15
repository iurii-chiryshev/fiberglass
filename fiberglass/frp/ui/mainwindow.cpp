
#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
/**
 * @brief initializeImageFileDialog
 * @param dialog
 * @param acceptMode
 * Инициализация диалогового окна в заваисимости от open or save dialog
 */
static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;


    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave){
        // установить принудительно т.к. по-умолчанию идет open
        dialog.setAcceptMode(acceptMode);
        dialog.setDefaultSuffix("jpg");
    }
}

/**
 * @brief MainWindow::MainWindow
 * @param parent
 * ctor
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ivSrc(NULL),
    m_ivDst(NULL)
{
    ui->setupUi(this);

    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::on_actionOpen);
    connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::on_actionZoom_In);
    connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::on_actionZoom_Out);
    connect(ui->actionFit_to_Window, &QAction::triggered, this, &MainWindow::on_actionFit_to_Window);
    connect(ui->actionNormal_Size, &QAction::triggered, this, &MainWindow::on_actionNormal_Size);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::on_actionSaveAs);
    connect(ui->actionCascade, &QAction::triggered, this, &MainWindow::on_actionCascade);
    connect(ui->actionTile, &QAction::triggered, this, &MainWindow::on_actionTile);

    // инициализация прогресса как модального окна
    // min = max = 0 - на Windows прогресс крутится постоянно
    m_progressDlg = new QProgressDialog("", "", 0, 0, this);
    m_progressDlg->setCancelButton(NULL);
    m_progressDlg->setAutoClose(false);
    m_progressDlg->setWindowModality(Qt::WindowModal);
    m_progressDlg->setModal(true);
    m_progressDlg->close();


    connect(&m_bgWoker,
            SIGNAL(started(const QString &)),
            this,
            SLOT(on_bgw_started(const QString &)));
    connect(&m_bgWoker,
            SIGNAL(finished(const QSharedPointer<AbstractTask>&)),
            this,
            SLOT(on_bgw_finished(const QSharedPointer<AbstractTask>&)));
}
/**
 * @brief MainWindow::~MainWindow
 * dctor
 */
MainWindow::~MainWindow()
{
    delete ui;
}
/**
 * @brief MainWindow::openImage
 * @param fileName
 * @return
 * Открытие изображения и запись в cv класс Mat
 * в случае успеха вернет true
 */
bool MainWindow::openImage(const QString &fileName)
{
    m_bgWoker.start(QSharedPointer<OpenTask>(new OpenTask(fileName)));
    return true;
}
/**
 * @brief MainWindow::saveImage
 * @param fileName
 * @param image
 * @return
 * Запись файла на диск
 */
bool MainWindow::saveImage(const QString &fileName,const QImage &image)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    return true;
}
/**
 * @brief MainWindow::resizeImage
 * @param value - % от исходного
 * Изменить размер изображения
 */
void MainWindow::resizeImage(int value)
{
    if(m_cvImage.empty()) return;
    float scaleFactor = value/ 100.0;
    m_bgWoker.start(QSharedPointer<ResizeTask>(new ResizeTask(m_cvImage,scaleFactor)));
}
/**
 * @brief MainWindow::removeSubWindows
 * Удалить все дочерние окна
 */
void MainWindow::removeSubWindows()
{
    if(m_ivSrc) {
        ui->mdiArea->removeSubWindow(m_ivSrc);
        delete m_ivSrc;
        m_ivSrc = NULL;
    }
    if(m_ivDst) {
        ui->mdiArea->removeSubWindow(m_ivDst);
        delete m_ivDst;
        m_ivDst = NULL;
    }
}
/**
 * @brief MainWindow::addSubWindow
 * @return - указатель на окно
 * Создать и добавить дочернее окно
 */
ImageViewer *MainWindow::createSubWindow()
{
    ImageViewer * iv = new ImageViewer();
    ui->mdiArea->addSubWindow(iv);
    return iv;
}
/**
 * @brief MainWindow::binarization
 * Основная функция бинаризации
 */
void MainWindow::binarization()
{
    if(m_cvImage.empty()) return;
    // получаем значение со всех контролов и запускаем executor-а
    int blockSize = ui->hSlider_BlockSize->value();
    blockSize = blockSize % 2 == 1 ? blockSize : cv::max(ui->hSlider_BlockSize->minimum(),blockSize - 1) ;
    double c = double(ui->hSlider_C->value());
    int gaussKSize = ui->hSlider_GaussKsize->value();
    gaussKSize = gaussKSize % 2 == 1 ? gaussKSize : cv::max(ui->hSlider_GaussKsize->minimum(),gaussKSize - 1);
    double maxValue = double(ui->hSlider_MaxValue->value());
    int adaptMethod = ui->comboBox_AdaptMethod->currentIndex() == 0 ? cv::ADAPTIVE_THRESH_GAUSSIAN_C : cv::ADAPTIVE_THRESH_MEAN_C;
    int threshType = ui->comboBox_ThresType->currentIndex() == 0 ? cv::THRESH_BINARY : cv::THRESH_BINARY_INV;
    // запускаем
    QSharedPointer<BinarizationTask> bt = QSharedPointer<BinarizationTask>(new BinarizationTask(blockSize,
                                                                                                c,
                                                                                                gaussKSize,
                                                                                                maxValue,
                                                                                                adaptMethod,
                                                                                                threshType,
                                                                                                m_cvSrcImage,
                                                                                                0));
    m_bgWoker.start(bt);


}
/**
 * @brief MainWindow::sendConsoleMsg
 * @param msg
 * Отправить в конслоль сообщение
 */
void MainWindow::sendConsoleMsg(const QString &msg)
{
    //
    ui->listWidget_Console->addItem(QString("[%1]   %2").arg(QDateTime::currentDateTime().toString(),msg));
    ui->listWidget_Console->scrollToBottom();
}
/**
 * @brief MainWindow::on_actionSaveAs
 * обработчик кликальнья на кнопку "сохранить"
 */
void MainWindow::on_actionSaveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);
    // найти активнок окно
    ImageViewer* iv = qobject_cast<ImageViewer*>(ui->mdiArea->activeSubWindow());
    if(iv){
        // пытаемся сохранить изображение из этого окна
        while (dialog.exec() == QDialog::Accepted && !saveImage(dialog.selectedFiles().first(),iv->getImage())) {}
    }


}
/**
 * @brief MainWindow::on_actionOpen
 * Обработчик кликальнья на кнопку "открыть"
 */
void MainWindow::on_actionOpen()
{


    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);
    while (dialog.exec() == QDialog::Accepted && !openImage(dialog.selectedFiles().first())) {}
}

/**
 * @brief MainWindow::on_actionZoom_In
 * Обработчик кликальнья на кнопку "увеличить масштаб"
 */
void MainWindow::on_actionZoom_In()
{
    // найти активнок окно
    ImageViewer* iv = qobject_cast<ImageViewer*>(ui->mdiArea->activeSubWindow());
    if(iv){
        iv->zoomIn();
    }
}
/**
 * @brief MainWindow::on_actionZoom_Out
 * Обработчик кликальнья на кнопку "уменьшить масштаб"
 */
void MainWindow::on_actionZoom_Out()
{
    // найти активнок окно
    ImageViewer* iv = qobject_cast<ImageViewer*>(ui->mdiArea->activeSubWindow());
    if(iv){
        iv->zoomOut();
    }
}
/**
 * @brief MainWindow::on_actionNormal_Size
 * Обработчик кликальнья на кнопку "нормальный размер"
 */
void MainWindow::on_actionNormal_Size()
{
    // найти активнок окно
    ImageViewer* iv = qobject_cast<ImageViewer*>(ui->mdiArea->activeSubWindow());
    if(iv){
        iv->normalSize();
    }
}
/**
 * @brief MainWindow::on_actionFit_to_Window
 * Обработчик кликальнья на кнопку "изображение в размер окна"
 */
void MainWindow::on_actionFit_to_Window()
{
    // найти активнок окно
    ImageViewer* iv = qobject_cast<ImageViewer*>(ui->mdiArea->activeSubWindow());
    if(iv){
        iv->fitToWindow();
    }
}
/**
 * @brief MainWindow::on_actionCascade
 * Обработчик - окна каскадом
 */
void MainWindow::on_actionCascade()
{
    ui->mdiArea->cascadeSubWindows();
}
/**
 * @brief MainWindow::on_actionTile
 * Обработчик - окна плиткой
 */
void MainWindow::on_actionTile()
{
    ui->mdiArea->tileSubWindows();
}

/**
 * @brief MainWindow::on_hSlider_ImgSize_valueChanged
 * @param value
 * Обработчик события слайдера на изменения рабочего размера изображения
 */
void MainWindow::on_hSlider_ImgSize_valueChanged(int value)
{
    resizeImage(value);
}

/**
 * @brief MainWindow::on_hSlider_GaussKsize_valueChanged
 * @param value
 * обработчик изменения "маска гаусса"
 */
void MainWindow::on_hSlider_GaussKsize_valueChanged(int value)
{
    bool odd = value % 2 == 1;
    // должно быть не четное число
    value = odd ? value : cv::max(ui->hSlider_GaussKsize->minimum(),value - 1);
    if(odd) {
        sendConsoleMsg(tr("Change gaussian kernel size, new value: ")
                       .append("%1")
                       .arg(value));
        binarization(); // бинаризовать если нечетное
    }
    else ui->hSlider_GaussKsize->setValue(value); // иначе на второй круг
}
/**
 * @brief MainWindow::on_hSlider_MaxValue_valueChanged
 * @param value
 * обработчик изменения "MaxValue адаптивной бинаризации"
 * Не особо что значит, это просто обозначает каким должен быть фон
 * белый: 255 или серый: 0-254
 * @see http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
 */
void MainWindow::on_hSlider_MaxValue_valueChanged(int value)
{
    sendConsoleMsg(tr("Change value assigned to the pixels for which the condition is satisfied, new value: ")
                   .append("%1")
                   .arg(value));
    binarization();
}
/**
 * @brief MainWindow::on_hSlider_C_valueChanged
 * @param value
 * обработчик изменения "константа"
 * @see http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
 */
void MainWindow::on_hSlider_C_valueChanged(int value)
{
    sendConsoleMsg(tr("Change subtracted constant (from the mean or weighted mean), new value: ")
                   .append("%1")
                   .arg(value));
    binarization();
}
/**
 * @brief MainWindow::on_comboBox_AdaptMethod_currentIndexChanged
 * @param index
 * Изменение метода адаптивной бинаризации
 * @see http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
 */
void MainWindow::on_comboBox_AdaptMethod_currentIndexChanged(int index)
{
    sendConsoleMsg(tr("Change adaptive thresholding algorithm to use, new value: ")
                   .append(ui->comboBox_AdaptMethod->itemText(index)));
    binarization();
}
/**
 * @brief MainWindow::on_comboBox_ThresType_currentIndexChanged
 * @param index
 * Изменение типа бинаризации (прямая или инвертированная)
 * Получишь либо белые пятна на черном фоне либо наоборот
 * @see http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
 */
void MainWindow::on_comboBox_ThresType_currentIndexChanged(int index)
{
    sendConsoleMsg(tr("Change thresholding type, new value: ")
                   .append(ui->comboBox_ThresType->itemText(index)));
    binarization();
}
/**
 * @brief MainWindow::on_hSlider_BlockSize_valueChanged
 * @param value
 * Изменение размера маски, по которой идет адаптивная бинаризация
 * @see http://docs.opencv.org/2.4/modules/imgproc/doc/miscellaneous_transformations.html#adaptivethreshold
 */
void MainWindow::on_hSlider_BlockSize_valueChanged(int value)
{
    bool odd = value % 2 == 1;
    value = odd ? value : cv::max(ui->hSlider_BlockSize->minimum(),value - 1);
    //должно быть нечетным
    if(odd) {
        sendConsoleMsg(tr("Change size of a pixel neighborhood that is used to calculate a threshold value, new value: ")
                       .append("%1")
                       .arg(value));
        binarization();// нечетное - бинаризуем
    }
    else ui->hSlider_BlockSize->setValue(value);// иначе подправляем и на второй заход
}

/**
 * @brief MainWindow::on_bgw_started
 * @param name - имя задачи
 * обработчик начала задачи
 */
void MainWindow::on_bgw_started(const QString &name)
{
    showProgressDialog(name);
}

/**
 * @brief MainWindow::on_bgw_finished
 * @param task - задача, которая была отправлена в BackGroundWoker::start()
 * Обработчик окончания задачи
 */
void MainWindow::on_bgw_finished(const QSharedPointer<AbstractTask> &task)
{
    AbstractTask* ptask = task.data();
    if(ptask->inherits("BinarizationTask")){
        //задача бинаризации
        BinarizationTask *bt = qobject_cast<BinarizationTask *>(task.data());
        double percent = bt->m_percent;
        m_cvDstImage = bt->m_dst;
        sendConsoleMsg(tr("Percentage of black pixs: ")
                       .append("%1 %")
                       .arg(percent)
                       .append(", size: ").append("%1").arg(m_cvSrcImage.cols).append("x%1").arg(m_cvSrcImage.rows)
                       .append(", params: [")
                       .append("gaussKsize : ").append("%1").arg(bt->m_gaussKSize).append(" , ")
                       .append("blockSize : ").append("%1").arg(bt->m_blockSize).append(" , ")
                       .append("const : ").append("%1").arg(bt->m_c).append(" , ")
                       .append("maxValue : ").append("%1").arg(bt->m_maxValue).append(" , ")
                       .append("adaptMethod : ").append("%1").arg(bt->m_adaptMethod).append(" , ")
                       .append("threshType : ").append("%1").arg(bt->m_threshType).append("]"));
        m_ivDst->setImage(bt->m_qtDst,false);
        // показываем выходное окно
        m_ivDst->show();
    }
    else if(ptask->inherits("OpenTask")){
        // задача открытия изображения
        OpenTask *ot = qobject_cast<OpenTask *>(task.data());
        if(!ot->m_cvImg.empty()){
            // толко если смогли открыть
            m_cvImage = ot->m_cvImg;
            ui->listWidget_Console->clear();
            sendConsoleMsg(tr("Open file:   ")
                           .append(ot->m_filename)
                           .append(", size: %1")
                           .arg(m_cvImage.cols)
                           .append("x%1")
                           .arg(m_cvImage.rows));
            //удалить все дочерние окна у mdi контейнера
            removeSubWindows();
            //создаем окна под входное и выходное
            m_ivSrc = createSubWindow(); m_ivSrc->setWindowTitle(tr("Input"));
            m_ivDst = createSubWindow(); m_ivDst->setWindowTitle(tr("Output"));
            //измение размеров
            resizeImage(ui->hSlider_ImgSize->value());
        }

    }
    else if(ptask->inherits("ResizeTask")){
        // задача изменеия размеров изображения
        ResizeTask *rt = qobject_cast<ResizeTask *>(task.data());
        // изменяем размер входного изображения
        m_cvSrcImage = rt->m_cvDst;
        sendConsoleMsg(tr("Change size, new value: ")
                       .append("%1")
                       .arg(m_cvSrcImage.cols)
                       .append("x%1")
                       .arg(m_cvSrcImage.rows));
        m_ivSrc->setImage(rt->m_qtDst,false);
        // показываем окно
        m_ivSrc->show();
        // запускаем алгоритм
        // т.к. размер входного изображения изменился
        binarization();
    }
    closeProgressDialog();
}

/**
 * @brief MainWindow::showProgressDialog
 * @param name - имя задачи
 * Показать модальный диалог с прогрессом
 */
void MainWindow::showProgressDialog(const QString &name)
{
    m_progressDlg->setLabelText(name + "...");
    m_progressDlg->show();
}

/**
 * @brief MainWindow::hideProgressDialog
 * Скрыть диалог хода выполнения задачи
 */
void MainWindow::closeProgressDialog()
{
    m_progressDlg->close();
}

