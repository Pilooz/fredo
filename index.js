global.__basedir  = __dirname;
var CONFIG        = {
	server : 'http://localhost:8080',
  data_dir : './data',
  sav_dir : './data/sav',
  lock_url : 'http://192.168.27.123/'
};

var app           = require('express')();
var express       = require('express');
var router        = express.Router();
var ejs           = require("ejs");
var server        = require('http').createServer(app);
const httpPort    = 8080;
var io            = require('socket.io').listen(server);
var ip            = require('ip');
var cookieParser  = require('cookie-parser');
var bodyParser    = require('body-parser');
var path          = require('path');
var formidable    = require('formidable'); // File upload
var fs            = require('fs');
const request     = require('request');

// Databases
var db_users       = require(CONFIG.data_dir + '/users.js');
var db_bikes       = require(CONFIG.data_dir + '/bikes.js');
var db_bookings    = require(CONFIG.data_dir + '/bookings.js');

// ------------------------------------------------------------------------
// Fonctions 
// ------------------------------------------------------------------------
const { write_file } = require("./lib/write_file");


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

//------------------------------------------------------------------------
// Init Socket to transmit Serial data to HTTP client
//------------------------------------------------------------------------
io.on('connection', function(socket) {
    //socket.emit(

    // Client acknowledgment when it has received a media element
    socket.on('client.acknowledgment', function(data){
      console.log(data.message);
    });
});


//------------------------------------------------------------------------
// HTTP Server configuration
//------------------------------------------------------------------------
server.listen( httpPort, '0.0.0.0', function( ) {
  CONFIG.server = 'http://' + ip.address() + ':' + httpPort;
  console.log( '------------------------------------------------------------' );
  console.log( 'server Ip Address is %s', ip.address() );     
  console.log( 'it is listening at port %d', httpPort );
  console.log( 'App entry point is http://' + ip.address() + ':' + httpPort + '/');
  console.log( '------------------------------------------------------------' );
});

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

// uncomment after placing your favicon in /public
//app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));
app.use('/js', express.static(__dirname + '/node_modules/bootstrap/dist/js')); // redirect bootstrap JS
app.use('/js', express.static(__dirname + '/node_modules/jquery/dist')); // redirect JS jQuery
app.use('/js', express.static(__dirname + '/node_modules/socket.io/dist')); // Socket.io
app.use('/js', express.static(__dirname + '/node_modules/hammerjs')); // Hammer.js
app.use('/css', express.static(__dirname + '/node_modules/bootstrap/dist/css')); // redirect CSS bootstrap

//-----------------------------------------------------------------------------
// Routing Middleware functions
// application logic is here / GET and POST on Index
//-----------------------------------------------------------------------------
var dataForTemplate = {};
var httpRequests = {};
var step = {};

router.all('/*', function (req, res, next) {
  // mettre toutes les requests dans un seul objet.
  httpRequests = req.query; // according to the use of express
  // add data
  dataForTemplate.config = CONFIG;

  next(); // pass control to the next handler
})

/* POST home page. */
.post('/', function(req, res, next) {
  res.render('index', { data: dataForTemplate });
})

/* GET home page. */
.get('/', function(req, res, next) {
  res.render('index', { data: dataForTemplate });
})

/* GET filter page. */
.get('/filter', function(req, res, next) {
  res.render('filter', { data: dataForTemplate });
})

/* GET home page. */
.get('/bookmybike', function(req, res, next) {
  res.render('bookmybike', { data: dataForTemplate });
})







/* GET home page. */
.get('/lock', function(req, res, next) {
  lock();
  res.render('index', { data: dataForTemplate });
})

/* GET home page. */
.get('/unlock', function(req, res, next) {
  unlock();
  res.render('index', { data: dataForTemplate });
})

/* Saving DBs. */
.get('/savedb', function(req, res, next) {
  write_file(CONFIG.data_dir, 'users.js', db_users, CONFIG.sav_dir);
  write_file(CONFIG.data_dir, 'bikes.js', db_bikes, CONFIG.sav_dir);
  write_file(CONFIG.data_dir, 'bookings.js', db_bookings, CONFIG.sav_dir);
  res.render('index', { data: dataForTemplate });
});

/* POST media page. */
// .post('/media-upload', function(req, res, next) {
//     var form = new formidable.IncomingForm();
//     form.uploadDir = path.join(__dirname, CONFIG.app.mediaPath);
//     form.keepExtensions = true; // keep original extension

//     form.parse(req, function (err, fields, files) {
//       // Let the media library do the rest of the job !
//       mediaDB.newMedia(fields, files);
//     });
//   // Routing to index
//   res.redirect('/');
// })

//-----------------------------------------------------------------------------
// Application express
//-----------------------------------------------------------------------------
app.use('/', router);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// all error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};
  // render the error page
  res.status(err.status || 500);
  res.render('error', {data: dataForTemplate});
});

module.exports = app;
