#include "pqp/query_runner_product.h"

#include <vector>

#include "base/debug.h"
#include "pqp/where_clause_helper_select.h"

QueryRunnerProduct::QueryRunnerProduct(QueryNode *query_node)
  : QueryRunner(query_node),
    error_code_(SqlErrors::NO_ERROR) {
}

QueryRunnerProduct::~QueryRunnerProduct() {
}

bool QueryRunnerProduct::Run(QueryResultCallback callback,
    SqlErrors::Type& error_code) {
  if (Node() == nullptr || Node()->ChildrenCount() != 2) {
    DEBUG_MSG("");
    error_code = SqlErrors::ERROR_CROSS_PRODUCT;
    return false;
  }

  SetCallback(callback);

  QueryNode *left_child = Node()->Child(0);
  QueryNode *right_child = Node()->Child(1);
  if (right_child->Type() == QueryNode::QUERY_NODE_TYPE_CROSS_PRODUCT) {
    SetChildRunner(Create(right_child));
    table_scan_child_ = Create(left_child);
  } else {
    SetChildRunner(Create(left_child));
    table_scan_child_ = Create(right_child);
  }

  ScanParams params;
  if (ChildRunner()->NodeType() != QueryNode::QUERY_NODE_TYPE_CROSS_PRODUCT) {
    params.num_blocks_ = 1;
  }

  ChildRunner()->PassScanParams(params);

  if (!ChildRunner()->Run(
      std::bind(&QueryRunnerProduct::ResultCallback,
          this, std::placeholders::_1, std::placeholders::_2),
      error_code)) {
    DEBUG_MSG("");
    if (error_code_ == SqlErrors::NO_ERROR) {
      error_code_ = SqlErrors::ERROR_CROSS_PRODUCT;
    }

    error_code = error_code_;
    return false;
  }

  return true;
}

bool QueryRunnerProduct::ResultCallback(std::vector<Tuple>& tuples,
    bool headers) {
  if (headers) {
    DEBUG_MSG("");
    if (first_headers_.empty()) {
      first_headers_ = tuples;
      return true;
    }

    std::vector<Tuple> merged_headers;
    if (!MergeTableHeaders(first_headers_, tuples, merged_headers)) {
      DEBUG_MSG("");
      return false;
    }

    return Callback()(merged_headers, headers);
  }

  ScanParams params;
  if (ChildRunner()->NodeType() != QueryNode::QUERY_NODE_TYPE_CROSS_PRODUCT) {
    params.num_blocks_ = 1;
    params.use_begin_blocks_ = false;
  }

  ChildRunner()->PassScanParams(params);

  if (!table_scan_child_->Run(
      std::bind(&QueryRunnerProduct::ResultCallback,
          this, std::placeholders::_1, std::placeholders::_2),
      error_code_)) {
    DEBUG_MSG("");
    error_code_ = SqlErrors::ERROR_CROSS_PRODUCT;
    return false;
  }

  return true;
}
