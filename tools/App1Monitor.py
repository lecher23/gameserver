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
        self.close_wait_limit = 30
        self.last_mail_time = 0
        self.mail_interval = 3600

    def getCloseWaitCount(self, port = 9876):
        cmd = "netstat -apn | grep %d | grep CLOSE_WAIT | wc -l" % port
        s, o = commands.getstatusoutput(cmd)
        if (s != 0):
            return -1
        return int(o)

    def monitorCloseWait(self):
        val = self.getCloseWaitCount()
        if val >= self.close_wait_limit:
            now = time.time()
            if now - self.last_mail_time > self.mail_interval:
                self.sendMail("Too much CLOSE_WAIT!", "Current count: %d\n" % val)
                self.last_mail_time = now

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
        #self.sendMail(incOneDay)
        self.prevDay = total
        # dump detail info
        self.detail = []

    def sendMail(self, title,  content):
        message = MIMEText(content, 'plain', 'utf-8')
        message['From'] = Header("Monitor", 'utf-8')
        message['To'] =  Header("Monitor", 'utf-8')
        message['Subject'] = Header(title, 'utf-8')
        smtp = smtplib.SMTP()
        smtp.connect('smtp.exmail.qq.com')
        smtp.login("licheng@touchtogame.com", "Lc90426")
        smtp.sendmail("licheng@touchtogame.com", "licheng@touchtogame.com", message.as_string())
        smtp.quit()

    def sendDailyMonitorMail(self, oneday):
        content = "Hi:\nHere is monitor data:\n" +\
                  "*****************\n\n"+\
                  "total:%d\n\ndetail:%s\n\nhistory:%d\n"\
                  "max CLOSE_WAIT:%d\n" %\
                  (oneday, self.detail, self.prevRound, self.max_close_wait_num);
        subject = 'Day monitor data %s.' % self.yesterday
        self.sendMail(subject, content)

    def debug(self):
        self.doItr()
        print self.prevRound
        print self.detail
        print self.yesterday
        print self.prevDay
    

## 每隔一定时间拉一次数据，记录上次的数据以及本次的数据差值，保存起来
if __name__ == "__main__":
    m = Monitor()
    count = 0
    while(True):
        try:
            m.monitorCloseWait()
        except Exception, e:
            print e
        time.sleep(30)

