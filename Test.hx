import hxSerial.Serial;

import haxe.io.BytesOutput;

class Test extends haxe.unit.TestCase{
	public function test():Void {
		trace("start testing...");
		trace(Serial.getDeviceList());
		
		var s = new Serial("COM7",true);
		this.assertTrue(s.isSetup);
		cpp.Sys.sleep(2);
		
		for (i in 0...2){
			this.assertTrue(s.writeByte(65));
			cpp.Sys.sleep(1.5);
			this.assertEquals(2,s.available());
			this.assertEquals(66,s.readByte());
			this.assertEquals(67,s.readByte());
		}
		
		for (i in 0...2){
			this.assertEquals(1,s.writeBytes("A"));
			cpp.Sys.sleep(1.5);
			this.assertEquals(2,s.available());
			this.assertEquals("BC",s.readBytes(2));
		}

		s.close();
	}

	
	static public function main():Void {
		var runner = new haxe.unit.TestRunner();
		runner.add(new Test());
		runner.run();
	}
}
