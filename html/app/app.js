
var globalEmail;
var app = angular.module('CGApp', ['ngRoute', 'ngCookies']);

//This configures the routes and associates each route with a view and a controller
app.config(function ($routeProvider) {
    $routeProvider
        .when('/login',
            {
                controller: 'LoginController',
                templateUrl: 'app/partials/login.html'
            })
        .when('/index',
            {
                controller: 'IndexController',
                templateUrl: 'app/partials/indexPage.html'
            })
        .when('/collection',
            {
                controller: 'CollectionController',
                templateUrl: 'app/partials/collection.html'
            })
	        .when('/play',
            {
                controller: 'PlayController',
                templateUrl: 'app/partials/play.html'
            })
        .otherwise({ redirectTo: '/login' });
});
