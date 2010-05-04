package hxSerial;

class Serial {
	static public function getDeviceList():Array<String> {
		return cast(_enumerateDevices(),String).split('\n');
	}

	public var portName(default,null):String;
	public var baud(default,null):Int;
	public var isSetup(default,null):Bool;
	
	public function new(portName:String, ?baud:Int = 9600, ?setupImmediately:Bool = false){
		isSetup = false;

		this.portName = portName;
		this.baud = baud;
		
		if (setupImmediately) {
			setup();
		}
	}

	public function setup():Bool {
		this.handle = _setup(portName,baud);
		isSetup = handle != null && handle != 0;
		return isSetup;
	}

	public function writeBytes(buffer:String):Int {
		return _writeBytes(handle,buffer,buffer.length);
	}

	public function readBytes(length:Int):String {
		return _readBytes(handle,length);
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
	
	private var handle:Int;

	private static var _enumerateDevices = cpp.Lib.load("hxSerial","enumerateDevices",0);
	private static var _setup = cpp.Lib.load("hxSerial","setup",2);
	private static var _writeBytes = cpp.Lib.load("hxSerial","writeBytes",3);
	private static var _readBytes = cpp.Lib.load("hxSerial","readBytes",2);
	private static var _writeByte = cpp.Lib.load("hxSerial","writeByte",2);
	private static var _readByte = cpp.Lib.load("hxSerial","readByte",1);
	private static var _flush = cpp.Lib.load("hxSerial","flush",3);
	private static var _available = cpp.Lib.load("hxSerial","available",1);
	private static var _breakdown = cpp.Lib.load("hxSerial","breakdown",1);
}
