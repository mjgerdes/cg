
app.controller('CollectionController', ['$scope', '$location', 'WebSockService', 'CardDataService', '$cookies',
function ($scope, $location, WebSockService, CardDataService, $cookies) {


$scope.card_ids = [];
$scope.cards = [];
$scope.system_ids = [];
	$scope.systems = []
$scope.showCardsp = false;
	$scope.showSystemsp = true;
$scope.showCards = function () {
	$scope.showCardsp = true;
	$scope.showSystemsp = false;
	};

	$scope.showSystems = function() {
		$scope.showCardsp = false;
		$scope.showSystemsp = true;
		};


// cardcollectionresponse
WebSockService.registerHandler(ServerMessageTypes.CardCollectionResponseType, function (msg) {
$scope.card_ids = msg.card_collection_response.card_ids;
	for(var i = 0; i < $scope.card_ids.length; i++) {
		var temp = CardDataService.get("cards", $scope.card_ids[i]);
		$scope.cards.push(temp);
		}
	});

// SystemCollectionResponse
WebSockService.registerHandler(ServerMessageTypes.SystemCollectionResponseType, function (msg) {
$scope.system_ids = msg.system_collection_response.system_ids;
	for(var i = 0; i < $scope.system_ids.length; i++) {
		var temp = CardDataService.get("systems", $scope.system_ids[i]);
		$scope.systems.push(temp);
		}

	});

var cardCollectionRequestMsg = new ClientMessage({
	"msgType" : "CardCollectionRequestType"});
	WebSockService.sendMsg(cardCollectionRequestMsg);

var systemCollectionRequestMsg = new ClientMessage({
	"msgType" : "SystemCollectionRequestType"});
WebSockService.sendMsg(systemCollectionRequestMsg);
// remember me stuff
WebSockService.registerHandler(ServerMessageTypes.LoginResponseType, function(msg) {
//	WebSockService.sendMsg(cardCollectionRequestMsg);
//WebSockService.sendMsg(systemCollectionRequestMsg);
	});
/*
if($cookies.get("email")) {
var msg = new ClientMessage({
	"msgType" : "LoginTokenType",
"login_token" :{"email":$cookies.get("email"), "token" : $cookies.get("token")}});
	globalEmail = $cookies.get("email");
	WebSockService.sendMsg(msg);
	}
*/
	}]);

