#pragma once

#include <assert.h>
#include <stddef.h>
#include <string>

namespace leptdb {
	class Slice {
	public:
		Slice() {}
		Slice(const char* d,size_t n):data_(d),size_(n){}
		Slice(const std::string& s):data_(s.data()),size_(s.size()){}
		Slice(const char* s):data_(s),size_(strlen(s)){}
		
		Slice(const Slice&) = default;
		Slice& operator=(const Slice&) = default;

		const auto* data() const { return data_; }
		auto size() const { return size_; }

		auto empty() const { return size_ == 0; }

		char operator[](size_t n) const {
			assert(n < size());
			return data_[n];
		}

		auto clear() {
			data_ = "";
			size_ = 0;
		}

		auto removePrefix(size_t n) {
			assert(n <= size());
			data_ += n;
			size_ -= n;
		}

		auto toString() const { return std::string(data_, size_); }

		auto compare(const Slice& b) const;

		auto startsWith(const Slice& x) const {
			return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
		}

	private:
		const char* data_ = "";
		size_t size_ = 0;
	};

	inline auto operator==(const Slice& x, const Slice& y) {
		return ((x.size() == y.size()) && 
				(memcmp(x.data(), y.data(), x.size()) == 0));
	}

	inline auto operator!=(const Slice& x, const Slice& y) { return !(x == y); }

	inline auto Slice::compare(const Slice& b) const {
		const auto min_len = (size_ < b.size_) ? size_ : b.size;
		auto r = memcmp(data_, b.data_, min_len);
		if( r==0) {
			if (size_ < b.size_) {
				r = -1;
			}
			else if (size_ > b.size_) {
				r = +1;
			}
		}
		return r;
	}

}