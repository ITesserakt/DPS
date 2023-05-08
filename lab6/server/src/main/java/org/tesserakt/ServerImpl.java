package org.tesserakt;

import Cell.StationPOA;
import Cell.TubeCallback;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.function.Consumer;

public class ServerImpl extends StationPOA {
    private final Map<String, TubeCallback> _registeredPhones;
    private final Logger _logger = LoggerFactory.getLogger(ServerImpl.class);
    private final Consumer<AuthoredMessage> _messagesSink;

    public ServerImpl(Consumer<AuthoredMessage> messagesSink) {
        _messagesSink = messagesSink;
        _registeredPhones = new HashMap<>();
    }

    @Override
    public int register(TubeCallback objRef, String phoneNum) {
        if (!phoneNum.matches("\\+?\\d-?\\d{3}-?\\d{3}-?\\d{2}-?\\d{2}"))
            return -1;

        _logger.info("New phone registered: {}", phoneNum);
        _registeredPhones.put(phoneNum, objRef);
        return 0;
    }

    @Override
    public int sendSMS(String fromNum, String toNum, String message) {
        if (!_registeredPhones.containsKey(toNum))
            return -1;

        int code = _registeredPhones.get(toNum).receiveSMS(fromNum, message);
        if (code == 0)
            _messagesSink.accept(new AuthoredMessage(fromNum, toNum, message));
        return code;
    }
}
