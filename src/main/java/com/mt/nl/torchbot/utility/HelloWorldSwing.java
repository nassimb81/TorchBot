

package com.mt.nl.torchbot.utility;

/*
 * GridLayoutDemo.java
 *
 */

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class HelloWorldSwing extends JFrame {
    private static final String gapList[] = {"0", "10", "15", "20"};
    private final static int maxGap = 20;
    private JButton applyButton = new JButton("Send Text");
    private GridLayout experimentLayout = new GridLayout(3, 1);

    public HelloWorldSwing(String name) {
        super(name);
        setResizable(true);
    }


    public void addComponentsToPane(final Container pane) {
        final JPanel compsToExperiment = new JPanel();
        compsToExperiment.setLayout(experimentLayout);
        JPanel controls = new JPanel();
        controls.setLayout(new GridLayout(2, 3));

        //Creating the MenuBar and adding components
        JMenuBar mb = new JMenuBar();
        JMenu m1 = new JMenu("FILE");
        mb.add(m1);
        JMenuItem m11 = new JMenuItem("Open");
        JMenuItem m22 = new JMenuItem("Save");
        m1.add(m11);
        m1.add(m22);


        //Set up components preferred size
        JButton b = new JButton("Fake button");
        Dimension buttonSize = b.getPreferredSize();
        compsToExperiment.setPreferredSize(new Dimension((int) (buttonSize.getWidth() * 1) + maxGap,
                (int) (buttonSize.getHeight() * 2) + maxGap * 2));

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


        //Add Components
        compsToExperiment.add(BorderLayout.NORTH, mb);

        //Add controls to set up horizontal and vertical gaps
        controls.add(applyButton);

        //Process the Apply gaps button press
        applyButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                //Get the horizontal gap value
//                String horGap = (String) horGapComboBox.getSelectedItem();
//                //Get the vertical gap value
//                String verGap = (String) verGapComboBox.getSelectedItem();
                //Set up the horizontal gap value
//                experimentLayout.setHgap(Integer.parseInt(horGap));
//                //Set up the vertical gap value
//                experimentLayout.setVgap(Integer.parseInt(verGap));
                //Set up the layout of the buttons
                experimentLayout.layoutContainer(compsToExperiment);
            }
        });
        pane.add(compsToExperiment, BorderLayout.NORTH);
        pane.add(new JSeparator(), BorderLayout.CENTER);
        pane.add(controls, BorderLayout.SOUTH);
    }

    /**
     * Create the GUI and show it.  For thread safety,
     * this method is invoked from the
     * event dispatch thread.
     */
    private static void createAndShowGUI() {
        //Create and set up the window.
        HelloWorldSwing frame = new HelloWorldSwing("GridLayoutDemo");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setPreferredSize(new Dimension(1000,1000));
        //Set up the content pane.
        frame.addComponentsToPane(frame.getContentPane());
        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }

    public static void main(String[] args) {
        /* Use an appropriate Look and Feel */
        try {
            //UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
            UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
        } catch (UnsupportedLookAndFeelException ex) {
            ex.printStackTrace();
        } catch (IllegalAccessException ex) {
            ex.printStackTrace();
        } catch (InstantiationException ex) {
            ex.printStackTrace();
        } catch (ClassNotFoundException ex) {
            ex.printStackTrace();
        }
        /* Turn off metal's use of bold fonts */
        UIManager.put("swing.boldMetal", Boolean.FALSE);

        //Schedule a job for the event dispatch thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }
}