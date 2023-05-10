package org.tesserakt;

public class Message {
    private final String _receiverPhone;
    private final String _message;

    public String getReceiverPhone() {
        return _receiverPhone;
    }

    public String getMessage() {
        return _message;
    }

    protected Message(String receiverPhone, String message) {
        _receiverPhone = receiverPhone;
        _message = message;
    }

}

