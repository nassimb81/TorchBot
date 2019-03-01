package com.mt.nl.torchbot.services;

import lombok.extern.slf4j.Slf4j;

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

@Slf4j
public class FileService {

    private static final String ARDUINO_SAVE_KEYWORD = "Send_Array";

    private static String fileNameCurrentlyExportedArray;
    private static String fileNameCurrentlyImportedArray;

    /**
     * Method is called when the arduino has send an array for saving
     * The array is saved in a temporary text file with a timestamp.
     *
     * @param array
     */
    void gettingArrayFromArduino(String array) {
        log.info("Saving File !");
        log.info("Array will be stored in output file");
        int startArray = array.indexOf(ARDUINO_SAVE_KEYWORD);
        int lengthSender = ARDUINO_SAVE_KEYWORD.length();

        try {
            String stringSeq = array.substring(startArray + lengthSender);
            String fileName = "temporary_import_array_"
                    + new SimpleDateFormat("yyyyMMdd_HHmmSS'.txt'").format(new Date());
            BufferedWriter writer = new BufferedWriter(new FileWriter(fileName));
            fileNameCurrentlyImportedArray = fileName;
            log.info("Temporary imported file: {}", fileNameCurrentlyImportedArray);
            writer.write(stringSeq);
            writer.close();
        } catch (IOException ioEx) {
            log.error("Error thrown during saving of the temporary array from the arduino");
        }
    }

    /**
     * Method is called when export button is clicked in the window
     * Stores the exported file to a temporary file that can be picked up later through the arduino
     *
     * @return The Specified Array from the PC
     * @throws IOException
     */
    public boolean exportFileTemp(String array) throws IOException {
        String fileName = "temp_export_array_"
                + new SimpleDateFormat("yyyyMMdd_HHmmSS'.txt'").format(new Date());

        File dir = new File("temp\\");
        dir.mkdirs();
        File file = new File(dir, fileName);

        BufferedWriter writer = new BufferedWriter(new FileWriter(file));
        writer.write(array);
        writer.close();

        fileNameCurrentlyExportedArray = fileName;

        log.info("Temporary Exported File: {}", fileNameCurrentlyExportedArray);

        return true;

    }


    List<String> getExportedArray() throws IOException {
        File file = new File("temp\\" + fileNameCurrentlyExportedArray);
        Path yourPath = file.toPath();
        byte[] encoded = Files.readAllBytes(yourPath);
        String textFile = new String(encoded, StandardCharsets.UTF_8).replace("\r\n", "");

        return Arrays.asList(textFile.split(","));

    }

    public String getImportedArray() throws IOException {
        fileNameCurrentlyImportedArray = "temp_import_array_20190214_1741166.txt";
        File file = new File("temp\\" + fileNameCurrentlyImportedArray);
        Path yourPath = file.toPath();
        byte[] encoded = Files.readAllBytes(yourPath);
        String textFile = new String(encoded, StandardCharsets.UTF_8).replace("\r\n", "");
        log.info("Fetching Temporary imported file from Arduino array is: {} ", textFile);
        return textFile;

    }

}
