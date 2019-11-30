#pragma once

#include <string>

namespace leptdb {
	class Index {
	public:

		Index() {
			
		}
		
		Index(std::string key)
			:key_(key) {
			
		}

		Index(std::string key,
			  uint32_t file_index,uint64_t file_offset,
			  uint32_t key_size,uint32_t value_size)
			:key_(key),file_index_(file_index),file_offset_(file_offset),
			 key_size_(key_size),value_size_(value_size) {
			
		}

		~Index() {
			
		}

		auto key() const { return key_; }
		auto fileIndex() const { return file_index_; }
		auto fileOffset() const { return file_offset_; }
		auto keySize() const { return key_size_; }
		auto valueSize() const { return value_size_; }
		auto dataSize() const { return key_size_ + value_size_; }

		uint32_t indexSize() const {
			return key_size_ + sizeof(uint32_t) * 3 + sizeof (uint64_t);
		}

		auto setDeleted() {
			key_size_ = 0;
			value_size_ = 0;
		}

		auto operator=(const Index& index) {
			this->key_ = index.key();
			this->file_index_ = index.fileIndex();
			this->file_offset_ = index.fileOffset();
			this->key_size_ = index.keySize();
			this->value_size_ = index.valueSize();
		}
		
	private:
		friend class DB;
		friend class DBImpl;
	private:

		std::string key_;
		uint32_t file_index_ = 0;
		uint64_t file_offset_ = 0;

		uint32_t key_size_ = 0;
		uint32_t value_size_ = 0;
	};
}