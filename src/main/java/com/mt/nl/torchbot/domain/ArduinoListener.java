package com.mt.nl.torchbot.domain;

import com.fazecast.jSerialComm.SerialPort;

public interface ArduinoListener {
    void exportFile(SerialPort port);
    void importFile(String file);
}
