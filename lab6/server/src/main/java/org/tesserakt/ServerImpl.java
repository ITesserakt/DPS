package org.tesserakt;

import Cell.StationPOA;
import Cell.TubeCallback;
import Cell.WrongPhone;
import Cell.WrongReceiver;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Consumer;

public class ServerImpl extends StationPOA {
    private final Map<String, TubeCallback> _registeredPhones;
    private final Logger _logger = LoggerFactory.getLogger(ServerImpl.class);
    private final Consumer<AuthoredMessage> _messagesSink;

    public ServerImpl(Consumer<AuthoredMessage> messagesSink) {
        _messagesSink = messagesSink;
        _registeredPhones = new ConcurrentHashMap<>();
    }

    @Override
    public void register(TubeCallback objRef, String phoneNum) throws WrongPhone {
        if (!phoneNum.matches("\\+?\\d-?\\d{3}-?\\d{3}-?\\d{2}-?\\d{2}"))
            throw new WrongPhone();

        _logger.info("New phone registered: {}", phoneNum);
        _registeredPhones.put(phoneNum, objRef);
    }

    @Override
    public void sendSMS(String fromNum, String toNum, String message) throws WrongReceiver {
        if (!_registeredPhones.containsKey(toNum))
            throw new WrongReceiver();

        _registeredPhones.get(toNum).receiveSMS(fromNum, message);
        _messagesSink.accept(new AuthoredMessage(fromNum, toNum, message));
    }

    @Override
    public void sendACK() {
        String code = UUID.randomUUID().toString();
        _registeredPhones.entrySet().stream().parallel()
                .filter(e -> {
                    try {
                        return !Objects.equals(e.getValue().receiveACK(code), code);
                    } catch (Exception ex) {
                        return true;
                    }
                })
                .map(Map.Entry::getKey)
                .forEach(k -> {
                    _logger.warn("Removing <{}> due to inactivity", k);
                    _registeredPhones.remove(k);
                });
    }
}
