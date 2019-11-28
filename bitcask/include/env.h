#pragma once

#include <string>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "../file/posix_file.h"

namespace leptdb {
	class Env {
	public:
		Env(){}
		virtual ~Env(){}

		static auto newFile(const std::string& fname,File** file) {
			auto fd = ::open(fname.data(), O_RDWR | O_CREAT | O_APPEND, 0645);
			if(fd<0) {
				file = nullptr;
				return false;
			}
			else {
				uint64_t offset;
				getFileSize(fname, &offset);
				*file = new PosixFile(fd, fname, offset);
				return true;
			}
		}

		static auto fileExists(const std::string& fname) {
			if(access(fname.data(),F_OK)==0) {
				return true;
			}
			else {
				return false;
			}
		}

		static bool createDir(const std::string& dirname) {
			if(mkdir(dirname.data(),0755)!=0) {
				return false;
			}
			else {
				return true;
			}
		}

		static bool getFileSize(const std::string& fname,uint64_t* size) {
			struct stat sbuf;
			if(stat(fname.data(),&sbuf)!=0) {
				*size = 0;
				return false;
			}
			else {
				*size = sbuf.st_size;
			}
			return true;
		}

		static bool getCurrentDir(std::string& dir) {
			char cwd[125];
			if(getcwd(cwd,sizeof cwd)!=nullptr) {
				dir = std::string(cwd);
				return true;
			}
			return false; 
		}
	};
}