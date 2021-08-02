#pragma once

//D3D11 Engine �������Ͽ��� D3D11 Framework ���̺귯�� ����� ����� �� �ֵ��� ������ ���ִ� Ŭ����

//�̱������� ����
class Editor_Helper final
{
private:
	//�⺻ ������
	Editor_Helper() = default;
	//�⺻ �Ҹ���
	~Editor_Helper() = default;

public:
    //���� ��� ���� ��ȯ �Լ�
	static Editor_Helper* GetInstance()
	{
		if (!instance)
		{
			instance = new Editor_Helper;
			return instance;
		}

		return instance;
	}

	//D3D11 Engine���� D3D11 Framework ���̺귯������ ������ Ŭ������ �����Ͽ� ����� �� �ֵ��� �ʱ�ȭ
	void Initialize(class Context* context);

	//Importer�� ���� model�� loading�ϴ� �Լ�
	void LoadModel(const std::string& path);

	//���� ��ŷ�� ������Ʈ�� ���͸� ����
	void Picking();

private:
    //���� ��� ����
	static Editor_Helper* instance;

public:
    
	class Context* context = nullptr;  //D3D11 Framework ���̺귯������ ISubsystem�� ��ӹ޴�(��� ����(�����, �׷���, �Է�, ������ ��)) ��� Ŭ�������� �����ϴ� Ŭ����
	class ResourceManager* resource_manager = nullptr; //���ҽ��� �����ϴ� Ŭ����
	class SceneManager* scene_manager = nullptr; //Scene�� �����ϴ� Ŭ����
	class Thread* thread = nullptr; //�����带 �����ϴ� Ŭ����
	class Renderer* renderer = nullptr; //�������� �����ϴ� Ŭ����
	class Input* input = nullptr; //�Է��� �����ϴ� Ŭ����

	std::weak_ptr<class Actor> selected_actor; //���� ���õ� ������Ʈ�� ���Ͱ�
};

