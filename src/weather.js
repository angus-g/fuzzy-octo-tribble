var xhrRequest = function(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var apiKey = '';

function locationSuccess(pos) {
  var url = 'https://api.forecast.io/forecast/' + apiKey + '/' +
      pos.coords.latitude + ',' + pos.coords.longitude + '?units=si&exclude=minutely,hourly,daily';
  
  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);
      var temp = Math.round(json.currently.temperature);
      var dict = {
        'KEY_TEMP': temp,
      };
      
      Pebble.sendAppMessage(dict, function(e) {}, function(e) {});
    });
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess, locationError,
    { timeout: 15000, maximumAge: 60000 }
  );
}

Pebble.addEventListener('ready',
  function(e) {
    getWeather();
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    getWeather();
  }
);