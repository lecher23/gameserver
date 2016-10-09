import sys
import os
from optparse import OptionParser
from serversql import ServerSql
from tables import table_defines

def init_db(obj):
    for k in table_defines.keys():
        print "init table %s" % k
        obj.createTable2(k)
    obj.createRankTables()

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-t", "--type", dest = "cmd_type", help = "Command type.")
    parser.add_option("-p", "--param", dest = "param", help = "Command param.")

    lbs = ServerSql()
    lbs.init()

    (options, args) = parser.parse_args()

    cmd = options.cmd_type

    if cmd == "init_db":
        init_db(lbs)
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
