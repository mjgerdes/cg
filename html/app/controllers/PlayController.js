

function getSystemsForIds(cds, system_ids) {
  var systemAcc = [];
	for(var i = 0; i < system_ids.length; i++) {
		var temp = cds.get("systems", system_ids[i]);
		systemAcc.push(temp);
		}
return systemAcc;
	}

function getCardsForIds(cds, cardMap, card_ids) {
var acc = [];
	for(var i = 0; i < card_ids.length(); i++) {
		var temp = cds.get("cards", card_ids[i]);
		}
	return acc;
}

app.controller('PlayController', ['$scope', '$location', 'WebSockService', 'CardDataService', '$cookies',
function ($scope, $location, WebSockService, CardDataService, $cookies) {

$scope.cardMap = {};
$scope.tableReadyp = false;



// table start message
WebSockService.registerHandler(ServerMessageTypes.TableStartMessageType, function(msg) {
	$scope.myPlayerNumber = msg.table_start_message.my_player_number;
});


// obfuscation table message
WebSockService.registerHandler(ServerMessageTypes.ObfuscationTableMessageType, function(msg) {
for(var i = 0; i < msg.obfuscation_table_message.length(); i++) {
$scope.cardMap[msg.obfuscation_table_message[i].obfuscation] = msg.obfuscation_table_message[i].real;
	}
	});

// table
WebSockService.registerHandler(ServerMessageTypes.TableType, function(msg) {
$scope.tableReadyp = true;
//$scope.model = msg.table;

	console.log(msg.table.toSource());

if( $scope.myPlayerNumber === 1) {
	// we are player 1
	$scope.enemySystems = getSystemsForIds(CardDataService, msg.table.player2_system_ids);
	$scope.playerSystems = getSystemsForIds(CardDataService, msg.table.player1_system_ids);
			$scope.playerHand = getCardsForIds(CardDataProvider, $scope.cardMap, msg.player1_hand);
	} else {
		// we are player 2
			$scope.enemySystems = getSystemsForIds(CardDataService, msg.table.player1_system_ids);
	$scope.playerSystems = getSystemsForIds(CardDataService, msg.table.player2_system_ids);
		$scope.playerHand = getCardsForIds(CardDataProvider, $scope.cardMap, msg.player2_hand);
		}
	});

// remember me
WebSockService.registerHandler(ServerMessageTypes.LoginResponseType, function(msg) {
	});


  // on startup we send a request to iniate play and be put in th queue
var playInitiationRequestMsg = new ClientMessage({
	"msgType" : "PlayInitiationRequestType"});
	WebSockService.sendMsg(playInitiationRequestMsg);


}]);

