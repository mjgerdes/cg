
app.controller('CollectionController', function ($scope, $location, WebSockService, CardDataService) {

$scope.test = CardDataService.get(CardDataService.types.basic_missile).description;

	});

