#include <opencv2/opencv.hpp>
#include <iostream>
  
using namespace cv;  
using namespace std;
  
int main(int argc, char** argv)  
{  
    char * filename = "test.jpg";
    if(argc > 1)
    {
        filename = argv[1];
    }
    Mat imageSource = imread(filename);  
    cvNamedWindow("before", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("before", imageSource);  
    vector<Mat> imageRGB;  
  
    //RGB��ͨ������  
    split(imageSource, imageRGB);  
  
    //super paras
    double SR, SG, SB;  
    SB = 111.245;
    SG = 114.335;
    SR = 116.251;

    //��ԭʼͼ���RGB�����ľ�ֵ  
    double R, G, B;  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
  
    //��Ҫ������RGB����������  
    double KR, KG, KB;  
    KB = (R + G + B) * SB / ((SB + SG + SR) * B);  
    KG = (R + G + B) * SG / ((SB + SG + SR) * G);  
    KR = (R + G + B) * SR / ((SB + SG + SR) * R);  
  
    //����RGB����ͨ�����Ե�ֵ  
    imageRGB[0] = imageRGB[0] * KB;  
    imageRGB[1] = imageRGB[1] * KG;  
    imageRGB[2] = imageRGB[2] * KR;  
  
    //RGB��ͨ��ͼ��ϲ�  
    merge(imageRGB, imageSource);  
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", imageSource);  
    cout << "B,G,R:" << B << " " << G << " " << R << endl;
    cout << "KB,KG,kR:" << KB << " " << KG << " " << KR << endl;
    //��任��ͼ��RGB�����ľ�ֵ  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
    cout << "B,G,R:" << B << " " << G << " " << R << endl;
  
    waitKey();  
    return 0;  
}
