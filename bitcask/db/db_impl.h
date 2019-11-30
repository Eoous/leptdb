#pragma once

#include <atomic>
#include <algorithm>

#include "../include/db.h"
#include "../include/options.h"
#include "../include/comparator.h"
#include "../include/skiplist.h"
#include "../util/arena.h"
#include "../file/file.h"
#include "../util/lock.h"

namespace leptdb {
	const std::string indexFileName = "/INDEX-";
	const std::string dataFileName ="/DATA-";

	using Table = SkipList<Index, Compare>;

	class DBImpl:public DB {
	public:
		DBImpl(const std::string& dbname, const Options& options);
		virtual ~DBImpl();


		virtual bool put(const std::string& key, const std::string& value) override;
		virtual bool get(const std::string& key, std::string* value) override;
		virtual bool remove(const std::string& key) override;

		auto indexCallback(File* file);
		auto recover();
		
		void operator=(const DBImpl&) = delete;
		DBImpl(const DBImpl&) = delete;
	private:
		friend DB;
	private:
		std::string dbname_;
		Options options_;
		std::atomic<uint32_t> cursor_{ 0 };

		uint32_t max_file_ = 0;
		std::vector<File*> data_files_;
		std::vector<File*> index_files_;

		uint32_t table_size_;
		std::vector<Table*> tables_;
	};
}
