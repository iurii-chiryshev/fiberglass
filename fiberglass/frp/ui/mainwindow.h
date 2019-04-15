
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QProgressDialog>
#include <opencv2/opencv.hpp>
#include "ui/imageviewer.h"
#include "util/mattoqimage.h"
#include "async/backgroundworker.h"
#include "task/binarizationtask.h"
#include "io/opentask.h"
#include "task/resizetask.h"


namespace Ui {
class MainWindow;
}
/**
 * @brief The MainWindow class
 * Класс главного окна приложения
 * основная часть накидана в дизайнере в mainwindow.ui
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /**
     * @brief ui
     * дизайнерская хрень
     */
    Ui::MainWindow *ui;
    /**
     * @brief m_ivSrc
     * указатель на окно с исходным изображением
     */
    ImageViewer* m_ivSrc;
    /**
     * @brief m_ivDst
     * указатель на окно с выходным (бинарным) изображением
     */
    ImageViewer* m_ivDst;
    /**
     * @brief m_cvImage
     * openсv изображение, которое открывает пользователь
     */
    cv::Mat m_cvImage;
    /**
     * @brief m_cvSrcImage
     * openсv изображение, с которым работает программа т.е. обрабатывает и выводит
     */
    cv::Mat m_cvSrcImage;
    /**
     * @brief m_cvDstImage
     * openсv выходное изображение, с таким же размером как m_cvSrcImage
     */
    cv::Mat m_cvDstImage;
    /**
     * @brief m_bgWoker
     */
    BackgroundWorker m_bgWoker;
    /**
     * @brief m_progressDlg
     * диалог прогресса для долгих операций
     */
    QProgressDialog* m_progressDlg;

    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName,const QImage &image);
    void resizeImage(int value);
    void removeSubWindows();
    ImageViewer *createSubWindow();
    void binarization();
    void sendConsoleMsg(const QString &msg);
    void showProgressDialog(const QString &name);
    void closeProgressDialog();


private slots:

    void on_actionSaveAs();
    void on_actionOpen();
    void on_actionZoom_In();
    void on_actionZoom_Out();
    void on_actionNormal_Size();
    void on_actionFit_to_Window();
    void on_actionCascade();
    void on_actionTile();

    void on_hSlider_ImgSize_valueChanged(int value);
    void on_hSlider_GaussKsize_valueChanged(int value);
    void on_hSlider_MaxValue_valueChanged(int value);
    void on_hSlider_C_valueChanged(int value);
    void on_comboBox_AdaptMethod_currentIndexChanged(int index);
    void on_comboBox_ThresType_currentIndexChanged(int index);
    void on_hSlider_BlockSize_valueChanged(int value);

    void on_bgw_started(const QString &name);
    void on_bgw_finished(const QSharedPointer<AbstractTask>& task);
};

#endif // MAINWINDOW_H
