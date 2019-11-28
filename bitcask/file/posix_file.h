#pragma once
#include <atomic>

#include "file.h"
#include "../util/lock.h"
#include "../util/coding.h"
namespace leptdb {

	class PosixFile:public File {
	public:
		PosixFile(int32_t fd,const std::string& fname,size_t file_offset)
			:fd_(fd),fname_(fname),file_offset_(file_offset) {
			
		}

		virtual ~PosixFile();

		bool read(uint64_t offset, uint32_t n, char* buf) override;

		bool appendData(const std::string& key, const std::string& value, uint64_t* file_offset) override;
		bool appendIndex(const std::string& key, uint32_t file_index, uint64_t file_offset, uint32_t key_size, uint32_t value_size) override;

		bool close() override;

		inline uint64_t fileOffset() override { return file_offset_; }
		
	private:
		uint32_t fd_;
		std::string fname_;
		std::atomic<uint64_t> file_offset_;

		char* buf_ = nullptr;
		char* base_buf_ = nullptr;
		uint32_t map_size_ = 4096 * 10;
		uint32_t map_pos_ = 0;
		uint32_t map_avail_ = 0;

		SpinLock file_lock_;
	};
}