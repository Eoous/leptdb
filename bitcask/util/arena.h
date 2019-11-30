#pragma once

#include <vector>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "../port/atomic_pointer.h"

namespace leptdb {
	class Arena {
	public:
		Arena();
		~Arena();

		auto allocate(size_t bytes);

		char* allocateAligned(size_t bytes);

		auto memoryUsage() const {
			return reinterpret_cast<uintptr_t>(memory_usage_.NoBarrier_Load());
		}

		Arena(const Arena&) = delete;
		void operator=(const Arena&) = delete;
	private:
		char* allocateFallback(size_t bytes);
		char* allocateNewBlock(size_t block_bytes);

	private:
		char* alloc_ptr_ = nullptr;
		size_t alloc_bytes_remaining_ = 0;

		std::vector<char*> blocks_;
		port::AtomicPointer memory_usage_ {0};
	};

	inline auto Arena::allocate(size_t bytes) {
		assert(bytes > 0);
		if(bytes<=alloc_bytes_remaining_) {
			auto result = alloc_ptr_;
			alloc_ptr_ += bytes;
			alloc_bytes_remaining_ -= bytes;
			return result;
		}
		return allocateFallback(bytes);
	}

}