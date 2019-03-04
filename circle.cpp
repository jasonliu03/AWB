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
    cout << imageSource.rows << imageSource.cols << endl;
    cout << rows << cols << endl;
    Mat avgMeanMat(rows, cols, CV_32FC3, Scalar(0,0,0));
    Mat avgDevMat(rows, cols, CV_32FC3, Scalar(0,0,0));
   
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            //cout << imageSource.cols*j/cols << imageSource.rows*i/rows << imageSource.cols/cols << imageSource.rows/rows << endl;
            Mat srcROI = imageSource(Rect(imageSource.cols*j/cols,imageSource.rows*i/rows,imageSource.cols/cols,imageSource.rows/rows));
            stringstream ssBlock;
            ssBlock << "block" << i << j << ".jpg";
            string sBlockName = ssBlock.str();
            cout << "sBlockName:" << sBlockName << endl;
            imwrite(sBlockName, srcROI);

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

    Mat mulMat = avgMeanMat.mul(avgDevMat);
    vector<Mat> splitMulMatBGR;
    split(mulMat, splitMulMatBGR);

    double avgMeanB, avgMeanG, avgMeanR;
    vector<Mat> devMatBGR;
    split(avgDevMat, devMatBGR);
    
    Scalar sumMeanMat = sum(mulMat);
    Scalar sumDevMat = sum(avgDevMat);
    for(int i=0; i<3; i++){
        cout << sumMeanMat[i]/sumDevMat[i] << endl;
    }
       
    
    //super paras
    double SR, SG, SB;  
    SB = 111.245;
    SG = 114.335;
    SR = 116.251;

    //求原始图像的RGB分量的均值  
    double R, G, B;  
    vector<Mat> imageRGB;  

    //RGB三通道分离  
    split(imageSource, imageRGB);  
                
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
  
    //RGB三通道图像合并  
    merge(imageRGB, imageSource);  
    cvNamedWindow("after", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("after", imageSource);  
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
