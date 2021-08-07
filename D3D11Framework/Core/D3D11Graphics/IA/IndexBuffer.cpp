#include "D3D11Framework.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Context * context)
{
    device = context->GetSubsystem<Graphics>()->GetDevice();
}

IndexBuffer::~IndexBuffer()
{
    Clear();
}

void IndexBuffer::Create(const std::vector<uint>& indices, const D3D11_USAGE & usage)
{
	stride = sizeof(uint);
	count = static_cast<uint>(indices.size());

	//버퍼를 정의(index buffer)
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = usage; //버퍼의 접근에 관련
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER; //=index buffer로 사용
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
	sub_data.pSysMem = indices.data(); //버퍼 데이터 초기화

	auto hr = device->CreateBuffer(&desc, &sub_data, &buffer);
	assert(SUCCEEDED(hr));
}

void IndexBuffer::Clear()
{
    SAFE_RELEASE(buffer);
    stride  = 0;
    offset  = 0;
    count   = 0;
}
