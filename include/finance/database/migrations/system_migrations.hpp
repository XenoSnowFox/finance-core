#pragma once

#include "finance/database/domain_schema.hpp"
#include "finance/database/migration_commands.hpp"

namespace finance::database::migrations {

const DomainMigration SYSTEM_MIGRATION_V1{
    .domain = "system",
    .version = 1,
    .commands = {CreateTableCommand{
        .name = "schema_domain_versions",
        .columns =
            {
                Column{
                    .name = "domain_name",
                    .type = ColumnType::VARCHAR,
                    .nullable = false,
                    .primaryKey = true,
                },
                Column{
                    .name = "version",
                    .type = ColumnType::INTEGER,
                    .nullable = false,
                    .default_value = "0",
                },
                Column{
                    .name = "updated_at",
                    .type = ColumnType::TIMESTAMP,
                    .nullable = false,
                    .default_value = "CURRENT_TIMESTAMP",
                },
            },
    }},
};

} // namespace finance::database::migrations