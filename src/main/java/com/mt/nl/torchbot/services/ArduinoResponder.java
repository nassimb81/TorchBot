package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.domain.ArduinoListener;


import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

public class ArduinoResponder implements ArduinoListener {

    private boolean listening = true;
    private String sendingArray = "Send_Array";

    @Override
    public void exportFile(String result) {
        System.out.println("Saving File !");
        System.out.println("Array will be stored in output file");
        int startArray = result.indexOf(sendingArray);
        int lengthSender = sendingArray.length();

        try {

            String stringSeq = result.substring(startArray + lengthSender);
            BufferedWriter writer = new BufferedWriter(new FileWriter("test.txt"));
            writer.write(stringSeq);
            writer.close();
        } catch (IOException ioEx){
            System.out.println("Error thrown during saving the file");
        }
    }

    @Override
    public void importFile(SerialPort port) {
        System.out.println("Sending File!");
        System.out.println("Arduino expects to get an array; sending array to arduino");
        arduinoMessager(port);
    }

    private void arduinoMessager(SerialPort port) {

        System.out.println("Messaging to arduino");
        OutputStream outputStream = port.getOutputStream();

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
}

