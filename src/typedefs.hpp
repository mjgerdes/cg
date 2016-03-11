

#ifndef __TYPEDEFS_HPP__
#define __TYPEDEFS_HPP__


#include <memory>
#include "websock/server_ws.hpp"
#include "websock/client_ws.hpp"



typedef SimpleWeb::SocketServer<SimpleWeb::WS> WSServer;
typedef SimpleWeb::SocketClient<SimpleWeb::WS> WsClient;
using WSRawConnection = WSServer::Connection;
typedef std::shared_ptr<WSRawConnection> WSConnection;
typedef std::shared_ptr<WSServer::Message> WSMessage;

#endif


