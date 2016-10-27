#include "gridsconfig.h"

BEGIN_NAMESPACE(slots)

GridsConfig::GridsConfig(){
}

GridsConfig::~GridsConfig(){
}

int32_t GridsConfig::start() {
    return 0;
}

int32_t GridsConfig::end() {
    return _cfg.size();
}

const RawGridItem &GridsConfig::getGrid(int32_t idx){
    return _cfg[idx];
}

bool GridsConfig::parseJsonDoc(rapidjson::Document &doc) {
    static const std::string sCol = "Col";
    static const std::string sRow = "Row";
    static const std::string sEleID = "Ele_ID";
    static const std::string sWeight = "Pro";

    rapidjson::Value::MemberIterator itr;
    for (int i = 0; i < doc.Capacity(); ++i) {
        GET_MEMBER_IN_DOC(doc[i], itr, sCol, colValue);
        GET_MEMBER_IN_DOC(doc[i], itr, sRow, rowValue);
        GET_MEMBER_IN_DOC(doc[i], itr, sEleID, eleValue);
        GET_MEMBER_IN_DOC(doc[i], itr, sWeight, weightValue);
        auto row = rowValue.GetInt();
        rowSize = row > rowSize ? row : rowSize;
        auto col = colValue.GetInt();
        colSize = col > colSize ? col : colSize;
        _cfg.push_back(
            RawGridItem(col, row, eleValue.GetInt(), weightValue.GetInt()));
    }
    return true;
}


END_NAMESPACE
