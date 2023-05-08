package org.tesserakt;

import Cell.WrongReceiver;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Main {
    public static void main(String[] args) throws AdapterInactive, ServantNotActive, NotFound {
        Logger logger = LoggerFactory.getLogger(Main.class);
        InteractiveConsole properties = InteractiveConsole.parse(args);

        Connection connection = new Connection(new String[]{
                "-ORBInitialPort", String.valueOf(properties.getPort()),
                "-ORBInitialHost", properties.getHost().getHostName()
        }, properties.getPhoneNumber());

        new Thread(() -> properties.run((msg) -> {
            try {
                connection.sendSMS(msg.getReceiverPhone(), msg.getMessage());
            } catch (WrongReceiver e) {
                logger.error("Phone <{}> isn't registered", msg.getReceiverPhone());
            }
        })).start();

        connection.connectToStation(properties.getStationName());

        connection.close();
    }
}