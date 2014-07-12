var Window = {
	Moving: false,
	Resizing: false,
	Target: null,
	Offset: {
		X: 0,
		Y: 0
	},
	LastZ: 1,
	Size: {
		W: 0,
		H: 0,
		Start: {
			X: 0,
			Y: 0
		}
	}
};

var Menus = {
	Current: null,
	Preserve: false
};

var Mouse = {
	X: 0,
	Y: 0
};

function ResizeStart( e ){
	
	var win = GetParentWindow( e.target );
	Window.Target = win;
	Window.Resizing = true;
	var rect = Window.Target.getBoundingClientRect();
	Window.Size.W = (rect.right - rect.left-2);
	Window.Size.H = (rect.bottom - rect.top-2);
	Window.Size.Start.X = e.clientX;
	Window.Size.Start.Y = e.clientY;
	
	
}

function WindowOnClose( e ){
	var win = GetParentWindow( e.target )
	win.parentNode.removeChild(win);
}

function ToggleMaximize( win ){

	win.style.zIndex = Window.LastZ++;
	if( win.style.width != "100%" ){
		var rect = win.getBoundingClientRect();
		var rectcontainer = GetContainer().getBoundingClientRect();
		
		win.style.width = "100%";
		win.style.height = "100%";
		
		win.style.marginLeft = ( rectcontainer.left - rect.left ) + 'px';
		win.style.marginTop = ( rectcontainer.top - rect.top ) + 'px';
		
		var resizer = win.getElementsByClassName("windowresizer");
		for( var i = 0; i < resizer.length; ++i ){
			resizer[i].style.visibility = "hidden";
		}
		
	}
	else{
		win.style.width = "0";
		win.style.height = "0";
		win.style.marginLeft = "0";
		win.style.marginTop = "0";
		var resizer = win.getElementsByClassName("windowresizer");
		for( var i = 0; i < resizer.length; ++i ){
			resizer[i].style.visibility = "visible";
		}
	}
}

function WindowOnMaximize( e ){
	var win = GetParentWindow( e.target );
	ToggleMaximize( win );
}

function WindowOnDoubleClick( e ){
	if( e.target.className == "windowtitle" || e.target.className == "windowtitlebartitle" ){
		ToggleMaximize(GetParentWindow( e.target ));
	}
}

function WindowBringToTop( win ){
	win.style.zIndex = Window.LastZ++;
}

function WindowOnMouseDown( e ){
	WindowBringToTop( GetParentWindow( e.target ) );
	if( e.target.className == "windowtitle" || e.target.className == "windowtitlebartitle"){
		Window.Target = GetParentWindow( e.target );
		Window.Moving = true;
		var rect = Window.Target.getBoundingClientRect();
		Window.Offset.X = Mouse.X - rect.left;
		Window.Offset.Y = Mouse.Y - rect.top;
		
		
	}
	if( Menus.Current != null && Menus.Preserve == false ){
		Menus.Current.style.visibility = "hidden";
		Menus.Current = null;
	}
	Menus.Preserve = false;
}

