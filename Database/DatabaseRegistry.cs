using System.Data.Common;
using System.Diagnostics;
using DPS.Database;
using LinqToDB;
using LinqToDB.Data;

namespace DSP.Database;

public class DatabaseRegistry {
    public readonly record struct DatabaseSource(DbConnectionStringBuilder builder, string provider);

    private static readonly IEnumerable<string> AvailableProviders = new[] {
        ProviderName.PostgreSQL,
        ProviderName.MySqlOfficial
    };

    private DatabaseSource ActiveSource { get; set; }

    public DatabaseRegistry(ILogger<DatabaseRegistry> logger) {
        DataConnection.WriteTraceLine = (message, displayName, level) => {
            var commonLevel = level switch {
                TraceLevel.Error => LogLevel.Error,
                TraceLevel.Warning => LogLevel.Warning,
                TraceLevel.Info => LogLevel.Information,
                TraceLevel.Verbose => LogLevel.Trace,
                _ => LogLevel.None
            };

            logger.Log(commonLevel, "{Message}", message);
        };
        DataConnection.TraceSwitch.Level = TraceLevel.Verbose;
        ActiveSource = new DatabaseSource();
    }

    public void ConfigureDatabase(DatabaseSource source) {
        Debug.Assert(AvailableProviders.Contains(source.provider));

        DataConnection.AddOrSetConfiguration(source.builder.ToString(), source.builder.ConnectionString, source.provider);
    }

    public void ActivateSource(DatabaseSource source) {
        ActiveSource = source;
    }

    public Connection CurrentConnection => new Connection(ActiveSource.provider, ActiveSource.builder.ConnectionString);
}