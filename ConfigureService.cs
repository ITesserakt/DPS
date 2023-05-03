using System.Data.Common;
using DSP.Database;

public class ConfigureServices {
    private readonly IServiceCollection _services;
    private readonly IConfigurationRoot _configuration;

    DatabaseRegistry LoadDatabasesFromConfig(IServiceProvider services) {
        var connections = _configuration.GetSection("Connections")
            .GetChildren()
            .ToArray();

        var registry = new DatabaseRegistry(services.GetRequiredService<ILogger<DatabaseRegistry>>());
        foreach (var connection in connections) {
            var provider = connection.GetValue<string>("Provider")!;
            var properties = new DbConnectionStringBuilder();
            foreach (var property in connection.GetRequiredSection("Connection").GetChildren())
                properties[property.Key] = property.Value;
            registry.ConfigureDatabase(properties, provider);
            if (connection.GetValue<bool>("Active") == true)
                registry.ActivateSource(properties, provider);
        }
        return registry;
    }

    public ConfigureServices(IServiceCollection services, IConfigurationRoot configuration) {
        _services = services;
        _configuration = configuration;
    }

    public void Build() {
        _services.AddSingleton<DatabaseRegistry>(LoadDatabasesFromConfig)
                 .AddRazorPages();
    }
}