package org.tesserakt;

import Cell.TubeCallbackPOA;

public class ClientImpl extends TubeCallbackPOA {
    private final String _phoneNumber;

    public ClientImpl(String phoneNumber) {
        _phoneNumber = phoneNumber;
    }

    @Override
    public void receiveSMS(String fromNum, String message) {
        System.out.printf("%s -> %s\n", fromNum, message);
    }

    @Override
    public String getNum() {
        return _phoneNumber;
    }

    @Override
    public String receiveACK(String code) {
        return code;
    }
}
