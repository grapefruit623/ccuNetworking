function circle(  context, i ) {
		console.log("circle");
		/*circle.x = 20*(i+1);
		circle.y = 20;
		*/
		this.x = 23*(i+1);
		this.y = 20;
		this.vec = 1;
		this.speed = Math.random();
		this.radius = 10;
		context.fillStyle="blue";
		context.beginPath();
		context.arc(this.x, this.y, this.radius, 0, Math.PI*2, true);
		context.closePath();
		context.fill();
};

circle.prototype.update = function( context ) {
		console.log(this, this.x, this.y);
		var nextX = this.x + Math.random()* ( (Math.random()*2 - 1) );
		var nextY = this.y + this.speed * this.vec;
		if ( nextX > 10 && nextX < 400 )
				this.x = nextX; 
		if (  0 != this.speed && nextY < 400 && nextY > 0  ) {
				this.y = nextY; 
				this.speed += this.vec * Math.random();
		}
		else {
				this.speed *= 0.7;
				this.vec *= -1;
		}
		context.beginPath();
		context.arc(this.x, this.y, this.radius, 0, Math.PI*2, true);
		context.closePath();
		context.fill();
};

function myTurn( context, balls ) {
		console.log("myTurn begin");
		context.clearRect(0, 0, 400, 400);
		for ( var i = 0; i < balls.length; i++ )
				balls[i].update( context );

		//setTimeout("myTurn("+context+","+balls+")", 1000);
		var count = 0;
		for ( var i = 0; i < balls.length; i++ ) {
				if ( 400 - balls[i].y < 20 )
						count++;
		}
		if ( balls.length == count )
				for ( var i =0; i < balls.length; i++ )
						balls[i].y = 20;

		setTimeout(function(){myTurn(context,balls)}, 10);
		console.log("myTurn end");
}
function init( context ) {
		var balls = new Array();
		for ( var i = 0; i < 10; i++ ) {
			balls.push(  new circle( context, i ) );
		}
		myTurn( context, balls );
}
window.onload = function() {
		var canvas = document.getElementById("myCanvas");
		var context = canvas.getContext("2d");
		init( context );
};
