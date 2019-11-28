
#include "coding.h"
#include <cassert>
#include <cstring>

namespace leptdb{


	auto encodeData(char* buf, const std::string& key, const std::string& value) -> void {
		memcpy(buf, key.data(), key.size());
		memcpy(buf + key.size(), value.data(), value.size());
		return;
	}

	auto decodeData(char* buf, uint32_t key_size, uint32_t value_size, std::string* value) -> void {
		*value = std::string(buf + key_size, value_size);
		return;
	}

	auto encodeIndex(char* buf, 
					 uint32_t file_index, uint64_t file_offset, 
					 uint32_t key_size,uint32_t value_size) -> void {
		assert(buf);
		size_t pos = 0;

		auto index_size = key_size + sizeof(uint32_t) * 3 + sizeof(uint64_t);

		memcpy(buf, &index_size, sizeof uint32_t);
		pos += sizeof uint32_t;
		memcpy(buf + pos, &file_index, sizeof uint32_t);
		pos += sizeof uint32_t;

		memcpy(buf + pos, &file_offset, sizeof(uint64_t));
		pos += sizeof uint64_t;

		memcpy(buf + pos, &key_size, sizeof uint32_t);
		pos += sizeof uint32_t;

		memcpy(buf + pos, key.data(), key_size);
		pos += key_size;
		memcpy(buf + pos, &value_size, sizeof uint32_t);

		return;
	}

	auto decodeIndex(char* buf, std::string* key, 
					 uint32_t* file_index, uint64_t* file_offset, 
					 uint32_t* key_size,uint32_t* value_size) -> void {
		assert(buf);
		size_t pos = 0;

		*file_index = decodeFixed32(buf);
		pos += sizeof uint32_t;

		*file_offset = decodeFixed64(buf + pos);
		pos += sizeof uint64_t;

		*key_size = decodeFixed32(buf + pos);
		pos += sizeof uint32_t;

		*key = std::string(buf + pos, *key_size);
		pos += *key_size;

		*value_size = decodeFixed32(buf + pos);
		
		return;
	}

	auto decodeFixed32(const char* ptr) -> uint32_t {
		uint32_t result;
		memcpy(&result, ptr, sizeof result);
		return result;
	}

	auto decodeFixed64(const char* ptr) -> uint64_t {
		uint64_t result;
		memcpy(&result, ptr, sizeof result);
		return result;
	}
}
