using DSP.Database;

var builder = WebApplication.CreateBuilder(args);

new ConfigureServices(builder.Services, builder.Configuration)
    .Build();

var app = builder.Build();

app.UseRouting();

app.UseStaticFiles();
app.MapRazorPages();

app.Services.GetRequiredService<DatabaseRegistry>();

app.Run();
