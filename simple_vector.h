#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include "array_ptr.h"

using namespace std;

class ReserveProxyObj {
public:
	ReserveProxyObj(size_t capacity_to_reserve) :capacity(capacity_to_reserve) {}
	~ReserveProxyObj() {}
	size_t GetCapacity() const { return capacity; };
private:
	size_t capacity = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
	return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
	using Iterator = Type * ;
	using ConstIterator = const Type*;

	SimpleVector() noexcept = default;

	// ������ ������ �� size ���������, ������������������ ��������� �� ���������
	explicit SimpleVector(size_t size) : capacity_(size), size_(size), data_(capacity_) {
		if (size > 0)
		{
			std::fill(begin(), end(), Type());
		}
	}
	// ������ ������ �� size ���������, ������������������ ��������� value
	SimpleVector(size_t size, const Type& value) : capacity_(size), size_(size), data_(capacity_) {
		if (size > 0)
		{
			std::fill(begin(), end(), value);
		}
	}

	// ������ ������ �� std::initializer_list
	SimpleVector(std::initializer_list<Type> init) : capacity_(init.size()), size_(init.size()), data_(capacity_) {
		if (init.size() > 0)
		{
			std::copy(init.begin(), init.end(), begin());
		}
	}

	~SimpleVector() {
		//delete data_.Release();
	}
	// ���������� ���������� ��������� � �������
	size_t GetSize() const noexcept {
		return size_;
	}

	// ���������� ����������� �������
	size_t GetCapacity() const noexcept {
		return capacity_;
	}

	// ��������, ������ �� ������
	bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	// ���������� ������ �� ������� � �������� index
	Type& operator[](size_t index) noexcept {
		assert(index < size_);
		return data_[index];
	}

	// ���������� ����������� ������ �� ������� � �������� index
	const Type& operator[](size_t index) const noexcept {
		assert(index < size_);
		return data_[index];
	}

	// ���������� ����������� ������ �� ������� � �������� index
	// ����������� ���������� std::out_of_range, ���� index >= size
	Type& At(size_t index) {
		if (index >= size_)
			throw std::out_of_range("index >= size");
		return data_[index];
	}

	// ���������� ����������� ������ �� ������� � �������� index
	// ����������� ���������� std::out_of_range, ���� index >= size
	const Type& At(size_t index) const {
		if (index >= size_)
			throw std::out_of_range("index >= size");
		return  data_[index];// const_cast<Type&>(data_[index])
	}

