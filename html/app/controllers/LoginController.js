
app.controller('LoginController', function ($scope, $location, WebSockService) {
$scope.msgp = false;

	WebSockService.registerHandler(ServerMessageTypes.LoginResponseType, function(msg) {
var success = msg.loginresponse.success;
	if(success) {
$scope.msgp = false;
$scope.msg = "";
$location.path('/index');
		} else {
			$scope.msgp = true;
			$scope.msg = "Sorry, invalid email or password!";
			}
	});

WebSockService.registerHandler(ServerMessage.ServerMessageType.RegistrationResponseType, function(msg) {
var success = msg.registrationresponse.success
	if(success) {
$scope.msgp = false;
$scope.msg = "";
		$location.path('/index');
		} else {
$scope.msgp = true;
					$scope.msg = "Could not register your account! Perhaps you are already registered or logged in?";
			}
	});

$scope.login = function() {
	var loginMsg = new ClientMessage({
		"msgType" : "LoginType",
		"login" : {"email" : $scope.email, "password" : $scope.password}});
	WebSockService.sendMsg(loginMsg);
	};

	$scope.register = function() {
var registerMsg = new ClientMessage({
	"msgType" : "RegistrationType",
	"registration" : { "email" : $scope.email, "password" : $scope.password}});
		WebSockService.sendMsg(registerMsg);
		};
});
			   
