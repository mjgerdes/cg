

#ifndef __TYPEDEFS_HPP__
#define __TYPEDEFS_HPP__


#include <memory>
#include "websock/server_ws.hpp"
#include "websock/client_ws.hpp"
#include "ClientMessage.pb.h"


typedef SimpleWeb::SocketServer<SimpleWeb::WS> WSServer;
typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;
typedef std::shared_ptr<WSServer::Connection> WSConnection;
typedef std::shared_ptr<WSServer::Message> WSMessage;
namespace msg {
	using pbmsg_type = ::google::protobuf::Message;
}
#endif


