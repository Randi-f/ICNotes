import math
from flask import Flask, render_template, jsonify, request, session
from lxml import html
import requests
import psycopg as db
import configparser

# import psycopg2 as db
import uuid
import hashlib

from dotenv import load_dotenv
import string
import smtplib
import random
import os

# from Controller.website1_JD import send_request_JD
from website1_JD import send_request_JD

# from Controller.website2_WPH import send_request_WPH
from website2_WPH import send_request_WPH


app = Flask(__name__)
app.secret_key = "your_unique_and_secret_key"


def get_db_connection():
    # server_params = {
    #     'dbname': 'nl1023',
    #     'host': 'db.doc.ic.ac.uk',
    #     'port': '5432',
    #     'user': 'nl1023',
    #     'password': 'aFZK-3CzFH*j3y',
    #     'client_encoding': 'utf-8'
    # }
    server_params = {
        "dbname": "sf23",
        "host": "db.doc.ic.ac.uk",
        "port": "5432",
        "user": "sf23",
        "password": "3048=N35q4nEsm",
        "client_encoding": "utf-8",
    }
    return db.connect(**server_params)


# homepage route for the app
@app.route("/")
def hello_world():
    return render_template("index.html")
    # return render_template("compare.html", result1={}, result2={})


@app.route("/keywordsubmit", methods=["POST"])
def keywordsubmit():
    keyword = request.form.get("keyword")
    products_list = send_request_JD(keyword)
    # if(products_list):

    result2 = send_request_WPH(keyword)
    # print(products_list[0])
    return render_template("compare.html", result1=products_list[0], result2=result2)
    # return products_list[0]


@app.route("/login")
def login():
    return render_template("login.html")


@app.route("/login", methods=["POST"])
def submit():
    data = request.json

    user_id = data.get("user_id")
    password = data.get("password")

    conn = get_db_connection()

    cursor = conn.cursor()

    query = "SELECT * FROM my_user WHERE user_id = %s"
    cursor.execute(query, (user_id,))
    user = cursor.fetchone()

    conn.close()

    # Check if user is not None before accessing its elements
    if user is not None:
        # Print or log the relevant information
        print(password, user[6])

        input_encrypted_password = hashlib.md5(password.encode()).hexdigest()

        if input_encrypted_password == user[6]:
            if user[7] is True:
                session["user"] = user_id
                return jsonify({"message": "Login successful"})
            else:
                return jsonify({"message": "Email is not verified"}), 401
        else:
            return (
                jsonify({"message": "The password is incorrect"}),
                401,
            )
    else:
        return jsonify({"message": "User not found"}), 401


@app.route("/register", methods=["GET", "POST"])
def register():
    if request.method == "POST":
        first_name = request.form["first_name"]
        last_name = request.form["last_name"]
        full_name = first_name + " " + last_name
        gender = request.form["gender"]
        email = request.form["email"]
        dob = request.form["dob"]
        postcode = request.form["postcode"]
        password = request.form["password"]

        encrypted_password = hashlib.md5(password.encode()).hexdigest()

        config = configparser.ConfigParser()
        config.read("dbtool.ini")

        # generate unique user_id
        sqlcommand = (
            "SELECT COUNT(*) AS row_count FROM my_user WHERE name = '"
            + first_name
            + " "
            + last_name
            + "';"
        )
        try:
            conn = db.connect(**config["connection"])
            curs = conn.cursor()
            curs.execute(sqlcommand)
            ret = curs.fetchone()
            # print(ret[0])
        except Exception as e:
            print(f"An error occurred: {e}")  # Log the error
        finally:
            if "curs" in locals():
                curs.close()
            if "conn" in locals():
                conn.close()

        # user_id_initials = (first_name[0] + last_name[0]).upper() + (ret[0]+1)
        user_id = (first_name[0] + last_name[0]).upper() + (str)(ret[0] + 1)
        print(user_id)
        # user_id_dob_part = dob[-2:]  # Last two digits of the year
        # user_id_postcode_part = postcode[-3:]  # Last three digits of the postcode
        # user_id = user_id_initials + user_id_dob_part + user_id_postcode_part

        verification_token = "".join(
            random.choices(string.ascii_letters + string.digits, k=32)
        )

        sqlcommand = """
            INSERT INTO my_user (user_id, name, gender, email, dob, postcode, password, email_verified, verification_token) 
            VALUES (%s, %s, %s, %s, %s, %s, %s, False, %s)
        """

        values = (
            user_id,
            full_name,
            gender,
            email,
            dob,
            postcode,
            encrypted_password,
            verification_token,
        )

        try:
            conn = db.connect(**config["connection"])
            curs = conn.cursor()
            curs.execute(sqlcommand, values)
            conn.commit()  # Commit to save changes
            send_verification_email(email, verification_token, user_id)
            message = "Registration successful"
        except Exception as e:
            print(f"An error occurred: {e}")  # Log the error
            message = "Registration failed due to a technical issue."
        finally:
            if "curs" in locals():
                curs.close()
            if "conn" in locals():
                conn.close()

        return render_template(
            "registration_result.html",
            message="Please check your email to confirm your registration!\n\n",
        )
    else:
        return render_template("register.html")


