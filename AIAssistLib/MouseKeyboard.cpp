#include "pch.h"
#include "MouseKeyboard.h"


//��ʼ����̬��Ա����
AssistConfig* MouseKeyboard::m_AssistConfig = AssistConfig::GetInstance();


MouseKeyboard::MouseKeyboard() {

    try {
        m_mouse.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("����豸��ʼ��ʧ��: ") + e.what() << std::endl;
    }

    try {
        m_keyboard.initialize();
    }
    catch (const std::runtime_error& e) {
        std::cout << std::string("�����豸��ʼ��ʧ��: ") + e.what() << std::endl;
    }

	return;
}

MouseKeyboard::~MouseKeyboard() {

	return;
}


//�ж��Ƿ��Ѿ���׼Ŀ��
bool MouseKeyboard::IsInTarget(DETECTRESULTS detectResult) {
    bool ret = false;

    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //ʹ�ü���õ���Ϸ��Ļ��������
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //������Ա���������꣬����Ϊ���ϵ�λ�ã�������ͷ
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width / 2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height / 4;

        //ǹ���ƶ�����Ա����ָ��λ�ú��Զ���ǹ
        if ((abs(x2 - x1) < rect.width / 3) && (abs(y2 - y1) < rect.height / 4)) {
            ret = true;
        }
    }

    return ret;
}

//�Զ�����
void MouseKeyboard::AutoFire(DETECTRESULTS detectResult) {

    m_mouse.leftButtonClick();

    return;
}


//�Զ��ƶ����
void MouseKeyboard::AutoMove(DETECTRESULTS detectResult) {
    
    if (detectResult.maxPersonConfidencePos >= 0 && detectResult.boxes.size() > 0) {
        //ʹ�ü���õ���Ϸ��Ļ��������
        LONG x1 = m_AssistConfig->detectCentX;
        LONG y1 = m_AssistConfig->detectCentY;

        //������Ա���������꣬����Ϊ���ϵ�λ�ã�������ͷ
        Rect rect = detectResult.boxes.at(detectResult.maxPersonConfidencePos);
        LONG x2 = m_AssistConfig->detectRect.x + rect.x + rect.width/2;
        LONG y2 = m_AssistConfig->detectRect.y + rect.y + rect.height/ 4;

        //������3D��Ϸ��λ����3ά���꣬����ԽԶ���ƶ�����Ҫ�˵�ϵ����Խ��
        //��ʱû�кõķ���ͨ��ͼƬ������3ά���꣬��ʹ�ö���Ĵ�С���Լ���z���꣬���ǿ�����Ĵ�С��ʱ�޷�����
        //Ϊ�˴���̫ԶͼƬ�����⣬�ڰ�����log����һ��������ʵ��λ��ԽԶ��������̫���Ч����
        //��������������ƶ����ʱ��Ҫ����������Ϊ3ά����У��ƶ��������ž���Ŵ�
        double z = 1;
        if (m_AssistConfig->maxModelWidth > 0 && m_AssistConfig->maxModelWidth  > rect.width)
        {
            //log����ȡֵ�������ƶ�����ƫ�࣬�ĳ����Լ���
            //z = log2(m_AssistConfig->maxModelWidth) / (rect.width);
            z = m_AssistConfig->maxModelWidth / (rect.width);
        }
        

        //�ƶ����
        //3D��Ϸ�ƶ����ĺ�����x1/y1Ϊ��Ϸ���ĵ����꣬x2/y2Ϊ��⵽���������ĵ����꣬zΪ��ά�����z�����
        //mouseMoveSlow�������ı���
        m_mouse.moveCursor(x1, y1, x2, y2, z, m_AssistConfig->mouseMoveSlow);
    }

    return;
}