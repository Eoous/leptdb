#pragma once

#include "comparator.h"
#include "env.h"

namespace leptdb {
	class Options {
	public:

		//Options(){}
		//~Options(){}

		Comparator* comparator_=nullptr;
		
	};
}