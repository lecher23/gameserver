friend_history = [
    ("uid", "INT NOT NULL PRIMARY KEY"),
    ("invite_count", "int NOT NULL DEFAULT 0"),
    ("reward_total", "BIGINT DEFAULT 0"),
    ("reward_remain", "BIGINT DEFAULT 0")
]

game_history = [
    ('uid', 'INT NOT NULL'),
    ('max_fortune', 'BIGINT DEFAULT 0'),
    ('max_earned', 'BIGINT DEFAULT 0'),
    ('total_earned', 'BIGINT DEFAULT 0'),
    ('total_bet', 'BIGINT DEFAULT 0'),
    ('last_login', 'BIGINT DEFAULT 0'),
    ('login_days', 'INT DEFAULT 0'),
    ('jackpot', 'INT DEFAULT 0')
]

theme_history = [
    ('uid', 'INT NOT NULL'),
    ('theme_id', 'INT NOT NULL'),
    ('tag', 'INT NOT NULL'), # history type
    ('value', 'INT DEFAULT 0'),
    ('', 'PRIMARY KEY(uid, theme_id, tag)')
]

cargo_history = [
    ('id','INT AUTO_INCREMENT PRIMARY KEY'),
    ('uid', 'INT NOT NULL'),
    ('cid', 'CHAR(6) NOT NULL'),
    ('tsid', 'CHAR(64) NOT NULL'),
    ('value', 'BIGINT NOT NULL'),
    ('timestamp', 'BIGINT NOT NULL')
]
