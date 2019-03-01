package com.mt.nl.torchbot.services;

public class ArrayValidator {

    public boolean isArrayValid(String array) {
        if (array.matches("^-?[0-9]{0,10}(,-?[0-9]{0,10})*$")) {
            return true;
        } else return false;
    }
}

