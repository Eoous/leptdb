#pragma once

#include"index.h"
#include <string.h>
namespace leptdb {
	class Comparator {
	public:
		virtual ~Comparator(){}
		virtual int compare(const std::string& a, const std::string& b)const = 0;
	};

	class BytewiseComparator:public Comparator {
	public:
		int compare(const std::string& a, const std::string& b) const override {
			return a.compare(b);
		}
		
	};

	class Compare {
	public:
		Compare(Comparator* comparator):comparator_(comparator){}

		auto operator()(const Index& a,const Index& b) const {
			return comparator_->compare(a.key(), b.key());
		}

		auto operator()(const std::string& a,const std::string& b) const {
			return memcmp(a.data(), b.data());
		}


	private:
		Comparator* comparator_;
	};
}