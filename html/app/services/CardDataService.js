app.service('CardDataService', function ($rootScope, $http) {
var DataBuilder = ProtoBuf.loadProtoFile("appdata/proto/CardData.proto");
var Data = DataBuilder.build("data");
var wrapper = new Data.CardDataWrapper();
var cards = {};
var ready = false;

this.types = Data.CardData.CardId;
this.get = function (id) {
	while(!ready) {}
	return cards[id];
	}


$.ajax({
	url : "appdata/data/cards/compile.dat",
	type : "GET",
	dataType : "binary",
	responseType : "arraybuffer",
	processResponse : false,
	success : function (data) {
var msg = Data.CardDataWrapper.decode(data);
for(var i = 0; i < msg.cards.length; i++) {
	cards[msg.cards[i].id] = msg.cards[i];
	}
		ready = true;
		}});


});
