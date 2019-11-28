#pragma once

#include <assert.h>
#include <atomic>
#include <mutex>

namespace leptdb {
	class SpinLock {
	public:
		auto lock() {
			while(lck.test_and_set(std::memory_order_acquire)){}
		}

		auto unlock() {
			lck.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag lck = ATOMIC_FLAG_INIT;
	};
} // namespace leptdb