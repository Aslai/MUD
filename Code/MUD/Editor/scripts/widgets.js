var Positioner={
	X: 100,
	Y: 100
};

var MenuBars={
	ID: 0
};

function MakeWindow( title, width ){
	var win = document.createElement( 'div' );
	win.className = 'window';
	win.style.minWidth = width + 'px';
	win.onmousedown=WindowOnMouseDown;
	win.ondblclick=WindowOnDoubleClick;
	win.style.zIndex = Window.LastZ++;
	
	var titlebar = document.createElement( 'div' );
	titlebar.className = 'windowtitle';
	titlebar.innerHTML = title;
	
	var closebutton = document.createElement( 'div' );
	closebutton.className = 'close';
	closebutton.innerHTML = 'X';
	closebutton.onclick=WindowOnClose;
	
	var maxbutton = document.createElement( 'div' );
	maxbutton.className = 'maximize';
	maxbutton.innerHTML = '&#9633;';
	maxbutton.onclick=WindowOnMaximize;
	
	var inner = document.createElement( 'div' );
	inner.className = 'windowinner';
	
	inner.style.top = '44px';
	
	var winresizer = document.createElement( 'div' );
	winresizer.className = 'windowresizer';
	winresizer.onmousedown = ResizeStart;
	
	titlebar.appendChild( closebutton );
	titlebar.appendChild( maxbutton );
	win.appendChild( titlebar );
	
	win.appendChild(
		MakeMenubar(
		{
			File: 
			{ 
				New: function(){ alert("NEW!");},
				Quit: function(){ alert("QUIT!");}
			},
			This:
			{
				Has_Some: function(){ alert("Has Some!");},
				Neat_Functionality: function(){ alert("Neat Functionality!");},
			},
			Is:
			{
				This_Awesome: function(){ alert("Yes it is!"); }
			},
			Cool:
			{
				Beans: function(){ alert("Beans are a magical fruit."); }
			}
		}
	));
	
	win.appendChild( inner );
	win.appendChild( winresizer );
	
	document.getElementById("TheContainer").appendChild( win );
	
	var rect = document.getElementById("TheContainer").getBoundingClientRect();
	if( Positioner.Y > rect.bottom - rect.top ){
		Positioner.Y = 100;
	}
	if( Positioner.X > rect.right - rect.left ){
		Positioner.X = 100;
	}
	
	win.style.left = Positioner.X + 'px';
	win.style.top = Positioner.Y + 'px';
	Positioner.X += 20;
	Positioner.Y += 20;
	
	
	return win;
	
}

function MakeMenubar( table ){
	if( Object.prototype.toString.call( table ) == "[object Object]" ){
		var menubar = document.createElement( 'div' );
		menubar.className = 'menubar';
		
		for( var value in table ){
			var id = 'MenuElement-'+MenuBars.ID;
			var menubarelement = document.createElement( 'div' );
			menubarelement.className = 'menubarelement';
			menubarelement.onmousemove = (function(ID){ return function(){ MenuHover(ID); }; })(id);
			menubarelement.onclick = (function(ID){ return function(){ ShowMenu(ID); }; })(id);
			menubarelement.innerHTML = value.replace("_", " ");
			
			var menubarmenu = document.createElement( 'div' );
			menubarmenu.className = 'menubarmenu';
			menubarmenu.id = 'MenuElement-'+MenuBars.ID;
			
			for( var valueinner in table[value] ){
				var menubarmenuelement = document.createElement( 'div' );
				menubarmenuelement.className = 'menubarmenuelement';
				var a = document.createElement( 'a' );
				a.className = "menubar";
				a.innerHTML = valueinner.replace("_", " ");
				a.onclick = table[value][valueinner];
				menubarmenuelement.appendChild( a );
				menubarmenu.appendChild( menubarmenuelement );
			}
			menubarelement.appendChild(menubarmenu);
			MenuBars.ID++;
			menubar.appendChild(menubarelement);
		}
		return menubar;
	}
	return null;
}