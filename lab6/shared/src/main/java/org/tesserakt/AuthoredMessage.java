package org.tesserakt;

public class AuthoredMessage extends Message {
    private final String _senderPhone;

    public AuthoredMessage(String senderPhone, String receiverPhone, String message) {
        super(receiverPhone, message);
        this._senderPhone = senderPhone;
    }

    public String getSenderPhone() {
        return _senderPhone;
    }
}
