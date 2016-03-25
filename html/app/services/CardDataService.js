app.service('CardDataService', function ($rootScope, $http) {

this.get = function (id) {
//var result = {};
//get_impl(id, result);
//	return result["result"];
	return cards[id];
	};

var DataBuilder = ProtoBuf.loadProtoFile("appdata/proto/CardData.proto");
var Data = DataBuilder.build("data");
var wrapper = new Data.CardDataWrapper();
var cards = {};
var ready = false;
this.types = Data.CardData.CardId;


$http({
	url : "appdata/data/cards/compile.dat",
	type : "GET",
	dataType : "binary",
	responseType : "arraybuffer",
	processResponse : false})
	.success(function (data) {
var msg = Data.CardDataWrapper.decode(data);
for(var i = 0; i < msg.cards.length; i++) {
	cards[msg.cards[i].id] = msg.cards[i];
	}
ready = true;
		});

var get_impl = function (id, result) {
	if(!ready) {
		setTimeout(function () { get_impl(id, result)}, 500);
		return;
		}
	result["result"] = cards[id];
	};



});
