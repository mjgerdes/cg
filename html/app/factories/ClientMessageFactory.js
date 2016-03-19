

var ProtoBuf = dcodeIO.ProtoBuf;
var Builder = ProtoBuf.loadProtoFile("proto/ClientMessage.proto");
ProtoBuf.loadProtoFile("proto/Login.proto", Builder);
ProtoBuf.loadProtoFile("proto/Registration.proto", Builder);



var Message = Builder.build("msg");
var ClientMessage = Message.ClientMessage;
var LoginMessage = Message.Login;
var RegistrationMessage = Message.Registration;


