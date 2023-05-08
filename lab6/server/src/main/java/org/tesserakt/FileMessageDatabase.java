package org.tesserakt;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.time.Instant;

public class FileMessageDatabase implements MessageDatabase, AutoCloseable {
    private final OutputStreamWriter _output;

    public FileMessageDatabase(String filename) throws IOException {
        _output = new OutputStreamWriter(Files.newOutputStream(Paths.get(filename), StandardOpenOption.CREATE));
    }

    @Override
    public void saveMessage(AuthoredMessage message) {
        try {
            Instant currentTime = Instant.now();
            String formatted = String.format("[%s] - <%s> => <%s>: %s",
                    currentTime.toString(),
                    message.getSenderPhone(),
                    message.getReceiverPhone(),
                    message.getMessage());
            _output.append(formatted);
            _output.flush();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void close() throws Exception {
        _output.close();
    }
}
