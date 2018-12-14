package com.mt.nl.torchbot.services;

public class FileRetriever {

    public static void main(String[] args) {

        String integer = "<Arduino is ready>";
        String notinteger = "abc";

        // Testing if regex is present
        System.out.print("Does integer contain integers: ");
        System.out.println(integer.matches("^[a-zA-Z<>\\s]+$"));

        System.out.print("Does non-integer contain integers: ");
        System.out.println(notinteger.matches("^[a-zA-Z<>\\s]+$"));

    }
}
