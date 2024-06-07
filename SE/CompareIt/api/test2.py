"""
Author: shihan
Date: 2023-11-28 19:50:03
version: 1.0
description: 
"""
import psycopg as db


def name(keyword):
    server_params = {
        "dbname": "sf23",
        "host": "db.doc.ic.ac.uk",
        "port": "5432",
        "user": "sf23",
        "password": "3048=N35q4nEsm",
        "client_encoding": "utf-8",
    }

    conn = db.connect(**server_params)
    curs = conn.cursor()

    gender = "male"
    keyword = "a" + " " + gender
    # male
    sql_command = "Insert a into my_user"
    curs.execute(sql_command)
    rec = curs.fetchone()
    print(rec)

    conn.close()
