#include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class Page
{
    int pageType;
    string entityName;
    string pageIndex;
    int columnCount; //not applicable for matrices
    int rowCount; // for table
    int elementCount; //for matrix
    vector<vector<int>> tableRows;
    
    public:

    vector<int> matrixElements;
    string pageName = "";
    Page();
    Page(string entityName, int pageIndex,int pageType);
    Page(string entityName, int pageIndex, vector<vector<int>> rows, int rowCount);
    void tablesPage(string tableName,int pageIndex);
    void matrixPage(string matrixName,int pageIndex);
    Page(string entityName,int pageIndex,vector<int> elements,int elementCount);
    vector<int> getRow(int rowIndex);
    //int getVector(int entityIndex);
    void writeTablePage();
    void writeMatrixPage();
};