
class Matrix
{
public:
    string sourceFileName = "";
    string matrixName = "";
    uint matrixDim = 0;
    uint blockCount = 0;
    uint maxElementsPerBlock = 0;
    vector<uint> elementsPerBlockCount;
    /*uint maxRowsPerBlock = 0;
    vector<uint> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn = "";
    IndexingStrategy indexingStrategy = NOTHING;*/
    
    //bool extractColumnNames(string firstLine);
    bool blockify();
    //void updateStatistics(vector<int> row);
    Matrix();
    Matrix(string matrixName);
    bool load();
    //bool isColumn(string columnName);
    //void renameColumn(string fromColumnName, string toColumnName);
    //void print();
    void makePermanent();
    bool isPermanent();
    void getNextPage(Cursor *cursor);
    Cursor getCursor();
    //int getColumnIndex(string columnName);
    void unload();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout)
{
    logger.log("Matrix::printRow");
    for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
    {
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << endl;
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Matrix::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}
};
