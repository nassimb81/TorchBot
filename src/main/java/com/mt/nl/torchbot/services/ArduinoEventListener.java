package com.mt.nl.torchbot.services;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.utility.ArduinoInitiator;
import lombok.extern.slf4j.Slf4j;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;

@Slf4j
public class ArduinoEventListener {


    private ArduinoInitiator initiater = new ArduinoInitiator();
    private ArduinoResponder responder = new ArduinoResponder();
    private String eol = "\n";


    ArduinoEventListener() {
        initiater.addListener(responder);
    }


    void startListening(SerialPort port) throws InterruptedException, IOException {
        String sendingArray = "Send_Array";
        String arduinoSearchingForConnection = "java_available";
        log.info("Listening to arduino");
        while (true) {
            try {
                String result = "";
                InputStream inputStream = port.getInputStream();
                Thread.sleep(300);
                int length = inputStream.available();
                byte[] buffer = new byte[length];
                result = receivingArdBytes(inputStream, buffer);

                if (result.contains(arduinoSearchingForConnection)) {
                    sendingConnectingMessageToArduino(port);
                } else if (result.contains(sendingArray)) {
                    String massResult = "";
                    log.info("Array will be stored in output file");
                    long t = System.currentTimeMillis();
                    System.out.println(t);
                    while ((System.currentTimeMillis() - t) < 6000) {
                        massResult = result + receivingArdBytes(inputStream, buffer);
                    }
                    System.out.println("Mass Result is" + massResult);
                    initiater.importFile(massResult);
                    inputStream.close();
                } else if (result.contains("Receiving_Array")) {
                    initiater.exportFile(port);
                    inputStream.close();
                }
            } catch (Exception e) {
                log.error("printing out exception in Arduino Listener: " + e);
                throw e;
            }
        }

    }

    private String receivingArdBytes(InputStream inputStream, byte[] buffer) throws IOException {
        if (inputStream.available() > 0) {
            int iBytes = inputStream.read(buffer);

            if (iBytes > 0) {
                System.out.printf("InputStream contains %d characters \n", iBytes);
            }

            String result = new String(buffer, StandardCharsets.UTF_8);

            log.info("Reading from Arduino: \n " + result);
            return result;
        } else {
            return "";
        }
    }

    private void sendingConnectingMessageToArduino(SerialPort port) throws IOException, InterruptedException {
        OutputStream outputStream = port.getOutputStream();
        String ardMessageReceived = "arduino_message_received";
        log.info("Sending to Arduino out : " + ardMessageReceived);
        outputStream.write(ardMessageReceived.getBytes());
        outputStream.write(eol.getBytes());
        Thread.sleep(10);
        outputStream.flush();
    }
}


