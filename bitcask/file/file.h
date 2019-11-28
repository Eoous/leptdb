#pragma once

#include <string>

namespace leptdb {
	class File {
	public:
		File(){}
		virtual ~File() {}

		virtual bool read(uint64_t offset, uint32_t n, char* buf) = 0;

		virtual bool appendData(const std::string& key, const std::string& value,
								uint64_t* file_offset) = 0;
		virtual bool appendIndex(const std::string& key,
								 uint32_t file_index, uint64_t file_offset,
								 uint32_t key_size, uint32_t value_size) = 0;

		virtual bool close() = 0;
		
		virtual uint64_t fileOffset() = 0;

	};
}