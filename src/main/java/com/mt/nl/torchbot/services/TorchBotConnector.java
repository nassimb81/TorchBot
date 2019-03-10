package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;
import lombok.extern.slf4j.Slf4j;

import java.io.File;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@Slf4j
public class TorchBotConnector  implements Runnable {

    private static SerialPort port = null;
    private String endArray = "-32000";
    private String sendingArray = "Send_Array";
    private String portWanted;

    public TorchBotConnector(String portWanted) {
        this.portWanted = portWanted;
    }

    public TorchBotConnector(){}

    private void arduinoMessager() {
        OutputStream outputStream;
        log.info("Messaging to arduino");
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
                    log.info("Printing out : " + line);
                    outputStream.write(line.getBytes());
                    outputStream.write(eol.getBytes());
                }
            }
            outputStream.close();
        } catch (Exception e) {
            log.error("printing out exception: " + e);
        }

    }

    public SerialPort openConnection() {
        SerialPort[] ports = SerialPort.getCommPorts();

        for (SerialPort p : ports) {
            if (p.getSystemPortName().equals(portWanted)) {
                port = p;
                log.info("Port selected: " + portWanted);
            }
        }

        if (port == null) {
            return null;
        }

        port.setComPortParameters(115200, 8, 1, 0); // default connection settings for Arduino
        port.setComPortTimeouts(SerialPort.LISTENING_EVENT_DATA_AVAILABLE, 0, 0); // block until bytes can be written

        if (port.openPort()) {
            log.info("Port is open :) and port is " + port.toString());
        } else {
            log.error("Failed to open port :(");
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

    public static SerialPort getConnection() {
        return port;
    }

    public void run() {
        try {
            SerialPort port = openConnection();
            new ArduinoEventListener().startListening(port);

        } catch (Exception ex) {
            log.error("Exception thrown while opening connection and startening to Listen: " + ex);
        }
    }
}

