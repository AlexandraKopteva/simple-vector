#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
public:
	// �������������� ArrayPtr ������� ����������
	ArrayPtr() = default;

	// ������ � ���� ������ �� size ��������� ���� Type.
	// ���� size == 0, ���� raw_ptr_ ������ ���� ����� nullptr
	explicit ArrayPtr(size_t size_) {
		if (size_ != 0)
			raw_ptr_ = new Type[size_];
	}

	// ����������� �� ������ ���������, ��������� ����� ������� � ���� ���� nullptr
	explicit ArrayPtr(Type* raw_ptr) noexcept {
		raw_ptr_ = raw_ptr;
	}

	// ��������� �����������
	ArrayPtr(const ArrayPtr&) = delete;
	~ArrayPtr() {
		delete[] raw_ptr_;
	}

	// ��������� ������������
	ArrayPtr& operator=(const ArrayPtr&) = delete;
	ArrayPtr& operator=(ArrayPtr&& other) 
	{
		if (this != &other) {
			this->swap(other);
		}
		return *this;
	}

	// ���������� ��������� �������� � ������, ���������� �������� ������ �������
	// ����� ������ ������ ��������� �� ������ ������ ����������
	[[nodiscard]] Type* Release() noexcept {
		return std::exchange(raw_ptr_, nullptr);

	}

	// ���������� ������ �� ������� ������� � �������� index
	Type& operator[](size_t index) noexcept {
		return *(raw_ptr_ + index);
	}

	// ���������� ����������� ������ �� ������� ������� � �������� index
	const Type& operator[](size_t index) const noexcept {
		return *(raw_ptr_ + index);
	}

	// ���������� true, ���� ��������� ���������, � false � ��������� ������
	explicit operator bool() const {
		if (raw_ptr_ == nullptr)
			return false;
		else
			return true;
	}

	// ���������� �������� ������ ���������,  ���������� a���� ������ �������
	Type* Get() const noexcept {
		return raw_ptr_;
	}

	// ������������ ��������� ��������� �� ������ � �������� other
	void swap(ArrayPtr& other) noexcept {
		std::swap(raw_ptr_, other.raw_ptr_);
	}

private:
	Type* raw_ptr_ = nullptr;
};