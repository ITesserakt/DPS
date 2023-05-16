package org.tesserakt;

import org.apache.commons.io.FileUtils;

import java.io.*;
import java.util.Base64;

public class FileMessage extends Message {
    public static final String FILE_MESSAGE_BOUNDARY = "gvwygkugkj;rthiue4y5nevy67y45;iotcysnlw7ctny4l55tys;45ty";

    private FileMessage(String receiverPhone, String message) {
        super(receiverPhone, message);
    }

    public static FileMessage fromFilepath(String receiverPhone, String filepath) throws IOException {
        File file = new File(filepath);
        if (!file.exists() || !file.isFile() || !file.canRead())
            throw new IllegalStateException("Cannot find specified file");

        StringBuilder messageBuilder = new StringBuilder(file.getName());
        messageBuilder.append(FILE_MESSAGE_BOUNDARY);
        byte[] bytes = FileUtils.readFileToByteArray(file);
        messageBuilder.append(Base64.getEncoder().encodeToString(bytes));

        return new FileMessage(receiverPhone, messageBuilder.toString());
    }
}
