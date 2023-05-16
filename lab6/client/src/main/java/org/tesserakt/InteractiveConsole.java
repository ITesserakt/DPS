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

    private String readMultilineString(String initial, BufferedReader reader) throws IOException {
        while (initial.endsWith("\\")) {
            initial = initial.replaceAll("\\\\", "\n");
            initial += reader.readLine();
        }
        ;

        return initial;
    }


    public String getPhoneNumber() {
        return _phoneNumber;
    }

    public void run(Consumer<Message> messageHandler) {
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        System.out.println("Message format: <phone>: (<sms> | <mms>) <- <message> [\\ <message continuation>]");

        while (true) {
            try {
                String msgString = in.readLine();
                if (msgString == null) break;
                String[] compound = msgString.split("<-", 2);
                if (compound.length == 1) {
                    System.out.println("Wrong format: <phone>: (<sms> | <mms>) <- <message> [\\ <message continuation>]");
                    continue;
                }

                String[] systemCompound = compound[0].split(":");
                String phone = systemCompound[0].trim();
                String text = readMultilineString(compound[1], in);

                if (systemCompound.length > 1 && systemCompound[1].trim().equals("sms"))
                    messageHandler.accept(new Message(phone, text.trim()));
                else if (systemCompound.length > 1 && systemCompound[1].trim().equals("mms"))
                    messageHandler.accept(FileMessage.fromFilepath(phone, text.trim()));
                else
                    System.out.println("Wrong format: <phone>: (<sms> | <mms>) <- <message> [\\ <message continuation>]");
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
