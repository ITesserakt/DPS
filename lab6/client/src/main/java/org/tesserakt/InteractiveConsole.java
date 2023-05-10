package org.tesserakt;


import picocli.CommandLine;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.function.Consumer;

import static picocli.CommandLine.*;

public class InteractiveConsole extends Console {
    @Parameters(description = "Client phone number to use", index = "0", arity = "1")
    private String _phoneNumber;

    public String getPhoneNumber() {
        return _phoneNumber;
    }

    public void run(Consumer<Message> messageHandler) {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

        while (true) {
            try {
                String msgString = in.readLine();
                if (msgString == null) break;
                String[] compound = msgString.split("<-", 2);
                if (compound.length == 1) {
                    System.out.println("Wrong format: <phone> <- <message> [\\ <message continuation>]");
                } else {
                    String text = compound[1];
                    while (text.endsWith("\\")) {
                        text = text.replaceAll("\\\\", "\n");
                        text += in.readLine();
                    }
                    messageHandler.accept(new Message(compound[0].trim(), text.trim()));
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    public static InteractiveConsole parse(String[] args) {
        InteractiveConsole console = new InteractiveConsole();
        new CommandLine(console).parseArgs(args);
        return console;
    }
}
