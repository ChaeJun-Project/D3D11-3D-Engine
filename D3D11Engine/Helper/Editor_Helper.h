#pragma once

//D3D11 Engine 실행파일에서 D3D11 Framework 라이브러리 기능을 사용할 수 있도록 지원을 해주는 클래스

//싱글톤으로 구현
class Editor_Helper final
{
private:
	//기본 생성자
	Editor_Helper() = default;
	//기본 소멸자
	~Editor_Helper() = default;

public:
    //정적 멤버 변수 반환 함수
	static Editor_Helper* GetInstance()
	{
		if (!instance)
		{
			instance = new Editor_Helper;
			return instance;
		}

		return instance;
	}

	//D3D11 Engine에서 D3D11 Framework 라이브러리에서 정의한 클래스를 참조하여 사용할 수 있도록 초기화
	void Initialize(class Context* context);

	//Importer를 통해 model을 loading하는 함수
	void LoadModel(const std::string& path);

	//현재 피킹된 오브젝트의 엑터를 저장
	void Picking();

private:
    //정적 멤버 변수
	static Editor_Helper* instance;

public:
    
	class Context* context = nullptr;  //D3D11 Framework 라이브러리에서 ISubsystem을 상속받는(기능 단위(오디오, 그래픽, 입력, 렌더링 등)) 모든 클래스들을 관리하는 클래스
	class ResourceManager* resource_manager = nullptr; //리소스를 관리하는 클래스
	class SceneManager* scene_manager = nullptr; //Scene을 관리하는 클래스
	class Thread* thread = nullptr; //스레드를 관리하는 클래스
	class Renderer* renderer = nullptr; //렌더링을 관리하는 클래스
	class Input* input = nullptr; //입력을 관리하는 클래스

	std::weak_ptr<class Actor> selected_actor; //현재 선택된 오브젝트의 엑터값
};

