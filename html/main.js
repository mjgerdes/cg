
var ProtoBuf = dcodeIO.ProtoBuf;
var Builder = ProtoBuf.loadProtoFile("proto/ClientMessage.proto");
ProtoBuf.loadProtoFile("proto/Login.proto", Builder);



var Message = Builder.build("msg");
var ClientMessage = Message.ClientMessage;
var LoginMessage = Message.Login;

var loginMsg = new LoginMessage("integr@gmail.com","secret");
var clientMsg = new ClientMessage({
    "msgType" : "LoginType",
    "login":{
 "email":"integr@gmail.com", "password":"secret"}});




var ws;
window.onload=function(){
  ws=new WebSocket("ws://localhost:8080/index");


    
  ws.onmessage = function(evt) {
      console.log(evt.data);
      var loginFormSection = document.getElementById("loginFormSection");
      if(evt.data == "AUTHPLS") {
	  loginFormSection.style.display = "block";
	  }
      if(evt.data == "AUTHOKTHX") {
loginFormSection.style.display = "none";
document.getElementById("portal").style.display = "block";
}
  };

  ws.onopen=function(evt){
//    ws.send("Hello");
      for(i = 0; i < 1000; i++) {
      ws.send(clientMsg.toArrayBuffer());
	  }
  };
};

window.onclose=function(){
  ws.close();
};

function loginSubmitEvent () {
    var email = document.getElementById("loginEmail").value;
    var password = document.getElementById("loginPassword").value;
    ws.send(email + ":" + password);
}

