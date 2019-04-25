const request = require('request');

// Deverrouiller le cadenas
function unlock() {
	resquesting_lock("on");
}


// Verrouiller le Cadenas
function lock() {
	resquesting_lock("off");
}

function resquesting_lock(order) {
	console.log("Requesting the lock at " + CONFIG.lock_url + order);
	request(CONFIG.lock_url + order, { json: true }, (err, res, body) => {
	  if (err) { 
	  	return console.log(err); 
	  }
	  console.log(body.url);
	  console.log(body.explanation);
	});
}