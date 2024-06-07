import requests
from lxml import html


# SERVER_URL = "your_server_url"
# accessToken = "your_access_token"
# appKey = "your_app_key"
# appSecret = "your_app_secret"

# # Construct the URL
# url = f"{SERVER_URL}/routerjson"

# # Prepare the request parameters
# params = {
#     "method": "jingdong.ware.product.search.list.get",
#     "accessToken": accessToken,
#     "appKey": appKey,
#     "appSecret": appSecret,
#     "isLoadAverageScore": True,
#     "isLoadPromotion": True,
#     "sort": 123,
#     "page": 123,
#     "pageSize": 123,
#     "keyword": "jingdong",
#     "client": "jingdong"
# }

# # Make the HTTP GET request
# response = requests.get(url, params=params)

# # Check the response status code
# if response.status_code == 200:
#     # The request was successful, you can parse the response content here
#     response_data = response.json()
#     print(response_data)
# else:
#     # Handle the error
#     print(f"Error: {response.status_code} - {response.text}")


def send_request_JD(keyword):
    # print("sending")
    products_list = []
    """ 爬取京东的商品数据 """

    url = "https://search.jd.com/Search?keyword=" + keyword + "&enc=utf-8"

    response = requests.get(url)
    # Check the status code
    if response.status_code == 200:
        print("Request was successful!")
        print("Status code:", response.status_code)
    else:
        print("Request failed!")
        print("Status code:", response.status_code)

    response.encoding = "utf-8"
    html_doc = response.text
    print(html_doc)
    selector = html.fromstring(html_doc)

    ul_list = selector.xpath('//div[@id="J_goodsList"]/ul/li')
    if len(ul_list) == 0:
        products_list.append(
            {
                "title": "network error",
                "price": "-",
                "link": "try later for JD goods",
                "store": "-",
                "referer": "JD",
            }
        )
        return products_list
    for li in ul_list:
        title = li.xpath(
            'div/div[@class="p-name p-name-type-2"]/a/em/text() | '
            'div/div[@class="p-name"]/a/@title'
        )

        link = li.xpath(
            'div/div[@class="p-name p-name-type-2"]/a/@href | '
            'div/div[@class="p-name"]/a/@href'
        )

        price = li.xpath(
            'div/div[@class="p-price"]/strong/i/text() | '
            'div/div[@class="p-price"]/strong/i/text()'
        )

        store = li.xpath(
            'div/div[@class="p-shop"]//a/text() | ' 'div//a[@class="curr-shop"]/@title'
        )
        products_list.append(
            {
                "title": title[0],
                "price": price[0],
                "link": "https:" + link[0],
                "store": store[0],
                "referer": "JD",
            }
        )

    return products_list
