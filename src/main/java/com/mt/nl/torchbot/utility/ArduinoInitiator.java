package com.mt.nl.torchbot.utility;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.domain.ArduinoListener;

import java.util.ArrayList;
import java.util.List;

public class ArduinoInitiator {

    private List<ArduinoListener> listeners = new ArrayList<ArduinoListener>();

    public void addListener(ArduinoListener toAdd) {
        listeners.add(toAdd);
    }

    // Array is vanuit Arduino naar Service verstuurd en wordt opgeslagen in het windows of linux systeem
    public void exportFile(String result) {
        for (ArduinoListener al : listeners)
            al.exportFile(result);
    }

    // Array wordt vanuit java service naar de arduino verstuurd.
    public void importFile(SerialPort port) {
        for (ArduinoListener al : listeners)
            al.importFile(port);
    }
}
