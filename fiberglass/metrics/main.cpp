/**
 * @file
 * @brief Программа считает метрики по бинарной картинке
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

/**
 * @brief confusion_matrix
 * @param truth - изображение с истинной (экспертной) разметкой
 * @param test - тестовое изображение
 * @param tp
 * @param fn
 * @param fp
 * @param tn
 * @param posValue - какой пиксель считать "положительным режением" в truth
 *
 * РАсчет таблицы сопряженности
 */
void confusionMatrix(const Mat &truth,
                   const Mat &test,
                   int &tp,
                   int &fn,
                   int &fp,
                   int &tn,
                   byte posValue = 0){
    //https://en.wikipedia.org/wiki/Sensitivity_and_specificity
    CV_Assert(test.cols == truth.cols &&
              test.rows == truth.rows &&
              test.type() == CV_8UC1 &&
              test.type() == truth.type());
    const int width = test.cols,
            height = test.rows,
            step = test.step;
    tp = fn = fp = tn = 0;
    for(int y = 0; y < height; y++){
        uchar* pTrue = truth.data + step * y;
        uchar* pTest = test.data + step * y;
        for(int x = 0; x < width; x++){
            if      (pTrue[x] == posValue && pTest[x] == posValue) tp++;
            else if (pTrue[x] == posValue && pTest[x] != posValue) fn++;
            else if (pTrue[x] != posValue && pTest[x] != posValue) tn++;
            else if (pTrue[x] != posValue && pTest[x] == posValue) fp++;
        }
    }
    CV_Assert( (tp + fn + tn + fp) == (width * height) );
}

/**
 * @brief measures
 * @param truth - изображение с истинной (экспертной) разметкой
 * @param test - тестовое изображение
 * @param IoU - Intersection over Union (Коэффициент Жаккара)
 * @param precision - точность
 * @param recall - полнота
 * @param specificity - специфичность
 * @param f1 - F1 метрика
 * @param fb - взвешеная Fb-мера
 * @param b - параметр для Fb (кому отдаем приоритет)
 * @param posValue - какой пиксель считать "положительным режением" в истинной разметке
 */
void measures(const Mat &truth,
                    const Mat &test,
                    double &IoU,
                    double &precision,
                    double &recall,
                    double &specificity,
                    double &f1,
                    double &fb,
                    double b = 1,
                    byte posValue = 0){
    //https://en.wikipedia.org/wiki/Sensitivity_and_specificity
    int tp,fn,fp,tn;
    confusionMatrix(truth,test,tp,fn,fp,tn,posValue);
    precision = tp == 0 ? 0 : (double) (tp) / (tp + fp);
    recall = tp == 0 ? 0 : (double)(tp) / (tp + fn); // TPR, hit rate
    specificity = tn == 0 ? 0 : (double) (tn) / (tn + fp);
    f1 = (precision == 0 && recall == 0) ? 0 : 2 * precision * recall / (precision + recall);
    if(b == 1 || f1 == 0) {
        fb = f1;
    }else{
        b *= b; // нужен квадрат
        fb = (1 + b) * precision * recall / ( b * precision + recall);
    }
    //https://en.wikipedia.org/wiki/Jaccard_index
    IoU = tp == 0 ? 0 : (double) (tp) / (tp + fn + fp);

}

int main(int argc, char *argv[])
{
    std::vector<string> thrus_images {"IMG_0_MASK.png",
                                      "IMG_2_MASK.png",
                                      "IMG_5_MASK.png",
                                      "IMG_11_MASK.png",
                                      "IMG_14_MASK.png",
                                      "IMG_19_MASK.png"};
    std::vector<string> result_images {"IMG_0.jpg",
                                      "IMG_2.jpg",
                                      "IMG_5.jpg",
                                      "IMG_11.jpg",
                                      "IMG_14.jpg",
                                      "IMG_19.jpg"};
    string thrus_path = "C:\\Users\\iurii.chiryshev\\Desktop\\images\\";
    string result_path = "global_thresh\\";


    double _IoU = 0;
    double _precision = 0;
    double _recall = 0;
    double _specificity = 0;
    double _f1 = 0;
    double _fb = 0;

    const int count = (int)thrus_images.size();

    for(int i = 0; i < count; i++){
        Mat img_true, img_result, gray_true, gray_result;

        img_true = cv::imread(thrus_path + thrus_images[i],cv::IMREAD_GRAYSCALE);
        img_result = cv::imread(thrus_path + result_path + result_images[i],cv::IMREAD_GRAYSCALE);

        double IoU;
        double precision;
        double recall;
        double specificity;
        double f1;
        double fb;

        measures(img_true,img_result,IoU,precision,recall,specificity,f1,fb,2,0);

        _IoU += IoU;
        _precision += precision;
        _recall += recall;
        _specificity += specificity;
        _f1 += f1;
        _fb += fb;

    }
    // среднее значение
    _IoU /= count;
    _precision /= count;
    _recall /= count;
    _specificity /= count;
    _f1 /= count;
    _fb /= count;

    cout << result_path << endl;
    cout << "_IoU"  << "\t"<< _IoU << endl;
    cout << "_precision"  << "\t"<< _precision << endl;
    cout << "_recall"  << "\t"<< _recall << endl;
    cout << "_specificity"  << "\t"<< _specificity << endl;
    cout << "_f1"  << "\t"<< _f1 << endl;
    cout << "_f2"  << "\t"<< _fb << endl;


    return 0;
}
