package org.tesserakt;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

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

    public static @Nullable Message parse(@NotNull String toParse) {
        String[] compound = toParse.split("->", 2);
        if (compound.length == 1)
            return null;

        return new Message(compound[0].trim(), compound[1].trim());
    }
}

