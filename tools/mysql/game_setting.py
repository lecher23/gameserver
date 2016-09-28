#coding: utf-8
import os
import json
CUR_PATH = os.path.split(os.path.realpath(__file__))[0]

''' vip level|extra exp per|extra gold per|extra login reward per|exp need|max bet'''
vip_cfg = [
    (1, 5, 5, 10, 10, 10),
    (2, 10, 10, 10, 40, 15),
    (3, 20, 20, 10, 100, 20),
    (4, 25, 25, 10, 200, 25),
    (5, 30, 30, 10, 400, 30),
    (6, 35, 35, 10, 800, 35),
    (7, 40, 40, 10, 1600, 40),
    (8, 45, 45, 10, 3200, 45),
    (9, 50, 50, 10, 6400, 50)
]

'''day n|reward'''
daily_reward_cfg = [
    (1, 2000),
    (2, 3000),
    (3, 4000),
    (4, 5000),
    (5, 6000),
    (6, 8000),
    (7, 10000),
]

''' index|gold|weight'''
runner_cfg = [
    [1, 1000, 10],
    [2, 300, 1],
    [3, 2000, 10],
    [4, 500, 1],
    [5, 3000, 10],
    [6, 600, 1],
    [7, 4000, 10],
    [8, 1000, 1],
    [9, 5000, 10],
    [10, 700, 1],
    [11, 7000, 10],
    [12, 400, 1],
]

'''level|exp need for next level|max bet|level reward'''
level_info = [
    (1, 250, 10, 1000),
    (2, 550, 10, 2000),
    (3, 900, 10, 3000),
    (4, 1300, 10, 4000),
    (5, 2200, 50, 5000),
    (6, 3200, 50, 6000)
]

''' bet|exp '''
bet_2_exp = [
    (5, 5),
    (10, 10),
    (50, 20),
    (100, 30),
    (500, 50)
]


def get_obj_from_file(fpath):
    obj = None
    with open(fpath) as f:
        obj = json.loads(f.read())
    if obj is None:
        print "Load line rate failed."
        exit(1)
    return obj

'''single line'''
def get_line_cfg():
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/LineConfig.json")
    obj = get_obj_from_file(path)
    ret = []
    for line in obj:
        sk = sorted([key for key in line.keys() if key.startswith("Column")])
        ret.append((','.join([str(line[key]) for key in sk]),))
    return ret

def format_line_cfg(line):
    result = []
    for t in line:
        x = [int(i) for i in t[0].split(",")]
        col = len(x)
        index = 0
        tmp = []
        for y in x:
            val = col * y + index
            index += 1
            tmp.append(val)
        result.append((",".join([str(item) for item in tmp]), ))
    return result

'''column|row|element id|weight'''
def get_grid_cfg():
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/LineRate.json")
    obj = get_obj_from_file(path)
    ret = [(i["Col"], i["Row"], i["Ele_ID"], i["Pro"]) for i in obj]
    check_grid(ret)
    return ret

def check_grid(x):
    max_col = 0
    max_row = 0
    ret = {} # {{},{},{}}
    for item in x:
        col = int(item[0])
        row = int(item[1])
        if col > max_col:
            max_col = col
        if row > max_row:
            max_row = row
        if col not in ret:
            ret[col] = {}
        ret[col][row] = 1;
    max_col += 1
    max_row += 1
    for i in xrange(0, max_col):
        for j in xrange(0, max_row):
            if i not in ret:
                print "col[%d] not exist!" % i
                exit(1)
            if j not in ret[i]:
                print "(col[%d], row[%d]) not exist!" % (i, j)
                exit(1)

'''element id| element type'''
def get_ele_info():
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/LineBonus.json")
    obj = get_obj_from_file(path)
    return [(i["ID"], i["Type"] if i["Type"] < 4 else 0, i["Repeat"]) for i in obj]

'''element id| line number| value'''
def get_line_bouns():
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/LineBonus.json")
    obj = get_obj_from_file(path)
    ret = []
    for i in obj:
        for key, value in i.items():
            if key.startswith("Link"):
                count = int(key.replace("Link", ""))
                ret.append((i['ID'], count, value))
    return ret;

'''config id|value|extra'''
def get_common_cfg():
    ret = []
    # free game times setting
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/Scatter.json")
    obj = get_obj_from_file(path)
    for item in obj:
        ret.append((30000 + item["Counts"], item["Numbers"]))
    # config for prize pool
    path = os.path.join(CUR_PATH, "game_config/TemplePrincess/Special.json")
    obj = get_obj_from_file(path)
    idx = 10000
    for item in obj:
        ret.append((idx + 1, item["PerBonus"]))
        ret.append((idx + 2, item["BaseBonus"]))
        ret.append((idx + 8, item["LimitBet"]))
        idx += 10000
    if idx > 30000:
        print "Error. invalid idx: %d" % idx
        exit(1)
    return ret

def get_theme_common_cfg():
    theme_common_cfg = [
        (10001, 5, 0), # tax for prize pool
        (10002, 5000, 0), # min prize pool
        (10003, 1013, 0), # element id for roll prize
        (10004, 6, 0), # element count for roll prize
        (10005, 0, 0), # enable repeat element, 0:false, 1:true
        (10006, 600, 0),# reserve room time: 600s
        (10007, 17280, 0), # force hit count
        (10008, 1000, 0), # min bet need to roll
        (20001, 1, 0), # tax for big prize pool
        (20002, 10000, 0), # min prize pool
        (20003, 1014, 0), # element id for roll big prize
        (20004, 6, 0), # element count for roll big prize
        (20005, 17280, 0), # force hit count for big prize
        (20008, 100, 0), # min bet need to roll
        (30000, 10001, 0), # element id for trigger free game
        (30003, 5, 0), # free times when element repeat 3 times
        (30004, 8, 0), # free times when element repeat 4(30004 - 30000) times
        (30005, 12, 0),
        (40000, 1002, 0), # element id for tigger tiny game
        (40003, 1, 0), # tiny game id when element repeat 3 times
        (50001, 5, 0), # big win
        (50002, 10, 0), # mega win
        (50003, 15, 0), # super win
    ]
    return theme_common_cfg
if __name__ == "__main__":
    print get_ele_info()
