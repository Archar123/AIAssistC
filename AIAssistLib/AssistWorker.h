#pragma once
#include<string>
#include<iostream>
#include <thread>
#include<opencv2/opencv.hpp>

#include "AssistConfig.h"
#include "ImageDetection.h"
#include "ImageDetectionTensorflow.h"
#include "MouseKeyboard.h"
#include "BlockQueue.h"

using namespace cv;
using namespace std;


//���������ṹ��
typedef struct DrawResults
{
    DETECTRESULTS out;
    Mat mat;
    double duration; //���ִ��ʱ��
}DRAWRESULTS;


// ���ֹ����࣬ʹ�ö��̷߳�ʽ���ͼ�����ִ�������̲���
class AssistWorker
{
public:

    //���캯������������
    AssistWorker();
    ~AssistWorker();

    //�޸����ú���Ҫ���³�ʼ��һЩ����
    void ReInit();

    //��⺯��������������
    void DetectWork();
    void FireWork();
    void MoveWork();
    void DrawWork();
    //������ͼmat���������ⲿ�̻߳�ͼ��������û������ʱ�����������ȴ�
    Mat PopDrawMat();

    //��������ֹͣ����
    void Start();
    void Pause();


private:
    // Ψһ��ʵ������
    static AssistConfig* m_AssistConfig;

    //ҵ�������
    //ImageDetection* imageDetection ;
    ImageDetectionTensorflow* imageDetection;
    MouseKeyboard* mouseKeyboard;

    //����̺߳��������̺߳ͻ�ͼ�߳�
    //��ǹ�̺߳��ƶ���׼�̺߳Ͷ��ж��ֿ�����֤��ǹ�ļ�ʹ�ԡ�
    thread* detectThread = NULL;
    thread* fireThread = NULL;
    thread* moveThread = NULL;
    thread* drawThread = NULL;

    //�߳̿��Ʊ���
    std::mutex m_detectMutex;
    std::condition_variable m_detectCondition;
    std::atomic_bool m_detectPauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_fireMutex;
    std::condition_variable m_fireCondition;
    std::atomic_bool m_firePauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_moveMutex;
    std::condition_variable m_moveCondition;
    std::atomic_bool m_movePauseFlag = true;   ///<��ͣ��ʶ

    std::mutex m_drawMutex;
    std::condition_variable m_drawCondition;
    std::atomic_bool m_drawPauseFlag = true;   ///<��ͣ��ʶ

    std::atomic_bool m_stopFlag = false;   ///<ֹͣ��ʶ

    //���������
    BlockQueue<DRAWRESULTS>* drawQueue;
    BlockQueue<DETECTRESULTS>* fireQueue;
    BlockQueue<DETECTRESULTS>* moveQueue;

    BlockQueue<Mat>* outDrawQueue;

};

