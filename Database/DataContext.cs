using LinqToDB;
using DPS.Models;

namespace DPS.Database;

public class DataContext {
    private readonly IDataContext _dataContext;

    public DataContext(IDataContext context) {
        _dataContext = context;
    }

    public ITable<elements> Elements => _dataContext.GetTable<elements>();
    public ITable<loadings> Loadings => _dataContext.GetTable<loadings>();
    public ITable<materials> Materials => _dataContext.GetTable<materials>();
    public ITable<nodes> Nodes => _dataContext.GetTable<nodes>();
}