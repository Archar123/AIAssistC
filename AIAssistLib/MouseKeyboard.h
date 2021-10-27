#pragma once

#include <iostream>
#include <string>
#include<math.h>

#include "mouse.h"
#include "keyboard.h"

#include "AssistConfig.h"
#include "ImageDetection.h"


// �����̲�����װ��
class MouseKeyboard
{
public:
	MouseKeyboard();
	~MouseKeyboard();

	//�ж��Ƿ��Ѿ���׼Ŀ��
	bool IsInTarget(DETECTRESULTS detectResult);
	//�ж��Ƿ���Ҫ�Զ�����
	void AutoFire(DETECTRESULTS detectResult);
	//�ƶ���굽��������������
	void AutoMove(DETECTRESULTS detectResult);

private:
	// Ψһ��ʵ������
	static AssistConfig* m_AssistConfig;

	Mouse m_mouse;
	Keyboard m_keyboard;
};

