package com.mt.nl.torchbot.services;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

class FileService {

    private static final String ARDUINO_SAVE_KEYWORD = "Send_Array";

    private static String filenameCurrentlyImportedArray;

    /**
     * Method is called when the arduino has send an array for saving
     * The array is saved in a temporary text file with a timestamp.
     *
     * @param array
     */
    void gettingArrayFromArduino(String array) {
        System.out.println("Saving File !");
        System.out.println("Array will be stored in output file");
        int startArray = array.indexOf(ARDUINO_SAVE_KEYWORD);
        int lengthSender = ARDUINO_SAVE_KEYWORD.length();

        try {
            String stringSeq = array.substring(startArray + lengthSender);
            String fileName = "temporary_export_array_"
                    + new SimpleDateFormat("yyyyMM_ddHHmm'.txt'").format(new Date());
            BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
            writer.write(stringSeq);
            writer.close();
        } catch (IOException ioEx) {
            System.out.println("Error thrown during saving the temporary array from the arduino");
        }
    }

    /**
     * Method is called when import button is clicked in the window
     * Stores the imported filed to a temporary file that can be picked up later through the arduino
     *
     * @return The Specified Array from the PC
     * @throws IOException
     */
    public void saveImportedFileToTempLocation(String array, String location) throws IOException {

        File file = new File("D:\\Overall_Projects\\TorchBot\\test.txt");
        Path yourPath = file.toPath();
        byte[] encoded = Files.readAllBytes(yourPath);
        String textFile = new String(encoded, StandardCharsets.UTF_8).replace("\r\n", "");

        String fileName = "temporary_import_array_"
                + new SimpleDateFormat("yyyyMM_ddHHmmSS'.txt'").format(new Date());

        BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
        writer.write(textFile);
        writer.close();

        filenameCurrentlyImportedArray = fileName;

        System.out.println("Temporary Imported Files: " + filenameCurrentlyImportedArray);

    }

    List<String> getImportedArray() throws IOException {
        File file = new File("D:\\Overall_Projects\\TorchBot\\" + filenameCurrentlyImportedArray);
        Path yourPath = file.toPath();
        byte[] encoded = Files.readAllBytes(yourPath);
        String textFile = new String(encoded, StandardCharsets.UTF_8).replace("\r\n", "");

        return Arrays.asList(textFile.split(","));

    }

}
