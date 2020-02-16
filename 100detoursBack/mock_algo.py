from flask import Flask

app = Flask("mock_algo")


@app.route("/", methods=["POST"])
def compute():
    return {"message": "Inputs recieved!"}


app.run(port=5005)
