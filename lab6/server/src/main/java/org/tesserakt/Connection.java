package org.tesserakt;

import Cell.Station;
import Cell.StationHelper;
import org.omg.CORBA.ORB;
import org.omg.CORBA.ORBPackage.InvalidName;
import org.omg.CORBA.Object;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Connection implements AutoCloseable {
    private final ORB _connection;
    private final POA _poaHelper;
    private final Logger _logger = LoggerFactory.getLogger(Connection.class);
    private final ServerImpl _impl;
    private Station _stationRef = null;
    private final String _stationName;

    public Connection(String[] args, String name, MessageDatabase database) throws AdapterInactive {
        _stationName = name;
        POA _poaHelperHack;
        _connection = ORB.init(args, null);
        try {
            _poaHelperHack = POAHelper.narrow(_connection.resolve_initial_references("RootPOA"));
            _poaHelperHack.the_POAManager().activate();
        } catch (InvalidName e) {
            _logger.error("Unknown service `RootPOA`", e);
            _poaHelperHack = null;
        }

        _poaHelper = _poaHelperHack;
        _impl = new ServerImpl(database::saveMessage);
    }

    public Station getStationRef() throws WrongPolicy, ServantNotActive, NotFound {
        if (_stationRef == null) {
            try {
                _stationRef = StationHelper.narrow(_poaHelper.servant_to_reference(_impl));
                Object nameServiceRef = _connection.resolve_initial_references("NameService");
                NamingContextExt nameService = NamingContextExtHelper.narrow(nameServiceRef);

                nameService.rebind(nameService.to_name(_stationName), _stationRef);
            } catch (InvalidName | org.omg.CosNaming.NamingContextPackage.InvalidName | CannotProceed e) {
                _logger.error("Error occurred", e);
                throw new RuntimeException(e);
            }
        }
        return _stationRef;
    }

    public void listenForClients() throws WrongPolicy, ServantNotActive, NotFound {
        getStationRef();
        new Thread(() -> {
            while (!Thread.interrupted()) {
                _stationRef.sendACK();
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    break;
                }
            }
        }).start();

        _connection.run();
    }

    @Override
    public void close() {
        _connection.shutdown(true);
    }
}
