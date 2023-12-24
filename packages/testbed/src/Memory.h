#ifndef MEMORY_H
#define MEMORY_H

// memory overloads primarily for debugging purposes
void *operator new(size_t size);
void operator delete(void *ptr) noexcept;

#endif	// MEMORY_H
