#include "./allocator.hpp"

#include <cstring>

allocator::allocator()
	: m_root( new block_t ),
	m_offset( 0 ),
	m_size( sizeof( block_t::data ) )
{
}

allocator::~allocator()
{
	block_t* current = m_root;

	do
	{
		block_t* next = current->next;

		delete current;

		current = next;
	} while ( current );
}

void* allocator::allocate( size_t size )
{
	constexpr size_t alignment = sizeof( uintptr_t );

	const uintptr_t aligned_size = ( size + alignment - 1 ) & ~( alignment - 1 );

	if ( m_offset + aligned_size <= m_size )
	{
		m_offset += aligned_size;

		return m_root->data + m_offset - aligned_size; // return existing memory
	}

	// we cant allocate enough memory, allocate a new block

	block_t* new_block = aligned_size > sizeof( block_t::data ) ? static_cast<block_t*>( operator new( aligned_size + sizeof( block_t::next ) ) ) : new block_t;

	new_block->next = m_root;

	m_offset = aligned_size;
	m_size = aligned_size > sizeof( block_t::data ) ? aligned_size : sizeof( block_t::data );

	m_root = new_block;

	return new_block->data;
}

