#include "global.h"

Cursor::Cursor(string entityName, int pageIndex,int entityType)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(entityName, pageIndex,entityType);
    this->pagePointer = 0;
    this->entityName = entityName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->entityName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}


Page Cursor::getPageContainingElement(int rowIndex,int columnIndex)
{
    string matrixName = this->entityName;
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);
    long long int elementNumber = (rowIndex + 1) * (matrix.matrixDim) + (columnIndex + 1);
    pageIndex = int(elementNumber / matrix.maxElementsPerBlock);
    Page page = bufferManager.getPage(matrixName,pageIndex,1);
    return page;    
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->entityName, pageIndex,0);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

