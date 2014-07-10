var Window = {
	Moving: false,
	Target: null,
	Offset: {
		X: 0,
		Y: 0
	},
	LastZ: 1
};

var Menus = {
	Current: null,
	Preserve: false
};

var Mouse = {
	X: 0,
	Y: 0
};

function WindowOnClose( e ){
	var win = e.target.parentNode.parentNode;
	win.parentNode.removeChild(win);
}

function ToggleMaximize( win ){

	win.style.zIndex = Window.LastZ++;
	if( win.style.width != "100%" ){
		var rect = win.getBoundingClientRect();
		var rectcontainer = win.parentNode.getBoundingClientRect();
		
		win.style.width = "100%";
		win.style.height = "100%";
		
		win.style.marginLeft = ( rectcontainer.left - rect.left ) + 'px';
		win.style.marginTop = ( rectcontainer.top - rect.top ) + 'px';
		
	}
	else{
		win.style.width = "0";
		win.style.height = "auto";
		win.style.marginLeft = "0";
		win.style.marginTop = "0";
		
	}
}

function WindowOnMaximize( e ){
	var win = e.target.parentNode.parentNode;
	ToggleMaximize( win );
}

function WindowOnDoubleClick( e ){
	if( e.target.className == "windowtitle" ){
		ToggleMaximize(e.target.parentNode);
	}
}

function WindowOnMouseDown( e ){
	if( e.target.className == "windowtitle" ){
		Window.Target = e.target.parentNode;
		Window.Moving = true;
		Window.Target.style.zIndex = Window.LastZ++;
		var rect = Window.Target.getBoundingClientRect();
		Window.Offset.X = Mouse.X - rect.left;
		Window.Offset.Y = Mouse.Y - rect.top;
		
		
	}
}

function HTMLOnMouseUp( e ){
	if( Window.Target != null ){
		var rect = Window.Target.parentNode.getBoundingClientRect();

		if( Mouse.Y <= 20 )
			Window.Target.style.top = (22 - Window.Offset.Y - rect.top) + 'px';
		if( Mouse.X <= 226 )
			Window.Target.style.left = (228 - Window.Offset.X - rect.left) + 'px';
		Window.Target = null;
	}
	Window.Moving = false;
	
	
	if( Menus.Current != null && Menus.Preserve == false ){
		Menus.Current.style.visibility = "hidden";
		Menus.Current = null;
	}
	Menus.Preserve = false;
}

function HTMLOnMouseMove( e ){
	if( Window.Moving == true ){
		if( Window.Target.style.width == "100%" ){
			Window.Target.style.width = "0";
			Window.Target.style.height = "auto";
			Window.Target.style.marginLeft = "0";
			Window.Target.style.marginTop = "0";
			
			var rect = Window.Target.getBoundingClientRect();
			Window.Offset.X = (rect.right - rect.left ) / 2;
		}
		var rect = Window.Target.parentNode.getBoundingClientRect();
		
		Window.Target.style.left = (e.clientX - Window.Offset.X - rect.left) + 'px';
		Window.Target.style.top = (e.clientY - Window.Offset.Y - rect.top) + 'px';
	}
	Mouse.X = e.clientX;
	Mouse.Y = e.clientY;
}

function ShowMenu( id ){
	var submenu = document.getElementById( id );
	if( Menus.Current != submenu && Menus.Current != null ){
		Menus.Current.style.visibility = "hidden";
		Menus.Current = null;
	}
	
	if( submenu.style.visibility != "visible" ){
		submenu.style.visibility = "visible";
		Menus.Current = submenu;
		

	}
	else{
		submenu.style.visibility = "hidden";
		Menus.Current = null;
	}
	Menus.Preserve = true;
}

function MenuHover( id ){
	if( Menus.Current != null ){
		var submenu = document.getElementById( id );
		if( Menus.Current != submenu ){
			ShowMenu( id );
		}
	}
}

function MenuMouseLeave( e ){
	Menus.Preserve = false;
}
function MenuMouseEnter( e ){
	Menus.Preserve = true;
}


function init(){
	var h=document.getElementsByClassName("html");
	for( i = 0; i < h.length; ++i ){
		h[i].onmousemove=HTMLOnMouseMove;
		h[i].onmouseup=HTMLOnMouseUp;
	}
	h=document.getElementsByClassName("window");
	for( i = 0; i < h.length; ++i ){
		h[i].onmousedown=WindowOnMouseDown;
		h[i].ondblclick=WindowOnDoubleClick;
		
	}
	h=document.getElementsByClassName("close");
	for( i = 0; i < h.length; ++i ){
		h[i].onclick=WindowOnClose;
	}
	h=document.getElementsByClassName("maximize");
	for( i = 0; i < h.length; ++i ){
		h[i].onclick=WindowOnMaximize;
	}
	h=document.getElementsByClassName("menubarelement");
	for( i = 0; i < h.length; ++i ){
		h[i].onmouseleave=MenuMouseLeave;
		h[i].onmouseenter=MenuMouseEnter;
		
	}
	
	
}

