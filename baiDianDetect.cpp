#include <opencv2/core/core.hpp>  
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;

float SQRT = 0.1; //0.1 ori
int ITE_NUMS = 30; //80 ori
float u=1,lamda=0.05,v=1;//u��Bͨ�����棬v��Rͨ������, ori lamda=0.05
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
//������������ ��תΪYUV��ע����opencv��ÿ����ɫ�����ǰ�BGR˳��洢��
    for(;it!=itend;++it,++rit)
    {
        (*rit)[0]=0.114*(*it)[0]+0.587*(*it)[1]+0.299*(*it)[2];//Y
        (*rit)[1]=0.5*(*it)[0]-0.331264*(*it)[1]-0.168736*(*it)[2];//Cb 
        (*rit)[2]=-0.081312*(*it)[0]-0.418688*(*it)[1]+0.5*(*it)[2];//Cr
    }

}


//����ͼ��result��ɫ��ΪTemperature����ֵphiĬ��Ϊ180
void CountTemperature(const Mat result,Mat &Temperature ,const short phi=PHI)
{
    Mat_<Vec3f>::const_iterator rit=result.begin<Vec3f>();
    Mat_<Vec3f>::const_iterator ritend=result.end<Vec3f>();
    //������������ ����ɫ��
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
    //�������ش�������
    Temperature.at<Vec3f>(0,0)[0]=Y/n;
    Temperature.at<Vec3f>(0,0)[1]=Cb/n;
    Temperature.at<Vec3f>(0,0)[2]=Cr/n;
}
//�������
void gain(Mat Temperature,float &u,float&v,float lamda=0.05)
{
    float Y_T=Temperature.at<Vec3f>(0,0)[0];
    float Cb_T=Temperature.at<Vec3f>(0,0)[1];
    float Cr_T=Temperature.at<Vec3f>(0,0)[2];
    //�������
//  float u=1,lamda=0.05,v=1;//u��Bͨ�����棬v��Rͨ������
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
//ɫ��У��

void correctionImage(const Mat image,Mat &result,const float u ,const float v)
{
    Mat_<Vec3b>::const_iterator it=image.begin<Vec3b>();
    Mat_<Vec3b>::const_iterator itend=image.end<Vec3b>();

    Mat_<Vec3b>::iterator rit=result.begin<Vec3b>();
    Mat_<Vec3b>::iterator ritend=result.end<Vec3b>();
    //������������ �ı�B��Rͨ����ֵ
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
    cout<<"ͼ�����ʧ��"<<endl;
    return -1;
    }   
    string op_imageName = argv[1];
    op_imageName = "baiDet_" + op_imageName;
    Mat result;//YUV��ʽ
    result.create(image.rows,image.cols,CV_32FC3);
    Mat final;//��ƽ����BGRͼ��
    final.create(image.rows,image.cols,image.type());

    Mat Temperature(1,1, CV_32FC3,Scalar(100));//��ʼ��ɫ��Ϊ100
    //BGRתΪYUV
    myBGR2YUV(image,result);
     //����ɫ��
    CountTemperature(result,Temperature);
    float Cb_T=Temperature.at<Vec3f>(0,0)[1];
    float Cr_T=Temperature.at<Vec3f>(0,0)[2];
    float C=sqrt(Cb_T*Cb_T+Cr_T*Cr_T);

    int index=0;//��������
    do
    {

        //������� u,v
        if (index>25)//
            lamda=0.02;
        if (index>40)//
            lamda=0.005;
            if (index>55)//
            lamda=0.001;
        gain(Temperature,u,v,lamda);

        //ɫ��У����ͼ��Ϊfinal
        correctionImage(image,final,u ,v);

        //BGRתΪYUV
        myBGR2YUV(final,result);
        //����ɫ��  
        CountTemperature(result,Temperature);
        //cb,cr�Ƿ��㹻С
        Cb_T=Temperature.at<Vec3f>(0,0)[1];
        Cr_T=Temperature.at<Vec3f>(0,0)[2];
        C=sqrt(Cb_T*Cb_T+Cr_T*Cr_T);
        ++index;
        cout<<"index="<<index<<" u="<<u<<"   v="<<v<<"  C="<<C<<endl;

    }
    while (C>SQRT&&index<ITE_NUMS);

    if(!index) image.copyTo(final);//����
    cout<<"C="<<C<<"   Cb="<<Cb_T<<"   Cr="<<Cr_T<<endl;

    //cout<<"ͼ����سɹ�"<<result.type()<<endl;
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
