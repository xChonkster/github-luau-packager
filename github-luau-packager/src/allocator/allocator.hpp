/*
*	allocator.hpp
* 
*	allocates memory in blocks...
*/

#pragma once

#include <cstdint>
#include <utility>

class allocator
{
public:
	allocator();

	allocator& operator=( allocator& other ) = delete;
	allocator( allocator& other ) = delete;

	~allocator();
	
public:
	void* allocate( size_t size );

	template <typename T, typename ...Ty>
	T* create(Ty... args)
	{
		void* memory = allocate( sizeof( T ) );

		new ( memory ) T( std::forward<Ty>( args )... );

		return reinterpret_cast<T*>( memory );
	}

public:
	static constexpr size_t BLOCK_SIZE = 0x1000;

	struct block_t
	{
		block_t* next{ 0 }; // actually previous but whatever

		char data[BLOCK_SIZE]{ 0 };
	};

private:
	block_t* m_root{ 0 };
	uintptr_t m_offset{ 0 };
	uintptr_t m_size{ 0 };
};

