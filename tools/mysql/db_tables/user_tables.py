user_achievement = [
    ("uid","INT NOT NULL"),
    ("aid", "INT NOT NULL"),
    ("is_recv_reward", "BOOL DEFAULT 0"),
    ("time", "BIGINT DEFAULT 0"),
    ("", "PRIMARY KEY(uid, aid)")
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
