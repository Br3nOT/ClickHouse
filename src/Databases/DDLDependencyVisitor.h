#pragma once
#include <Core/QualifiedTableName.h>
#include <Parsers/IAST_fwd.h>
#include <Interpreters/InDepthNodeVisitor.h>

namespace DB
{

class ASTFunction;
class ASTFunctionWithKeyValueArguments;
class ASTStorage;

using TableNamesSet = std::unordered_set<QualifiedTableName>;

TableNamesSet getDependenciesSetFromCreateQuery(ContextPtr global_context, const QualifiedTableName & table, const ASTPtr & ast);


class DDLMatcherBase
{
public:
    static bool needChildVisit(const ASTPtr & node, const ASTPtr & child);
    static ssize_t getPositionOfTableNameArgument(const ASTFunction & function);
};

/// Visits ASTCreateQuery and extracts names of table (or dictionary) dependencies
/// from column default expressions (joinGet, dictGet, etc)
/// or dictionary source (for dictionaries from local ClickHouse table).
/// Does not validate AST, works a best-effort way.
class DDLDependencyVisitor : public DDLMatcherBase
{
public:
    struct Data
    {
        String default_database;
        TableNamesSet dependencies;
        ContextPtr global_context;
        ASTPtr create_query;
    };

    using Visitor = ConstInDepthNodeVisitor<DDLDependencyVisitor, true>;

    static void visit(const ASTPtr & ast, Data & data);

private:
    static void visit(const ASTFunction & function, Data & data);
    static void visit(const ASTFunctionWithKeyValueArguments & dict_source, Data & data);
    static void visit(const ASTStorage & storage, Data & data);

    static void extractTableNameFromArgument(const ASTFunction & function, Data & data, size_t arg_idx);
};

class NormalizeAndEvaluateConstants : public DDLMatcherBase
{
public:
    struct Data
    {
        ContextPtr create_query_context;
    };

    using Visitor = ConstInDepthNodeVisitor<NormalizeAndEvaluateConstants, true>;

    static void visit(const ASTPtr & ast, Data & data);

private:
    static void visit(const ASTFunction & function, Data & data);
    static void visit(const ASTFunctionWithKeyValueArguments & dict_source, Data & data);

};

using NormalizeAndEvaluateConstantsVisitor = NormalizeAndEvaluateConstants::Visitor;

}
