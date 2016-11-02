cargo_config = [
    ("cargo_id", "int not null primary key"),
    ("price", "bigint not null"),
    ("item_type", "tinyint default 1"),
    ("item_num", "bigint not null"),
    ("extra", "int default 0"),
    ("sales_type", "tinyint default 0"),
    ("vip_point", "bigint default 0"),
    ("display", 'bool default "1"')
]

theme_common_cfg = [
    ('code', 'INT NOT NULL PRIMARY KEY'),
    ('value', 'BIGINT NOT NULL'),
    ('extra', 'BIGINT DEFAULT 0')
]
