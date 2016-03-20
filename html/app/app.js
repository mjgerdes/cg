var app = angular.module('CGApp', ['ngRoute']);

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
        .otherwise({ redirectTo: '/login' });
});
