package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.services.ArduinoEventListener;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.net.URL;

import static com.mt.nl.torchbot.services.ArduinoConnector.openConnection;

public class TorchBotApplication extends Application {

    public static void main(String[] args) {
        try {
            launch(args);
//            run();
        } catch (Exception e) {
            System.out.println("Exception Occured during startup of the application " + e);

        }
    }

    @Override
    public void start(Stage primaryStage) throws Exception{
//        URL path = getClass().getResource("/torchbot_looks.fxml");
//        FXMLLoader loader = new FXMLLoader(path);
//
//        loader.setController(loader.getController());
////        root = loader.load();
//        Parent root = FXMLLoader.load(path);

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
            System.out.println("Exception thrown while opening connection and startening to Listen: " + ex);
        }
    }
}
