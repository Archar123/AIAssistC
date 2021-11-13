#pragma once

#include <iostream>
#include <string>
#include<math.h>

#include "mouse.h"
#include "keyboard.h"

#include "AssistConfig.h"
#include "ImageDetection.h"


enum MouseKeyboardType { MKTYPE_USBDEVICE, MKTYPE_HIDDRIVER, MKTYPE_WINDOWSEVENT };

// �����̲�����װ��
// ���ȼ�˳��Ӳ��USB�豸����δʵ�֣���HIDDriver������windows�¼�
class MouseKeyboard
{
public:
	MouseKeyboard();
	~MouseKeyboard();

	//����ʹ�õ�ģ���������
	MouseKeyboardType m_type = MKTYPE_USBDEVICE;

	//��������ƶ����
	//����һ��3D��Ϸ�ƶ����ĺ�����x1/y1Ϊ��Ϸ���ĵ����꣬x2/y2Ϊ��⵽���������ĵ����꣬zΪ��ά�����z�����
	//mouseMoveSlow�������ı���
	void MouseMove(LONG x1, LONG y1, LONG x2, LONG y2, double z, double mouseMoveSlow);
	//��굥��
	void MouseLBClick();

	//�ж��Ƿ��Ѿ���׼Ŀ��
	bool IsInTarget(DETECTRESULTS detectResult);
	//�ж��Ƿ���Ҫ�Զ�����
	void AutoFire(DETECTRESULTS detectResult);
	//�ƶ���굽��������������
	void AutoMove(DETECTRESULTS detectResult);

private:
	// Ψһ��ʵ������
	static AssistConfig* m_AssistConfig;

	//HIDDriver������ģ���������
	Mouse m_hidMouse;
	Keyboard m_hidKeyboard;
};

