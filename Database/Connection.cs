using LinqToDB.Data;

namespace DPS.Database;

public partial class Connection : DataConnection {
    public DataContext Femdb { get; set; } = null;

    private void InitSchemas() {
        Femdb = new DataContext(this);
    }

    public Connection() {
        InitSchemas();
    }

    public Connection(string provider, string config) : base(provider, config) {
        InitSchemas();
    }
}
