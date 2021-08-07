#pragma once

class ConstantBuffer final : public D3D11_Object
{
public:
	ConstantBuffer(class Context* context);
	~ConstantBuffer();

	auto GetResource() const->ID3D11Buffer* { return buffer; }

	template <typename T>
	void Create(const D3D11_USAGE& usage = D3D11_USAGE_DYNAMIC);
	void Clear();

	template <typename T>
	auto Map()->T*; //CPU가 constant buffer에 접근하여 데이터를 쓰기위해 접근할 수 있도록 잠시 lock을 거는 함수
	void Unmap(); //CPU가 constant buffer에 접근 후 데이터를 사용하고 걸었던 lock을 푸는 함수 

private:
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
	ID3D11Buffer* buffer = nullptr;
};

template<typename T>
inline void ConstantBuffer::Create(const D3D11_USAGE & usage)
{
	//버퍼를 정의(constant buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //버퍼의 접근에 관련
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //=contant buffer로 사용
	desc.ByteWidth = sizeof(T); //크기

	//버퍼의 접근에 관련된 flag
	switch (usage)
	{
		//GPU에 의해서 읽고 쓰기 접근이 가능
		//CPU 접근 불가
	case D3D11_USAGE_DEFAULT:
		break;

		//GPU에 의해서 읽기 가능
		//CPU 접근 불가
		//해당 옵션에서는 자원 수정이 불가능
	case D3D11_USAGE_IMMUTABLE:
		desc.CPUAccessFlags = 0;
		break;

		//GPU에 의해서 읽기 가능
		//CPU에서 쓰기 가능
		//CPU에서 자원을 읽지는 못하지만 수정은 가능
		//보통 ConstantBuffer를 만들 때 사용
	case D3D11_USAGE_DYNAMIC:
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

		//GPU 메모리에서 CPU 메모리로 복사 허용
	case D3D11_USAGE_STAGING:
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		break;
	}

	//constant buffer는 다른 buffer들과 다르게 D3D11_SUBRESOURCE_DATA가 필요하지않음
	//고정되어 있는 실제 데이터가 아니고 계속 변경되는 값이기 때문
	auto hr = device->CreateBuffer(&desc, nullptr, &buffer);
	assert(SUCCEEDED(hr));
}

//Constant Buffer는 여러 종류가 있기 때문에 멤버함수 템플릿 사용
//GLOBAL_DATA, TRANSFORM_DATA, MATERIAL_DATA, BLUR_DATA 등
template<typename T>
inline auto ConstantBuffer::Map() -> T *
{
	D3D11_MAPPED_SUBRESOURCE mapped_resource; //constant buffer에 값을 넣을 포인터 선언
	//constant buffer에 lock을 걺
	auto hr = device_context->Map
	(
		buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapped_resource
	);

	assert(SUCCEEDED(hr));

	return reinterpret_cast<T*>(mapped_resource.pData);
}
