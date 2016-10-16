app.service('WebSockService', function ($rootScope, $location, $cookies, $timeout) {
var ready = false;
	var rememberMeDone = true;
	if($cookies.get("email")) {
		rememberMeDone = false;
		}

var sendMsg = function(clientMsg) {
	if(!ready || !rememberMeDone) {
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
	rememberMeDone = true;
},
6: function(msg) {
$cookies.put("email", "");
$cookies.put("token","");
rememberMeDone = true;
}};


var ws = new WebSocket("ws://golm.local:8080/index");
ws.binaryType = 'arraybuffer';
	ws.onopen = function (evt) {
		ready = true;
if($cookies.get("email")) {
var msg = new ClientMessage({
	"msgType" : "LoginTokenType",
"login_token" :{"email":$cookies.get("email"), "token" : $cookies.get("token")}});
	globalEmail = $cookies.get("email");
	ws.send(msg.toArrayBuffer());
	}
		};

ws.onclose = function(e) {
console.log("Websock close: " ,e);
}

ws.onerror = function(e) {
	console.log("Websock error: ", e);
	}

  ws.onmessage = function(evt) {
    try {
        // Decode the Message
var bytebuffer = dcodeIO.ByteBuffer;
var bb = new bytebuffer();
	  bb = bytebuffer.wrap(evt.data); //, "utf8");
        var msg = ServerMessage.decode(bb);
//		console.log(evt.toSource());

//        var msg = ServerMessage.decode(evt.data);
//	  console.log(msg.msgType);
//	  alert(msg.msgType);
		$rootScope.$apply(function() {handlers[msg.msgType](msg)});
    } catch (err) {
		console.log(err.toSource());
   }
};
});
