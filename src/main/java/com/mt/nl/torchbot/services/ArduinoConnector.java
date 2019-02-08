package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class ArduinoConnector {

    private static SerialPort port = null;
    private String endArray = "-32000";
    private String sendingArray = "Send_Array";
    OutputStream outputStream;


    private void arduinoMessager() {

        System.out.println("Messaging to arduino");
        outputStream = port.getOutputStream();

        try {
            File file = new File("D:\\Overall_Projects\\TorchBot\\test.txt");
            Path yourPath = file.toPath();
            byte[] encoded = Files.readAllBytes(yourPath);
            String textFile = new String(encoded, StandardCharsets.UTF_8).replace("\r\n", "");
            List<String> commaSeparatedList = Arrays.asList(textFile.split(","));

            String eol = "\n";
            Thread.sleep(100);
            for (String line : commaSeparatedList) {
                if (line.matches("^-?[0-9]+$")) {
                    System.out.println("Printing out : " + line);
                    outputStream.write(line.getBytes());
                    outputStream.write(eol.getBytes());
                }
            }
            outputStream.close();
        } catch (Exception e) {
            System.out.println("printing out exception: " + e);
        }

    }

    public static SerialPort openConnection(String portWanted) {
        SerialPort[] ports = SerialPort.getCommPorts();

        for (SerialPort p : ports) {
            if (p.getSystemPortName().equals(portWanted)) {
                port = p;
                System.out.println("Port selected: " + portWanted);
            }
        }

        if (port == null) {
            return null;
        }

        port.setComPortParameters(115200, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.LISTENING_EVENT_DATA_AVAILABLE, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            System.out.println("Port is open :) and port is " + port.toString());
        } else {
            System.out.println("Failed to open port :(");
            return null;
        }

        return port;
    }

    public List<String> getPorts() {
        SerialPort[] ports = SerialPort.getCommPorts();
        List<String> portsList = new ArrayList<>();
        for (SerialPort p : ports) {
            portsList.add(p.getSystemPortName());
        }

        if (portsList.isEmpty()) {
            System.err.println("No Ports Detected, check if Arduino is connected");
        }

        return portsList;
    }

    public static SerialPort getConnection(){
        return port;
    }
}

