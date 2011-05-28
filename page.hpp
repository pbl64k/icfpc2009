#ifndef W_OM_HPP_PAGE

#define W_OM_HPP_PAGE

#include <cstring>

#include "om.hpp"

W_OM_NS_START

	template<typename T, unsigned char A> class Page {
		public:
			Page();
			virtual ~Page();

			T &operator[](Addr ix);

		private:
			Addr size_;
			T *mem_;
	};

	template<typename T, unsigned char A> Page<T, A>::Page():
			size_(1 << A), mem_(new T[size_]) {
		assert(A != 0);
		assert(A <= (sizeof(Addr) * CHAR_BIT));
		assert(size_ > 0);
		assert(mem_ != NULL);

		std::memset(mem_, 0, size_ * sizeof(T));
	}

	template<typename T, unsigned char A> Page<T, A>::~Page() {
		assert(mem_ != NULL);

		delete[] mem_;
	}

	template<typename T, unsigned char A>
			T &Page<T, A>::operator[](Addr ix) {
		#ifdef W_OM_OPT_BOUNDS_CHECKING
			assert(ix < size_);
		#endif

		return mem_[ix];
	}

W_OM_NS_END

#endif
