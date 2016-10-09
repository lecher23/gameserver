import os
import sys
import importlib

table_defines = {
}

table_dir_name = "db_tables"

def parse_dict_to_db_sql(table_name, table_define = []):
    sql_tpl = "CREATE TABLE %s (%s)"
    col_sql = ", ".join([t[0] + " " + t[1] for t in table_define])
    table_sql = sql_tpl % (table_name, col_sql)
    return table_sql

def get_table_define_files():
    cur_dir = os.path.split(os.path.realpath(__file__))[0]
    table_dir = os.path.join(cur_dir, table_dir_name)
    files = os.listdir(table_dir)
    ret = []
    for f in files:
        if f.endswith("_tables.py"):
            ret.append(".".join([table_dir_name, f[:-3]]))
    return ret

def fill_table_defines(modules = []):
    global table_defines
    for m in modules:
        module = importlib.import_module(m)
        for attr_name in dir(module):
            if not attr_name.startswith("__"):
                attr = getattr(module, attr_name)
                if isinstance(attr, list):
                    table_defines[attr_name] = attr

def prepare():
    ret = get_table_define_files()
    fill_table_defines(ret)

prepare()

if __name__ == "__main__":
    for k, v in table_defines.items():
        print "%s: %s" % (k, v)
