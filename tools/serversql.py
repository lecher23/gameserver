import MySQLdb as mysqldb

HOST = "localhost"
USER = "root"
PASSWD = "111222"


class ServerSql:
    def __init__(self):
        self.host = HOST
        self.user = USER
        self.pwd = PASSWD
        self.conn = None
        self.cursor = None
        self.table_sqls = {}

    def init(self):
        self.generateTableSql();        
        self.conn = mysqldb.connect(host = HOST, user = USER, passwd = PASSWD)
        self.conn.select_db('SLOTS')
        self.cursor = self.conn.cursor()

    def release(self):
        self.conn.close()

    def clearTable(self, table):
        sql = 'delete from %s' % table
        self.cursor.execute(sql)

    def startTransaction(self):
        self.cursor.execute("start transaction")

    def endTransaction(self, success = True):
        self.cursor.execute("commit") if success else self.cursor.execute("rollback")

    def refreshRankData(self):
        self.refreshOrder("user_resource", "cur_fortune_rank", "fortune")
        self.refreshOrder("history", "cur_earned_rank", "total_earned")
        #self.refreshOrder("user_resource", "cur_acmt_rank", "fortune")
        self.refreshLevelOrder()
    
    def refreshOrder(self, src_table, dest_table, colum):
        sql = 'select A.uid, A.fname, A.country, B.%s from user_info as A inner join %s as B on A.uid in (select t.uid from (select uid,%s from %s limit 0,1000) as t ) and A.uid = B.uid order by B.%s desc' % (colum, src_table, colum, src_table, colum)
        count = self.cursor.execute(sql)
        res = self.cursor.fetchmany(count)
        sqls = []
        for r in res:
            sqls.append('insert into %s (uid, fname, country, value) values (%s, "%s", %s, %s)' % (dest_table, r[0], "" if r[1] is None else r[1], 0 if r[2] is None else r[2],r[3]))
        self._transaction(dest_table, sqls)

    def refreshLevelOrder(self):
        sql = 'select A.uid, A.fname, A.country, B.level from user_info as A inner join user_resource as B on A.uid in (select t.uid from (select uid,level,exp from user_resource limit 0,1000) as t ) and A.uid = B.uid order by B.exp desc'
        count = self.cursor.execute(sql)
        res = self.cursor.fetchmany(count)
        sqls = []
        for r in res:
            sqls.append('insert into cur_level_rank (uid, fname, country, value) values (%s, "%s", %s, %s)' % (r[0], "" if r[1] is None else r[1], 0 if r[2] is None else r[2],r[3]))
        self._transaction("cur_level_rank", sqls)

    def saveOrder(self, src_table, dest_table):
        sql = "select * from %s" % src_table
        count = self.cursor.execute(sql)
        res = self.cursor.fetchmany(count)
        sqls = []
        for r in res:
            sqls.append("insert into %s (uid) values (%s)" % (dest_table, r[0]))
        self._transaction(dest_table, sqls)

    ''' Table creator'''
    def createTable(self, table):
        self._dropTable(table)
        self.cursor.execute(self.table_sqls[table])

    def createRankTables(self):
        tables = ["cur_level_rank", "cur_fortune_rank", "cur_earned_rank", "cur_acmt_rank",
                  "lw_level_rank", "lw_fortune_rank", "lw_earned_rank", "lw_acmt_rank",
                  "tw_earned_rank"]
        for table in tables:
            self.createTable(table)
            
    def generateTableSql(self):
        table = "achievement"
        sql='create table %s(uid INT NOT NULL, aid INT NOT NULL, detail CHAR(255) DEFAULT "", get_reward BOOL DEFAULT 0, gain_time BIGINT NOT NULL, PRIMARY KEY(uid, aid))' % table
        self.table_sqls[table] = sql
        
        table = "g_history"
        sql = 'create table %s( uid INT NOT NULL PRIMARY KEY, friend_num INT DEFAULT 0, friend_gifts_num INT DEFAULT 0, consitive_login INT DEFAULT 0, tiny_game_times INT DEFAULT 0, bigwin varchar(255) DEFAULT "0", megawin varchar(255) DEFAULT "0", free_times varchar(255) DEFAULT "0", game_times varchar(255) DEFAULT "0", jackpot_times varchar(255) DEFAULT "0", g_jackpot_times INT DEFAULT "0", four_line_times varchar(255) DEFAULT "0", five_line_times varchar(255) DEFAULT "0")' % table
        self.table_sqls[table] = sql

        table = "m_attachment"
        sql = "create table %s(attid BIGINT AUTO_INCREMENT PRIMARY KEY, type INT NOT NULL, value varchar(64) NOT NULL)" % table
        self.table_sqls[table] = sql

        tables = ["cur_level_rank", "cur_fortune_rank", "cur_earned_rank", "cur_acmt_rank",
                  "lw_level_rank", "lw_fortune_rank", "lw_earned_rank", "lw_acmt_rank",
                  "tw_earned_rank"]
        for table in tables:
            sql = "create table %s(id BIGINT AUTO_INCREMENT PRIMARY KEY, uid INT NOT NULL, fname CHAR(24) DEFAULT \"Guest\", country INT NOT NULL DEFAULT 86, value BIGINT NOT NULL)" % table
            self.table_sqls[table] = sql
        

    ''' Help function'''
    def _transaction(self, dest_table, sqls):
        self.startTransaction()
        try:
            self.clearTable(dest_table)            
            for sql in sqls:
                self.cursor.execute(sql)
        except Exception, e:
            print e
            self.endTransaction(False)
            return
        self.endTransaction()

    def _dropTable(self, table):
        try:
            sql = "drop table %s" % table
            self.cursor.execute(sql)
        except Exception, e:
            print "Table not exist."

if __name__ == "__main__":
    
    lbs = ServerSql()
    lbs.init()
    #lbs.refreshRankData()
    #lbs.createRankTables();
    #lbs.createAchievementTable()
    #lbs.createAttachmentTable()

