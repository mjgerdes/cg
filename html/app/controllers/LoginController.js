
app.controller('LoginController', function ($scope, WebSockService) {

WebSockService.registerHandler(ServerMessage.ServerMessageType.RegistrationResponseType, function(msg) {
	alert("Success is " + msg.registrationresponse.success);
	});


	$scope.register = function() {
var registerMsg = new ClientMessage({
	"msgType" : "RegistrationType",
	"registration" : { "email" : $scope.email, "password" : $scope.password}});
		WebSockService.sendMsg(registerMsg);
		};
});
			   
