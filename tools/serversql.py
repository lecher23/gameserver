import MySQLdb as mysqldb
import sys 
#from optparse import OptionParser
import getopt
import random

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

    def doSpecial(self):
        # exp_range = (1, 5000000)
        # fortune_range = (0, 10000000)
        # level_range = (1, 50)
        # uid_range = (14116, 15114)
        # self.startTransaction()
        # try:
        #     for i in range(uid_range[0], uid_range[1]):
        #         sql = "update user_resource set level=%d, exp=%d, fortune=%d where uid=%d" \
        #               % (random.randint(level_range[0], level_range[1]),
        #                  random.randint(exp_range[0], exp_range[1]),
        #                  random.randint(fortune_range[0], fortune_range[1]), i)
        #         print sql
        #         self.cursor.execute(sql)
        #         sql = "update history set total_earned=%d, tw_earned=%d where uid=%d" \
        #               %(random.randint(exp_range[0], exp_range[1]),
        #                 random.randint(fortune_range[0], fortune_range[1]), i)
        #         print sql
        #         self.cursor.execute(sql)
        # except Exception,e:
        #     print e
        #     self.endTransaction(False)
        #     return
        # self.endTransaction(True)
        pass

    def release(self):
        self.conn.close()

    def runQuery(self, q):
        self.startTransaction()
        self.cursor.execute(q)        
        self.endTransaction(True)

    def addMail(self, title, content, keep_days = 7):
        sql = "insert into mails(title, content, ctime ,keep_days)values(\"%s\", \"%s\", now(), %d)" % (title, content, keep_days)
        self.runQuery(sql)

    def sendMail(self, uid, mailid):
        sql = "insert into mail_info(uid, mail_id)values(%d, %d)" % (uid, mailid)
        self.runQuery(sql)

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
        # This is mail tables in MySQL
        table = "mail_detail"
        sql = 'create table %s( mail_id INT NOT NULL AUTO_INCREMENT, dest_id INT NOT NULL DEFAULT 0, title VARCHAR(127) NOT NULL, content VARCHAR(256) NOT NULL, attachment INT DEFAULT 0, ctime DATETIME NOT NULL, keep_days INT DEFAULT 7, PRIMARY KEY(mail_id))' % table
        self.table_sqls[table] = sql

        table = "user_mails"
        sql = 'create table %s( uid INT NOT NULL, mail_id INT NOT NULL, b_read BOOL DEFAULT false, b_get BOOL DEFAULT false, b_delete BOOL DEFAULT false, PRIMARY KEY(uid, mail_id))' % table
        self.table_sqls[table] = sql

        table = "attachment"
        sql = "create table %s(attid BIGINT AUTO_INCREMENT PRIMARY KEY, type INT NOT NULL, value varchar(64) NOT NULL)" % table
        self.table_sqls[table] = sql
        
        table = "user_achievement"
        sql='create table %s(uid INT NOT NULL, uaid INT NOT NULL, is_recv_reward BOOL DEFAULT 0, progress BIGINT DEFAULT 0, is_gain BOOL DEFAULT 0, time BIGINT DEFAULT 0, PRIMARY KEY(uid, uaid))' % table
        self.table_sqls[table] = sql

        table = "achievement_item"
        sql='create table %s(aiid INT AUTO_INCREMENT PRIMARY KEY, target BIGINT NOT NULL, reward BIGINT DEFAULT 0, reward_type INT DEFAULT 0, type INT NOT NULL)' % table
        self.table_sqls[table] = sql
        
        
        table = "g_history"
        sql = 'create table %s( uid INT NOT NULL PRIMARY KEY, friend_num INT DEFAULT 0, friend_gifts_num INT DEFAULT 0, consitive_login INT DEFAULT 0, tiny_game_times INT DEFAULT 0, bigwin varchar(255) DEFAULT "0", megawin varchar(255) DEFAULT "0", free_times varchar(255) DEFAULT "0", game_times varchar(255) DEFAULT "0", jackpot_times varchar(255) DEFAULT "0", g_jackpot_times INT DEFAULT "0", four_line_times varchar(255) DEFAULT "0", five_line_times varchar(255) DEFAULT "0")' % table
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
    if len(sys.argv) < 2:
        exit(1)
    lbs = ServerSql()
    lbs.init()    
    cmd = sys.argv[1]
    if cmd == "add_mail":
        nxt = sys.argv[2]
        if nxt == "-h":
            print "add_mail title content [keep_days]"
        elif nxt == "-a":
            #title_[start, end]
            mail_start = int(sys.argv[3])
            mail_end = int(sys.argv[4])
            for i in range(mail_start, mail_end + 1):
                lbs.addMail("mail_%d" % i, "content :%d" % i)
        else:
            lbs.addMail(sys.argv[2], sys.argv[3], 7 if len(sys.argv) < 7 else int(sys.argv[4]))
        print "Done!"
        exit(0)

    if cmd == "send_mail":
        nxt = sys.argv[2]
        if nxt == "-h":
            print "send_mail uid mid"
        elif nxt == "-a":
            # send_mail -a uid mail_start mail_end
            mail_start = int(sys.argv[4])
            mail_end = int(sys.argv[5])
            for i in range(mail_start, mail_end + 1):
                lbs.sendMail(int(sys.argv[3]), i)
        else:
            lbs.sendMail(int(sys.argv[2]), int(sys.argv[3]))
        print "Done!"
        exit(0)

    if cmd == "special":
        lbs.doSpecial();
        exit(0)

    if cmd == "rank":
        nxt = sys.argv[2]
        if nxt == "-h":
            print "..."
        elif nxt == "all":
            lbs.refreshRankData()
        exit(0)
        
    for k, v in lbs.table_sqls.items():
        print '"%s":"%s",' % (k, v.replace(",", ",\n").replace('"', '\\"'))
    #lbs.createTable("achievement_item")
    #lbs.createTable("user_achievement")
    #lbs.refreshRankData()
    #lbs.createRankTables();
    #lbs.createAchievementTable()
    #lbs.createAttachmentTable()

