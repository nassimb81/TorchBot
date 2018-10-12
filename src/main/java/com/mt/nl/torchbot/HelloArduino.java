package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;

import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class HelloArduino {

    public static void main(String[] args) throws IOException, InterruptedException {
        SerialPort[] ports = SerialPort.getCommPorts();
        SerialPort port = null;

        for (SerialPort p : ports) {
            if (p.getSystemPortName().equals("/dev/COM3"))
                ; /*for your question, <required_port> would be ttyACM0, but this can change if you reconnect the device, or if multiple devices are connected.*/
            port = p;
        }

        port.setComPortParameters(9600, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.TIMEOUT_WRITE_BLOCKING, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :)");
        } else {
            System.out.println("Failed to open port :(");
            return;
        }

        for (Integer i = 1; i < 10; ++i) {
            InputStream inputStream;
            System.out.println("Sending in byte number is: " + i);
            port.getOutputStream().write(i.byteValue());
            port.getOutputStream().flush();
            System.out.println("Is it blinking?");
            inputStream = port.getInputStream();

            byte[] buffer= new byte[5];
            int in = 1;
            while (in < 5) {
                buffer[in++] = (byte) inputStream.read();
            }

            System.out.println("Buffer is " + Arrays.toString(buffer));
            String result = new String(buffer, "UTF8");
            System.out.println("Buffer is " + result);

            Thread.sleep(3000);
        }

        if (port.closePort()) {
            System.out.println("Port is closed :)");
        } else {
            System.out.println("Failed to close port :(");
            return;
        }

    }
}

