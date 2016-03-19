
app.controller('LoginController', function ($scope, WebSockService) {



    init();

    function init() {
}

	$scope.register = function() {
var registerMsg = new ClientMessage({
	"msgType" : "RegistrationType",
	"registration" : { "email" : $scope.email, "password" : $scope.password}});
		WebSockService.sendMsg(registerMsg);
		};
});
			   
