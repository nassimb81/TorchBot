package com.mt.nl.torchbot.controller;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.services.TorchBotConnector;
import com.mt.nl.torchbot.services.ArrayValidator;
import com.mt.nl.torchbot.services.FileService;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextArea;
import javafx.scene.layout.AnchorPane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import lombok.extern.slf4j.Slf4j;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.ResourceBundle;

import static com.mt.nl.torchbot.services.TorchBotConnector.getConnection;
import static com.mt.nl.torchbot.services.TorchBotConnector.openConnection;

@Slf4j
public class ControllerGUI implements Initializable {

    private String portChoosen;

    @FXML
    private Menu openRecentTab;

    @FXML
    private TextArea textArea;

    @FXML
    private AnchorPane ap;

    @FXML
    private MenuItem startConnectionMenuItem;

    public ControllerGUI() {

        log.info("ControllerGUI has been initialized");

    }

    private TorchBotConnector arduinoConnector = new TorchBotConnector();
    private FileService fileService = new FileService();

    @FXML
    private void openFile(ActionEvent event) throws IOException {
        Stage stage = (Stage) ap.getScene().getWindow();
        FileChooser fileChooser = new FileChooser();
        File selectedFile = fileChooser.showOpenDialog(stage);

        Path yourPath = selectedFile.toPath();
        byte[] encoded = Files.readAllBytes(yourPath);
        String textFile = new String(encoded, StandardCharsets.UTF_8);

        textArea.clear();
        textArea.appendText(textFile);
    }

    @FXML
    private void saveFile(ActionEvent event) throws IOException {
        ArrayValidator av = new ArrayValidator();
        String text = textArea.getText().replace("\n", "");

        if (av.isArrayValid(text)) {
            Stage stage = (Stage) ap.getScene().getWindow();
            FileChooser fileChooser = new FileChooser();
            FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("txt files (*.txt)", "*.txt");
            fileChooser.getExtensionFilters().add(extFilter);
            File saveFile = fileChooser.showSaveDialog(stage);
            log.info("Array being saved to the pc with name {} and content {} ", saveFile.getName(), text);
            PrintWriter writer;
            writer = new PrintWriter(saveFile);
            writer.println(text);
            writer.close();
            textArea.clear();
            textArea.appendText("File saved with success");
        } else {
            log.info("Array is not valid");
            textArea.appendText("\nArray is not valid for export, only +/- integers and commas are allowed.");
        }
    }


    @FXML
    private void getPortSelection(ActionEvent event) {
        event.getEventType().getName();
        MenuItem menuItemClicked = (MenuItem) event.getTarget();
        if (!menuItemClicked.getText().equals("Start Connection")) {
            portChoosen = menuItemClicked.getText();
            log.info(portChoosen);
            textArea.clear();
            textArea.appendText("port " + portChoosen + " choosen");
        }
    }

    @FXML
    private void startConnection(ActionEvent event) {
        SerialPort port = getConnection();

        if (port == null) {
            port = openConnection(portChoosen);
            if (port != null) {
                textArea.clear();
                textArea.appendText("Connection has been made!");
            } else {
                textArea.clear();
                textArea.appendText("No Connection has been made, have you selected the port?");
            }
        } else {
            textArea.clear();
            textArea.appendText("You already made a connection!");
        }
    }

    @FXML
    private void exportArray() {
        ArrayValidator av = new ArrayValidator();
        String text = textArea.getText().replace("\n", "");
        if (av.isArrayValid(text) && getConnection() != null) {
            try {
                fileService.exportFileTemp(text);
                textArea.clear();
                textArea.appendText("File is exported correctly\n" +
                        "Switch from manual to play on the TorchBot to start this program");
            } catch (IOException io) {
                System.err.println("Exception during exporting of File");
                textArea.clear();
                textArea.appendText("File is not exported correctly\ntry exporting again or check the temp folder");
            }
        } else {
            log.info("Array is not valid or no Connection has been made");
            textArea.clear();
            textArea.appendText("\nArray is not valid for export, only +/- integers and commas are allowed.");
        }
    }

    @FXML
    private void importArray() {
        try {
            String importedArray = fileService.getImportedArray();
            textArea.clear();
            textArea.appendText(importedArray);
        } catch (IOException io) {
            System.err.println("Exception during exporting of File");
            textArea.clear();
            textArea.appendText("File is not exported correctly\ntry exporting again or check the temp folder");
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

        if (portsAvailable.isEmpty()) {
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
