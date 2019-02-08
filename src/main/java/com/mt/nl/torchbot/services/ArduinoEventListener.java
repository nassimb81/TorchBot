package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.utility.ArduinoInitiator;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;

public class ArduinoEventListener {

    private boolean listening = true;
    private String sendingArray = "Send_Array";

    private ArduinoInitiator initiater = new ArduinoInitiator();
    private ArduinoResponder responder = new ArduinoResponder();


    public ArduinoEventListener() {
        initiater.addListener(responder);
    }


    public void startListening(SerialPort port) throws InterruptedException, IOException {
        System.out.println("Listening to arduino");
        while (listening) {
            try {
                InputStream inputStream = port.getInputStream();
                Thread.sleep(500);
                int length = inputStream.available();
                byte[] buffer = new byte[length];
                while (inputStream.available() > 0) {

                    int iBytes = inputStream.read(buffer);

                    if (iBytes > 0) {
                        System.out.printf("InputStream contains %d characters \n", iBytes);
                    }

                    String result = new String(buffer, StandardCharsets.UTF_8);

                    System.out.println("Reading from Arduino: \n " + result);

                    if (result.contains(sendingArray)) {
                        System.out.println("Array will be stored in output file");
                        initiater.exportFile(result);
                        inputStream.close();
                    } else if (result.contains("Request_Array_From_PC")) {
                        initiater.importFile(port);
                        inputStream.close();
                    }
                }
            } catch (Exception e) {
                System.out.println("printing out exception in Arduino Listener: " + e);
                throw e;
            }
        }
    }
}


