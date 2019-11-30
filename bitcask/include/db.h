#pragma once

#include "options.h"
#include "env.h"
#include "comparator.h"

namespace leptdb {
	class DB {
	public:
		DB(){}
		virtual ~DB() {}

		static bool open(const std::string& dbname, const Options& options, DB*& dbref);
		virtual bool put(const std::string& key, const std::string& value) = 0;
		virtual bool get(const std::string& key, std::string* value) = 0;
		virtual bool remove(const std::string & key) = 0;

		void operator=(const DB&)=delete;
		DB(const DB&) = delete;
	};
}