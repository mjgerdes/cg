

var ProtoBuf = dcodeIO.ProtoBuf;
var Builder = ProtoBuf.loadProtoFile("proto/ClientMessage.proto");
ProtoBuf.loadProtoFile("proto/Login.proto", Builder);
ProtoBuf.loadProtoFile("proto/Registration.proto", Builder);
ProtoBuf.loadProtoFile("proto/ServerMessage.proto", Builder);
ProtoBuf.loadProtoFile("proto/RegistrationResponse.proto", Builder);
ProtoBuf.loadProtoFile("proto/LoginResponse.proto", Builder);
ProtoBuf.loadProtoFile("proto/DataModuleMessages.proto", Builder);
ProtoBuf.loadProtoFile("proto/Table.proto", Builder);

var Message = Builder.build("msg");
var ClientMessage = Message.ClientMessage;
var ServerMessage = Message.ServerMessage;
var ServerMessageTypes = ServerMessage.ServerMessageType;
var LoginMessage = Message.Login;
var RegistrationMessage = Message.Registration;


