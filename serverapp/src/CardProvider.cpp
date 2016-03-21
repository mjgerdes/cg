

#include <fcntl.h>
#include "CardProvider.hpp"
#include <boost/filesystem.hpp>
#include <iostream>
#include "google/protobuf/io/zero_copy_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

using namespace boost::filesystem;
using namespace google::protobuf;
using namespace google::protobuf::io;

CardProvider::CardProvider(const std::string& dataPath) : m_cards() {
	init(dataPath);
}

void CardProvider::init(const std::string& dataPath) {
	m_cards.reserve(128);
	path p = dataPath.c_str();
	std::cout << "Loading cards from " << p << std::endl;

			data_type card;
ZeroCopyInputStream* is;
	for(directory_entry& file : directory_iterator(p)) {
			std::cout << "Loading " << file.path() << std::endl;

			int fd = open(file.path().string().c_str(), O_RDONLY);
is = new FileInputStream(fd);
			if(google::protobuf::TextFormat::Parse(is, &card)) {
				m_cards.insert(card);
			} else {
				std::cout << "Could not parse" << std::endl;
			}
delete is;
close(fd);

	} // for
} // init
