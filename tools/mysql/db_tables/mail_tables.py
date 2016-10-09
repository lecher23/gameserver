mail_detail = [
    ('mail_id', 'INT NOT NULL AUTO_INCREMENT PRIMARY KEY'),
    ('dest_id', 'INT NOT NULL'),
    ('title', 'VARCHAR(127) NOT NULL'),
    ('content', 'VARCHAR(256) NOT NULL'),
    ('attachment', 'INT DEFAULT 0'),
    ('ctime', 'DATETIME NOT NULL'),
    ('keep_days', 'INT DEFAULT 7')
]

user_mails = [
    ('uid', 'INT NOT NULL'),
    ('mail_id', 'INT NOT NULL'),
    ('b_read', 'BOOL DEFAULT false'),
    ('b_get', 'BOOL DEFAULT false'),
    ('b_delete', 'BOOL DEFAULT false'),
    ('', 'PRIMARY KEY(uid, mail_id)')
]

attachment = [
    ('attid', 'INT AUTO_INCREMENT PRIMARY KEY'),
    ('type', 'INT NOT NULL'),
    ('value', 'VARCHAR(64) NOT NULL')
]
