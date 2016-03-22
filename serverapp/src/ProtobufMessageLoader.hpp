

#ifndef __PROTOBUFMESSAGELOADER_HPP__
#define __PROTOBUFMESSAGELOADER_HPP__

#include <functional>
#include <iostream>
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <boost/filesystem.hpp>
#include <fcntl.h>

template <typename msg_T>
struct ProtobufMessageLoader {
	void operator()(const std::string& dataPath,
					std::function<bool(msg_T&)> push) {
		using namespace boost::filesystem;
		using namespace google::protobuf;
		using namespace google::protobuf::io;

		path p = dataPath.c_str();
if(!is_directory(p)) {
std::cerr << "Warning in ProtobufMessageLoader: " << p << " is not a directory; no files were loaded!" << std::endl;
}

		msg_T msg;
		ZeroCopyInputStream* is;
		for (directory_entry& file : directory_iterator(p)) {
			int fd = open(file.path().string().c_str(), O_RDONLY);
			is = new FileInputStream(fd);
			if (TextFormat::Parse(is, &msg)) {
				if (!push(msg)) {
					std::cerr << "Warning in ProtobufMessageLoader: Verify "
								 "failed on file " << file.path() << std::endl;
				}
			} else {
				std::cerr << "Warning in ProtobufMessageLoader: Could not parse"
						  << file.path() << std::endl;
			}
			delete is;
			close(fd);
		}  // for
	}
};  // end struct ProtobufMessageLoader

#endif