	// �������� ������ �������, �� ������� ��� �����������
	void Clear() noexcept {
		if(IsEmpty()) return;
		size_ = 0;
	}
	// �������� ������ �������.
	// ��� ���������� ������� ����� �������� �������� �������� �� ��������� ��� ���� Type
	void Resize(size_t new_size) {
		// 3 ������
		//����� ������ ������ ��� ����� ��������
		if (new_size <= size_) {
			size_ = new_size;
		}
		//����� ������ �� ��������� ��� �����������
		else {
			//����� ������ ��������� ������� ����������� �������
			if (new_size > capacity_)
			{
				Reserve(new_size);
			}
			std::generate(end(), end() + new_size - size_, [] { return Type(); });
			size_ = new_size;
		}
	}
	// ���������� �������� �� ������ �������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	Iterator begin() noexcept {
		return data_.Get();
	}
	// ���������� �������� �� �������, ��������� �� ���������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	Iterator end() noexcept {
		return data_.Get() + size_;
	}

	// ���������� ����������� �������� �� ������ �������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	ConstIterator begin() const noexcept {
		return data_.Get();
	}

	// ���������� �������� �� �������, ��������� �� ���������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	ConstIterator end() const noexcept {
		return data_.Get() + size_;
	}

	// ���������� ����������� �������� �� ������ �������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	ConstIterator cbegin() const noexcept {
		return data_.Get();
	}

	// ���������� �������� �� �������, ��������� �� ���������
	// ��� ������� ������� ����� ���� ����� (��� �� �����) nullptr
	ConstIterator cend() const noexcept {
		return data_.Get() + size_;
	}
	SimpleVector(const SimpleVector& other) : capacity_(other.capacity_), size_(other.size_)
	{
		if (this != &other && !other.IsEmpty())
		{
			ArrayPtr<Type> data_new(other.capacity_);
			std::copy(other.begin(), other.end(), data_new.Get());
			data_.swap(data_new);
		}
	}
	SimpleVector& operator=(const SimpleVector& rhs) {
		if (this != &rhs) {
			auto rhs_copy(rhs);
			swap(rhs_copy);
		}
		return *this;
	}

	SimpleVector(SimpleVector&& other)
	{
		if (this != &other && !other.IsEmpty())
		{
			this->swap(other);
		}
	}

	SimpleVector& operator=(SimpleVector&& other)
	{
		if (this != &other) {
			this->swap(other);
		}
		return *this;

	}
	// ��������� ������� � ����� �������
	// ��� �������� ����� ����������� ����� ����������� �������
	void PushBack(const Type& item)
	{
		if (capacity_ == size_)
			Reserve(std::max<size_t>(1, capacity_ * 2));

		data_[size_] = item;
		++size_;
	}

	void PushBack(Type&& item)
	{
		if (capacity_ == size_) 
			Reserve(std::max({ static_cast<size_t>(1),static_cast<size_t>(capacity_ * 2) }));
		
		data_[size_] = std::move(item);
		++size_;
	}

	// ��������� �������� value � ������� pos.
	// ���������� �������� �� ����������� ��������
	// ���� ����� �������� �������� ������ ��� �������� ���������,
	// ����������� ������� ������ ����������� �����, � ��� ������� ������������ 0 ����� ������ 1
	Iterator Insert(ConstIterator pos, const Type& value)
	{
		//����� ���������, ��� �������� �������(����� ����� ������� � ������)
		assert(pos != nullptr && begin() <= pos && pos <= end());
		auto diff = std::distance(begin(), pos);
		rif_insert(pos);
		data_[diff] = value;
		++size_;
		return Iterator(begin() + diff);
	}

	//�� ����������, ������������ Insert
	Iterator Insert(Iterator pos, Type&& value)
	{
		assert(begin() <= pos && pos <= end());
		auto diff = std::distance(begin(), pos);
		rif_insert(pos);
		data_[diff] = std::move(value);
		++size_;
		return Iterator(begin() + diff);
	}
	// "�������" ��������� ������� �������. ������ �� ������ ���� ������
	void PopBack() noexcept {
		assert(!IsEmpty());
		--size_;
	}

	// ������� ������� ������� � ��������� �������
	Iterator Erase(ConstIterator pos)
	{
		assert(pos != nullptr && begin() <= pos && pos <= end());

		std::move(pos + 1, end(), pos);
		--size_;
		return pos;
	}
	//��� ����� ������ � ���� � ide ������ ������ � ��������� �� ������� � Erase(ConstIterator pos)
	Iterator Erase(Iterator pos)
	{
		assert(pos != nullptr && begin() <= pos && pos <= end());

		std::move(pos + 1, end(), pos);
		--size_;
		return pos;
	}
	// ���������� �������� � ������ ��������
	void swap(SimpleVector& other) noexcept {
		data_.swap(other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}
	// ���������� �������� � ������ ��������
	void swap(SimpleVector&& other) noexcept {
		data_.swap(other.data_);
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
	}
	//������ ������� �������
	//���� new_capacity ������ ������� capacity, ������ ������ ���� ������������,
	//� �������� ������� ����������� � ����� ������� ������.
	void Reserve(size_t new_capacity) {
		if (new_capacity > capacity_)
		{
			ArrayPtr<Type> data_new_(new_capacity);
			std::move(begin(), end(), data_new_.Get());
			data_.swap(data_new_);
			capacity_ = new_capacity;
		}
	}

	SimpleVector(const ReserveProxyObj& reserve)
	{
		if (reserve.GetCapacity() > 0)
			Reserve(reserve.GetCapacity());
	}

private:
	void rif_insert(Iterator pos)
	{
		if (size_ < capacity_) {
			std::move_backward(pos, end(), begin() + size_ + 1);
		}
		else {
			if (capacity_ == 0)
				capacity_ = 1;
			else
				capacity_ *= 2;
			//������� � ������ move, ����� ���������� diff(move_backward - ����� ���������� �� �����)
			auto diff = std::distance(begin(), pos);

			ArrayPtr<Type> data_new_(capacity_);
			std::move(begin(), pos, data_new_.Get());
			std::move(pos, end(), data_new_.Get() + diff + 1);
			data_.swap(data_new_);
		}
	}
	// ������ ������ ��������� ����� ������������ ����� ���������, ����� ��� ArrayPtr
	size_t capacity_ = 0;
	size_t size_ = 0;
	ArrayPtr<Type> data_;

};

template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs == rhs);
}

template<typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
	return  std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return rhs < lhs;
}
template <typename Type>
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs > rhs);
}

template <typename Type>
bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return !(lhs < rhs);
}