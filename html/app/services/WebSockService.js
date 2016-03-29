app.service('WebSockService', function ($rootScope, $location, $cookies, $timeout) {
var ready = false;
var sendMsg = function(clientMsg) {
	if(!ready) {
		$timeout(sendMsg, 100, false, clientMsg);
		return;
		}
ws.send(clientMsg.toArrayBuffer());
};
this.sendMsg = sendMsg;
this.registerHandler = function(msgType, f) {
	handlers[msgType] = f;
	};


var handlers = {
4 : function(msg) {
	$cookies.put("email", globalEmail);
	$cookies.put("token", msg.login_token_issue.token);
}};


var ws = new WebSocket("ws://golm.local:8080/index");
	ws.onopen = function (evt) {
		ready = true;
		}

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
