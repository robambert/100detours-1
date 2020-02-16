import React from 'react';
import clsx from 'clsx';
import { makeStyles } from '@material-ui/core/styles';
import Button from '@material-ui/core/Button';
import Drawer from '@material-ui/core/Drawer';
import CssBaseline from '@material-ui/core/CssBaseline';
import AppBar from '@material-ui/core/AppBar';
import Toolbar from '@material-ui/core/Toolbar';
import List from '@material-ui/core/List';
import Typography from '@material-ui/core/Typography';
import Divider from '@material-ui/core/Divider';
import IconButton from '@material-ui/core/IconButton';
import MenuIcon from '@material-ui/icons/Menu';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemText from '@material-ui/core/ListItemText';
import MailIcon from '@material-ui/icons/Mail';

import Planning from './Planning.js'
import MessageList from './Messages.js'
import AccountSettings from "./Account.js"
import NurseManagement from "./NurseManagement.js"
import PatientManagement from "./PatientManagement.js"
import TreatementManagement from "./TreatementManagement.js"

import Cookies from 'universal-cookie'

import {CalendarToday, AccountCircle, PowerSettingsNew, SupervisorAccount} from "@material-ui/icons"


const drawerWidth = 240;

const useStyles = makeStyles(theme => ({
    root: {
        display: 'flex',
    },
    appBar: {
        transition: theme.transitions.create(['margin', 'width'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
    },
    appBarShift: {
        width: `calc(100% - ${drawerWidth}px)`,
        marginLeft: drawerWidth,
        transition: theme.transitions.create(['margin', 'width'], {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen,
        }),
    },
    menuButton: {
        marginRight: theme.spacing(2),
    },
    hide: {
        display: 'none',
    },
    drawer: {
        width: drawerWidth,
        flexShrink: 0,
    },
    drawerPaper: {
        width: drawerWidth,
    },
    drawerHeader: {
        display: 'flex',
        alignItems: 'center',
        padding: theme.spacing(0, 1),
        ...theme.mixins.toolbar,
        justifyContent: 'flex-end',
    },
    content: {
        flexGrow: 1,
        padding: theme.spacing(3),
        transition: theme.transitions.create('margin', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen,
        }),
        marginLeft: -drawerWidth,
    },
    contentShift: {
        transition: theme.transitions.create('margin', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen,
        }),
        marginLeft: 0,
    },
}));


export default function MainScreen(props)
{

    // ENUM
    const Pages = {
            PLANNING   : 0,
            MESSAGES   : 1,
            ACCOUNT    : 2,
            LOGOUT     : 3,
            NURSEMANAGEMENT : 4,
            PATIENTMANAGEMENT : 5,
            TREATEMENTMANAGEMENT : 6,

    }

    const classes = useStyles();

    const [open, setOpen] = React.useState(false);

    const [page, setPage] = React.useState(0);

    const [name, setName] = React.useState("Planning")


    const handleDrawerOpen = () => {
        setOpen(true);
    };

    const handleDrawerClose = () => {
        setOpen(false);
    };

    const MainScreenArea = (props) => {

        switch(props.page)
        {
                case Pages.PLANNING:
                        return (<Planning token={props.token} />);
                case Pages.MESSAGES:
                        return (<MessageList />);
                case Pages.ACCOUNT:
                        return (<AccountSettings/>);
                case Pages.PATIENTMANAGEMENT:
                        return (<PatientManagement/>);
                case Pages.TREATEMENTMANAGEMENT:
                        return (<TreatementManagement/>);
                case Pages.NURSEMANAGEMENT:
                        return(<NurseManagement />)
                default:
                        return null;
        }
    }

    const launchAlgo = async () => {

            const response = await fetch('/algo', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json',
                                'Authorization':  "Bearer " + new Cookies().get('token').access_token,
                           },
                            })

            try
            {
              let status = response.status;
              if (status === 200)
              {
                console.log("Algo running")
              }
              else
              {
                 console.error(`Request rejected with status ${status}`)
              }
            }
            catch (err)
            {
                console.error(`Error launching algo ` + err)
            }
    }

