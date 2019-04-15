/**
 * @file
 * @brief Программа создаст наборы данных и "экспертную бинарную разметку"
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * */

#include <iostream>
#include<windows.h>
#include <conio.h>
#include <time.h>
#define _USE_MATH_DEFINES // for C++
#include <math.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

struct Params{
    Params(  int gaussSize = 3,
            int blockSize = 131,
            double c = 1.0,
            int adaptMethod = 1,
            int threshType = 0){
        _gaussSize = gaussSize;
        _blockSize = blockSize;
        _c = c;
        _adaptMethod = adaptMethod;
        _threshType = threshType;
    }

    int _gaussSize;
    int _blockSize;
    double _c;
    int _adaptMethod;
    int _threshType;
};




void cutImage(const Mat &src, const cv::Size & size, const cv::Size &stride,vector<Mat> &images){
    images.clear();
    CV_Assert(src.data && "src.data = null");
    const int height = src.rows, width = src.cols;
    if(size.width > width || size.height > height) return;
    const cv::Point step((width - size.width) / stride.width + 1,(height - size.height) / stride.height + 1);
    for(int y = 0; y < step.y; y++){
        for(int x = 0; x < step.x; x++){
            cv::Rect rect( x * stride.width, y * stride.height, size.width, size.height);
            images.push_back(src(rect).clone()); // todo now do deep copy
        }
    }
}

void binary(const Mat &src, const Params &params, Mat &dst){
    cv::Mat gray, gauss;
    cv::Size ssize = src.size();
    CV_Assert(ssize.area() > 0 && "ssize.area() > 0");
    const int stype = src.type();
    // работаем только с gray или rgb
    CV_Assert(stype == CV_8UC1  || stype == CV_8UC3);
    // приводим к серому
    if(stype == CV_8UC3){
        cv::cvtColor(src,gray,CV_BGR2GRAY);
    }else{
        gray = src;
    }
    // сглаживание по гауссу
    cv::GaussianBlur(gray,gauss,cv::Size(params._gaussSize,params._gaussSize),0);
    // адаптивная бинаризация
    cv::adaptiveThreshold(gauss,dst,255,params._adaptMethod,params._threshType,params._blockSize,params._c);
}

void binary(const vector<Mat> &src, const vector<Params> &params, vector<Mat> &dst){
    dst.clear();
    CV_Assert(src.size() == params.size() && "src.size() == params.size()");
    for(int i = 0; i < src.size(); i++){
        Mat img;
        binary(src[i],params[i],img);
        dst.push_back(img);

    }
}

/**
 * @brief stategy1
 * Стратегия №1
 * 1. сначала режем изображение на куски
 * 2. бинаризуем каждую часть
 *
 */
void strategy1(){
    const string srcPath = "D:\\2017\\projects\\foto\\fiberglass\\IMG_3565.JPG"; // путь к исходному файлу
    const string dstFolder = ""; // выходная папка для файлов
    const cv::Size dstSize(1024,768); // размер выходных картинок
    const cv::Size dstStride(1024,768); // c каким шагом вытаскиваем картинки
    // читаем файл
    cout << "reading image..." << endl;
    Mat src = cv::imread(srcPath);
    vector<Mat> images;
    // разрезаем на куски
    cout << "cut image..." << endl;
    cutImage(src,dstSize,dstStride,images);
    CV_Assert(images.size() > 0 && "images.size() > 0");
    // бинаризуем
    vector<Mat> bin_images;
    vector<Params> params(images.size());
    cout << "binary images..." << endl;
    binary(images,params,bin_images);
    CV_Assert(images.size() == bin_images.size() && "images.size() == bin_images.size()");
    // сохраняем
    cout << "save images..." << endl;
    for(int i = 0; i < images.size(); i++){
        cv::imwrite(dstFolder + "IMG_" + std::to_string(i) + ".jpg",images[i]/*,{CV_IMWRITE_JPEG_QUALITY,95}*/);
        cv::imwrite(dstFolder + "IMG_" + std::to_string(i) + "_MASK.png",bin_images[i]/*,{CV_IMWRITE_JPEG_QUALITY,95}*/);
    }
    cout << "success" << endl;
}

/**
 * @brief stategy2
 * Стратегия №2
 * 1. Уменьшаем и бинаризуем исходное
 * 2. Восстанавливам в размерах и нарезаем
 * 3. сохраняем
 */
void strategy2(){
    const string srcPath = "D:\\2017\\projects\\foto\\fiberglass\\IMG_3565.JPG"; // путь к исходному файлу
    const string dstFolder = ""; // выходная папка для файлов
    const cv::Size dstSize(1024,768); // размер выходных картинок
    const cv::Size dstStride(1024,768); // c каким шагом вытаскиваем картинки
    // читаем файл
    cout << "reading image..." << endl;
    Mat src = cv::imread(srcPath);
    Mat smal_src;
    cout << "down scale image..." << endl;
    float s_factor = 2048.f / src.cols;
    cv::resize(src,smal_src,cv::Size(),s_factor,s_factor,INTER_LANCZOS4);
    Mat smal_bin, bin;
    cout << "binary image..." << endl;
    Params params;
    binary(smal_src,params,smal_bin);
    cout << "up scale image..." << endl;
    cv::resize(smal_bin,bin,cv::Size(),1 / s_factor,1 / s_factor,INTER_LANCZOS4);
    Mat tmp_bin;
    cv::threshold(bin,tmp_bin,128,255,THRESH_BINARY + THRESH_OTSU);
    bin = tmp_bin;
    cout << "cut image..." << endl;
    vector<Mat> images, bin_images;
    cutImage(src,dstSize,dstStride,images);
    cutImage(bin,dstSize,dstStride,bin_images);
    cout << "save images..." << endl;
    for(int i = 0; i < images.size(); i++){
        cv::imwrite(dstFolder + "IMG_" + std::to_string(i) + ".jpg",images[i]/*,{CV_IMWRITE_JPEG_QUALITY,95}*/);
        cv::imwrite(dstFolder + "IMG_" + std::to_string(i) + "_MASK.png",bin_images[i]/*,{CV_IMWRITE_JPEG_QUALITY,95}*/);
    }
    cout << "success" << endl;

}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 * точка входа в приложение
 */
int main(int argc, char *argv[])
{
    strategy1();
    //strategy2();

    return 0;
}
