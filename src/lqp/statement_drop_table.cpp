#include "lqp/statement_drop_table.h"

#include "base/debug.h"

StatementDropTable::StatementDropTable(const SqlNode *root_node)
  : Statement(root_node) {
}

StatementDropTable::~StatementDropTable() {
}

void StatementDropTable::Execute(SqlErrors::Type& error_code) {
  if (RootNode() == nullptr ||
      RootNode()->Type() != SqlNode::NODE_TYPE_DROP_TABLE_STATEMENT) {
    DEBUG_MSG("");
    error_code = SqlErrors::UNACCEPTABLE_STATEMENT;
    return;
  }

  std::string table_name;
  if (!RootNode()->Child(0)->TableName(table_name)) {
    DEBUG_MSG("");
    error_code = SqlErrors::INVALID_TABLE_NAME;
    return;
  }

  if (!Storage()->DeleteRelation(table_name)) {
    DEBUG_MSG("");
    error_code = SqlErrors::INEXISTENT_TABLE;
  }
}

bool StatementDropTable::SetTables(const std::vector<std::string> tables) {
  if (tables.size() != 1) {
    DEBUG_MSG("");
    return false;
  }

  return Statement::SetTables(tables);
}
