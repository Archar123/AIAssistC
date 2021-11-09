#include "pch.h"
#include "AssistWorker.h"


//��ʼ����̬��Ա����
AssistConfig* AssistWorker::m_AssistConfig = AssistConfig::GetInstance();


AssistWorker::AssistWorker()
{
    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //��������
    //���������г�������Ϊ1��Ŀ����ֻ�������¼������
    fireQueue = new BlockQueue<DETECTRESULTS>(1);
    moveQueue = new BlockQueue<DETECTRESULTS>(1);

    drawQueue = new BlockQueue<DRAWRESULTS>(10);
    outDrawQueue = new BlockQueue<Mat>(10);

    //�����߳�
    detectThread = new thread(std::bind(&AssistWorker::DetectWork, this));
    detectThread->detach();

    fireThread = new thread(std::bind(&AssistWorker::FireWork, this));
    fireThread->detach();

    moveThread = new thread(std::bind(&AssistWorker::MoveWork, this));
    moveThread->detach();

    drawThread = new thread(std::bind(&AssistWorker::DrawWork, this));
    drawThread->detach();

    //����ͼƬ��������������
    //imageDetection = new ImageDetection();
    imageDetection = new ImageDetectionTensorflow();
    mouseKeyboard = new MouseKeyboard();

    return;
}

AssistWorker::~AssistWorker()
{
    m_stopFlag = true;
    
    if (imageDetection != NULL)
        delete imageDetection;
    if (mouseKeyboard != NULL)
        delete mouseKeyboard;

    if (drawQueue != NULL)
        delete drawQueue;
    if (fireQueue != NULL)
        delete fireQueue;
    if (moveQueue != NULL)
        delete moveQueue;

    if (detectThread != NULL)
        delete detectThread;
    if (fireThread != NULL)
        delete fireThread;
    if (moveThread != NULL)
        delete moveThread;
    if (drawThread != NULL)
        delete drawThread;

    return;
}

//�޸����ú���Ҫ���³�ʼ��һЩ����
void AssistWorker::ReInit() {
    
    //��ֹͣ���й����߳�
    Pause();
    Sleep(100);

    //�����¼����������������
    m_AssistConfig->ReCalDetectionRect();

    //������ж���
    drawQueue->Clear();
    outDrawQueue->Clear();
    fireQueue->Clear();
    moveQueue->Clear();

    //�ؽ���Ҫ�ؽ��Ķ���
    if (imageDetection != NULL) {
        imageDetection->ReInit();
    }
    {
        //�½�����
        //imageDetection = new ImageDetection();
        imageDetection = new ImageDetectionTensorflow();
    }

    //���������߳�
    Start();

    return;
}

void AssistWorker::DetectWork()
{
    while (!m_stopFlag)
    {
        if (m_detectPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_detectMutex);
            while (m_detectPauseFlag)
            {
                m_detectCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //ͼ����
            double duration;
            clock_t start, finish;
            start = clock();

            //��Ļ����ͼ����
            imageDetection->getScreenshot();
            DETECTRESULTS detectResult = imageDetection->detectImg();

            finish = clock();
            duration = (double)(finish - start) * 1000 / CLOCKS_PER_SEC;

            if (detectResult.classIds.size() > 0) {
                //�м�鵽���࣬����ŵ�������,��֪ͨ�����߳����Ѽ����
                //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                //fireQueue->PushBackForce(detectResult);
                moveQueue->PushBackForce(detectResult);
            }

            //Ȼ����mat��������ǰ����ʾ
            Mat mat = imageDetection->getImg();

            DRAWRESULTS  drawResult{detectResult, mat, duration};
            bool push = drawQueue->PushBack(drawResult);

            //���������û�����ͳɹ������ֹ��ͷ�clone��mat����
            if (!push) {
                mat.release();
                mat = NULL;
            }
            
        }
    }

    return;
}

void AssistWorker::FireWork()
{
    while (!m_stopFlag)
    {
        if (m_firePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_fireMutex);
            while (m_firePauseFlag)
            {
                m_fireCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = fireQueue->PopFront(detectResult);
            if (ret) {
                //ִ���Զ���ǹ����
                //�ȼ���Ƿ��������Զ���ǹ��־
                if (m_AssistConfig->autoFire) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //����Ѿ���׼��ִ���Զ���ǹ����
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        //mouseKeyboard->AutoFire(detectResult);
                    }
                }
            }
            
        }
    }

    return;
}

