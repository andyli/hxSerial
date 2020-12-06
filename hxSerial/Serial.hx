package hxSerial;

#if neko
import neko.Lib;
import neko.vm.Loader;
#elseif cpp
import cpp.Lib;
#end

class Serial {
	static public function getDeviceList():Array<String> {
		init();
		var str:String = _enumerateDevices();
		return str == null ? [] : str.split('\n');
	}

	public static function init()
	{
		if (!isInit)
		{
			isInit = true;
            
            #if neko 
            
            function tryLoadNeko() {
                var initNeko = Lib.loadLazy("hxSerial", "neko_init", 5);
                if (initNeko != null) 
                    initNeko(
                        function(s) return new String(s),
                        function(len:Int) { var r = []; if (len > 0) r[len - 1] = null; return r; },
                        null, true, false
                    );
                else
                    trace("Could not init neko api for hxSerial");
            }
            
            try {
                tryLoadNeko();    
            } catch (e:Dynamic) {
                // we've tried to load neko ndll, it didnt work, less assume its because 
                // it was 32bit and we want 64bit, lets add the 64bit path to loader (based
                // on current loader path) and try again, if it works, we'll load all the 
                // functions, if it doesnt, something else went wrong and the initial failure
                // was probably valid
                for (p in Loader.local().getPath()) {
                    p = haxe.io.Path.normalize(p);
                    if (p.indexOf("/" + Sys.systemName()) != -1) {
                        var parts = p.split("/");
                        parts.pop();
                        parts.push(Sys.systemName() + "64");
                        Loader.local().addPath(parts.join("/") + "/");
                    }
                }
                tryLoadNeko();
            }
            
            // now we should have the correct .ndll
            _enumerateDevices = Lib.load("hxSerial","enumerateDevices",0);
            _setup = Lib.load("hxSerial","setup",2);
            _writeBytes = Lib.load("hxSerial","writeBytes",3);
            _readBytes = Lib.load("hxSerial","readBytes",2);
            _writeByte = Lib.load("hxSerial","writeByte",2);
            _readByte = Lib.load("hxSerial","readByte",1);
            _flush = Lib.load("hxSerial","flush",3);
            _available = Lib.load("hxSerial","available",1);
            _breakdown = Lib.load("hxSerial","breakdown",1);
            
            #end
		}
	}

	public var portName(default,null):String;
	public var baud(default,null):Int;
	public var isSetup(default,null):Bool;
	
	public function new(portName:String, ?baud:Int = 9600, ?setupImmediately:Bool = false){
		isSetup = false;
		init();
		
		this.portName = portName;
		this.baud = baud;
		
		if (setupImmediately) {
			setup();
		}
	}

	public function setup():Bool {
		this.handle = _setup(portName,baud);
		isSetup = handle != null && handle > 0;
		return isSetup;
	}

	public function writeBytes(buffer:String):Int {
		return _writeBytes(handle,buffer,buffer.length);
	}

	public function readBytes(length:Int):String {
		#if neko
		return neko.NativeString.toString(_readBytes(handle,length));
		#else
		return _readBytes(handle,length);
		#end
	}

	public function writeByte(byte:Int):Bool {
		return _writeByte(handle,byte);
	}

	public function readByte():Int {
		return _readByte(handle);
	}

	public function flush(?flushIn:Bool = false, ?flushOut = false):Void {
		_flush(handle,flushIn,flushOut);
	}

	public function available():Int {
		return _available(handle);
	}

	public function close():Int {
		isSetup = false;
		return _breakdown(handle);
	}
	
	private var handle:Null<Int>;
	private static var isInit = #if neko false #else true #end ;

    #if neko // load neko differently now! 
    
	private static var _enumerateDevices = null;
	private static var _setup = null;
	private static var _writeBytes = null;
	private static var _readBytes = null;
	private static var _writeByte = null;
	private static var _readByte = null;
	private static var _flush = null;
	private static var _available = null;
	private static var _breakdown = null;
    
    #else
    
	private static var _enumerateDevices = Lib.load("hxSerial","enumerateDevices",0);
	private static var _setup = Lib.load("hxSerial","setup",2);
	private static var _writeBytes = Lib.load("hxSerial","writeBytes",3);
	private static var _readBytes = Lib.load("hxSerial","readBytes",2);
	private static var _writeByte = Lib.load("hxSerial","writeByte",2);
	private static var _readByte = Lib.load("hxSerial","readByte",1);
	private static var _flush = Lib.load("hxSerial","flush",3);
	private static var _available = Lib.load("hxSerial","available",1);
	private static var _breakdown = Lib.load("hxSerial", "breakdown", 1);
    
    #end
}
