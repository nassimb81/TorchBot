package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;

import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

public class ArduinoListenerMessagerTest {

    private static SerialPort port = null;
    private static InputStream inputStream;
    private static OutputStream outputStream;
    private boolean listening = true;

    public static void main(String[] args) {
        try {

            SerialPort[] ports = SerialPort.getCommPorts();

            try {
                port = openConnection(ports, port);
                System.out.println("port is "+ port);

            } catch (Exception ex) {
                System.out.println("Exception thrown while opening connection in constructor: " + ex);
            }

            InputStream inputStream = port.getInputStream();
            OutputStream outputStream = port.getOutputStream();

            ArduinoListenerMessagerTest a = new ArduinoListenerMessagerTest();
            ArduinoListenerMessagerTest b = new ArduinoListenerMessagerTest();

            new Thread(() -> a.arduinoListener(inputStream)).start();

            new Thread(() -> b.arduinoMessager(outputStream)).start();
//            new Thread(a::arduinoMessager(inputStream)).start();

        } catch (Exception e) {
            System.out.println("printing out exception: " + e);

        }
    }

    private void arduinoMessager(OutputStream outputStream) {

        System.out.println("Messaging to arduino");
        outputStream = port.getOutputStream();

        try {
            for (Integer i = 1; i < 10; ++i) {
                Thread.sleep(1000);
                StringBuilder sb = new StringBuilder();
                System.out.println("Iteration: " + i);
                String panda = i.toString();
                outputStream.write(panda.getBytes());
                outputStream.flush();
            }
        } catch (Exception e) {
            System.out.println("printing out exception: " + e);
        }
    }

    private void arduinoListener(InputStream inputStream) {
        System.out.println("Listening to arduino");
        while (listening) {
            try {
                inputStream = port.getInputStream();
                Thread.sleep(1000);
                int length = inputStream.available();
                System.out.println("Length is "+ length);
                byte[] buffer = new byte[length];
                while (inputStream.available() > 0) {

                    inputStream.read(buffer);
                    System.out.println("Inside Listener Port is "+ port);
                    System.out.println("Inputstream is "+inputStream);

                    String result = new String(buffer, StandardCharsets.UTF_8);
                    byte[] newData = new byte[port.bytesAvailable()];
                    int numRead = port.readBytes(newData, newData.length);
                    System.out.println("Read " + numRead + " bytes.");
                    System.out.println("Reading from Arduino: \n " + result);

                }
            } catch (Exception e) {
                System.out.println("printing out exception in Arduino Listener: " + e);
            }
        }
    }

    private static SerialPort openConnection(SerialPort[] ports, SerialPort port) {
        for (SerialPort p : ports) {
            System.out.println(p.getSystemPortName());
            if (p.getSystemPortName().equals("COM4"))  /*for your question, <required_port> would be ttyACM0, but this can change if you reconnect the device, or if multiple devices are connected.*/
                port = p;
        }

        if (port == null) {
            return null;
        }

        port.setComPortParameters(9600, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :) and port is "+ port.toString());
        } else {
            System.out.println("Failed to open port :(");
            return null;
        }

        return port;
    }
}

