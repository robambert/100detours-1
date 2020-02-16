import React, { useState } from "react";
import TextField from '@material-ui/core/TextField';
import Button  from "@material-ui/core/Button";
import { makeStyles } from '@material-ui/core/styles';
import Cookies from 'universal-cookie'


export default function AccountSettings(){

    const [current_password, setCurrentPassword] = useState("");
    const [password, setPassword] = useState("");
    const [confirm_password, setConfirmPassword] = useState("");

    const [error, setError] = useState(false);

    function validateForm()
    {
        return (current_password.length > 0 && password === confirm_password)
    }

    async function handleSubmit(event) {

        event.preventDefault();

        if (validateForm)
        {
            var cookie_token = new Cookies().get('token')

            let data = {
              current_password: current_password,
              new_password: password
            }

            const response = await fetch('/users/' + cookie_token.user_rid + "/password", {
                                  method: 'PUT',
                                  headers: { 'Content-Type': 'application/json',
                                        'Authorization':  "Bearer " + cookie_token.access_token, },
                                  body: JSON.stringify(data),

                                    })

            try
            {
                let status = response.status;
                if (status === 204)
                {
                }
                else
                {
                    alert("Current password is not correct")
                    console.error(`Request rejected with status ${status}`)
                }
            }
            catch (err)
            {
                  console.error(`Request rejected with status ${response.status}`)
            }
        }
    }

    return(
        <>
<form onSubmit = {handleSubmit}>
        <TextField
          autoFocus
          id="standard-password-input"
          label="Current Password"
          error = {error}
          type="password"
          required
          value = {current_password}
          autoComplete="current-password"
          onChange = {e => setCurrentPassword(e.target.value)}
        />

        <TextField
          id="standard-password-input"
          label="New Password"
          type="password"
          error = {error}
          value = {password}
          required
          autoComplete="new-password"
          onChange = {e => setPassword(e.target.value)}
        />
        <TextField
          id="standard-password-input"
          label="Confirm New Password"
          type="password"
          error = {error}
          required
          value = {confirm_password}
          autoComplete="confirm-password"
          onChange = {e => setConfirmPassword(e.target.value)}
        />
        <Button type = "submit" variant = "contained" color = "primary" id = "login_button" disabled = {!validateForm()} onClick = {handleSubmit}>
        Change Password
        </Button>
            </form>
        </>
        )
}
