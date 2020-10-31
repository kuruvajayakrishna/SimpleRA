#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->entityName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->tableRows.clear();
    this->matrixElements.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string entityName, int pageIndex,int entityType)
{
    if(entityType == 0)
    {
        this->pageType = entityType;
        tablesPage(entityName,pageIndex);
    }
    else if(entityType == 1)
    {
        this->pageType = entityType;
        matrixPage(entityName,pageIndex); 
    }   
    
}


void Page::tablesPage(string tableName,int pageIndex)
{
    logger.log("Page::Page");
    this->entityName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->entityName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    uint maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->tableRows.assign(maxRowCount, row);
    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->tableRows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

void Page::matrixPage(string matrixName,int pageIndex)
{
    logger.log("Page::Page");
    this->entityName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->entityName + "_Page" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    //this->columnCount = matrix.matrixDim;
    uint maxElementCount = matrix.maxElementsPerBlock;
    this->matrixElements.assign(maxElementCount,0);
    ifstream fin(pageName, ios::in);
    this->elementCount = matrix.elementsPerBlockCount[pageIndex];
    int number;
    
    for (int elementCounter = 0; elementCounter < this->elementCount; elementCounter++)
    {
        fin >> number;
        this->matrixElements[elementCounter] = number;
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->tableRows[rowIndex];
}

/*int Page::getVector(int elementIndex)
{
    logger.log("Page::getRow");
    int result = -1;
    if (elementIndex >= this->elementCount)
        return result;
    return this->matrixElements[elementIndex];
}*/



Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    this->pageType = 0;
    logger.log("Page::Page");
    this->entityName = tableName;
    this->pageIndex = pageIndex;
    this->tableRows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->entityName + "_Page" + to_string(pageIndex);
}


Page::Page(string matrixName, int pageIndex, vector<int> elements, int elementCount)
{
    this->pageType = 1;
    logger.log("Page::Page");
    this->entityName = matrixName;
    this->pageIndex = pageIndex;
    this->matrixElements = elements;
    this->elementCount = elementCount;
    //this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->entityName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writeTablePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->tableRows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}


void Page::writeMatrixPage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    
    for (int elementCounter = 0; elementCounter < this->elementCount; elementCounter++)
    {
        if (elementCounter != 0)
            fout << " ";
        fout << this->matrixElements[elementCounter];
    }
        fout << endl;

    fout.close();
}
