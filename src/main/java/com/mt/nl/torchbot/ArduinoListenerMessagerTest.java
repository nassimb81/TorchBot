package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

public class ArduinoListenerMessagerTest {

    private SerialPort port = null;
    private static InputStream inputStream;
    private static OutputStream outputStream;

    private ArduinoListenerMessagerTest() {
        SerialPort[] ports = SerialPort.getCommPorts();
        String response = null;
        try {
            response = openConnection(ports, port);
        } catch (Exception ex) {
            System.out.println("Exception thrown while opening connection in constructor: " + ex);
        }

        System.out.println("Opened connection with response: "+ response);
    }

    public static void main(String[] args) throws IOException, InterruptedException {
        try {
            ArduinoListenerMessagerTest a = new ArduinoListenerMessagerTest();
            // starts new thread and executes countA in it
//            new Thread(a::arduinoMessager).start();

            // starts new thread and executes countB in it
            new Thread(a::arduinoListener).start();

        } catch (Exception e) {
            System.out.println("printing out exception: " + e);

        }
    }

    private void arduinoMessager() {

        try {
            for (Integer i = 1; i < 10; ++i) {

                StringBuilder sb = new StringBuilder();
                System.out.println("Iteration: " + i);
                String panda = i.toString();
                outputStream.write(panda.getBytes());
                outputStream.flush();
            }
        } catch (Exception e) {
            System.out.println("printing out exception: " + e);
        }
        if (port.closePort()) {
            System.out.println("Port is closed :)");
        } else {
            System.out.println("Failed to close port :(");
        }

    }

    private void arduinoListener() {

        try {
            int length = inputStream.available();
            byte[] buffer = new byte[length];

            while (inputStream.available() > 0) {
                inputStream.read(buffer);
                System.out.println("Buffer is: " + Arrays.toString(buffer));

                String result = new String(buffer);
                System.out.println("Reading from Arduino: \n " + result);
            }
        } catch (Exception e) {
            System.out.println("printing out exception in Arduino Listener: " + e);
        }
    }

    private String openConnection(SerialPort[] ports, SerialPort port) {
        for (SerialPort p : ports) {
            System.out.println(p.getSystemPortName());
            if (p.getSystemPortName().equals("COM4"))  /*for your question, <required_port> would be ttyACM0, but this can change if you reconnect the device, or if multiple devices are connected.*/
            port = p;
        }

        if (port == null) {
            return "Port is null";
        }

        port.setComPortParameters(9600, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :)");
        } else {
            System.out.println("Failed to open port :(");
            return null;
        }

        inputStream = port.getInputStream();
        outputStream = port.getOutputStream();

        return "Connection made input and output stream set";
    }
}

