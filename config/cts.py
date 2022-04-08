import pandas as pd
from bs4 import BeautifulSoup as bs
import os 

pwd = os.path.dirname(os.path.realpath(__file__)) 
html = pwd + "\\mtk.html"

def save_to_csv(result):
    data = pd.DataFrame(result[1:],columns=["test","resulotion"])
    # print(data)
    data.to_csv(pwd+'\\cts_resulotion.csv',index=False,header=None)


def readHtml():
    print("hello")
    with open(html,"r",encoding='utf-8') as file:
        soup = bs(file.read(),"html.parser")
        table = soup.select('table > tbody > tr ')
        # print(table)

    result = []
    for item in table:
        # print("+++++++++++++++")
        test = item.text.split("\xa0")[0].split('\n')[1]
        # result.append(test)
        url_format = ""
        if(item.select('td a')!=[]):
            url = item.select('td  a')[0].get("href").replace(".inc",".com")
            # '<a href="http://online.mediatek.com/FAQ#/SW/FAQ13800">[FAQ13815]'
            url_format = "<a href='%s'>[%s]"%(url,url.split("/")[-1])
        result.append([test,url_format])
        # print("---------------")
    # print(result)
    return result


def main():
    result = readHtml()
    save_to_csv(result)

if __name__ == "__main__":
    main()