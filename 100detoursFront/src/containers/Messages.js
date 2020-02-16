import React from 'react'
import Paper from "@material-ui/core/Paper";
import { MuiThemeProvider, createMuiTheme } from "@material-ui/core/styles";
import DraftsIcon from '@material-ui/icons/Drafts';
import ListItem from '@material-ui/core/ListItem'
import List from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import { blue } from "@material-ui/core/colors";
import "./Messages.css";


const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

const messages = ["Important","Soins de mardi annulés","Reunion jeudi 14h"]

const flexContainer = {
  display: 'flex',
  flexDirection: 'column',
  padding: 0,

};

export default function MessageList(){

    return (
        <MuiThemeProvider theme={theme} className="messageList">
            <Paper>
                <h1> No new notification </h1>
                <h2> Last messages: </h2>
                <List style={flexContainer}>
                    {messages.map((messages, index) => (
                     <ListItem key={index} className="message">
                        <ListItemIcon>
                             <DraftsIcon/>
                         </ListItemIcon>
                        <ListItemText primary={messages}/>
                    </ListItem>))}
                </List>
            </Paper>
        </MuiThemeProvider>
    )
}
