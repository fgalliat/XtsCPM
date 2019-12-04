
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.Reader;
import java.util.Properties;

import jssc.SerialPort;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public class Terminal {

	protected static SerialPort serialPort;
	protected static Terminal instance;

	protected boolean LISTENER_LOCKED = false;

	public boolean isListenerLocked() {
		return LISTENER_LOCKED;
	}

	protected Terminal() {
		try {
			File propsF = new File("./config.props");
			if ( propsF.exists() ) {
				Properties props = new Properties();
				Reader reader = new FileReader(propsF);
				System.out.println("Reading props from "+propsF.getPath());
				props.load(reader);
				reader.close();

				if ( props.containsKey("comm.port") ) {
					commPort = props.getProperty("comm.port");
					System.out.println("Will use port : "+commPort);
				}

			} else {
				System.out.println("Skip reading props from "+propsF.getPath());
			}
		} catch(Exception ex) {
			ex.printStackTrace();
		}
	}


	public static synchronized Terminal getInstance() {
		if (instance == null) {
			instance = new Terminal();
		}
		return instance;
	}

	public static void main(String[] args) throws Exception {
		new Terminal().doWork(args);
	}

	// =====================================

	// protected String commPort = "COM16";
	protected String commPort = "/dev/ttyACM0";

	public String getCommPort() {
		return commPort;
	}

	public void setCommPort(String port) {
		this.commPort = port;
	}

	public boolean reconnect() {
		return reconnect(false);
	}

	public boolean reconnect(boolean takeHand) {
		try {
			serialPort.closePort();
		} catch (Exception ex) {
		}
		try {
			// serialPort = new SerialPort("/dev/ttyUSB0");
			serialPort = new SerialPort(getCommPort());
			System.out.println("opening " + serialPort.getPortName());
			serialPort.openPort();// Open serial port

			System.out.println("setting");
			serialPort.setParams(SerialPort.BAUDRATE_115200, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);
//			serialPort.setParams(SerialPort.BAUDRATE_9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
//					SerialPort.PARITY_NONE);
			serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_NONE);
		} catch (Exception ex) {
			ex.printStackTrace(); // TODO: better
			return false;
		}

		if (takeHand) {
			try {
				return takeHand();
			} catch (Exception ex) {
				ex.printStackTrace(); // TODO: better
				return false;
			}
		}

		return true;
	}

	public void doWork(String[] args) throws Exception {
		reconnect();

		// if ( args == null || args.length < 1 ) {
		// throw new Exception("missing file/dir name");
		// }

		// String entName = args[0];
		// File entry = new File(entName);

		try {
			// if ( entry.isDirectory() ) {
			// File[] content = entry.listFiles();
			// for(File ent : content) {
			// if ( ent.isDirectory() ) { continue; }
			// doUpload( ent.getAbsolutePath() );
			// Zzz(500);
			// }
			// } else {
			// doUpload( entry.getAbsolutePath() );
			// }

			String arg1 = (args != null && args.length > 0) ? args[0] : null;

			takeHand();
			sendCommand("print \"Hello world\"; ");

			if (arg1 != null) {
				if (arg1.equalsIgnoreCase("reboot")) {
					sendCommand("boot; ");
				} else if (arg1.equalsIgnoreCase("demo")) {
					sendCommand("girl; runprg(\"loop\")");
				} else {
					sendCommand(arg1);
				}
			}

		} catch (Exception ex) {
			throw ex;
		} finally {
			try {
				serialPort.closePort();
			} catch (Exception ex2) {
			}
		}
	}

	// 0===================================
	protected boolean takeHand() throws Exception {
		serWrite(0x04); // hangup keyboard
		Zzz(300);
		flushRX();
		return true;
	}

	protected boolean reboot() {
		try {
			return sendCommand("boot; ");
		} catch (Exception ex) {
			ex.printStackTrace();
			return false;
		}
	}

	protected boolean serial() {
		try {
			boolean ok = sendCommand("console(0,0); ");
			flushRX();
			return ok;
		} catch (Exception ex) {
			ex.printStackTrace();
			return false;
		}
	}

	protected boolean cat() {
		try {
			return sendCommand("cat; ");
		} catch (Exception ex) {
			ex.printStackTrace();
			return false;
		}
	}

	protected boolean sendCommand(String cmd) throws Exception {
		serPrintln(cmd);
		Zzz(300);
		return true;
	}

	// ============

	public boolean addPortListener(SerialPortEventListener listener) {
		try {
			serialPort.addEventListener(listener);
		} catch (Exception ex) {
			ex.printStackTrace();
			return false;
		}
		return true;
	}

	String readString(int howMany) throws SerialPortException {
		return serialPort.readString(howMany);
	}

	// 0===================================

