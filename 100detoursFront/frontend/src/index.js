import React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import * as serviceWorker from './serviceWorker';
import Login from './containers/Login.js'
import MainScreen from './containers/MainScreen.js'
import Cookies from 'universal-cookie';




function RenderLogin(props)
{
    // Login page or MainScreen if logged in
    return(props.is_logged_in ?
        <MainScreen
             handleLogout = {props.handleLogout}
             status={props.status}
             username={props.username}
             access_token={props.access_token}
             />
            :
        <Login
            handleLogin = {props.handleLogin}
            />)
}

class App extends React.Component
{
    constructor(props)
    {
        super(props)
        this.state = {
            apiResponse: "",
            is_logged_in: false,
            status: "",
            username: "",
            access_token: "",
            rid: -1
        }
        this.handleLogin = this.handleLogin.bind(this)
        this.handleLogout = this.handleLogout.bind(this)
    }

    handleLogin(response, credentials)
    {
        // Setting cookie to retrieve user information
        const cookies = new Cookies();
        cookies.set('token',
                     {
                        username: credentials.username, 
                        access_token: response.access_token,
                        status: response.usertype,
                        rid: response.nurse_rid,
                        user_rid: response.user_rid
                      },
                      { path: '/' });

        this.setState({
                        is_logged_in : true,
                        username     : credentials.username,
                        status       : response.usertype,
                        access_token : response.access_token,
                        rid: response.nurse_rid

                    }, function() {
            console.log(credentials.username + " successfuly logged in with rank: " + response.usertype)
        });
    }

    handleLogout()
    {
        this.setState({is_logged_in:false, status:"", username:"", access_token: "", rid: -1}, function() {
            console.log("Successfuly logged out");
        });
        const cookies = new Cookies();
        cookies.remove('token', {path: '/'});
    }

    render()
    {

        return(
            <RenderLogin
                handleLogin={this.handleLogin}
                handleLogout={this.handleLogout}
                is_logged_in={this.state.is_logged_in}
                status = {this.state.status}
                username = {this.state.username}
                // rid={this.state.rid}
                access_token={this.state.access_token}

                />
        )
    }
}

ReactDOM.render(<App />, document.getElementById('root'));

serviceWorker.unregister();
