#include <iostream>
#include <sstream>
#include <string>

#include "base/debug.h"
#include "base/sql_errors.h"
#include "query_manager.h"

int main(int argc, char *argv[]) {
  logging::DisableConsoleLogs();

  // Start query Manager
  QueryManager *query_manager = QueryManager::Get();

  // Main run loop
  while (true) {
    std::string new_line;
    std::cout.clear();

    std::cout << "TinySQL> ";
    std::getline(std::cin, new_line);
    std::cout.setstate(std::ios_base::failbit);

    // Set Query
    SqlErrors::Type error_code = SqlErrors::NO_ERROR;
    query_manager->SetQuery(new_line, error_code);
    if (error_code == SqlErrors::EMPTY_STATEMENT) {
      std::cout << std::endl;
      continue;
    }

    // Start executing query
    query_manager->ExecuteQuery(error_code);

    ERROR_MSG(error_code << std::endl);
  }

  delete query_manager;
  return 0;
}
