#pragma once
#include<string>
#include<iostream>
#include<windows.h>
#include<winuser.h>
#include<opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// ��������ģ���࣬��UI������Ϣ��Ӧ������ģʽ̫�鷳��ֱ�Ӷ���һ����̬����
class AssistConfig
{
public:
    // ��ȡ��ʵ��
    static AssistConfig* GetInstance();

    //�����������������
    //����ʵ�ʼ������ֻ�����׼���ĵ�һС�����򣬱��ϼ���ٶ�
    void ReCalDetectionRect();

    //��ȡ��Ļ���ű���
    double static getScreensZoom();

    //��������ģ��
    //ѡ�����Ϸ���̣���ͬ����Ϸ������������һ��
    string processNames[2] = { "��ս", "TslGame" };
    //string processName = "TslGame";
    string processName = "��ս"; 


    //�Ƿ�����ͼ����
    bool detectImg = true;
    //�û����õ�ʵ�ʼ���Ⱥ͸߶�
    int detectWidth = 320, detectHeight = 320;
    //��Ϸ����ռ�õ�ԭʼ��Ļ����
    cv::Rect screenRect = {0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN) };
    //�ü����ʵ��ͼ��������
    cv::Rect detectRect = { 0,0,detectWidth,detectHeight };
    //����������ĵ�
    int detectCentX = detectRect.x + detectRect.width / 2, detectCentY = detectRect.y + detectRect.height / 2;
    //������Ļ���ź�ģ��ü����ʵ��ͼ��������
    cv::Rect detectZoomRect = { 0,0,detectWidth,detectHeight };


    //�Ƿ��Զ�׷��
    bool autoTrace = false;
    //�Զ�׷�����ͣ�1�ǳ���׷�١�2������Ҽ���׼����׷��
    double mouseMoveSlow = 0.5; //�����ƶ�̫�죬��һ��������Ҫ�����ƶ��ٶȱ���,ֵҪ����ΪС��1
    int autoTraceType = 1;
    int autoTraceTimeOut = 0; //����Ҽ���׼����׷�ٵ���ʱ��ȱʡ��30������

    //�Ƿ��Զ�����
    bool autoFire = false;
    //ǹе�������,1�ǵ��㡢2��3���㡢3��6���㡢4�ǳ�����ǹ
    int gunType = 1;
    //������ǹʱ��
    int autoFireTime = 600;

    //�Ƿ��Զ�ѹǹ
    bool autoPush = false;


    //��Ϸ�������
    //��Ϸ��ұ��˵�ͼ������λ��
    int playerCentX = screenRect.width/2; 
    //��Ϸģ�������
    //int maxModelWidth = detectWidth / 4;
    int maxModelWidth = 150;

private:
    AssistConfig();
    ~AssistConfig();
    void test();

private:
    // Ψһ��ʵ������
    static AssistConfig* m_config;
};

