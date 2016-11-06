#pragma once
template<typename Derived, typename T = unsigned int>
class GenericId
{
public:
	typedef T IdType;

public:
	static Derived InvalidId() { return Derived(0xffffffff); }

public:
	GenericId() : mId(InvalidId().id()) {}
	explicit GenericId(T id) : mId(id) {}
	GenericId(const GenericId<Derived, T>& other) : mId(other.mId) {}

public:
	T Id() const { return mId; }
	bool IsValid() const { return mId != InvalidId().id(); }


	T id() const { return mId; }
	bool isValid() const { return mId != InvalidId().id(); }

public:
	bool operator < (const GenericId<Derived, T>& other) const
	{
		return mId < other.mId;
	}

	bool operator <= (const GenericId<Derived, T>& other) const
	{
		return mId <= other.mId;
	}

	bool operator > (const GenericId<Derived, T>& other) const
	{
		return mId > other.mId;
	}

	bool operator >= (const GenericId<Derived, T>& other) const
	{
		return mId >= other.mId;
	}

	bool operator == (const GenericId<Derived, T>& other) const
	{
		return mId == other.mId;
	}

	bool operator != (const GenericId<Derived, T>& other) const
	{
		return mId != other.mId;
	}

	GenericId<Derived, T>& operator = (const GenericId<Derived, T>& other)
	{
		mId = other.mId;
		return *this;
	}

private:
	T mId;
};

#define VCL_MESH_VOLUMEMESH_CREATEID(type_name) class type_name : public GenericId<type_name, unsigned int> { public: type_name(){} explicit type_name(unsigned int id) : GenericId<type_name, unsigned int>(id) {}}
