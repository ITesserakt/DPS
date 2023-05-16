package org.tesserakt;

import Cell.TubeCallbackPOA;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Base64;

public class ClientImpl extends TubeCallbackPOA {
    private final String _phoneNumber;
    private final Logger _logger = LoggerFactory.getLogger(ClientImpl.class);

    public ClientImpl(String phoneNumber) {
        _phoneNumber = phoneNumber;
    }

    @Override
    public void receiveSMS(String fromNum, String message) {
        String[] parts = message.split(FileMessage.FILE_MESSAGE_BOUNDARY, 2);
        if (parts.length != 2) {
            System.out.printf("%s -> %s", fromNum, message);
            return;
        }
        String filename = parts[0];
        String contents = parts[1];

        System.out.printf("Received file `%s`", filename);
        Path filepath = new File("downloads/", filename).toPath();
        try (OutputStream download = Files.newOutputStream(filepath,
                StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING)) {
            byte[] fileBytes = Base64.getDecoder().decode(contents);
            download.write(fileBytes);
        } catch (IOException e) {
            _logger.error("Cannot write message contents to file", e);
        }
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
