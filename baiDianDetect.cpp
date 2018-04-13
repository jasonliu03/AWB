#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;

float SQRT = 0.1; //0.1 ori
int ITE_NUMS = 30; //80 ori
float u=1,lamda=0.05,v=1;//u是B通道增益，v是R通道增益, ori lamda=0.05
int PHI = 165; //180 ori

void showMeanLab(const Mat labMap)
{
    //mean Lab
    double L, a, b;
    vector<Mat> imageLab;
    split(labMap, imageLab);
    L = mean(imageLab[0])[0];
    a = mean(imageLab[1])[0];
    b = mean(imageLab[2])[0];
    cout << "L,a,b:" << L << " " << a << " " << b << endl;
}

void myBGR2YUV(const Mat image,Mat &result)
{
    Mat_<Vec3b>::const_iterator it=image.begin<Vec3b>();
    Mat_<Vec3f>::iterator rit=result.begin<Vec3f>();
    Mat_<Vec3b>::const_iterator itend=image.end<Vec3b>();
//遍历所有像素 并转为YUV，注意在opencv中每个彩色像素是按BGR顺序存储的
    for(;it!=itend;++it,++rit)
    {
        (*rit)[0]=0.114*(*it)[0]+0.587*(*it)[1]+0.299*(*it)[2];//Y
        (*rit)[1]=0.5*(*it)[0]-0.331264*(*it)[1]-0.168736*(*it)[2];//Cb 
        (*rit)[2]=-0.081312*(*it)[0]-0.418688*(*it)[1]+0.5*(*it)[2];//Cr
    }

}


//估计图像result的色温为Temperature，阈值phi默认为180
void CountTemperature(const Mat result,Mat &Temperature ,const short phi=PHI)
{
    Mat_<Vec3f>::const_iterator rit=result.begin<Vec3f>();
    Mat_<Vec3f>::const_iterator ritend=result.end<Vec3f>();
    //遍历所有像素 估计色温
    float Y=0,Cb=0,Cr=0,n=0,Z=0,Y1=0,Cb1=0,Cr1=0;
    //const int phi=180;
    for(;rit!=ritend;++rit,++n)
    {
        Y1=(*rit)[0];
        Cb1=(*rit)[1];
        Cr1=(*rit)[2];
        Z=Y1-abs(Cb1)-abs(Cr1);
        if (Z>phi)
            {Y+=Y1;
             Cb+=Cb1;
             Cr+=Cr1;
            }
    }
    //所有像素处理完了
    Temperature.at<Vec3f>(0,0)[0]=Y/n;
    Temperature.at<Vec3f>(0,0)[1]=Cb/n;
    Temperature.at<Vec3f>(0,0)[2]=Cr/n;
}
//增益计算
void gain(Mat Temperature,float &u,float&v,float lamda=0.05)
{
    float Y_T=Temperature.at<Vec3f>(0,0)[0];
    float Cb_T=Temperature.at<Vec3f>(0,0)[1];
    float Cr_T=Temperature.at<Vec3f>(0,0)[2];
    //增益计算
//  float u=1,lamda=0.05,v=1;//u是B通道增益，v是R通道增益
    if(abs(Cb_T)>abs(Cr_T))
        if(Cb_T>0)
            u-=lamda;
        else
           u+=lamda;
    else
        if(Cr_T>0)
            v-=lamda;
        else
           v+=lamda;

}
//色温校正

void correctionImage(const Mat image,Mat &result,const float u ,const float v)
{
    Mat_<Vec3b>::const_iterator it=image.begin<Vec3b>();
    Mat_<Vec3b>::const_iterator itend=image.end<Vec3b>();

    Mat_<Vec3b>::iterator rit=result.begin<Vec3b>();
    Mat_<Vec3b>::iterator ritend=result.end<Vec3b>();
    //遍历所有像素 改变B和R通道的值
    for(;it!=itend;++it,++rit)
    {
        (*rit)[0]=saturate_cast<uchar>(u*(*it)[0]);//B
        (*rit)[1]=saturate_cast<uchar>((*it)[1]);//G
        (*rit)[2]=saturate_cast<uchar>(v*(*it)[2]);//R

    }


}
int main(int argc, char** argv)
{
    Mat image=imread("./test.jpg"); 
    if(argc > 1)
    {
        image = imread(argv[1]);
    }
    if(!image.data)
    {   
    cout<<"图像加载失败"<<endl;
    return -1;
    }   
    string op_imageName = argv[1];
    op_imageName = "baiDet_" + op_imageName;
    Mat result;//YUV格式
    result.create(image.rows,image.cols,CV_32FC3);
    Mat final;//白平衡后的BGR图像
    final.create(image.rows,image.cols,image.type());

    Mat Temperature(1,1, CV_32FC3,Scalar(100));//初始化色温为100
    //BGR转为YUV
    myBGR2YUV(image,result);
     //估计色温
    CountTemperature(result,Temperature);
    float Cb_T=Temperature.at<Vec3f>(0,0)[1];
    float Cr_T=Temperature.at<Vec3f>(0,0)[2];
    float C=sqrt(Cb_T*Cb_T+Cr_T*Cr_T);

    int index=0;//迭代次数
    do
    {

        //增益计算 u,v
        if (index>25)//
            lamda=0.02;
        if (index>40)//
            lamda=0.005;
            if (index>55)//
            lamda=0.001;
        gain(Temperature,u,v,lamda);

        //色温校正后图像为final
        correctionImage(image,final,u ,v);

        //BGR转为YUV
        myBGR2YUV(final,result);
        //估计色温  
        CountTemperature(result,Temperature);
        //cb,cr是否足够小
        Cb_T=Temperature.at<Vec3f>(0,0)[1];
        Cr_T=Temperature.at<Vec3f>(0,0)[2];
        C=sqrt(Cb_T*Cb_T+Cr_T*Cr_T);
        ++index;
        cout<<"index="<<index<<" u="<<u<<"   v="<<v<<"  C="<<C<<endl;

    }
    while (C>SQRT&&index<ITE_NUMS);

    if(!index) image.copyTo(final);//拷贝
    cout<<"C="<<C<<"   Cb="<<Cb_T<<"   Cr="<<Cr_T<<endl;

    //cout<<"图像加载成功"<<result.type()<<endl;
    namedWindow("OriginalImage", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("OriginalImage",image);
    namedWindow("WhiteBalance", CV_WINDOW_NORMAL| CV_WINDOW_KEEPRATIO| CV_GUI_EXPANDED);
    imshow("WhiteBalance",final);
    imwrite(op_imageName, final);

    Mat labMap = image.clone();
    cvtColor(labMap, labMap, COLOR_BGR2Lab);
    showMeanLab(labMap);

    labMap = final.clone();
    cvtColor(labMap, labMap, COLOR_BGR2Lab);
    showMeanLab(labMap);

    waitKey();
    
    image.release();
    result.release();
    final.release();
    Temperature.release();

    labMap.release();

    return 0;
}
