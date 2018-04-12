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
    string op_imagename = filename;
    op_imagename = "gw_" + op_imagename;
    Mat imageSource = imread(filename);  
    cvNamedWindow("before", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("before", imageSource);  
    vector<Mat> imageRGB;  

    //Lab-ori
    Mat labMap_ori = imageSource.clone();
    cvtColor(imageSource, labMap_ori, COLOR_BGR2Lab);    
    cvNamedWindow("Lab_ori", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("Lab_ori", labMap_ori);
    
    //mean Lab
    double L, a, b;
    vector<Mat> imageLab_ori;  
    split(labMap_ori, imageLab_ori);
    L = mean(imageLab_ori[0])[0];
    a = mean(imageLab_ori[1])[0];
    b = mean(imageLab_ori[2])[0];
    cout << "imageLab_ori L,a,b:" << L << " " << a << " " << b << endl;

  
    //RGB三通道分离  
    split(imageSource, imageRGB);  
  
    //super paras for BGR
    double SR, SG, SB;  
    SB = 111.245;
    SG = 114.335;
    SR = 116.251;

    //求原始图像的RGB分量的均值  
    double R, G, B;  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
  
    //super para for value
    double KValue = (SB+SG+SR)/(B+G+R);
    double FIX_KValue = 1.0;

    //需要调整的RGB分量的增益  
    double KR, KG, KB;  
    KB = FIX_KValue * KValue * (R + G + B) * SB / ((SB + SG + SR) * B);  
    KG = FIX_KValue * KValue * (R + G + B) * SG / ((SB + SG + SR) * G);  
    KR = FIX_KValue * KValue * (R + G + B) * SR / ((SB + SG + SR) * R);  
  
    //调整RGB三个通道各自的值  
    imageRGB[0] = imageRGB[0] * KB;  
    imageRGB[1] = imageRGB[1] * KG;  
    imageRGB[2] = imageRGB[2] * KR;  
  
    //RGB三通道图像合并  
    merge(imageRGB, imageSource);  
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", imageSource);  
    cout << "before B,G,R:" << B << " " << G << " " << R << endl;
    cout << "KB,KG,kR:" << KB << " " << KG << " " << KR << endl;

    //求变换后图像RGB分量的均值  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
    cout << "after B,G,R:" << B << " " << G << " " << R << endl;


    //Lab
    Mat labMap = imageSource.clone();
    cvtColor(imageSource, labMap, COLOR_BGR2Lab);    
    cvNamedWindow("Lab", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("Lab", labMap);
    
    //mean Lab
    vector<Mat> imageLab;  
    split(labMap, imageLab);
    L = mean(imageLab[0])[0];
    a = mean(imageLab[1])[0];
    b = mean(imageLab[2])[0];
    cout << "after L,a,b:" << L << " " << a << " " << b << endl;
    
    //super paras for Lab
    double SL, Sa, Sb;
    SL = 117.565;
    Sa = 128.386;
    Sb = 129.879;

    double KL = SL / L;
    imageLab[0] = imageLab[0] * KL;
    
    merge(imageLab, labMap);

    //求变换后图像Lab分量的均值  
    L = mean(imageLab[0])[0];  
    a = mean(imageLab[1])[0];  
    b = mean(imageLab[2])[0];  
    cout << "afterL L,a,b:" << L << " " << a << " " << b << endl;

    cvtColor(labMap, imageSource, COLOR_Lab2BGR);    
    cvNamedWindow("afterL", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("afterL", imageSource);

    imwrite(op_imagename.c_str(), imageSource);
    
    imageSource.release();
    labMap_ori.release();
    labMap.release();
  
    waitKey();  
    return 0;  
}
