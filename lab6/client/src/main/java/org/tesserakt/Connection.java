package org.tesserakt;

import Cell.*;
import org.omg.CORBA.ORB;
import org.omg.CORBA.ORBPackage.InvalidName;
import org.omg.CosNaming.NameComponent;
import org.omg.CosNaming.NamingContextExt;
import org.omg.CosNaming.NamingContextExtHelper;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAHelper;
import org.omg.PortableServer.POAManagerPackage.AdapterInactive;
import org.omg.PortableServer.POAPackage.ServantAlreadyActive;
import org.omg.PortableServer.POAPackage.ServantNotActive;
import org.omg.PortableServer.POAPackage.WrongPolicy;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Connection implements AutoCloseable {
    private final ORB _connection;
    private final POA _poaHelper;
    private TubeCallback _callbackRef = null;
    private final ClientImpl _impl;
    private Station _stationRef = null;
    private final Logger _logger = LoggerFactory.getLogger(Connection.class);

    public Connection(String[] args, String phoneNumber) throws AdapterInactive {
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
        _impl = new ClientImpl(phoneNumber);
    }

    public TubeCallback getTubeRef() throws ServantNotActive {
        if (_callbackRef == null) {
            try {
                _poaHelper.activate_object(_impl);
                _callbackRef = TubeCallbackHelper.narrow(_poaHelper.servant_to_reference(_impl));
            } catch (ServantAlreadyActive | WrongPolicy e) {
                _logger.error("Servant already activated in current orb", e);
            }
        }
        return _callbackRef;
    }

    public void connectToStation(String stationName) throws NotFound, ServantNotActive, IllegalStateException {
        try {
            TubeCallback ref = getTubeRef();
            NamingContextExt nameService = NamingContextExtHelper.narrow(_connection.resolve_initial_references("NameService"));
            NameComponent[] nameComponent = new NameComponent[]{new NameComponent(stationName, "")};
            _stationRef = StationHelper.narrow(nameService.resolve(nameComponent));

            try {
                _stationRef.register(ref, ref.getNum());
                _connection.run();
            } catch (WrongPhone e) {
                throw new IllegalStateException("Phone didn't registered in station");
            }
        } catch (InvalidName | org.omg.CosNaming.NamingContextPackage.InvalidName | CannotProceed e) {
            _logger.error("Wrong name to station used", e);
        }
    }

    public void sendSMS(String to, String message) throws WrongReceiver {
        _stationRef.sendSMS(_impl.getNum(), to, message);
    }

    @Override
    public void close() {
        _connection.shutdown(true);
    }
}
