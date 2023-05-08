package org.tesserakt;

import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAPackage.ServantNotActive;

public class Main {
    public static void main(String[] args) throws AdapterInactive, ServantNotActive, NotFound {
        InteractiveConsole properties = InteractiveConsole.parse(args);

        Connection connection = new Connection(new String[]{
                "-ORBInitialPort", String.valueOf(properties.getPort()),
                "-ORBInitialHost", properties.getHost().getHostName()
        }, properties.getPhoneNumber());

        new Thread(() -> properties.run((msg) -> connection.sendSMS(msg.getReceiverPhone(), msg.getMessage())))
                .start();

        connection.connectToStation(properties.getStationName());

        connection.close();
    }
}