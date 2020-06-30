

var http=require('http');

var server=http.createServer(function(req, res){
	console.log('start server');
	res.write('success');
	res.end();
});

server.listen(8080);
