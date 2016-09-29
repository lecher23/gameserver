import MySQLdb as mysqldb
import sys
import os
#from optparse import OptionParser
import getopt
import random

HOST = "139.196.148.39"
USER = "slot_master"
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
            self.createTable(table)

    def generateTableSql(self):
        table = "f_history"
        sql = 'create table %s(uid INT NOT NULL PRIMARY KEY, invite_count int NOT NULL DEFAULT 0, reward_total BIGINT DEFAULT 0, reward_remain BIGINT DEFAULT 0)' % table
        self.table_sqls[table] = sql

        table = "user_info"
        sql = 'create table %s(uid INT AUTO_INCREMENT, mid CHAR(48) NOT NULL, fname CHAR(24), lname CHAR(2) DEFAULT "", avatar INT DEFAULT 0, male BOOL DEFAULT 1, country INT DEFAULT 86, PRIMARY KEY(uid))' % table
        self.table_sqls[table] = sql

        table = "user_resource"
        sql = 'create table %s(uid INT NOT NULL, level INT DEFAULT 1, exp BIGINT DEFAULT 0, fortune BIGINT DEFAULT 0, vip_level INT DEFAULT 0, vip_point BIGINT DEFAULT 0, vip_tmp INT DEFAULT 0, vip_tmp_end BIGINT DEFAULT 0, PRIMARY KEY(uid))' % table
        self.table_sqls[table] = sql

        table = "history"
        sql = 'create table %s(uid INT NOT NULL, max_fortune BIGINT DEFAULT 0, max_earned BIGINT DEFAULT 0, total_earned BIGINT DEFAULT 0, total_bet BIGINT DEFAULT 0, tw_earned BIGINT DEFAULT 0, lw_earned_sort INT DEFAULT 999, lw_level_sort INT DEFAULT 9999, lw_fortune_sort INT DEFAULT 9999, lw_acheivement_sort INT DEFAULT 9999, PRIMARY KEY(uid))' % table
        self.table_sqls[table] = sql

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

        table = "cargo_history"
        sql = 'create table %s(id INT AUTO_INCREMENT PRIMARY KEY, uid INT NOT NULL, cid CHAR(6) NOT NULL, tsid CHAR(64) NOT NULL, value BIGINT NOT NULL, timestamp BIGINT NOT NULL)' % table
        self.table_sqls[table] = sql

        table = "login_config"
        sql = 'create table %s(id INT NOT NULL PRIMARY KEY, val BIGINT NOT NULL, extra_val INT DEFAULT 0)' % table
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

    if cmd == "del":
        nxt = sys.argv[2]
        if nxt == 'all':
            lbs.deleteAllUser(int(sys.argv[3]), int(sys.argv[4]))
        exit(0)

    if cmd == "mktb":
        table_name = sys.argv[2]
        if table_name != "all":
            lbs.createTable(table_name)
        else:
            for k in lbs.table_sqls:
                lbs.createTable(k)
        exit(0)

    if cmd == "mktb2":
        table_name = sys.argv[2]
        lbs.createTable2(table_name)
        exit(0)

    if cmd == "runner_cfg":
        nxt = sys.argv[2]
        if nxt == "clear":
            lbs.clearRunnerConfig()
        elif nxt == "default":
            lbs.clearRunnerConfig()
            cfg = runner_cfg
            sm = 0.0
            for tp in cfg:
                sm += tp[2]
            sm2 = 0
            for tp in cfg:
                tp[2] = int(tp[2] / sm * 1000)
                sm2 += tp[2]
            if sm2 != 1000:
                cfg[-1][2] = cfg[-1][2] + 1000 - sm2
            for tp in cfg:
                lbs.addRunnerConfig(tp[0], tp[1], tp[2])
        else:
            x = int(sys.argv[3])
            y = int(sys.argv[4])
            z = float(sys.argv[5])
            lbs.addRunnerConfig(x, y, z)
        exit(0)

    if cmd == "dayrw_cfg":
        nxt = sys.argv[2]
        if nxt == "clear":
            lbs.clearDayLoginReward()
        elif nxt == "default":
            lbs.clearDayLoginReward()
            cfg = daily_reward_cfg
            for tp in cfg:
                lbs.addLoginRewardConfig(tp[0], tp[1])
        exit(0)

    if cmd == "viprw_cfg":
        nxt = sys.argv[2]
        if nxt == "clear":
            lbs.clearVipRewardConfig()
        elif nxt == "default":
            lbs.clearVipRewardConfig()
            cfg = [
                (1, 10),
                (2, 10),
                (3, 10),
                (4, 10),
                (5, 10),
                (6, 10),
                (7, 10),
                (8, 10),
                (9, 10),
            ]
            for tp in cfg:
                lbs.addVipRewardConfig(tp[0], tp[1])
        exit(0)

    if cmd == "vip_cfg":
        nxt = sys.argv[2]
        if nxt == "clear":
            lbs.clearVipConfig()
        elif nxt == "default":
            lbs.clearVipConfig()
            cfg = vip_cfg
            for row in cfg:
                lbs.addVipConfig(*row)
        exit(0)

    if cmd == "initdb":
        lbs.initTables()
        exit(0)

    if cmd == "level_cfg":
        lc = gs.level_info;
        for item in level_info:
            lbs.addLevelInfos(*item)

    if cmd == "bet_exp_cfg":
        for item in gs.bet_2_exp:
            lbs.addColumn("bet_exp_cfg", [], [str(k) for k in item])

    if cmd == "sgs":
        second = sys.argv[2]
        if second == "all":
            for item in gs.line_cfg:
                lbs.addColumn("line_config", ["line"], [str(k) for k in item])
            for item in gs.grid_cfg:
                lbs.addColumn("free_grid_config", [], [str(k) for k in item])
            for item in gs.ele_info:
                lbs.addColumn("ele_info", [], [str(k) for k in item])
            for item in gs.ele_cfg:
                lbs.addColumn("ele_config", ["ele_id", "line_num", "value"],
                              [str(k) for k in item])
        elif second == "grid":
            lbs.createTable2("grid_config")
            for item in gs.get_grid_cfg():
                lbs.addColumn("grid_config", ["col", "row", "ele_id", "weight"],
                              [str(k) for k in item])

            lbs.createTable2("ele_info")
            for item in gs.get_ele_info():
                lbs.addColumn("ele_info", [], [str(k) for k in item])
        elif second == "line":
            for item in gs.format_line_cfg(gs.line_cfg):
                lbs.addColumn("line_config", ["line"], [str(k) for k in item])
        elif second == "common":
            for item in gs.get_theme_common_cfg():
                lbs.addColumn("theme_common_cfg", ["code", "value", "extra"], [str(k) for k in item])
        elif second == "tp":
            tmap = {
                "ele_info": gs.get_ele_info(),
                "ele_config": gs.get_line_bouns(),
                "tiny_game_config": gs.get_tiny_game_cfg(),
                "free_grid_config": gs.get_free_game_grid_cfg(),
                "grid_config": gs.get_grid_cfg(),
                "line_config": gs.get_line_cfg(),
                "theme_common_cfg": gs.get_common_cfg()
            }
            for k, v in tmap.items():
                print "Add config for table: %s" % k
                lbs.createTable2(k)
                for item in v:
                    lbs.addColumn(k, [], [str(val) for val in item])
        exit(0)

    if cmd == "debug":
        nxt = sys.argv[2]
        if nxt == "last_login":
            uid = int(sys.argv[3])
            ts = int(sys.argv[4])
            lbs.modifyLastLogin(uid, ts)
        if nxt == "constive_login":
            uid = int(sys.argv[3])
            dayn = int(sys.argv[4])
            lbs.modifyConsitiveLogin(uid, dayn)
