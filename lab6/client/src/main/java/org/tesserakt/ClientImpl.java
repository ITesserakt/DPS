package org.tesserakt;

import Cell.TubeCallbackPOA;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ClientImpl extends TubeCallbackPOA {
    private final String _phoneNumber;
    private final Logger _logger = LoggerFactory.getLogger(ClientImpl.class);

    public ClientImpl(String phoneNumber) {
        _phoneNumber = phoneNumber;
    }

    @Override
    public void receiveSMS(String fromNum, String message) {
        _logger.info("{} -> {}", fromNum, message);
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