@app.route("/verify_email/<verification_token>")
def verify_email(verification_token):
    # You should implement logic here to check the verification token in your database
    # If the token is valid, update the 'email_verified' column for the user
    sqlcommand = (
        "UPDATE my_user SET email_verified = True WHERE verification_token = %s"
    )
    print(sqlcommand)

    server_params = {
        "dbname": "sf23",
        "host": "db.doc.ic.ac.uk",
        "port": "5432",
        "user": "sf23",
        "password": "3048=N35q4nEsm",
        "client_encoding": "utf-8",
    }

    try:
        conn = db.connect(**server_params)
        curs = conn.cursor()
        curs.execute(sqlcommand, (verification_token,))
        conn.commit()  # Commit to save changes
        message = "Registration successful"
    except Exception as e:
        print(f"An error occurred: {e}")  # Log the error
        message = "Registration failed due to a technical issue."
    finally:
        if "curs" in locals():
            curs.close()
        if "conn" in locals():
            conn.close()
    # For now, let's assume it's successful
    return render_template("email_verified.html")


def send_verification_email(receiver_mail, verification_token, user_id):
    # Retrieve email configuration from environment variables
    email = os.getenv("EMAIL")
    # email = "price.project23@gmail.com"
    password = os.getenv("PASSWORD")
    # password = "dkto zovm nnwx csqo"

    # Construct the email message
    subject = "Please verify your email"
    verification_link = (
        f"http://127.0.0.1:5000/verify_email/{verification_token}"  # check email!!
    )
    message = (
        f"Welcome to CompareIt! \n\n Thank you for signing up! Your user id is: {user_id}."
        f"Your user id will be used to login in along with your chosen password.\n\n"
        f"Please click on the following link to verify your email:\n\n{verification_link} \n\n\n"
        f"CompareIt \n"
        f"South Kensington, London SW7 2AZ \n"
        f"Phone: (555) 555-5555\n"
        f"Email: price.project23@gmail.com\n"
    )
    text = f"Subject: {subject}\n\n{message}"

    # Send the email
    server = smtplib.SMTP("smtp.gmail.com", 587)
    server.starttls()
    server.login(email, password)
    server.sendmail(email, receiver_mail, text)
    server.quit()

    print(f"Verification email has been sent to {receiver_mail}")


# Profile route
@app.route("/profile")
def profile():
    server_params = {
        "dbname": "sf23",
        "host": "db.doc.ic.ac.uk",
        "port": "5432",
        "user": "sf23",
        "password": "3048=N35q4nEsm",
        "client_encoding": "utf-8",
    }
    if "user" in session:
        username = session["user"]

        # Connect to the database
        try:
            conn = db.connect(**server_params)
            curs = conn.cursor()
            # curs.execute(sqlcommand, values)
            # conn.commit()  # Commit to save changes

            # config=configparser.ConfigParser()
            # config.read('dbtool.ini')

            # conn = db.connect(**config['connection'])
            # curs = conn.cursor()

            # Perform the profile query to fetch user data based on the username
            curs.execute("SELECT * FROM my_user WHERE user_id = %s", (username,))
            user = curs.fetchone()

            # Check if the user is found
            if user:
                # The user is a tuple; you may want to convert it to a dictionary for easier handling
                user_dict = {
                    "username": user[1],
                    "email": user[3],
                    "gender": user[2],
                    "dob": user[4],
                    "postcode": user[5],
                    # Add other properties as needed
                }

                return render_template("profile.html", user=user_dict)
            else:
                return "User not found."
        except Exception as e:
            print(f"An error occurred: {e}")  # Log the error
            return "Error fetching profile data."
        finally:
            if "curs" in locals():
                curs.close()
            if "conn" in locals():
                conn.close()
    else:
        return "You are not logged in. Please log in."


@app.route("/compare")
def compare():
    return render_template("compare.html", result1={}, result2={})


if __name__ == "__main__":
    app.run()
