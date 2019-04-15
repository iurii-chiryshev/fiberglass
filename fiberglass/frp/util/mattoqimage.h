#ifndef MATTOQIMAGE_H
#define MATTOQIMAGE_H


#include <QImage>
#include <opencv2/opencv.hpp>

/**
 * @brief MatToQImage
 * @param mat
 * @return
 * @author Chiryshev Iurii <iurii.chiryshev@mail.ru>
 * Мы работаем с cv::mat а выводить на экран нужно QImage,
 * поэтому нужна функция-адаптер м/у представлением изображений в qt и opencv
 */
QImage MatToQImage(const cv::Mat& mat);


#endif // MATTOQIMAGE_H
