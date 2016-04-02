

#ifndef __PROTOBUFMESSAGELOADER_HPP__
#define __PROTOBUFMESSAGELOADER_HPP__

#include <functional>
#include <iostream>
#include <fstream>
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#include <boost/filesystem.hpp>
#include <fcntl.h>

template <typename msg_T>
struct ProtobufMessageLoader {
	void operator()(const std::string dataPath,
					std::function<bool(msg_T&)> push,
					std::function<void(msg_T&)>* callback = nullptr) {
		using namespace boost::filesystem;
		using namespace google::protobuf;
		using namespace google::protobuf::io;
		std::cout << "Loading files in " << dataPath << std::endl;
		path p = dataPath.c_str();

		// for some reason this crashes - boost.filesystem seems to be a bit
		// wonky
		// if (!boost::filesystem::is_directory(p)) {
		//			std::cerr << "Warning in ProtobufMessageLoader: " <<
		// dataPath
		//					  << " is not a directory; no files were loaded!"
		//					  << std::endl;
		//		}

		msg_T msg;
		msg.Clear();
		ZeroCopyInputStream* is;
		for (directory_entry& file : directory_iterator(p)) {
			if(file.path().extension() != ".pb") {
//				std::cerr << "    Ignoring " << file.path().filename() << std::endl;
			continue;
			}
			int fd = open(file.path().string().c_str(), O_RDONLY);
			is = new FileInputStream(fd);
			if (TextFormat::Parse(is, &msg)) {
				if (!push(msg)) {
					std::cerr << "    Warning in ProtobufMessageLoader: Verify "
								 "failed on file " << file.path() << std::endl;
				}
// FIXME: enable for verbose mode
//				std::cout << "Loaded " << msg.name() << " id " << msg.id() << std::endl;
			} else {
				std::cerr << "    Warning in ProtobufMessageLoader: Could not parse"
						  << file.path() << std::endl;
			}
			if (callback) {
				callback->operator()(msg);
			}
			msg.Clear();
			delete is;
			close(fd);
		}  // for
	}
};  // end struct ProtobufMessageLoader

template <typename msg_T, typename wrapperMsg_T>
struct ProtobufMessageCompileLoader {
	void operator()(const std::string dataPath,
					std::function<bool(msg_T&)> push) {
		auto wrapper = wrapperMsg_T{};
		std::function<void(msg_T&)> f = [&wrapper](msg_T& msg) {
			auto newMsg = wrapper.add_wrapped_msgs();
			*newMsg = msg;
		};

		auto loader = ProtobufMessageLoader<msg_T>{};
		loader(dataPath, push, &f);

		std::ofstream file;
		file.open(dataPath + std::string("/compile.dat"), std::ios::binary | std::ios::out); 
		wrapper.SerializeToOstream(&file);
		file.close();

// test if everything serialized correctly
		auto testWrapper = wrapperMsg_T{};
		auto testFile = std::ifstream{};
		testFile.open(dataPath + std::string("/compile.dat"), std::ios::binary | std::ios::in);
		testWrapper.ParseFromIstream(&testFile);
		testFile.close();
	}
};  // end struct ProtobufMessageCompileLoader
#endif
