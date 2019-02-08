package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.domain.ArduinoListener;


import java.io.IOException;
import java.io.OutputStream;
import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

public class ArduinoResponder implements ArduinoListener {

    private FileService fileService = new FileService();

    private boolean listening = true;

    /**
     * When the java service receives a string from the arduino it sends it to the fileservice
     * The file will be saved under a name specified by the end-user
     * @param array
     */
    public void exportFile(String array) {
        fileService.gettingArrayFromArduino(array);
    }

    public void importFile(SerialPort port) {
        System.out.println("Sending File!");
        System.out.println("Arduino expects to get an array; sending array to arduino");
        arduinoMessager(port);
    }

    /**
     * When the java service receives a signal from the arduino that it's waiting for a file
     * @param port
     */
    private void arduinoMessager(SerialPort port) {

        System.out.println("Messaging to arduino");
        OutputStream outputStream = port.getOutputStream();

        try {
            List<String> commaSeparatedList = fileService.getImportedArray();

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


}

