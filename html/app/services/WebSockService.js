app.service('WebSockService', function () {

this.sendMsg = function(clientMsg) {
ws.send(clientMsg.toArrayBuffer());
};

this.registerHandler = function(msgType, f) {
	handlers[msgType] = f;
	};

var handlers = {}
var ws = new WebSocket("ws://localhost:8080/index");
  ws.onmessage = function(evt) {
    try {
        // Decode the Message
        var msg = Message.decode(evt.data);
		handlers[msg.msgType()](msg);
    } catch (err) {
alert("Could not decode message.");
    }
};
});
