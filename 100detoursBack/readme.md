WIP
===
- Processing outputs of algorithm service

Next steps
==========
- Missing unit tests
- Missing authorization information and protected routes in API documentation

Prerequisite
============
You must have [mongodb server](https://www.mongodb.com/download-center/community) installed and running.

Instalation
===========
In package root dir:
```
pip install -r requirements.txt
(pip install -e .)
```

Usage
=====
Running app
-----------
From project root dir run:
```
python run.py
```

Testing
-------
Open [http://127.0.0.1:5000/dev/hello](http://127.0.0.1:5000/dev/hello) in your browser
or use curl command:
```
curl --request GET \
  --url http://127.0.0.1:5000/dev/hello
```
Expected response
```
{
	"messagge": "Hello World!"
}
```

Available routes
----------------
Open [http://127.0.0.1:5000/](http://127.0.0.1:5000/) in your browser to see the API documentation (swagger-ui).
!!! Documentation is incomplete (missing authentification information for protected routes).

Protected routes
----------------
For protected routes (nearly all of them) you need to first login through the route `/auth/login/`
Curl request:
```
curl --request POST \
  --url http://127.0.0.1:5000/auth/login \
  --header 'content-type: application/json' \
  --data '{
	"username": "admin",
	"password": "100detours"
}'
```
You'll recieve an answer looking like:
```
{
    "message": "Logged in as admin",
    "access_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1ODAzMzU2NjYsIm5iZiI6MTU4MDMzNTY2NiwianRpIjoiOTc1MGZjZTQtZjNkNC00NDVkLThmZTQtNjg3ZWNhM2JhZmJjIiwiZXhwIjoxNTgwMzM2NTY2LCJpZGVudGl0eSI6MSwiZnJlc2giOmZhbHNlLCJ0eXBlIjoiYWNjZXNzIiwidXNlcl9jbGFpbXMiOnsidXNlcm5hbWUiOiJhZG1pbiIsInVzZXJ0eXBlIjowfX0.TakPuB5TJUhc0z_GTlhxVe1OT8uJ3kn24pdZjX1oQCo",
    "refresh_token": "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1ODAzMzU2NjYsIm5iZiI6MTU4MDMzNTY2NiwianRpIjoiZmJhN2Q3NDAtOWZhNS00YTAyLWIzN2MtZTNhNGU4NGRmNzYzIiwiZXhwIjoxNTgyOTI3NjY2LCJpZGVudGl0eSI6MSwidHlwZSI6InJlZnJlc2gifQ.AMfc0ZIYIUXNHdJtejx-8UbtJQLACu5i82z-YVWM8s8"
}

```
For every subsquent request you'll need to send the access token in the `Authorization` header with the format `"Bearer ${access_header}"`
where `${access_header}` designes the encoded string
Example, getting treatment type list `GET http://127.0.0.1:5000/treatments/`
Curl request:
```
curl --request GET \
  --url http://127.0.0.1:5000/treatments/ \
  --header 'authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpYXQiOjE1ODAzMzU0OTYsIm5iZiI6MTU4MDMzNTQ5NiwianRpIjoiZGRhY2EzNTItMWM5Ny00YmM4LWE0ZTEtNGVlNWUyZGMyY2YxIiwiZXhwIjoxNTgwMzM2Mzk2LCJpZGVudGl0eSI6MSwiZnJlc2giOmZhbHNlLCJ0eXBlIjoiYWNjZXNzIiwidXNlcl9jbGFpbXMiOnsidXNlcm5hbWUiOiJhZG1pbiIsInVzZXJ0eXBlIjowfX0.ffCpY_e4gN6h6524785_pO6Yv3pJE3MkRb6WQnKWMfo'
```

Sending request to algo
-----------------------
In a terminal, at project root dir execute commmand
```
flask run-algo
```
The command take a date as an optional positional parameter, for example
```
flask run-algo 2020-01-25
```
runs prepares and sends the input to the algorithm service for the Jan. 25th of 2020. 

Configuration
=============
Default configuration can be found in [instance/default.cfg](instance/default.cfg)

You can override the default configuration by defining your own configuration file `my_conf.cfg` in the `instance` folder,
and set `FLASK_CONF` variable to `my_conf.cfg`.

Finally, you can partially override the app configuration through environment variable, especially:

| Environment Variable | Description | Default | Notes |
| --- | --- | --- | --- |
| `FLASK_CONF` | Path to configuration file | `None` | Relative to `instance` folder |
| `SERVER_NAME` | Flask app URI | `127.0.0.1:5000` | Must specify host with dot notation (not 'localhost') |
| `FLASK_HOST` | Flask app host | `127.0.0.1` | Different from SERVER_NAME |
| `FLASK_PORT` | Flask app port | `5000` | Diffent from SERVER_NAME |
| `MONGODB_URI` | MongoDB URI | `127.0.0.1:27017` |  |
| `ALGO_URI` | Algorithm service URI | `127.0.0.1:5005` |  |

Lookup table
------------
1. [instance/default.cfg](instance/default.cfg)
2. custom configuration file pointed by `FLASK_CONF`
3. environment variable

Default admin user
============
```
{
    "username": "admin",
    "password": "100detours",
    "usertype": 0,
}
```
Data
====
See [data folder](back/data) for inital database state.

Notes
=====
User type ids: 
- Admin `usertype=0`
- Nurse `usertype=1`