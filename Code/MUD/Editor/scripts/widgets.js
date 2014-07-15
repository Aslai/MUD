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
	var titlebartitle = document.createElement( 'div' );
	titlebartitle.className = 'windowtitlebartitle';
	titlebartitle.innerHTML = title;
	
	var closebutton = document.createElement( 'div' );
	closebutton.className = 'close';
	closebutton.innerHTML = 'X';
	closebutton.onclick=WindowOnClose;
	
	var maxbutton = document.createElement( 'div' );
	maxbutton.className = 'maximize';
	maxbutton.innerHTML = '&#9633;';
	maxbutton.onclick=WindowOnMaximize;
	var buttons = document.createElement( 'div' );
	buttons.className = 'windowtitlebarbuttons';
	
	
	var inner = document.createElement( 'div' );
	inner.className = 'windowinner';
	
	
	var winresizer = document.createElement( 'div' );
	winresizer.className = 'windowresizer';
	winresizer.onmousedown = ResizeStart;
	
	titlebar.appendChild( titlebartitle );
	buttons.appendChild( closebutton );
	buttons.appendChild( maxbutton );
	titlebar.appendChild( buttons );
	win.appendChild( titlebar );
	
	/*if( menu != null ){
		inner.style.top = '44px';
		win.appendChild( menu );
	}*/
	
	win.appendChild( inner );
	win.appendChild( winresizer );
	
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
		var menubarelementcontainer = document.createElement( 'div' );
		menubarelementcontainer.className = "menubarelementcontainer";
		
		for( var value in table ){
			var id = 'MenuElement-'+MenuBars.ID;
			var menubarelement = document.createElement( 'div' );
			menubarelement.className = 'menubarelement';
			menubarelement.onmousemove = (function(ID){ return function(){ MenuHover(ID); }; })(id);
			menubarelement.onclick = (function(ID){ return function(){ 
																		var win = GetParentWindow( this );
																		if( win != null ){
																			WindowBringToTop( win );
																		}
																		ShowMenu(ID); }; })(id);
			menubarelement.innerHTML = value.replace("_", " ");
			menubarelement.onmouseleave=MenuMouseLeave;
			menubarelement.onmouseenter=MenuMouseEnter;
			
			var menubarmenu = document.createElement( 'div' );
			menubarmenu.className = 'menubarmenu';
			menubarmenu.id = id;
			
			
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
			menubarelementcontainer.appendChild( menubarelement );
		}
		menubar.appendChild(menubarelementcontainer);
		return menubar;
	}
	return null;
}

function RequestURL( url, target, argumentName, argument, onComplete ){
	var request = new XMLHttpRequest();
	request.open('get', url);
	request.onreadystatechange = function(){
		if (request.readyState == 4)
		{
			var response = request.responseText;
			target.innerHTML="";
			/*var frame = document.createElement( 'iframe' );
			target.appendChild( frame );
			target = frame;*/
			var start = 0;
			var end = 0;
			do{
				var end = response.indexOf("<script>", start);
				if( end == -1 ){
					target.innerHTML += response.substr(start);
					break;
				}
				target.innerHTML += response.substring(start, end);
				start = end + 8;
				end = response.indexOf("</script>", start);
				if( end == -1 ){
					eval("(function("+ argumentName +"){" + response.substr(start) + "})")(argument);
					break;
				}
				eval("(function("+ argumentName +"){" + response.substring(start, end) + "})")(argument);
				start = end + 9;
				
			} while(true);
			onComplete();
		}
	};
	request.send(null);
}

function WindowAddMenuBar( win, menubar ){
	win.appendChild( menubar );
	var inners = win.getElementsByClassName("windowinner");
	var inner = inners[0];
	if( inner == null )
		return;
	inner.style.top = "44px";
	
	var minWidth = WindowGetMinWidth( win );
	var rect = win.getBoundingClientRect();
	if( rect.right - rect.left < minWidth ){
		win.style.minWidth = minWidth + 'px';
	}
}

function WindowSetTitle( win, title ){
	var titles = win.getElementsByClassName("windowtitlebartitle");
	var titlebar = titles[0];
	if( titlebar == null )
		return;
	titlebar.innerHTML = title;
}

function WindowGetMinWidth( win ){
	var container = win.getElementsByClassName("menubarelementcontainer");
	container = container[0];
	var minWidth = 150;
	if( container != null ){
		var rect = container.getBoundingClientRect();
		minWidth = rect.right - rect.left + 3;
	}
	return minWidth;
}

function WindowSetDimensions( win, wid, hig ){		
	minWidth = WindowGetMinWidth( win );
	if( wid < minWidth ) wid = minWidth;
	if( hig < 100 ) hig = 100;
	win.style.minWidth = wid + 'px';
	win.style.minHeight = hig + 'px';
	win.style.height = "0";
	win.style.width = "0";
}

function WindowVerifyDimensions( win ){		
	minWidth = WindowGetMinWidth( win );
	var rect = win.getBoundingClientRect();
	var wid = rect.right - rect.left - 2;
	var hig = rect.bottom - rect.top - 2;
	
	if( wid < minWidth ) wid = minWidth;
	if( hig < 100 ) hig = 100;
	win.style.minWidth = wid + 'px';
	win.style.minHeight = hig + 'px';
	win.style.height = "0";
	win.style.width = "0";
}

function FillWindowWithContent( win, url, onComplete ){
	var inners = win.getElementsByClassName("windowinner");
	var inner = inners[0];
	if( inner == null )
		return;
	inner.innerHTML = "Loading...";
	RequestURL( url, inner, "window", win, onComplete );
}

var StatusCount = 0;

function ChangeStatus( status ){
	document.getElementById("status").innerHTML = status;
	document.getElementById("status").style.visibility = "visible";
	StatusCount++;
	setTimeout(function() 	{
								StatusCount--; 
								if( StatusCount <= 0 ){
									StatusCount = 0;
									document.getElementById("status").style.visibility = "hidden";
								}
							},2000);
}

function MakeWindowFromContent( url ){
	var win = MakeWindow( "Loading content...", 300 );
	ChangeStatus("Loading "+url+" ...");
	
	
	FillWindowWithContent( win, url, function(){ 
										GetContainer().appendChild(win); 
										WindowVerifyDimensions(win);
										var h = win.getElementsByTagName("*");
										for( i = 0; i < h.length; ++i ){
											h[i].draggable = false;
											h[i].ondragstart = function() { return false; };
										}
										ChangeStatus("Finished loading "+url);
										
										} );
}