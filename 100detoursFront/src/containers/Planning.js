import React from 'react'
import Paper from "@material-ui/core/Paper";
import { ViewState } from "@devexpress/dx-react-scheduler";
import {
  Scheduler,
  WeekView,
  Appointments,
  AppointmentTooltip,
  DateNavigator,
  Toolbar as ToolbarScheduler,
  TodayButton

} from "@devexpress/dx-react-scheduler-material-ui";
import { MuiThemeProvider, createMuiTheme, withStyles } from "@material-ui/core/styles";
import LocalHospital from '@material-ui/icons/LocalHospital'
import Grid from '@material-ui/core/Grid';
import Room from '@material-ui/icons/Room';

import { blue } from "@material-ui/core/colors";
import { appointments } from "./data";
import Cookies from 'universal-cookie';

const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

const style = ({ palette }) => ({
  icon: {
    color: palette.action.active,
  },
  textCenter: {
    textAlign: 'center',
  },
  header: {
    height: '260px',
    backgroundSize: 'cover',
  },
  commandButton: {
    backgroundColor: 'rgba(255,255,255,0.65)',
  },
});

function get_today_date()
{
    var today = new Date();
    var dd = String(today.getDate()).padStart(2, '0');
    var mm = String(today.getMonth() + 1).padStart(2, '0'); //January is 0!
    var yyyy = today.getFullYear();

    today = yyyy + '-' + mm + '-' + dd;

    return today
}

export default class Planning extends React.PureComponent
{
    constructor(props)
    {
        super(props);


        this.state = {
          data: appointments,
          ids: 0,
          currentDate: get_today_date(),
          fetched: false
        };


        this.appointments_callback = this.appointments_callback.bind(this)
        this.fetch_planning = this.fetch_planning.bind(this)
        this.currentDateChange = this.currentDateChange.bind(this)
        this.cookie_token = new Cookies().get('token')

    }

    async fetch_planning(nurse, date, success_callback, set_error)
    {
        var response;
        
        // Admin case
        if (this.cookie_token.status == 0)
        {
          response = await fetch('/treatments/', {
                                  method: 'GET',
                                    headers: { 'Authorization':  "Bearer " + this.cookie_token.access_token,
                                             'JoinRID': '["patient", "nurse", "ttype"]',
                                            },
                                })
        }
        // Nurse case
        else
        {
          response = await fetch('/treatments/?nurse=' + String(this.cookie_token.rid), {
                                    method: 'GET',
                                    headers: { 'Authorization':  "Bearer " + this.cookie_token.access_token,
                                             'JoinRID': '["patient", "nurse", "ttype"]',
                                            },
            })
        }

        let status = response.status;
        if (status === 200)
        {
            let res = await response.json();
            this.appointments_callback(res);
        }
        else
        {
           set_error("Could not fetch planning for nurse ")
           console.error(`Request rejected with status ${status}}`)
        }

    }

    currentDateChange(currentDate)
    {
          this.setState({currentDate});
          // TO DO : Fetch function to new week
    };

    appointments_callback(treatments_array)
    {

            var data = []

            var ids = this.state.ids;
            for (var i = 0; i < treatments_array.length; ++i)
            {
                const treatment = treatments_array[i];

                // Shiould never be the case
                if (treatment.nurse__join == {}
                      || (this.cookie_token.status == 1 && treatment.nurse__join.rid !== this.cookie_token.rid)
                      || treatment["time"] == null
                     )
                 {
                    continue;
                 }

                let date = treatment["date"].split("-")

                let year = parseInt(date[0])
                let month = parseInt(date[1])
                let day = parseInt(date[2])

                let patient_name = treatment.patient__join.name;
                let nurse_name = treatment.nurse__join.name;

                let time = treatment["time"].split("T")[1].split(":")

                let hour_start = parseInt(time[0])
                let minutes_start = parseInt(time[1])

                let duration = treatment.ttype__join.duration

                let carry_hour = parseInt((minutes_start + duration) / 60)
                let remain_minutes = (minutes_start + duration) % 60

                let hour_end = hour_start + carry_hour
                let minutes_end = remain_minutes

                var new_data = {
                    "title": patient_name,
                    "nurse": nurse_name,
                    "startDate": new Date(year, month, day,  hour_start, minutes_start),
                    "endDate": new Date(year, month, day,  hour_end, minutes_end),
                    "location": treatment["patient__join"].address,
                    "treatment": treatment.ttype__join.name
                }

                data.push(new_data)

            };

            this.setState({data: data, ids: ids, fetched: true})
    }

    componentDidMount()
    {
        this.fetch_planning({id: 1, token: this.props.token}, "", this.appointments_callback, console.log, this)
    }

    render()
    {

        const {data, currentDate} = this.state;

        const Content = withStyles(style, {name: 'Content'})(({
          children, appointmentData, classes, ...restProps
        }) => (
          <AppointmentTooltip.Content {...restProps} appointmentData={appointmentData}>

            <Grid container alignItems="center">
              <Grid item xs={2} className={classes.textCenter}>
                <Room className={classes.icon} />
              </Grid>
              <Grid item xs={4}>
                <span>{appointmentData.location}</span>
              </Grid>
            </Grid>


            <Grid container alignItems="center">
                <Grid item xs={2} className={classes.textCenter}>
                    <LocalHospital className={classes.icon} />
                </Grid>
                <Grid item xs={4}>
                    <span>{appointmentData.nurse}</span>
                </Grid>
            </Grid>

            <Grid container alignItems="center">
                <Grid item xs={2} className={classes.textCenter}>
                  <LocalHospital className={classes.icon} />
                </Grid>
                <Grid item xs={4}>
                  <span>{appointmentData.treatment}</span>
                </Grid> 
            </Grid>
          </AppointmentTooltip.Content>
        ));

        const CommandButton = withStyles(style, { name: 'CommandButton' })(({
          classes, ...restProps
        }) => (
          <AppointmentTooltip.CommandButton {...restProps} className={classes.commandButton} />
        ));


        return (!this.state.fetched ? <div></div> :
          <MuiThemeProvider theme={theme}>
            <Paper>
              <Scheduler data={data}>
                <ViewState
                    currentDate={currentDate}
                    onCurrentDateChange={this.currentDateChange}
                />
                <WeekView startDayHour={7} endDayHour={24} />
                <ToolbarScheduler/>
                <DateNavigator />
                <TodayButton />
                <Appointments />
                <AppointmentTooltip 
                  contentComponent={Content}
                  commandButtonComponent={CommandButton}
                  showCloseButton/>
              </Scheduler>
            </Paper>
          </MuiThemeProvider>
        );
    }
}