const handleUserMenuClick = (index) => {

        switch(index)
        {
            case Pages.PLANNING:
                    setPage(Pages.PLANNING)
                    setName("Planning")
                    break;
            case Pages.MESSAGES:
                    setPage(Pages.MESSAGES)
                    setName("Messages")
                    break;
            case Pages.ACCOUNT:
                    setPage(Pages.ACCOUNT)
                    setName("Account settings")
                    break;
            case Pages.LOGOUT:
                    setPage(Pages.LOGOUT)
                    setName("Log out")
                    props.handleLogout()
                    break;
            case Pages.NURSEMANAGEMENT:
                    setPage(Pages.NURSEMANAGEMENT)
                    setName("Nurse management")
                    break;
            case Pages.TREATEMENTMANAGEMENT:
                    setPage(Pages.TREATEMENTMANAGEMENT)
                    setName("Treatement management")
                    break;
            case Pages.PATIENTMANAGEMENT:
                    setPage(Pages.PATIENTMANAGEMENT)
                    setName("Patient management")
                    break;
            default:
                    return null;
        }
}

    const userIconSwitch = (index) => {

            switch(index)
            {
                case 0:
                        return(<CalendarToday />)
                case 1:
                        return(<MailIcon />)
                case 2:
                        return(<AccountCircle />)
                case 3:
                        return(<PowerSettingsNew />)
                default:
                        return null;
                }
    }

    return (

        <div className={classes.root}>
            <CssBaseline />

            { /* bar Bleue en haut */ }
            <AppBar
                position="fixed"
                className={clsx(classes.appBar, {
                    [classes.appBarShift]: open,
                })}
            >

                <Toolbar>
                    <IconButton
                        color="inherit"
                        aria-label="open drawer"
                        onClick={handleDrawerOpen}
                        edge="start"
                        className={clsx(classes.menuButton, open && classes.hide)}
                    >
                        <MenuIcon />
                    </IconButton>
                    <Typography variant="h6" noWrap>
                        {name}
                    </Typography>
                </Toolbar>
            </AppBar>

            { /* Drawer sur le cote */ }
            <Drawer
                className={classes.drawer}
                variant="persistent"
                anchor="left"
                open={open}
                classes={{
                    paper: classes.drawerPaper,
                }}
            >
                <div className={classes.drawerHeader}>
                    <IconButton onClick={handleDrawerClose}>
                         <ChevronLeftIcon />
                    </IconButton>
                </div>
                <Divider />

                <List>

                    {['Planning', 'Messages', 'Account', "Log out"].map((text, index) => (
                        <ListItem button key={text} onClick = {() =>handleUserMenuClick(index)}>
                            <ListItemIcon>{userIconSwitch(index)}</ListItemIcon>
                            <ListItemText primary={text} />
                        </ListItem>
                    ))}

                </List>

                <Divider />
                    {/* If admin, manager bar*/}
                    {props.status === 0 ?
                        (
                            <>
                            <List>
                                    <ListItem key="admin">
                                            <ListItemText primary={"Admin section"}/>
                                    </ListItem>
                                    {['Nurse management', 'Patient management', 'Treatement Management'].map((text, index) =>
                                      <ListItem button key={text} onClick = {() =>handleUserMenuClick(index + 4)}>
                                          <ListItemIcon><SupervisorAccount/></ListItemIcon>
                                          <ListItemText primary={text} />
                                      </ListItem>
                                    )}
                                    <ListItem>
                                        <Button variant="contained" color="primary" onClick={() => launchAlgo()}>
                                          Calculate Plannings
                                        </Button>
                                    </ListItem>
                            </List>
                            </>
                        ) : null
                    }

                <Divider />

            </Drawer>

            <main
                className={clsx(classes.content, {
                    [classes.contentShift]: open,
                               }
                            )
                           }
            >
                {/* Used as padding */}
                <div className={classes.drawerHeader} />

                <div className="mainScreenArea">
                        <MainScreenArea page={page} token={props.access_token}/>
                </div>
            </main>
        </div>
    );
}