function HTMLOnMouseUp( e ){
	if( Window.Target != null && Window.Moving == true && Window.Target.style.width != "100%"){
		var rect = GetContainer().getBoundingClientRect();
		var winrect = Window.Target.getBoundingClientRect();

		if( Mouse.Y <= 20 )
			Window.Target.style.top = (22 - Window.Offset.Y - rect.top) + 'px';
		if( Mouse.X <= 226 )
			Window.Target.style.left = (228 - Window.Offset.X - rect.left) + 'px';
		
		if( winrect.left - rect.left < 10 && winrect.left - rect.left > -20 )
			Window.Target.style.left = '0';
		if( winrect.right > rect.right - 10 && winrect.right < rect.right + 20  )
			Window.Target.style.left = ((rect.right-rect.left) - (winrect.right-winrect.left)) + 'px';
		if( winrect.top - rect.top < 10 && winrect.top - rect.top > -20 )
			Window.Target.style.top = '0';
		if( winrect.bottom > rect.bottom - 10 && winrect.bottom < rect.bottom + 20  )
			Window.Target.style.top = ((rect.bottom-rect.top) - (winrect.bottom-winrect.top)) + 'px';
		
		var others = document.getElementsByClassName("window");
		for( var i = 0; i < others.length; ++i ){
			var win = others[i];
			if( win == null ) continue;
			if( win == Window.Target )
				continue;
			var otherrect = win.getBoundingClientRect();
			if( winrect.bottom > otherrect.top && winrect.top < otherrect.bottom ){
				if( winrect.left < otherrect.right + 10 && winrect.left > otherrect.right - 20 )
					Window.Target.style.left = (otherrect.right - rect.left - 1 ) + 'px';
				if( winrect.right > otherrect.left - 10 && winrect.right < otherrect.left + 20 )
					Window.Target.style.left = (otherrect.left - (winrect.right - winrect.left) - rect.left + 1 ) + 'px';
			}
			if( winrect.left < otherrect.right && winrect.right > otherrect.left ){
				if( winrect.top < otherrect.bottom + 10 && winrect.top > otherrect.bottom - 20 )
					Window.Target.style.top = (otherrect.bottom - rect.top - 1 ) + 'px';
				if( winrect.bottom > otherrect.top - 10 && winrect.bottom < otherrect.top + 20 )
					Window.Target.style.top = (otherrect.top - (winrect.bottom - winrect.top) - rect.top + 1 ) + 'px';
			}
			
		}
		
		Window.Target = null;
		
	}
	if( Window.Target != null && Window.Resizing == true ){
		var rect = GetContainer().getBoundingClientRect();
		var winrect = Window.Target.getBoundingClientRect();
		
		if( winrect.right > rect.right - 10 && winrect.right < rect.right + 20  )
			Window.Target.style.minWidth = (rect.right-winrect.left-2) + 'px';
		if( winrect.bottom > rect.bottom - 10 && winrect.bottom < rect.bottom + 20  )
			Window.Target.style.minHeight = ((rect.bottom-winrect.top-2)) + 'px';
		
		var others = document.getElementsByClassName("window");
		for( var i = 0; i < others.length; ++i ){
			var win = others[i];
			if( win == null ) continue;
			if( win == Window.Target )
				continue;
			var otherrect = win.getBoundingClientRect();
			if( winrect.bottom > otherrect.top && winrect.top < otherrect.bottom ){
				if( winrect.right > otherrect.left - 10 && winrect.right < otherrect.left + 20 )
					Window.Target.style.minWidth = (otherrect.left - winrect.left - 1) + 'px';
			}
			if( winrect.left < otherrect.right && winrect.right > otherrect.left ){
				if( winrect.bottom > otherrect.top - 10 && winrect.bottom < otherrect.top + 20 )
					Window.Target.style.minHeight = (otherrect.top - winrect.top - 1) + 'px';
			}
			
		}
		
		Window.Target = null;
	}
	Window.Moving = false;
	Window.Resizing = false;
	
	

}

function HTMLOnMouseMove( e ){
	if( Window.Moving == true ){
		if( Window.Target.style.width == "100%" ){
			Window.Target.style.width = "0";
			Window.Target.style.height = "0";
			Window.Target.style.marginLeft = "0";
			Window.Target.style.marginTop = "0";
			var resizer = Window.Target.getElementsByClassName("windowresizer");
			for( var i = 0; i < resizer.length; ++i ){
				resizer[i].style.visibility = "visible";
			}
			
			var rect = Window.Target.getBoundingClientRect();
			Window.Offset.X = (rect.right - rect.left ) / 2;
		}
		var rect = GetContainer().getBoundingClientRect();
		
		Window.Target.style.left = (e.clientX - Window.Offset.X - rect.left) + 'px';
		Window.Target.style.top = (e.clientY - Window.Offset.Y - rect.top) + 'px';
	}
	else if( Window.Resizing == true ){
		var w = Window.Size.W + e.clientX - Window.Size.Start.X;
		var h = Window.Size.H + e.clientY - Window.Size.Start.Y;
			
		var minWidth = WindowGetMinWidth( Window.Target );
		if( w < minWidth ) w = minWidth;
		if( h < 100 ) h = 100;
		
		Window.Target.style.minWidth = (w) + 'px';
		Window.Target.style.minHeight = (h) + 'px';
		Window.Target.style.width = '0';
		Window.Target.style.height = '0';
		
		
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
	if( submenu != null ){
		if( submenu.style.visibility != "visible" ){
			submenu.style.visibility = "visible";
			Menus.Current = submenu;
			

		}
		else{
			submenu.style.visibility = "hidden";
			Menus.Current = null;
		}
	}
	Menus.Preserve = true;
}

function MenuHover( id ){
	Menus.Preserve = true;
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
function GetParentWindow( self ){
	while( self != null && self.className != "window" ){
		self = self.parentNode;
	}
	return self;
}

function GetContainer(){
	return document.getElementById("TheContainer");
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
	h=document.getElementsByClassName("windowresizer");
	for( i = 0; i < h.length; ++i ){
		h[i].onmousedown = ResizeStart;
		
	}
	h = document.getElementById("NewThings");
	h.onclick = function(){MakeWindowFromContent('windows/testa.html');}
}

