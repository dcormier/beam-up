var VERSION = "2.5";

/******************************** Pebble API **********************************/

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL('https://dl.dropboxusercontent.com/u/10824180/pebble%20config%20pages/beamup-basalt-configuration-staging.html?version=' + VERSION);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var json = JSON.parse(decodeURIComponent(e.response));

  var options = {
    'PERSIST_KEY_DATE': '' + json.date,
    'PERSIST_KEY_ANIM': '' + json.animations,
    'PERSIST_KEY_BT': '' + json.bluetooth,
    'PERSIST_KEY_BATTERY': '' + json.battery,
    'PERSIST_KEY_HOURLY': '' + json.hourly,
    'PERSIST_KEY_THEME': true,
    'PERSIST_KEY_FG_R': parseInt(json.foreground.substring(2, 4), 16),
    'PERSIST_KEY_FG_G': parseInt(json.foreground.substring(4, 6), 16),
    'PERSIST_KEY_FG_B': parseInt(json.foreground.substring(6), 16),
    'PERSIST_KEY_BG_R': parseInt(json.background.substring(2, 4), 16),
    'PERSIST_KEY_BG_G': parseInt(json.background.substring(4, 6), 16),
    'PERSIST_KEY_BG_B': parseInt(json.background.substring(6), 16),
    'PERSIST_KEY_LEADING_ZERO': '' + json.leading_zero
  };

  console.log('Sending option: ' + JSON.stringify(options));

  Pebble.sendAppMessage(options, function(e) {
    console.log('Settings update successful!');
  }, function(e) {
    console.log('Settings update failed: ' + JSON.stringify(e));
  });
}); 