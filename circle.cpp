#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
  
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
    
    int rows = imageSource.rows/16;
    int cols = imageSource.cols/16;
    cout << imageSource.rows << " " << imageSource.cols << endl;
    cout << rows << " " << cols << endl;
    Mat avgMeanMat(rows, cols, CV_32FC3, Scalar(0,0,0));
    Mat avgDevMat(rows, cols, CV_32FC3, Scalar(0,0,0));
   
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            Mat srcROI = imageSource(Rect(imageSource.cols*j/cols,imageSource.rows*i/rows,imageSource.cols/cols,imageSource.rows/rows));
            stringstream ssBlock;
            ssBlock << "block" << i << j << ".jpg";
            string sBlockName = ssBlock.str();
            cout << "sBlockName:" << sBlockName << endl;
            //imwrite(sBlockName, srcROI);

            Scalar mean2;
            Scalar stddev;
            meanStdDev( srcROI, mean2, stddev );
            avgMeanMat.at<Vec3f>(i,j)[0] = mean2[0];
            avgMeanMat.at<Vec3f>(i,j)[1] = mean2[1];
            avgMeanMat.at<Vec3f>(i,j)[2] = mean2[2];
            avgDevMat.at<Vec3f>(i,j)[0] = stddev[0];
            avgDevMat.at<Vec3f>(i,j)[1] = stddev[1];
            avgDevMat.at<Vec3f>(i,j)[2] = stddev[2];
        }
    } 

    // circle based on mean,dev
    Mat mulMat = avgMeanMat.mul(avgDevMat);
    vector<Mat> splitMulMatBGR;
    split(mulMat, splitMulMatBGR);

    double avgMeanB, avgMeanG, avgMeanR;
    vector<Mat> devMatBGR;
    split(avgDevMat, devMatBGR);
    
    Scalar sumMeanMat = sum(mulMat);
    Scalar sumDevMat = sum(avgDevMat);
    double WB, WG, WR;
    WB = sumMeanMat[0]/sumDevMat[0]; 
    WG = sumMeanMat[1]/sumDevMat[1]; 
    WR = sumMeanMat[2]/sumDevMat[2]; 
    double Bg, Gg, Rg;
    Bg = (WB + WG + WR)/(3*WB);
    Gg = (WB + WG + WR)/(3*WG);
    Rg = (WB + WG + WR)/(3*WR);
    cout << "Bg,Gg,Rg:" << Bg << " " << Gg << " " << Rg << endl; 

    //calc hist, discrete entropy
    double hisB[256] = {0};
    double hisG[256] = {0};
    double hisR[256] = {0};
    for(int i=0; i<imageSource.rows; i++){
        for(int j=0; j<imageSource.cols; j++){
            hisB[imageSource.at<Vec3b>(i,j)[0]] += 1;
            hisG[imageSource.at<Vec3b>(i,j)[1]] += 1;
            hisR[imageSource.at<Vec3b>(i,j)[2]] += 1;
        }
    }
    for(int i=0; i<256; i++){
        hisB[i] /= imageSource.rows*imageSource.cols;
        hisG[i] /= imageSource.rows*imageSource.cols;
        hisR[i] /= imageSource.rows*imageSource.cols;
    }
    double HB=0, HG=0, HR=0;
    for(int i=0; i<256; i++){
        if(hisB[i] != 0) HB += hisB[i] * (log(hisB[i])/log(2));
        if(hisG[i] != 0) HG += hisG[i] * (log(hisG[i])/log(2));
        if(hisR[i] != 0) HR += hisR[i] * (log(hisR[i])/log(2));
    }
    HB = -HB/8;
    HG = -HG/8;
    HR = -HR/8;
    double kB=0, kG=0, kR=0;
    kB = (Bg -1)*HB + 1;
    kG = (Gg -1)*HG + 1;
    kR = (Rg -1)*HR + 1;
    cout << "H:" << HB << HG << HR << endl;
    cout << "k:" << kB << kG << kR << endl;
    

    //super paras
    double SR=0, SG=0, SB=0;  
    SB = 111.245;
    SG = 114.335;
    SR = 116.251;

    //求原始图像的RGB分量的均值  
    double R, G, B;  
    vector<Mat> imageRGB;  
    vector<Mat> imageRGB2;  

    //RGB三通道分离  
    split(imageSource, imageRGB);  
    split(imageSource, imageRGB2);  
                
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
                
    //需要调整的RGB分量的增益  
    double KR, KG, KB;  
    KB = (R + G + B) * SB / ((SB + SG + SR) * B);  
    KG = (R + G + B) * SG / ((SB + SG + SR) * G);  
    KR = (R + G + B) * SR / ((SB + SG + SR) * R);  
  
    //调整RGB三个通道各自的值  
    imageRGB[0] = imageRGB[0] * KB;  
    imageRGB[1] = imageRGB[1] * KG;  
    imageRGB[2] = imageRGB[2] * KR;  
  
    imageRGB2[0] = imageRGB2[0] * kB;  
    imageRGB2[1] = imageRGB2[1] * kG;  
    imageRGB2[2] = imageRGB2[2] * kR;  

    //RGB三通道图像合并  
    merge(imageRGB, imageSource);  
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", imageSource);  

    merge(imageRGB2, imageSource);  
    cvNamedWindow("after2", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after2", imageSource);  

    cout << "B,G,R:" << B << " " << G << " " << R << endl;
    cout << "KB,KG,kR:" << KB << " " << KG << " " << KR << endl;
    //求变换后图像RGB分量的均值  
    B = mean(imageRGB[0])[0];  
    G = mean(imageRGB[1])[0];  
    R = mean(imageRGB[2])[0];  
    cout << "B,G,R:" << B << " " << G << " " << R << endl;
  
    waitKey();  
    return 0;  
}
