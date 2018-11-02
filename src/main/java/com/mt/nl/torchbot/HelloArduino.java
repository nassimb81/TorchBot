package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class HelloArduino {

    static private SerialPort[] ports = SerialPort.getCommPorts();
    static private SerialPort port = null;


    public static void main(String[] args) throws IOException, InterruptedException {
        DateTimeFormatter df = DateTimeFormatter.ofPattern("HH:mm:ss:A");

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
            public int getListeningEvents() {
                return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
            }

            @Override
            public void serialEvent(SerialPortEvent event) {
                try {
                    Thread.sleep(1000);
                } catch (Exception ex) {
                    System.out.println("interrupted " + ex);

                }
                System.out.println("------------ Beginning of Listening Event ------------");
                System.out.println("Read from arduino at " + LocalDateTime.now().format(df));
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
                    return;
                byte[] newData = new byte[port.bytesAvailable()];
                int numRead = port.readBytes(newData, newData.length);
                System.out.println("Read " + numRead + " bytes.");
                readMessage(inputStream);
            }
        });

        System.out.println("------------ Beginning of Writing to Arduino ------------");
        outputStream.write("a".getBytes());
        System.out.println("Wrote to arduino at " + LocalDateTime.now().format(df));
        Thread.sleep(1000);
        outputStream.write("hello\\n".getBytes());
        System.out.println("Wrote to arduino at " + LocalDateTime.now().format(df));
//        Thread.sleep(1000);


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
    }

    private static void readMessage(InputStream inputStream) {
        DateTimeFormatter df = DateTimeFormatter.ofPattern("HH:mm:ss:A");
        System.out.println("Reading message at" + LocalDateTime.now().format(df));
        try {
            Thread.sleep(1000);
            int length = inputStream.available();
            byte[] buffer = new byte[length];
            System.out.println(inputStream.available());
            while (inputStream.available() > 0) {
                inputStream.read(buffer);

                String result = new String(buffer, "UTF8");
                System.out.println("Reading from Arduino: \n " + result);
                if (result.contains("#1138")) {
                    System.out.println("breaking away");
                    break;
                }
            }
        } catch (Exception ex) {
            System.out.println("Getting an exception " + ex);
        }

//            String result = new String(buffer, "UTF8");
//            System.out.println("Getting this string from Arduino: " + result);
//            Thread.sleep(1000);


    }
}

