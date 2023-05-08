package org.tesserakt;

import Cell.TubeCallbackPOA;

public class ClientImpl extends TubeCallbackPOA {
    private String _phoneNumber;

    public ClientImpl(String phoneNumber) {
        _phoneNumber = phoneNumber;
    }

    @Override
    public int receiveSMS(String fromNum, String message) {
        System.out.printf("Received a message from `%s`: %s%n", fromNum, message);
        return 0;
    }

    @Override
    public String getNum() {
        return _phoneNumber;
    }
}