//	void doUpload(String file) throws Exception {
//		serialPort.purgePort(serialPort.PURGE_RXCLEAR | serialPort.PURGE_TXCLEAR);
//
//		// final int PACKET_LENGTH = 64;
//		final int PACKET_LENGTH = 32;
//
//		try {
//			serialPort.readBytes(1, 1000);
//		} catch (Exception ex) {
//		}
//
//		System.out.println("SENT SIG UPLOAD");
//
//		if (false) {
//			// upload via bridge SIGNAL
//			serWrite(0x04);
//		} else {
//			// upload via SERIAL SIGNAL
//			serWrite(0x06);
//		}
//		Zzz(200);
//
//		// wait for 0xFF -- ACK
//		System.out.println("WAIT ACK");
//		int ok = serialPort.readBytes(1)[0];
//
//		File f = new File(file);
//		String newFentry = "/" + f.getName().toUpperCase();
//
//		serPrintln(newFentry);
//		Zzz(200);
//		serPrintln("" + f.length());
//		Zzz(500);
//
//		FileInputStream fis = new FileInputStream(f);
//		byte[] buffer = new byte[PACKET_LENGTH];
//
//		// for(int i=0; i < f.length(); i+= PACKET_LENGTH) {
//		int total = 0;
//		while (true) {
//			int readed = fis.read(buffer);
//			serWrite(buffer, readed);
//			// for(int i=0; i< readed; i++) {
//			// serWrite(buffer[i]);
//			// Zzz(2);
//			// }
//			total += readed;
//
//			System.out.println("Waiting HandShake (" + total + " / " + f.length() + ") => " + f.getName());
//			int ACK = serialPort.readBytes(1)[0]; // waits for 0xFE
//
//			// volountary after HandShake read
//			if (total >= f.length()) {
//				break;
//			}
//
//			// Zzz(15);
//		}
//		fis.close();
//		// Zzz(200);
//		System.out.println("Waiting -EOT-");
//		int ACK = serialPort.readBytes(1)[0]; // waits for 0xFF
//
//		System.out.println("-EOF-");
//	}

	boolean silent = false;
	public void setSilent(boolean silent) {
		this.silent = silent;
	}

	void serPrint(String str) throws Exception {
		if (!silent) System.out.println(">>> " + str);
		serWrite(str.getBytes(), str.length());
	}

	void serPrintln(String str) throws Exception {
		String str2 = str + "\n";
		serPrint(str2);
	}

	void serWrite(byte[] arry, int bteCount) throws Exception {
		// System.out.println(">>> BIN CONTENT ("+ bteCount +" bytes)");

		if (bteCount > arry.length) {
			bteCount = arry.length;
		}

		byte[] used = arry;
		if (arry.length != bteCount) {
			used = new byte[bteCount];
			System.arraycopy(arry, 0, used, 0, bteCount);
		}

		serialPort.writeBytes(used);
//		for (int i = 0; i < bteCount; i++) {
//			serialPort.writeByte(used[i]);
//			// Zzz(2);
//		}
		

//		Zzz(5);
		Zzz(1);
		// Zzz(10);
		
		// serialPort.purgePort( serialPort.PURGE_TXCLEAR );
		// serialPort.purgePort( serialPort.PURGE_RXCLEAR | serialPort.PURGE_TXCLEAR );
	}

	void serWrite(int bte) throws Exception {
		// System.out.println(">>> BIN CONTENT ("+ 1 +" byte)");

		serialPort.writeByte((byte) bte);
	}

	static void Zzz(long millis) {
		try {
			Thread.sleep(millis);
		} catch (Exception ex) {
		}
	}

	protected char readSerChar(int timeout) throws Exception {
		LISTENER_LOCKED = true;
		int ch0 = -1;
		try {
			ch0 = serialPort.readBytes(1, timeout)[0];
		} catch (Exception ex) {
		}
		char ch = (ch0 < 0) ? (char) (256 + ch0) : (char) ch0;
		LISTENER_LOCKED = false;
		return ch;
	}

	protected String readSerLine(int timeout, boolean consumeBackRtoo) throws Exception {
		String line = null;
		LISTENER_LOCKED = true;
		int ch0 = -1;
		try {
			ch0 = serialPort.readBytes(1, timeout)[0];
			if (ch0 == '\n' || ch0 == '\r') {
				if (ch0 == '\r' && consumeBackRtoo) {
					ch0 = serialPort.readBytes(1, timeout)[0];
				}
				LISTENER_LOCKED = false;
				return "";
			}
			// line = "" + (char) ch0;
			line = "";
			while (true) {
				char ch = (ch0 < 0) ? (char) (256 + ch0) : (char) ch0;
				line += ch;

				// TODO : better
				ch0 = serialPort.readBytes(1, timeout)[0];
				if (ch0 == '\n' || ch0 == '\r') {
					if (ch0 == '\r' && consumeBackRtoo) {
						ch0 = serialPort.readBytes(1, timeout)[0];
					}
					break;
				}
			}

		} catch (Exception ex) {
			LISTENER_LOCKED = false;
			return null;
		}
		LISTENER_LOCKED = false;
		return line;
	}

	public boolean sendFile(String src, String dstName, boolean overwrite) {
		LISTENER_LOCKED = true;
		File f = new File(src);
		if (!f.exists()) {
			System.err.println("File not found >>\" + src + \"<<");
			return false;
		}
		System.out.println("Will try to send >>" + src + "<< as <<" + dstName + ">>");
		try {
			int flen = (int) f.length();

			LISTENER_LOCKED = true;
			serPrintln("write( \"" + dstName + "\", " + (flen) + " )");
//			readString(1024); // read cmd echo

			String echo = readSerLine(1000, true);
			System.err.println("readed echo " + echo);

			LISTENER_LOCKED = true;
			// char ch = readSerChar(1000);
			String said = readSerLine(1000, true);
			char ch = said.charAt(0);
			System.err.println("readed ch >" + ch + "<");
			if (ch == '-') {
//				String said = readSerLine(1000, true);
				System.err.println("SAID>> " + said);
				if (overwrite) {
					serPrintln("y");
				} else {
					serPrintln("n");
					return false;
				}

				LISTENER_LOCKED = true;
				// ch = readSerChar(1000);
				ch = readSerLine(1000, true).charAt(0);
			}
			System.err.println("readed ch >" + ch + "<");

			LISTENER_LOCKED = true;
			if (ch != '+') {
				LISTENER_LOCKED = false;
				System.err.println("Oups '" + ch + "'");
				return false;
			}
			System.err.println("YATL waits for datas");

			FileInputStream fis = new FileInputStream(f);

//			final int blen = 32;
//			final int blen = 128;
			final int blen = 512;
			byte[] bloc = new byte[blen];

			LISTENER_LOCKED = false;
			for (int i = 0; i < flen; i += blen) {
				// TODO : read
				int readed = fis.read(bloc, 0, bloc.length);
				serWrite(bloc, readed);
				if (false) {
					System.err.print("+");
					if ((i / blen) % 20 == 19) {
						System.err.println();
					}
				} else {
					if ((i / blen) % 20 == 19) {
						System.err.print("+");
					}
				}
			}
			fis.close();
			System.err.println();
			
			flushRX();

		} catch (Exception e) {
			e.printStackTrace();
			LISTENER_LOCKED = false;
			return false;
		}
		LISTENER_LOCKED = false;
		return true;
	}

	public void flushRX() throws Exception {
		String  said;
		while( (said = readSerLine(1000, true) ) != null) {
			System.err.println("> "+said);
		}
	}
	
}