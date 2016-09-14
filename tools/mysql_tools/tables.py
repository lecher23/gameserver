cargo_info = [
    ("cargo_id", "int not null primary key"),
    ("price", "bigint not null"),
    ("item_type", "tinyint default 1"),
    ("item_num", "bigint not null"),
    ("extra", "int default 0"),
    ("sales_type", "tinyint default 0"),
    ("vip_point", "bigint default 0"),
    ("display", 'bool default "1"')
]

f_history = [
    ("uid", "INT NOT NULL PRIMARY KEY"),
    ("invite_count", "int NOT NULL DEFAULT 0"),
    ("reward_total", "BIGINT DEFAULT 0"),
    ("reward_remain", "BIGINT DEFAULT 0")
]

user_info = [
    ('uid', 'INT AUTO_INCREMENT PRIMARY KEY'),
    ('mid', 'CHAR(48) NOT NULL'),
    ('fname', 'CHAR(24)'),
    ('lname', 'CHAR(24)'),
    ('avatar', 'INT DEFAULT 0'),
    ('male', 'BOOL DEFAULT 1'),
    ('country', 'INT DEFAULT 86'),
]

user_resource = [
    ('uid', 'INT NOT NULL PRIMARY KEY'),
    ('level', 'INT DEFAULT 1'),
    ('exp', 'BIGINT DEFAULT 0'),
    ('fortune', 'BIGINT DEFAULT 0'),
    ('vip_level', 'INT DEFAULT 0'),
    ('vip_point', 'BIGINT DEFAULT 0'),
    ('vip_tmp', 'INT DEFAULT 0'),
    ('vip_tmp_end', 'BIGINT DEFAULT 0')
]

history = [
    ('uid', 'INT NOT NULL'),
    ('max_fortune', 'BIGINT DEFAULT 0'),
    ('max_earned', 'BIGINT DEFAULT 0'),
    ('total_earned', 'BIGINT DEFAULT 0'),
    ('total_bet', 'BIGINT DEFAULT 0'),
    ('tw_earned', 'BIGINT DEFAULT 0'),
    ('lw_earned_sort', 'INT DEFAULT 999'),
    ('lw_level_sort', 'INT DEFAULT 9999'),
    ('lw_fortune_sort', 'INT DEFAULT 9999'),
    ('lw_acheivement_sort', 'INT DEFAULT 9999')
]

table_defines = {
    "cargo_info": cargo_info
}

def parse_dict_to_db_sql(table_name, table_define = []):
    sql_tpl = "CREATE TABLE %s (%s)"
    col_sql = ", ".join([t[0] + " " + t[1] for t in table_define])
    table_sql = sql_tpl % (table_name, col_sql)
    return table_sql
