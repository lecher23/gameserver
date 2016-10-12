import MySQLdb as mysqldb
import sys
import os
#from optparse import OptionParser
import getopt
import random

HOST = "localhost"
USER = "root"
PASSWD = "111222"

cur_path = os.path.split(os.path.realpath(__file__))[0]
sys.path.append(cur_path)
from tables import table_defines, parse_dict_to_db_sql
import game_setting as gs

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

    def addColumn(self, tablename, fields = [], values = []):
        vstr = '","'.join(values)
        sql = "insert into %s " % tablename
        if not fields:
            sql += 'values("%s")' % vstr
        else:
            sql += "(%s) values (\"%s\")" % (",".join(fields), vstr)
        self.runQuery(sql)

    def addLevelInfos(self, level, exp_need, max_bet, level_reward):
        sql = "insert into level_config values(%d, %d, %d, %d)"\
              % (level, exp_need, max_bet, level_reward)
        self.runQuery(sql)

    def deleteAllUser(self, begin, end):
        tbs = ['user_info','user_resource','f_history','history','g_history']
        sqls = []
        if end == 0:
            for t in tbs:
                sqls.append("delete from %s where uid >= %d" % (t, begin))
        else:
            for t in tbs:
                sqls.append("delete from %s where uid >= %d and uid <= " % (t, begin, end))
        self.startTransaction()
        for q in sqls:
            print q
            self.cursor.execute(q)
        self.endTransaction(True)

    def clearVipConfig(self):
        sql = "delete from vip_config"
        self.runQuery(sql)

    def addVipConfig(self, level, exp_ext, bounus_ext, login_ext, exp_need, max_bet):
        sql = "insert into vip_config values(%s, %s, %s, %s, %s, %s)" % \
              (level, exp_ext, bounus_ext, login_ext, exp_need, max_bet)
        self.runQuery(sql)

    def clearVipRewardConfig(self):
        sql = "delete from login_config where id < 20000"
        self.runQuery(sql)

    def addVipRewardConfig(self, vip, add):
        sql = "insert into login_config (id, val) values(%d, %d)" % \
              (vip + 10000, add)
        self.runQuery(sql)

    def addLoginRewardConfig(self, dayn, reward):
        sql = "insert into login_config (id, val) values(%d, %d)" % \
              (dayn + 20000, reward)
        self.runQuery(sql)

    def modifyLastLogin(self, uid, ts):
        sql = "update g_history set last_login=%d where uid=%d" % (ts, uid)
        self.runQuery(sql)

    def modifyConsitiveLogin(self, uid, dayn):
        sql = "update g_history set constive_login=%d where uid=%d" % (dayn, uid)
        self.runQuery(sql)

    def clearDayLoginReward(self):
        sql = "delete from login_config where id >= 20000 and id < 30000"
        self.runQuery(sql)

    def clearRunnerConfig(self):
        sql = "delete from login_config where id >= 30000 and id < 40000"
        self.runQuery(sql)

    def addRunnerConfig(self, idx, bonus, chance):
        sql = "insert into login_config (id, val, extra_val) values (%d, %d, %f)" % \
               (idx + 30000, bonus, chance)
        self.runQuery(sql)

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
        sql = "insert into mail_detail(title, content, ctime ,keep_days)values(\"%s\", \"%s\", now(), %d)" % (title, content, keep_days)
        self.runQuery(sql)

    def sendMail(self, uid, mailid):
        sql = "insert into user_mails(uid, mail_id)values(%d, %d)" % (uid, mailid)
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

    def initTables(self):
        for key in self.table_sqls:
            print "Create table: %s" % key
            self.createTable(key)

    ''' Table creator'''
    def createTable(self, table):
        if table not in self.table_sqls:
            print "Sql config not exist for table[%s]" % table
            exit(1)
        self._dropTable(table)
        self.cursor.execute(self.table_sqls[table])
        print "Create table done."

    def createTable2(self, table):
        if table not in table_defines:
            print "Sql config not exist for table[%s]" % table
            exit(1)
        self._dropTable(table)
        self.cursor.execute(parse_dict_to_db_sql(table, table_defines[table]))
        print "Create table done."

    def createRankTables(self):
        tables = ["cur_level_rank", "cur_fortune_rank", "cur_earned_rank", "cur_acmt_rank",
                  "lw_level_rank", "lw_fortune_rank", "lw_earned_rank", "lw_acmt_rank",
                  "tw_earned_rank"]
        for table in tables:
            print "init table %s" % table
            self.createTable(table)

    def generateTableSql(self):
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

