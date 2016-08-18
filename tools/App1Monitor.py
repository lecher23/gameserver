#coding:utf-8
import commands
import time
import smtplib
from email.mime.text import MIMEText
from email.header import Header


GKW = "getUserSetting"
LOG = "/root/CgServerPlus/server.logs"

class Monitor:
    def __init__(self):
        self.prevRound = 0
        self.prevDay = 0
        self.detail = []
        self.yesterday = ""
        self.max_close_wait_num = 0

    def getCloseWaitCount(self):
        cmd = "netstat -apn | grep 9876 | grep CLOSE_WAIT | wc -l"
        s, o = commands.getstatusoutput(cmd)
        if (s != 0):
            return -1
        return o

    def getCount(self):
        cmd = "grep %s %s | wc -l" % (GKW, LOG)
        s, o = commands.getstatusoutput(cmd)
        if s == 0 and self.max_close_wait_num < int(o):
            self.max_close_wait_num = int(o)

    def doItr(self):
        today = time.strftime('%Y-%m-%d',time.localtime(time.time()))
        cnt = self.getCount()
        thisInterval = cnt - self.prevRound;
        self.prevRound = cnt;
        self.detail.append(thisInterval);
        if today != self.yesterday:
            self.summary(cnt)
            self.yesterday = today

    def summary(self, total):
        incOneDay = total - self.prevDay
        self.sendMail(incOneDay)
        self.prevDay = total
        # dump detail info
        self.detail = []

    def sendMail(self, oneday):
        content = "Hi:\nHere is monitor data:\n" +\
                  "*****************\n\n"+\
                  "total:%d\n\ndetail:%s\n\nhistory:%d\n"\
                  "max CLOSE_WAIT:%d\n" %\
                  (oneday, self.detail, self.prevRound, self.max_close_wait_num);
        message = MIMEText(content, 'plain', 'utf-8')
        message['From'] = Header("App1 Monitor", 'utf-8')
        message['To'] =  Header("Monitor", 'utf-8')
        subject = 'Day monitor data %s.' % self.yesterday
        message['Subject'] = Header(subject, 'utf-8')
        smtp = smtplib.SMTP()
        smtp.connect('smtp.exmail.qq.com')
        smtp.login("licheng@touchtogame.com", "Lc90426")
        smtp.sendmail("licheng@touchtogame.com", "593869744@qq.com", message.as_string())
        smtp.quit()


    def debug(self):
        self.doItr()
        print self.prevRound
        print self.detail
        print self.yesterday
        print self.prevDay
    

## 每隔一定时间拉一次数据，记录上次的数据以及本次的数据差值，保存起来
if __name__ == "__main__":
    m = Monitor()
    while(True):
        try:
            m.doItr()
        except Exception, e:
            print e
        time.sleep(1200)

