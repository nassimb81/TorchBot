package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.*;

public class WriteReadSerialPort implements Runnable {
    static SerialPort[] ports = SerialPort.getCommPorts();
    static SerialPort port = null;
    static Enumeration portList;
    InputStream inputStream;
    Thread readThread;

    static String messageString = "Hello, world!";
    static OutputStream outputStream;
    static boolean outputBufferEmptyFlag = false;

    public static void main(String[] args) {
        boolean portFound = false;
        String defaultPort;

        // determine the name of the serial port on several operating systems
        String osname = System.getProperty("os.name", "").toLowerCase();
        if (osname.startsWith("windows")) {
            // windows
            defaultPort = "COM4";
        } else if (osname.startsWith("linux")) {
            // linux
            defaultPort = "/dev/ttyS0";
        } else if (osname.startsWith("mac")) {
            // mac
            defaultPort = "????";
        } else {
            System.out.println("Sorry, your operating system is not supported");
            return;
        }

        if (args.length > 0) {
            defaultPort = args[0];
        }

        System.out.println("Set default port to " + defaultPort);

        for (SerialPort p : ports) {
            if (p.getSystemPortName().equals("/dev/COM3"))
                ; /*for your question, <required_port> would be ttyACM0, but this can change if you reconnect the device, or if multiple devices are connected.*/
            port = p;
        }

        port.setComPortParameters(9600, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :)");
            WriteReadSerialPort reader = new WriteReadSerialPort();
        } else {
            System.out.println("Failed to open port :(");
            return;
        }
    }

    public void initwritetoport() {
        // initwritetoport() assumes that the port has already been opened and
        //    initialized by "public WriteReadSerialPort()"

        try {
            // get the outputstream
            outputStream = port.getOutputStream();
        } catch (Exception e) {
            System.out.println("Exception getting output stream");
        }

        try {
            // activate the OUTPUT_BUFFER_EMPTY notifier
            port.getOutputStream();
        } catch (Exception e) {
            System.out.println("Error setting event notification");
            System.out.println(e.toString());
            System.exit(-1);
        }

    }

    public void writetoport() {
        System.out.println("Writing \"" + messageString + "\" to " + port.getPortDescription());
        try {
            // write string to serial port
            outputStream.write(messageString.getBytes());
        } catch (IOException e) {
        }
    }

    public WriteReadSerialPort() {
        try {
            inputStream = port.getInputStream();
        } catch (Exception e) {
        }

        // start the read thread
        readThread = new Thread(this);
        readThread.start();

    }

    public void run() {
        // first thing in the thread, we initialize the write operation
        initwritetoport();
        try {
            while (true) {
                // write string to port, the serialEvent will read it
                writetoport();
                Thread.sleep(1000);
            }
        } catch (InterruptedException e) {
        }
    }

    public void serialEvent(SerialPortEvent event) {
        System.out.println(".............." + event.getEventType());

        // we get here if data has been received
        byte[] readBuffer = new byte[20];
        try {
            // read data
            while (inputStream.available() > 0) {
                int numBytes = inputStream.read(readBuffer);
            }
            // print data
            String result = new String(readBuffer);
            System.out.println("Read: " + result);
        } catch (IOException e) {
        }
    }
}


