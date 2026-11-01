#pragma once

#include "finance/database/domain_schema.hpp"
#include "finance/database/migration_commands.hpp"

namespace finance::database::migrations {

const DomainMigration CURRENCY_MIGRATION_V1{
    .domain = domain::CURRENCY,
    .version = 1,
    .commands = {CreateTableCommand{
        .name = "currency_v1",
        .columns =
            {
                Column{
                    .name = "code",
                    .comment = "ISO 4217 code (e.g. 'USD',  'JPY')",
                    .type = ColumnType::CHAR, // e.g.
                    .nullable = false,
                    .primaryKey = true,
                },
                Column{
                    .name = "name",
                    .comment = "Name of the currency (e.g. 'US Dollar', "
                               "'Japanese Yen')",
                    .type = ColumnType::VARCHAR,
                    .nullable = false,
                },
                Column{
                    .name = "decimal_places",
                    .type = ColumnType::TINYINT,
                    .nullable = false,
                    .default_value = "2",
                },
                Column{
                    .name = "symbol",
                    .type = ColumnType::VARCHAR,
                    .nullable = false,
                    .default_value = "'$",
                },
            },
    }},
};

} // namespace finance::database::migrations