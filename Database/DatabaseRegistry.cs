using System.Data.Common;
using System.Diagnostics;
using LinqToDB;
using LinqToDB.Data;

namespace DSP.Database;

public class DatabaseRegistry {
    private static readonly IEnumerable<string> AvailableProviders = new[] {
        ProviderName.PostgreSQL,
        ProviderName.MySqlOfficial
    };

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
    }

    public void ConfigureDatabase(DbConnectionStringBuilder connectionString, string provider) {
        Debug.Assert(AvailableProviders.Contains(provider));

        DataConnection.AddOrSetConfiguration(connectionString.ToString(), connectionString.ConnectionString, provider);
    }

    public void ActivateSource(DbConnectionStringBuilder dbConnectionString, string provider) {
        DataConnection.DefaultDataProvider = provider;
        DataConnection.DefaultConfiguration = dbConnectionString.ToString();
    }
}