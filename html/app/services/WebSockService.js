app.service('WebSockService', function ($rootScope) {

this.sendMsg = function(clientMsg) {
ws.send(clientMsg.toArrayBuffer());
};

this.registerHandler = function(msgType, f) {
	handlers[msgType] = f;
	};

var handlers = {}
var ws = new WebSocket("ws://localhost:8080/index");
  ws.onmessage = function(evt) {
//    try {
        // Decode the Message
var bytebuffer = dcodeIO.ByteBuffer;
var bb = new bytebuffer();
	  bb = bytebuffer.wrap(evt.data, "utf8");
        var msg = ServerMessage.decode(bb);
		$rootScope.$apply(function() {handlers[msg.msgType](msg)});
//    } catch (err) {
//    }
};
});
