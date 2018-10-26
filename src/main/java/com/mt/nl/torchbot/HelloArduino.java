package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class HelloArduino {

    static private SerialPort[] ports = SerialPort.getCommPorts();
    static private SerialPort port = null;


    public static void main(String[] args) throws IOException, InterruptedException {


        for (SerialPort p : ports) {
            if (p.getSystemPortName().equals("COM4"))  /*for your question, <required_port> would be ttyACM0, but this can change if you reconnect the device, or if multiple devices are connected.*/
                port = p;
        }

        port.setComPortParameters(96000, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :)");
        } else {
            System.out.println("Failed to open port :(");
            return;
        }

        InputStream inputStream = port.getInputStream();
        OutputStream outputStream = port.getOutputStream();

        port.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
            @Override
            public void serialEvent(SerialPortEvent event)
            {
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
                    return;
                byte[] newData = new byte[port.bytesAvailable()];
                int numRead = port.readBytes(newData, newData.length);
                System.out.println("Read " + numRead + " bytes.");
                readMessage(inputStream);
            }
        });

        outputStream.write("a".getBytes());
        Thread.sleep(1000);
        outputStream.write("hello\\n".getBytes());
        Thread.sleep(1000);


//        for (Integer i = 1; i <= 3; ++i) {
//
//            StringBuilder sb = new StringBuilder();
//            System.out.println("Iteration: " + i);
//            String panda = "h";
//            outputStream.write(panda.getBytes());
//            outputStream.flush();
//            Thread.sleep(1000);
//
////            getMessages(inputStream);
//        }
//
//        if (port.closePort()) {
//            System.out.println("Port is closed :)");
//        } else {
//            System.out.println("Failed to close port :(");
//            return;
//        }

    }

    private static void readMessage(InputStream inputStream) {
        try {
            Thread.sleep(100);
            int length = inputStream.available();
            byte[] buffer = new byte[length];
//
            while (inputStream.available() > 0) {
                inputStream.read(buffer);

                String result = new String(buffer, "UTF8");
                System.out.println("Reading from Arduino: \n " + result);

//                for (byte b : buffer) {
//                    sb.append(new String(new byte[]{b}));
//                }
            }
        } catch (Exception ex) {
            System.out.println("Getting an exception " + ex);
        }

//            String result = new String(buffer, "UTF8");
//            System.out.println("Getting this string from Arduino: " + result);
//            Thread.sleep(1000);


    }
}

