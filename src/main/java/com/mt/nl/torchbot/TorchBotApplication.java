package com.mt.nl.torchbot;

import com.fazecast.jSerialComm.SerialPort;
import com.mt.nl.torchbot.services.ArduinoEventListener;

import javax.swing.*;

import java.awt.*;

import static com.mt.nl.torchbot.services.ArduinoConnector.openConnection;

public class TorchBotApplication {

    public static void main(String[] args) {
        try {
            javax.swing.SwingUtilities.invokeLater(new Runnable() {
                public void run() {
                    createAndShowGUI();
                }
            });
            run();
        } catch (Exception e) {
            System.out.println("Exception Occured during startup of the application " + e);

        }
    }

    private static void run() {
        try {
            SerialPort port = openConnection();
            new ArduinoEventListener().startListening(port);

        } catch (Exception ex) {
            System.out.println("Exception thrown while opening connection and startening to Listen: " + ex);
        }
    }

    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    private static void createAndShowGUI() {
        JFrame frame = new JFrame("TorchBot");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(500,500);

        //Creating the MenuBar and adding components
        JMenuBar mb = new JMenuBar();
        JMenu m1 = new JMenu("FILE");
        mb.add(m1);
        JMenuItem m11 = new JMenuItem("Open");
        JMenuItem m22 = new JMenuItem("Save");
        m1.add(m11);
        m1.add(m22);

        //Creating the panel at bottom and adding components
        JPanel panel = new JPanel(); // the panel is not visible in output
        JLabel label = new JLabel("Enter Text");


        JTextField tf = new JTextField(50); // accepts up to 50 characters
        JButton send = new JButton("Send");
        JButton reset = new JButton("Reset");
        panel.add(label); // Components Added using Flow Layout
        panel.add(label); // Components Added using Flow Layout
        panel.add(tf);
        panel.add(send);
        panel.add(reset);

        // Text Area at the Center
        JTextArea ta = new JTextArea();

        //Adding Components to the frame.
        frame.getContentPane().add(BorderLayout.SOUTH, panel);
        frame.getContentPane().add(BorderLayout.NORTH, mb);
        frame.getContentPane().add(BorderLayout.CENTER, ta);
        frame.setVisible(true);
    }


}
