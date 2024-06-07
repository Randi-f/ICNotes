import requests
import time

# Get the current timestamp (time since epoch in seconds)
timestamp = str((int)(time.time()))
print(timestamp)
url = (
    "https://partner.shopeemobile.com/api/v2/product/get_item_list?access_token=c09222e3fc40ffb25fc947f738b1abf1&item_status=NORMAL&offset=0&page_size=10&partner_id=1&shop_id=1&sign=sign&timestamp="
    + timestamp
    + "&update_time_from=1611311600&update_time_to=1611311631"
)

payload = {}
headers = {}
response = requests.request(
    "GET", url, headers=headers, data=payload, allow_redirects=False
)

print(response.text)
