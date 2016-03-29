app.service('CardDataService', function ($rootScope, $http) {
this.get = function (type, id) {
var result = {};
get_impl(type, id, result);
	return result["result"];
	};

var DataBuilder = ProtoBuf.loadProtoFile("appdata/proto/CardData.proto");
ProtoBuf.loadProtoFile("appdata/proto/SystemData.proto", DataBuilder);
var DataMsgs = DataBuilder.build("data");
var wrapper = new DataMsgs.CardDataWrapper();
	var systemWrapper = new DataMsgs.SystemDataWrapper();
	var data = {"cards" : {}, "systems": {}};
var cardReady = false;
	var systemReady = false;
this.types = DataMsgs.CardData.CardId;

function requestBinaryData(dataUrl, handler) {
$http({
	url : dataUrl, //"appdata/data/cards/compile.dat",
	type : "GET",
//	dataType : "binary",
	responseType : "arraybuffer",
	processResponse : false})
	.success(handler);
	}

	function cardHandler(respdata) {
var msg = DataMsgs.CardDataWrapper.decode(respdata);
for(var i = 0; i < msg.wrapped_msgs.length; i++) {
	data["cards"][msg.wrapped_msgs[i].id] = msg.wrapped_msgs[i];
	}
cardReady = true;
		}

	function systemHandler(respdata) {
var msg = DataMsgs.SystemDataWrapper.decode(respdata);
for(var i = 0; i < msg.wrapped_msgs.length; i++) {
	data["systems"][msg.wrapped_msgs[i].id] = msg.wrapped_msgs[i];
	}
systemReady = true;
		}

requestBinaryData("appdata/data/cards/compile.dat", cardHandler);
requestBinaryData("appdata/data/systems/compile.dat", systemHandler);

var get_impl = function (type, id, result) {
	if(!cardReady || !systemReady) {
		setTimeout(function () { get_impl(type, id, result)}, 500);
		return;
		}
	result["result"] = data[type][id];
	};



});
