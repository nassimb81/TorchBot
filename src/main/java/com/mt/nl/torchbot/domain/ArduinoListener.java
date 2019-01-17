package com.mt.nl.torchbot.domain;

import com.fazecast.jSerialComm.SerialPort;

public interface ArduinoListener {
    void importFile(SerialPort port);
    void exportFile(String file);
}
