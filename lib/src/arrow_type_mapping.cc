#include "duckdb/web/arrow_type_mapping.h"

#include <arrow/result.h>
#include <arrow/status.h>
#include <arrow/type_fwd.h>

#include "arrow/type.h"
#include "duckdb/common/types.hpp"
#include "duckdb/web/webdb.h"

namespace duckdb {
namespace web {

/// Map arrow type
arrow::Result<duckdb::LogicalType> mapArrowType(const arrow::DataType& type) {
    switch (type.id()) {
        case arrow::Type::type::MAX_ID:
        case arrow::Type::type::NA:
            return duckdb::LogicalType::INVALID;
        case arrow::Type::type::BOOL:
            return duckdb::LogicalType::BOOLEAN;
        case arrow::Type::type::UINT8:
            return duckdb::LogicalType::UTINYINT;
        case arrow::Type::type::INT8:
            return duckdb::LogicalType::TINYINT;
        case arrow::Type::type::UINT16:
            return duckdb::LogicalType::USMALLINT;
        case arrow::Type::type::INT16:
            return duckdb::LogicalType::SMALLINT;
        case arrow::Type::type::UINT32:
            return duckdb::LogicalType::UINTEGER;
        case arrow::Type::type::INT32:
            return duckdb::LogicalType::INTEGER;
        case arrow::Type::type::UINT64:
            return duckdb::LogicalType::UBIGINT;
        case arrow::Type::type::INT64:
            return duckdb::LogicalType::BIGINT;
        case arrow::Type::type::HALF_FLOAT:
            return duckdb::LogicalType::FLOAT;
        case arrow::Type::type::FLOAT:
            return duckdb::LogicalType::FLOAT;
        case arrow::Type::type::DOUBLE:
            return duckdb::LogicalType::DOUBLE;
        case arrow::Type::type::STRING:
            return duckdb::LogicalType::VARCHAR;
        case arrow::Type::type::BINARY:
            return duckdb::LogicalType::BLOB;
        case arrow::Type::type::FIXED_SIZE_BINARY:
            return duckdb::LogicalType::BLOB;
        case arrow::Type::type::DATE32:
            return duckdb::LogicalType::DATE;
        case arrow::Type::type::DATE64:
            return duckdb::LogicalType::DATE;
        case arrow::Type::type::TIMESTAMP:
            return duckdb::LogicalType::TIMESTAMP;
        case arrow::Type::type::TIME32:
            return duckdb::LogicalType::TIMESTAMP;
        case arrow::Type::type::TIME64:
            return duckdb::LogicalType::TIMESTAMP;
        case arrow::Type::type::INTERVAL_MONTHS:
            return duckdb::LogicalType::INTERVAL;
        case arrow::Type::type::INTERVAL_DAY_TIME:
            return duckdb::LogicalType::INTERVAL;
        case arrow::Type::type::DECIMAL128: {
            auto& decimal = *dynamic_cast<const arrow::Decimal128Type*>(&type);
            return duckdb::LogicalType::DECIMAL(decimal.precision(), decimal.scale());
        }
        case arrow::Type::type::DECIMAL256: {
            auto& decimal = *dynamic_cast<const arrow::Decimal256Type*>(&type);
            return duckdb::LogicalType::DECIMAL(decimal.precision(), decimal.scale());
        }
        case arrow::Type::type::LIST: {
            ARROW_ASSIGN_OR_RAISE(auto inner, mapArrowType(*dynamic_cast<const arrow::ListType*>(&type)->value_type()));
            return duckdb::LogicalType::LIST(inner);
        }
        case arrow::Type::type::STRUCT: {
            child_list_t<LogicalType> children;
            for (auto& field : type.fields()) {
                ARROW_ASSIGN_OR_RAISE(auto t, mapArrowType(*field->type()));
                children.push_back({field->name(), t});
            }
            return duckdb::LogicalType::STRUCT(children);
        }
        case arrow::Type::type::DICTIONARY: {
            child_list_t<LogicalType> children;
            for (auto& field : type.fields()) {
                ARROW_ASSIGN_OR_RAISE(auto t, mapArrowType(*field->type()));
                children.push_back({field->name(), t});
            }
            return duckdb::LogicalType::MAP(children);
        }
        case arrow::Type::type::MAP: {
            child_list_t<LogicalType> children;
            for (auto& field : type.fields()) {
                ARROW_ASSIGN_OR_RAISE(auto t, mapArrowType(*field->type()));
                children.push_back({field->name(), t});
            }
            return duckdb::LogicalType::MAP(children);
        }
        case arrow::Type::type::FIXED_SIZE_LIST: {
            ARROW_ASSIGN_OR_RAISE(auto inner,
                                  mapArrowType(*dynamic_cast<const arrow::FixedSizeListType*>(&type)->value_type()));
            return duckdb::LogicalType::LIST(inner);
        }
        case arrow::Type::type::DURATION:
            return duckdb::LogicalType::TIME;
        case arrow::Type::type::LARGE_STRING:
            return duckdb::LogicalType::VARCHAR;
        case arrow::Type::type::LARGE_BINARY:
            return duckdb::LogicalType::BLOB;
        case arrow::Type::type::LARGE_LIST: {
            ARROW_ASSIGN_OR_RAISE(auto inner,
                                  mapArrowType(*dynamic_cast<const arrow::LargeListType*>(&type)->value_type()));
            return duckdb::LogicalType::LIST(inner);
        }
        case arrow::Type::type::EXTENSION:
        case arrow::Type::type::SPARSE_UNION:
        case arrow::Type::type::DENSE_UNION:
            return arrow::Status::NotImplemented("DuckDB type mapping for: ", type.ToString());
    }
    return duckdb::LogicalType::INVALID;
}

}  // namespace web
}  // namespace duckdb