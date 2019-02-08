package com.mt.nl.torchbot.controller;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.services.ArduinoConnector;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextArea;
import javafx.scene.layout.BorderPane;

import java.net.URL;
import java.util.List;
import java.util.ResourceBundle;

import static com.mt.nl.torchbot.services.ArduinoConnector.getConnection;
import static com.mt.nl.torchbot.services.ArduinoConnector.openConnection;

public class ControllerGUI implements Initializable {
    private String portChoosen;

    @FXML
    private Menu openRecentTab;

    @FXML
    private TextArea textArea;

    @FXML
    private MenuItem startConnectionMenuItem;

    public ControllerGUI() {

        System.out.println("ControllerGUI has been initialized");
//        MenuItem menuItem1 = new MenuItem("Menu Item 1");
//        MenuItem menuItem2 = new MenuItem("Menu Item 2");
//        openRecentTab.getItems().addAll(menuItem1, menuItem2);
    }


    private ArduinoConnector arduinoConnector = new ArduinoConnector();

    @FXML
    private void handleButtonAction(ActionEvent event) {
        // Button was clicked, do something
        System.out.println("hello");
        textArea.appendText("Button Action\n");

    }

    @FXML
    private void getPortSelection(ActionEvent event){
        event.getEventType().getName().toString();
        MenuItem menuItemClicked = (MenuItem) event.getTarget();
        portChoosen = menuItemClicked.getText();
        System.out.println(portChoosen);
    }

    @FXML
    private void startConnection(ActionEvent event){
        SerialPort port = getConnection();

        if(port == null){
            openConnection(portChoosen);
        }

    }

    @FXML
    private void clickConnectMenu() {
        List<String> portsAvailable = arduinoConnector.getPorts();
        for (String pa : portsAvailable) {
            MenuItem menuItemPort = new MenuItem(pa);
            openRecentTab.getItems().add(menuItemPort);
        }
    }

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        List<String> portsAvailable = arduinoConnector.getPorts();

        if(portsAvailable.isEmpty()){
            textArea.appendText("No Open Ports Found, make sure the arduino is connected");
            return;
        }

        for (String pa : portsAvailable) {
            MenuItem menuItemPort = new MenuItem(pa);
            openRecentTab.getItems().add(menuItemPort);
        }

        startConnectionMenuItem.setVisible(true);
    }
}