void AssistWorker::MoveWork()
{
    while (!m_stopFlag)
    {
        if (m_movePauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_moveMutex);
            while (m_movePauseFlag)
            {
                m_moveCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DETECTRESULTS detectResult;
            bool ret = moveQueue->PopFront(detectResult);
            if (ret) {
                //ִ��������
                //std::cout << to_string(detectResult.classIds.size());
                //�ȼ���Ƿ��������Զ�׷��
                if (m_AssistConfig->autoTrace) {
                    //�ڼ���Ƿ��Ѿ���׼��
                    bool isInTarget = mouseKeyboard->IsInTarget(detectResult);
                    //û����׼������£���ִ�����׷�ٲ���
                    if (isInTarget) {
                        //��ǹ������ƶ��������ڲ�ͬ�̣߳����²������ѣ��ȷŻ�ͬһ���̴߳���
                        mouseKeyboard->AutoFire(detectResult); 
                    }
                    else {
                        mouseKeyboard->AutoMove(detectResult);
                    }
                }
            }
        }
    }

    return;
}

void AssistWorker::DrawWork()
{
    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::startWindowThread();

    while (!m_stopFlag)
    {
        if (m_drawPauseFlag)
        {
            //�����̱߳�־�����߳�����
            unique_lock<mutex> locker(m_drawMutex);
            while (m_drawPauseFlag)
            {
                m_drawCondition.wait(locker); // Unlock _mutex and wait to be notified
            }
            locker.unlock();
        }
        else {
            //��ȡ�����д�ŵļ����
            DrawResults drawResult;
            bool ret = drawQueue->PopFront(drawResult);
            if (ret) {
                //ִ�л�ͼ����
                DETECTRESULTS out = drawResult.out;
                Mat mat = drawResult.mat;
                double duration = drawResult.duration;
                if (!mat.empty()) {
                    string times = format("%.2f ms", duration);
                    putText(mat, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);

                    //ע����Ϸ��Ļ���ģ��ͼ�����������λ�ò�һ�������������������΢����1/10
                    Rect center = { mat.cols/2 -5,mat.rows / 2 + mat.rows /10 - 5,10,10 };
                    rectangle(mat, center, Scalar(0, 0, 250), 2);

                    if (out.classIds.size() > 0) {
                        Rect rect = out.boxes[0];
                        rectangle(mat, { rect.x + rect.width/2 -4, rect.y + rect.height / 2 - 4, 8, 8 }, Scalar(255, 178, 50), 2);
                    }

                    for (int i = 0; i < out.classIds.size(); i++) {
                        rectangle(mat, out.boxes[i], Scalar(255, 178, 50), 2);

                        //Get the label for the class name and its confidence
                        string label = format("%.2f", out.confidences[i]);
                        //label = m_classLabels[classIds[i]-1] + ":" + label;
                        label = "" + to_string(out.classIds[i]) + ", " + label;                      

                        //Display the label at the top of the bounding box
                        int baseLine;
                        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 2, &baseLine);
                        int top = max(out.boxes[i].y, labelSize.height);
                        putText(mat, label, Point(out.boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 250), 1);
                    }
                    
                    //�Ѵ�����õ�mat��������ⲿʹ�õĶ�����
                    bool push = outDrawQueue->PushBack(mat);
                    //�����������ʧ�ܣ��ֹ�����clone��mat����
                    if (!push) {
                        mat.release();
                        mat = NULL;
                    }
                   
                    //cv::imshow("opencvwindows", mat);
                    //waitKey(10);
                }
            }
        }
    }

    return;
}

Mat AssistWorker::PopDrawMat() {
    Mat mat;
    outDrawQueue->PopFront(mat);

    //ע�ⷵ�ؿ�¡���󣬲��ܰ�ȫ�ش���ͼ������
    Mat mat2 = mat.clone();

    //�ͷ���mat
    mat.release();
    mat = NULL;

    return mat2;
}

void AssistWorker::Start()
{
    m_detectPauseFlag = false;
    m_detectCondition.notify_all();
    
    m_firePauseFlag = false;
    m_fireCondition.notify_all();

    m_movePauseFlag = false;
    m_moveCondition.notify_all();

    m_drawPauseFlag = false;
    m_drawCondition.notify_all();

    return;
}

void AssistWorker::Pause()
{
    m_detectPauseFlag = true;
    m_detectCondition.notify_all();

    m_firePauseFlag = true;
    m_fireCondition.notify_all();

    m_movePauseFlag = true;
    m_moveCondition.notify_all();

    m_drawPauseFlag = true;
    m_drawCondition.notify_all();

    return;
}
