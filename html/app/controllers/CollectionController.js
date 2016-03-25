
app.controller('CollectionController', ['$scope', '$location', 'WebSockService', 'CardDataService', '$cookies',
function ($scope, $location, WebSockService, CardDataService, $cookies) {
//alert(CardDataService.get);
//$scope.test = CardDataService.get(CardDataService.types.basic_missile).description;
$scope.card_ids = [];
$scope.cards = [];

WebSockService.registerHandler(ServerMessageTypes.CardCollectionResponseType, function (msg) {
$scope.card_ids = msg.card_collection_response.card_ids;
	for(var i = 0; i < $scope.card_ids.length; i++) {
		var temp = CardDataService.get($scope.card_ids[i]);
		$scope.cards.push(temp);
		}
	});

var cardCollectionRequestMsg = new ClientMessage({
	"msgType" : "CardCollectionRequestType"});
	WebSockService.sendMsg(cardCollectionRequestMsg);
//	alert(CardDataService.get);



WebSockService.registerHandler(ServerMessageTypes.LoginResponseType, function(msg) {
	WebSockService.sendMsg(cardCollectionRequestMsg);
	});
if($cookies.get("email")) {
var msg = new ClientMessage({
	"msgType" : "LoginTokenType",
"login_token" :{"email":$cookies.get("email"), "token" : $cookies.get("token")}});
	globalEmail = $cookies.get("email");
	WebSockService.sendMsg(msg);
	}
	}]);

