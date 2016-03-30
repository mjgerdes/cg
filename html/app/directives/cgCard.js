

			function cssClassForType(type) {
				if(type == 1) {
					return "card-science";
					} else if(type == 2) {
						return "card-military";
						}
				return "card-support";
				}


var stringForTag = {
	1 : "Missile",
	2: "Fighter",
	3: "Decoy"
	}
		

app.directive('cgSms', function() {
	return {
restrict: 'E',
		replace: true,
		scope: {type : '@'},
		controller: ['$scope', function ($scope) {
			if($scope.type == "science") {
				$scope.cssclass = "science-icon";
				$scope.sms = "S";
				}
			if($scope.type == "military") {
					$scope.cssclass = "military-icon";
					$scope.sms = "M";
					}
			if($scope.type == 'support') {
						$scope.cssclass = "support-icon";
						$scope.sms = "S";
						}
			}],
		template: "<span class='{{cssclass}}'>{{sms}}</span>"}
	});

app.directive('cgCard', function () {

	return {
		restrict: 'E',
		replace: true,
		templateUrl : 'app/partials/cgCard.html',
		scope : {
			card : '='},
		
		controller: ['$scope', function($scope) {
function smsPart(cost, prefix) {
var result = "";
if(cost && cost != 0) {
		result = prefix + cost;
		}
	return result;
	}

			function stringsForTags(tags) {
				var res = [];
for(var i = 0; i < tags.length; i++) {
	res.push(stringForTag[tags[i]]);
	}
				return res;
				}

$scope.smsScience = smsPart($scope.card.science_cost, "");
			$scope.smsMilitary = smsPart($scope.card.military_cost, "");
			$scope.smsSupport = smsPart($scope.card.support_cost, "");
$scope.attack = $scope.card.attack;
			$scope.health = $scope.card.health;
			$scope.showStats = $scope.attack || $scope.health;
			$scope.description = $scope.card.description;

			$scope.tags = stringsForTags($scope.card.tags);
if($scope.card.category == 2) {
	$scope.tags.push("Crew");
	$scope.smsScience = smsPart($scope.card.science_provide, "+");
			$scope.smsMilitary = smsPart($scope.card.military_provide, "+");
			$scope.smsSupport = smsPart($scope.card.support_provide, "+");

	}
			if($scope.card.category == 3) {
				$scope.tags.push("Tech");
				}
						$scope.showTags = $scope.tags;
			$scope.cardType = cssClassForType($scope.card.type);
			}]
			};
	});
		
