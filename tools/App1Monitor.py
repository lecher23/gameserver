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

    def getCount(self):
        cmd = "grep %s %s | wc -l" % (GKW, LOG)
        s, o = commands.getstatusoutput(cmd)
        return int(o)

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
        content = "Hi:\nHere is yesterday monitor data:\n" +\
                  "*****************\n\n"+\
                  "total:%d\n\ndetail:%s\n\nhistory:%d\n" %\
                  (oneday, self.detail, self.prevRound);
        message = MIMEText(content, 'plain', 'utf-8')
        message['From'] = Header("App1 Monitor", 'utf-8')
        message['To'] =  Header("Monitor", 'utf-8')
        subject = 'Day monitor data before %s.' % self.yesterday
        message['Subject'] = Header(subject, 'utf-8')
        smtp = smtplib.SMTP()
        smtp.connect('smtp.exmail.qq.com')
        smtp.login("licheng@touchtogame.com", "Lc90426")
        smtp.sendmail("licheng@touchtogame.com", "licheng@touchtogame.com", message.as_string())
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

