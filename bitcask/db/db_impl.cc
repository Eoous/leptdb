
#include <iostream>
#include <thread>

#include "db_impl.h"
#include "../util/coding.h"
#include "../util/hash.h"

using namespace leptdb;

DBImpl::DBImpl(const std::string& dbname, const Options& options)
	:dbname_(dbname),options_(options){
	
}

DBImpl::~DBImpl() {
	for (auto& file : data_files_) {
		delete file;
	}
	for (auto& file : index_files_) {
		delete file;
	}
	for (auto& table : tables_) {
		delete table;
	}
}

bool DBImpl::put(const std::string& key, const std::string& value) {
	auto cursor = cursor_++;
	uint8_t file_index = (cursor & (max_file_ - 1));
	auto data_file = data_files_[file_index];
	uint64_t file_offset;
	auto s = data_file->appendData(key, value, &file_offset);
	if(likely(s)) {
		auto index_file = index_files_[file_index];
		s = index_file->appendIndex(key, file_index, file_offset, key.size(), value.size());
		if(likely(s)) {
			Index index(key, file_index, file_offset, key.size(), value.size());
			int32_t slot = (hash(key.data()) & (table_size_ - 1));
			tables_[slot]->Insert(index);
		}
	}
	return s;
}

bool DBImpl::get(const std::string& key, std::string* value) {
	int32_t slot = (hash(key.data()) & (table_size_ - 1));
	Index search_index(key), internal_index;
	auto s = tables_[slot]->Get(search_index, &internal_index);
	if(likely(s)) {
		if(unlikely(internal_index.keySize()==0 && 
					internal_index.valueSize()==0)) {
			*value = "";
			return false;
		}
		char buf[internal_index.dataSize()];
		auto s = data_files_[internal_index.fileIndex()]->read(internal_index.fileOffset(),
			internal_index.dataSize(),
			buf);
		if (likely(s)) {
			decodeData(buf,
				internal_index.key_size_,
				internal_index.value_size_,
				value);
			return true;
		}
		else {
			return s;
		}
	}
	else {
		return s;
	}
}

bool DBImpl::remove(const std::string& key) {
	int32_t slot = (hash(key.data()) & (table_size_ - 1));
	Index search_index(key), internal_index;
	auto s = tables_[slot]->Get(search_index, &internal_index);
	if (likely(s)) {
		internal_index.setDeleted();
		tables_[slot]->Insert(internal_index);
		return s;
	}
	else {
		return s;
	}
}

auto DBImpl::indexCallback(File* file) {
	uint64_t file_offset = file->fileOffset();
	if (file_offset != 0) { // Load index files into skiplist
		uint64_t pos = 0;
		while (pos < file_offset) {
			char size_buf[sizeof(uint32_t)];
			auto s = file->read(pos, sizeof(uint32_t), size_buf);
			if (s) {
				pos += sizeof(uint32_t);
				uint32_t index_size = decodeFixed32(size_buf);
				char index_buf[index_size];
				auto s = file->read(pos, index_size, index_buf);
				if (s) {
					pos += index_size;
					Index index;
					decodeIndex(index_buf,
						&index.key_,
						&index.file_index_,
						&index.file_offset_,
						&index.key_size_,
						&index.value_size_);
					int32_t slot = (hash(index.key_.data()) & (table_size_ - 1));
					tables_[slot]->Insert(index);
				}
			}
		}
	}
}

auto DBImpl::recover() {
	std::vector<std::thread > threads;
	for (uint32_t i = 0; i < max_file_; i++) {
		std::thread recover(&DBImpl::indexCallback, this, index_files_[i]);
		threads.push_back(std::move(recover));
	}

	for (uint32_t i = 0; i < max_file_; i++) {
		threads[i].join();
	}
	return true;
}


bool DB::open(const std::string& dbname, const Options& options, DB*& dbref) {
	dbref = nullptr;
	DBImpl* impl = new DBImpl(dbname, options);

	// Get the number of processor
	int32_t num_processor = sysconf(_SC_NPROCESSORS_CONF);
	if (num_processor == -1) {
		return false;
	}

	impl->max_file_ = num_processor * 2;
	impl->table_size_ = num_processor;
	// Create skiplist tables
	for (uint32_t i = 0; i < impl->table_size_; ++i) {
		Table* table = new Table(Compare(options.comparator_));
		impl->tables_.push_back(table);
	}

	// Create data directory
	std::string current_dir;
	if (!Env::getCurrentDir(current_dir)) {
		return false;
	}

	std::string data_dir = current_dir + "/" + dbname;
	if (!Env::fileExists(data_dir)) {
		auto s = Env::createDir(data_dir);
		if (!s) {
			return false;
		}
	}

	// Open data
	for (uint32_t i = 1; i <= impl->max_file_; i++) {
		std::string data_file = data_dir + dataFileName + std::to_string(i);
		File* file;
		auto s = Env::newFile(data_file, &file);
		if (s) {
			impl->data_files_.push_back(file);
		}
		else {
			return false;
		}
	}

	// Open index
	for (uint32_t i = 1; i <= impl->max_file_; i++) {
		std::string index_file = data_dir + indexFileName + std::to_string(i);
		File* file;
		auto s = Env::newFile(index_file, &file);
		if (s) {
			impl->index_files_.push_back(file);
		}
		else {
			return false;
		}
	}

	// Recover
	impl->recover();

	dbref = impl;
	return true;
}