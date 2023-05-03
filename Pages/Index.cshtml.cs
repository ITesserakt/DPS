using DPS.Database;
using DSP.Database;
using Microsoft.AspNetCore.Mvc.RazorPages;

namespace DPS.Pages;

public class IndexModel : PageModel {
    public readonly record struct Point(double X, double Y);
    public readonly record struct Triangle(Point a, Point b, Point c);

    public double Scale => 5;
    public double PointSize => 5;

    private readonly ILogger<IndexModel> _logger;
    private readonly DatabaseRegistry _databases;

    public IEnumerable<Triangle> Triangles { get; private set; }
    public IEnumerable<Point> Points => Triangles.SelectMany(x => new[] { x.a, x.b, x.c }).Distinct();

    public IndexModel(ILogger<IndexModel> logger, DatabaseRegistry databases) {
        _logger = logger;
        _databases = databases;
        Triangles = Array.Empty<Triangle>();
    }

    public void OnGet() {
        using var db = _databases.CurrentConnection;
        _logger.LogInformation("Opened a fresh connection to db. Trying to fetch data from it...");

        var query = from element in db.Femdb.Elements
                    join n1 in db.Femdb.Nodes on element.n1 equals n1.id
                    join n2 in db.Femdb.Nodes on element.n2 equals n2.id
                    join n3 in db.Femdb.Nodes on element.n3 equals n3.id
                    select new Triangle(new Point(n1.x, -n1.y), new Point(n2.x, -n2.y), new Point(n3.x, -n3.y));

        Triangles = query.ToArray();
        var minX = Points.MinBy(x => x.X).X;
        var minY = Points.MinBy(x => x.Y).Y;

        Triangles = Triangles.Select(x => new Triangle(
            new Point((x.a.X - minX) * Scale + PointSize, (x.a.Y - minY) * Scale + PointSize),
            new Point((x.b.X - minX) * Scale + PointSize, (x.b.Y - minY) * Scale + PointSize),
            new Point((x.c.X - minX) * Scale + PointSize, (x.c.Y - minY) * Scale + PointSize)
        ));

        _logger.LogInformation("Collected all triangles");
    }
}
