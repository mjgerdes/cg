
app.controller('CollectionController', ['$scope', '$location', 'WebSockService', 'CardDataService',
function ($scope, $location, WebSockService, CardDataService) {
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
	}]);

