

app.directive('cgSystem', function() {
	return {
restrict: 'E',
		replace: true,
		scope: {system : '=system',
			   currentHealth: "=health",
			   currentCards: "=cards"},
				templateUrl : 'app/partials/cgSystem.html',
		controller: ['$scope', function ($scope) {

$scope.name = $scope.system.name;
			$scope.type = $scope.system.type;
			$scope.capacity = $scope.system.capacity;
			$scope.proc = $scope.system.proc;
			if($scope.proc) {
				$scope.showProc = true;
				} else { $scope.showProc = false;}
			$scope.procLabel = "Strength";
			$scope.health = $scope.system.health;
$scope.ability = $scope.system.ability;
			$scope.description = $scope.system.description;
			$scope.cardType = cssClassForType($scope.type);
}]};
	});
