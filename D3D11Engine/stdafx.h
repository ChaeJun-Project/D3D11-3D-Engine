//�̸� �����ϵ� ���(D3D11Engine����)
#pragma once

#include "D3D11Framework.h"
//_Libraries ���丮 -> Lib_x64 ���丮��
//D3D11Framework�� ���� ���̺귯���� ����� ������
//���ŵǰ� �װ��� D3D11Engine���� �����(���̺귯�� ��ũ)
#pragma comment(lib, "D3D11Framework.lib")

#include <map>

//Imgui (����: https://github.com/ocornut/imgui)
#include "ImGui/Source/imgui.h" //ImGui�� ������� �ݵ�� imgui.h���� �ڿ� ���ԵǾ�� �ϹǷ� ���� �տ� ����
#include "ImGui/Source/imgui_internal.h"
#include "ImGui/Source/imgui_stdlib.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

//Helper
#include "Helper/ImGui_Helper.h"
#include "Helper/Editor_Helper.h"
#include "Helper/Icon_Provider.h"
#include "Helper/DragDropEvent.h"
#include "Helper/ColorPicker.h"
#include "Helper/ScriptEditor.h"
