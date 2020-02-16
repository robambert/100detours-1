import React, { useState } from "react";
import { makeStyles } from '@material-ui/core/styles';
import Button  from "@material-ui/core/Button";
import TextField from "@material-ui/core/TextField"
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import Typography from '@material-ui/core/Typography';
import "./Login.css";

import { fetch_login } from '../api_communication/login_api.js'


const useStyles = makeStyles(theme => ({
  root: {
    flexGrow: 1,
  },
  menuButton: {
    marginRight: theme.spacing(2),
  },
  title: {
    flexGrow: 1,
  },
}));

function Header()
{
  const classes = useStyles();

  return (
    <div className={classes.root}>
      <AppBar position="static">
        <Toolbar>
          <Typography variant="h6" className={classes.title}>
            Nurse app
          </Typography>
        </Toolbar>
      </AppBar>
    </div>
  );
}

export default function Login(props)
{
    const [username, setUsername] = useState("");
    const [password, setPassword] = useState("");
    const [error, setError] = useState(false);


    function validateForm()
    {
        return (username.length > 0 && password.length > 0)
    }

    function handleSubmit(event) {

        event.preventDefault();

        if (validateForm)
        {
            let credentials = {
                'username': username,
                'password': password
            };

            fetch_login(credentials, props.handleLogin, setError)

        }
    }

    return (
    <>
        <Header />

        <div className = "Login">
            <form onSubmit = {handleSubmit}>
                <div className = "inputFields" >
                    <TextField
                    autoFocus
                    error = {error}
                    required
                    id = "standard-required_username"
                    label = "Username"
                    value = {username}
                    onChange = {e => setUsername(e.target.value)} />

                    <TextField
                    error = {error}
                    required
                    id = "standard-required_password"
                    label = "password"
                    value = {password}
                    type = "password"
                    onChange = {e => setPassword(e.target.value)} />
                </div>
                <Button type = "submit" variant = "contained" color = "primary" id = "login_button" disabled = {!validateForm()} onClick = {handleSubmit}>
                Login
                </Button>
            </form>

        </div>
    </>
    );
    }
