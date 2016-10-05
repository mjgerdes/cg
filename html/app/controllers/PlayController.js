
app.controller('PlayController', ['$scope', '$location', 'WebSockService', 'CardDataService', '$cookies',
function ($scope, $location, WebSockService, CardDataService, $cookies) {


	
  // on startup we send a request to iniate play and be put in th queue
var playInitiationRequestMsg = new ClientMessage({
	"msgType" : "PlayInitiationRequestType"});
	WebSockService.sendMsg(playInitiationRequestMsg);



// remember me
WebSockService.registerHandler(ServerMessageTypes.LoginResponseType, function(msg) {
//	WebSockService.sendMsg(cardCollectionRequestMsg);
//WebSockService.sendMsg(systemCollectionRequestMsg);
	});
}]);

