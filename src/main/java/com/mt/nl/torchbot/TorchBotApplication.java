package com.mt.nl.torchbot;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import lombok.extern.slf4j.Slf4j;

@Slf4j
public class TorchBotApplication extends Application {

    public static void main(String[] args) {
        try {
            launch(args);
        } catch (Exception e) {
            log.error("Exception Occured during startup of the application " + e);

        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception{
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/torchbot_looks.fxml"));
        Parent root = loader.load();
        primaryStage.setTitle("TorchBot - Mat-Tech B.V.");
        primaryStage.setScene(new Scene(root,500, 500));
        primaryStage.show();
    }

    private static void run() {
        try {
//            SerialPort port = openConnection();
//            new ArduinoEventListener().startListening(port);

        } catch (Exception ex) {
            log.error("Exception thrown while opening connection and startening to Listen: " + ex);
        }
    }
}
