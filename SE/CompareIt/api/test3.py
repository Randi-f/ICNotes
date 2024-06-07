"""
Author: shihan
Date: 2023-12-07 22:20:22
version: 1.0
description: 
"""
from lxml import html
import requests

# https://api.ebay.com/buy/browse/v1/item_summary/search?q=drone&limit=3

# https://developer.ebay.com/api-docs/static/rest-response-components.html#HTTP
# url = "https://api.ebay.com/buy/browse/v1/item_summary/search?q=drone&limit=3"+"&enc=utf-8"
url = "http://api.fanyi.baidu.com/api/trans/vip/translate?q=apple&from=en&to=zh&appid=20231208001904336&salt=1435660288&sign=f89f9594663708c1605f3d736d01d2d4"
response = requests.get(url)
# Check the status code
if response.status_code == 200:
    print("Request was successful!")
    print("Status code:", response.status_code)
    print(response)
    response.encoding = "utf-8"
    html_doc = response.text
    print(html_doc)
else:
    print("Request failed!")
    print("Status code:", response.status_code)
