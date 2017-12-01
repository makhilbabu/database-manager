#ifndef SRC_PQP_QUERY_RUNNER_PROJECTION_H
#define SRC_PQP_QUERY_RUNNER_PROJECTION_H

#include "base/sql_errors.h"
#include "pqp/query_runner.h"

class QueryRunnerProjection : public QueryRunner {
 public:
  QueryRunnerProjection(QueryNode *query_node);
  ~QueryRunnerProjection();

  bool Run(QueryResultCallback callback, SqlErrors::Type& error_code) final;
  bool ResultCallback(std::vector<Tuple>& tuples) final;

 private:
  QueryRunner *child_runner_;
  QueryResultCallback callback_;
};

#endif // SRC_PQP_QUERY_RUNNER_PROJECTION_H