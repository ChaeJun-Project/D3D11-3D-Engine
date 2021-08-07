#pragma once

class VertexBuffer final : public D3D11_Object
{
public:
    VertexBuffer(class Context* context);
    ~VertexBuffer();

    auto GetResource() const -> ID3D11Buffer* { return buffer; }
    auto GetStride() const -> const uint& { return stride; }
    auto GetOffset() const -> const uint& { return offset; }
    auto GetCount() const -> const uint& { return count; }

	auto Map() -> void*;
	void Unmap();

    template <typename T>
    void Create(const std::vector<T>& vertices, const D3D11_USAGE& usage = D3D11_USAGE_DEFAULT);

    void Clear();

private:
    ID3D11Device* device    = nullptr;
	ID3D11DeviceContext* device_context = nullptr;
    ID3D11Buffer* buffer    = nullptr;
    uint stride             = 0;
    uint offset             = 0;
    uint count              = 0;
};

//D3D11_INPUT_ELEMENT_DESC의 정의는 사용자에 따라 변경될 수 있으므로
//멤버함수 템플릿 사용
template<typename T>
inline void VertexBuffer::Create(const std::vector<T>& vertices, const D3D11_USAGE & usage)
{
	stride = sizeof(T);
	count = static_cast<uint>(vertices.size());

	//버퍼를 정의(vertex buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //버퍼의 접근에 관련
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //=vertex buffer로 사용
	desc.ByteWidth = stride * count; //크기

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

	//리소스 내에 있는 실제 데이터를 정의
	D3D11_SUBRESOURCE_DATA sub_data;
	ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
	sub_data.pSysMem = vertices.data(); //버퍼 데이터 초기화

	//버퍼 생성
	auto hr = device->CreateBuffer(&desc, &sub_data, &buffer);
	assert(SUCCEEDED(hr));
}