import hxSerial.Serial;

class Test extends haxe.unit.TestCase{
	public function test():Void {
		var devs = Serial.getDeviceList();
		assertTrue(devs.length >= 0);
	}
	
	static public function main():Void {
		// var s = new Serial("/dev/tty.usbserial-A4001tkb",true);
		// //this.assertTrue(s.isSetup);
		// Sys.sleep(2);
		
		// for (i in 0...2){
		// 	this.assertTrue(s.writeByte(65));
		// 	Sys.sleep(1.5);
		// 	this.assertEquals(2,s.available());
		// 	this.assertEquals(66,s.readByte());
		// 	this.assertEquals(67,s.readByte());
		// }
		
		// for (i in 0...2){
		// 	this.assertEquals(1,s.writeBytes("A"));
		// 	Sys.sleep(1.5);
		// 	this.assertEquals(2,s.available());
		// 	this.assertEquals("BC",s.readBytes(2));
		// }

		// s.close();

		var runner = new haxe.unit.TestRunner();
		runner.add(new Test());
		var success = runner.run();
		Sys.exit(success ? 0 : 1);
	}
}
