package org.tesserakt;

import picocli.CommandLine;

import java.net.InetAddress;

import static picocli.CommandLine.*;

@Command(name = "client", helpCommand = true)
public class Console {
    @Option(names = {"-p", "--port"}, defaultValue = "1050")
    private short _port;

    @Option(names = {"-h", "--host"}, defaultValue = "localhost")
    private InetAddress _host;

    @Option(names = {"-s", "--station"}, defaultValue = "Base station")
    private String _stationName;

    public String getStationName() {
        return _stationName;
    }

    public short getPort() {
        return _port;
    }

    public InetAddress getHost() {
        return _host;
    }

    public static Console parse(String[] args) {
        Console console = new Console();
        ParseResult result = new CommandLine(console).parseArgs(args);
        return console;
    }
}
