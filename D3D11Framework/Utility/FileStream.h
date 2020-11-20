#pragma once

enum FILE_STREAM_MODE : uint
{
	FILE_STREAM_READ = 1U << 0,
	FILE_STREAM_WRITE = 1U << 1,
	FILE_STREAM_APPEND = 1U << 2,
};

//���� ����� �� �߰��� ����ϴ� Ŭ���� (���ҽ��� �а� �����ϴµ��� ���)
class FileStream final
{
public:
	FileStream(const std::string& path, const uint& flags);
	~FileStream();

	auto IsOpen() const -> const bool { return is_open; }
	void Close();
	void Skip(const uint& n);

	//=================================================================================================================
	//[Write] (���α׷��� �����͸� ���̳ʸ� �������� ���Ͽ� �����ϱ�)
	//=================================================================================================================
	template <typename T, typename = typename std::enable_if< //std::enable_if�� ����Ͽ� true�� ��쿡�� ���ø� ġȯ�� �����ϵ��� ��
		std::is_same<T, bool>::value || //std::is_same�� ����Ͽ� T�� Ư�� �ڷ������� ġȯ�� ��츸 true
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
		void Write(const T& value)
	{
		f_out.write(reinterpret_cast<const char*>(&value), sizeof(T)); //���ڷ� ���� value���� ���Ͽ� ���̳ʸ� �������� ����
	}

	//Write�Լ� �����ε�
	void Write(const void* value, const uint& size);
	void Write(const std::string& value);
	void Write(const std::vector<std::byte>& value);

	//=================================================================================================================
	//[Read] (���̳ʸ� �������� ���Ͽ� ����� �����͸� ���α׷����� ����ϱ�)
	//=================================================================================================================
	template <typename T, typename = typename std::enable_if< //std::enable_if�� ����Ͽ� true�� ��쿡�� ���ø� ġȯ�� �����ϵ��� ��
		std::is_same<T, bool>::value || //std::is_same�� ����Ͽ� T�� Ư�� �ڷ������� ġȯ�� ��츸 true
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
		void Read(T& value)
	{
		f_in.read(reinterpret_cast<char*>(&value), sizeof(T)); //���Ͽ� ���̳ʸ� �������� ����� �����͸� ���ڷ� ���� value�� ����
	}

	//Read�Լ� �����ε�
	void Read(std::string& value);
	void Read(std::vector<std::byte>& value);

	template <typename T, typename = typename std::enable_if<
		std::is_same<T, bool>::value ||
		std::is_same<T, char>::value ||
		std::is_same<T, byte>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, uint>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, unsigned long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, unsigned long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value ||
		std::is_same<T, std::byte>::value ||
		std::is_same<T, Vector2>::value ||
		std::is_same<T, Vector3>::value ||
		std::is_same<T, Vector4>::value ||
		std::is_same<T, Color4>::value ||
		std::is_same<T, Quaternion>::value ||
		std::is_same<T, Matrix>::value>::type>
		const T Read()
	{
		T value;
		Read(value);

		return value;
	}

private:
	std::ofstream f_out; //���α׷� ���� �����͸� ���α׷��� ��Ʈ�� ���Ͽ� ����
	std::ifstream f_in;  //���α׷��� ��Ʈ�� ������ �����͸� ���α׷� ���� ���

	uint flags = 0; //���� ��Ʈ���� � �뵵�� ����ϴ���
	bool is_open = false; //������ ���������� ���ȴ��� Ȯ��
};