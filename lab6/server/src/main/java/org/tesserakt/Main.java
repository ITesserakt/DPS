package org.tesserakt;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;

import java.io.IOException;

public class Main {
    public static void main(String[] args) throws AdapterInactive, WrongPolicy, ServantNotActive, NotFound, IOException {
        Console properties = Console.parse(args);

        Connection connection = new Connection(new String[]{
                "-ORBInitialPort", String.valueOf(properties.getPort()),
                "-ORBInitialHost", properties.getHost().getHostName()
        }, properties.getStationName(), new FileMessageDatabase("messages.dat"));

        connection.listenForClients();

        connection.close();
    }
}