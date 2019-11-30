#include <assert.h>
#include <iostream>

#include "arena.h"

namespace leptdb {
	static constexpr int kBlockSize = 4096;

	Arena::Arena() {
	}

	Arena::~Arena() {
		for(auto& it :blocks_) {
			delete[] it;
		}
	}

	char* Arena::allocateFallback(size_t bytes){
		if(bytes > kBlockSize/4) {
			auto result = allocateNewBlock(bytes);
			return result;
		}

		alloc_ptr_ = allocateNewBlock(kBlockSize);
		alloc_bytes_remaining_ = kBlockSize;

		auto result = alloc_ptr_;
		alloc_ptr_ += bytes;
		alloc_bytes_remaining_ -= bytes;
		return result;
	}

	auto Arena::allocateAligned(size_t bytes) {
		constexpr int align = (sizeof(void*)) > 8 ? sizeof(void*) : 8;
		assert((align & (align - 1)) == 0);
		size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_)& (align - 1);
		size_t slop = (current_mod == 0 ? 0 : align - current_mod);
		size_t needed = bytes + slop;
		char* result;
		if(needed<=alloc_bytes_remaining_) {
			result = alloc_ptr_ + slop;
			alloc_ptr_ += needed;
			alloc_bytes_remaining_ -= needed;
		}
		else {
			result = allocateFallback(bytes);
		}
		assert(reinterpret_cast<uintptr_t>(result)& (align - 1) == 0);
		return result;
	}

	char* Arena::allocateNewBlock(size_t block_bytes) {
		auto result = new char[block_bytes];
		blocks_.push_back(result);
		memory_usage_.NoBarrier_Store(
			reinterpret_cast<void*>(memoryUsage() + block_bytes + sizeof(char*)));
		return result;
	}
}