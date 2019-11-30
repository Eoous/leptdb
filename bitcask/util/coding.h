#pragma once

#include <iostream>

namespace leptdb {
#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

	auto encodeData(char* buf, 
					const std::string& key, const std::string& value)-> void;
	
	auto decodeData(char* buf, uint32_t key_size, 
					uint32_t value_size, std::string* value)-> void;
	
	auto encodeIndex(char* buf,const std::string& key ,
					 uint32_t file_index, uint64_t file_offset, 
					 uint32_t key_size, uint32_t value_size)-> void;

	auto decodeIndex(char* buf, std::string* key,
					 uint32_t* file_index, uint64_t* file_offset,
					 uint32_t* key_size, uint32_t* value_size) -> void;

	auto decodeFixed32(const char* buf)->uint32_t;
	auto decodeFixed64(const char* buf)->uint64_t;
}