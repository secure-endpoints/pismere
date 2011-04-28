#pragma once

#include <string>
#include <cstddef>
#include <map>
#include <vector>
#include <deque>
#include <utility>
#include <new>
#include <list>

#include "CCache.debug.h"

#if !TARGET_OS_MAC
// If not on macintosh, shared data allocated as any other data
template <class T>
class CCISharedDataAllocator:
	public std::allocator <T> {
	
	public:
	
     CCISharedDataAllocator (
     	const CCISharedDataAllocator&) throw () {}
     	
     CCISharedDataAllocator () throw () {}

     ~CCISharedDataAllocator () throw () {}
};

#else // !TARGET_OS_MAC

#include <Memory.h>

/*
 * We introduce our own allocator which allocates in system heap. 
 * The purpose is to allow us to allocate STL objects in the system
 * heap when we are sharing the data by putting it in the system heap
 */

template <class T> class CCISharedDataAllocator;

template <>
class CCISharedDataAllocator<void> {
public:
     typedef std::size_t      size_type;
     typedef std::ptrdiff_t   difference_type;
     typedef void*            pointer;
     typedef const void*      const_pointer;
     typedef void             value_type;

     template <class U>
     struct rebind { 
          typedef CCISharedDataAllocator<U>  other;
     };
};

template <class T>
class CCISharedDataAllocator {
public:
	typedef std::size_t                   size_type;
	typedef std::ptrdiff_t                difference_type;
	typedef T*                            pointer;
	typedef const T*                      const_pointer;
	typedef T&                            reference;
	typedef const T&                      const_reference;
	typedef T                             value_type;

     template <class U>
     struct rebind {
          typedef CCISharedDataAllocator <U>  other; 
     };

	pointer allocate (
	size_type n, 
	typename CCISharedDataAllocator<void>::const_pointer hint = 0) {  
		hint;

		T* tmp = (T*)(::NewPtrSys ((Size)(n*sizeof (T))));
		if (tmp == 0)
			throw std::bad_alloc ();
		return tmp; 
	}

	void deallocate (pointer p, size_type) { 
		::DisposePtr ((Ptr)p); 
	}

	pointer address (reference x) const { 
		return static_cast <pointer> (&x); 
	}

	const_pointer address (const_reference x) const { 
		return static_cast <const_pointer> (&x); 
	}

	size_type max_size () const throw ();

	void construct (pointer p, const T& val) { 
		new ((void*)p) T(val); 
	}

     void destroy (pointer p) { 
		((T*)p)->~T(); 
     }

     template<class U>
     CCISharedDataAllocator (const CCISharedDataAllocator<U>&) throw () {}

     CCISharedDataAllocator () throw () {}

     CCISharedDataAllocator (
     	const CCISharedDataAllocator&) throw () {}

     ~CCISharedDataAllocator () throw () {}
};

template <class T>
typename CCISharedDataAllocator <T>::size_type
CCISharedDataAllocator <T>::max_size () const throw ()
{ 
	return std::max (size_type (1), size_type (size_type (-1)/sizeof (T))); 
}

#endif // TARGET_OS_MAC

class CCISharedData {
	public:
		// Override operator new to force allocation in system heap
		void* operator new (std::size_t inSize) throw (std::bad_alloc) {

			CCISharedDataAllocator <char>		allocator;

			for(;;) {
				try {
					return allocator.allocate (inSize, 0);
				} catch (std::bad_alloc&) {
					std::new_handler handler = std::set_new_handler (NULL);
					std::set_new_handler (handler);
					
					if(handler == NULL) {
						throw;
					}
					handler ();
				}
			}
			CCISignal_ ("Fell out the bottom of CCISharedData::operator new");
			throw std::bad_alloc ();
		}

		// Override placement new because we overrode new
		void* operator new (std::size_t, void* inPointer) throw () {
			return inPointer;
		}

		// Override operator delete to deallocate from system heap
		void  operator delete (void* inPointer) throw () {
			CCISharedDataAllocator <char>		allocator;
			allocator.deallocate (reinterpret_cast <char*> (inPointer), 0);
		}
};

class CCICCacheData;

namespace CallImplementations {
	class String {
		public:
		typedef std::basic_string <char, std::char_traits <char>,
			CCISharedDataAllocator <char> >												Shared;
	};
	template <class T>
	class List {
		public:
		typedef	typename std::list <T, CCISharedDataAllocator <T> >						Shared;
	};
	template <class T, class U>
	class Map {
		public:
		typedef	typename std::map <T, U, 
			std::less <T>, CCISharedDataAllocator <std::pair <const T, U> > >			Shared;
	};
	template <class T>
	class Vector {
		public:
		typedef typename std::vector <T, CCISharedDataAllocator <T> >					Shared;
	};
	template <class T>
	class Deque {
		public:
		typedef typename std::deque <T, CCISharedDataAllocator <T> >					Shared;
	};
	template <class T>
	class Allocator {
		public:
		typedef typename CCISharedDataAllocator <T>										Shared;
	};

	typedef CCISharedData																SharedData;
}
