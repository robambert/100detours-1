import React from 'react'
import Paper from "@material-ui/core/Paper";
import { MuiThemeProvider, createMuiTheme } from "@material-ui/core/styles";
import { blue } from "@material-ui/core/colors";
import MaterialTable from "material-table"
import Cookies from 'universal-cookie'

import * as ManagementApi from "../api_communication/management_api.js"

const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

export default class TreatementManagement extends React.Component
{
    constructor(props)
    {
        super(props);

        this.state = {
            columns: [
                    { title: 'Patient Name', field: 'patient__join.name' },
                    { title: 'Patient id', field: 'patient' },
                    { title: 'Treatment Type', field: 'ttype' },
                    { title: 'Date', field: 'date' },
                ],
            data: []
        }
        this.get_treatments_callback = this.get_treatments_callback.bind(this)
        this.set_error = this.set_error.bind(this)

        this.cookie_token = new Cookies().get('token')
    }

    set_error(e)
    {
        console.error(e)
    }

    get_treatments_callback(res)
    {
        console.log("aaaaa , ", res)
      this.setState({data:res}, () =>{console.log(this.state.data)})
    }

  async fetch_treatments_list(get_treatments_callback, set_error)
  {

      const response = await fetch('/treatments/', {
                              method: 'GET',
                              headers: { 'Authorization':  "Bearer " + this.cookie_token.access_token,
                                           'JoinRID': '["patient", "nurse"]',

                            },
      })

      let res = await response.json();
      let status = response.status;
      if (status === 200)
      {
          get_treatments_callback(res);
      }
      else
      {
         set_error("Could not fetch planning for nurse ")
         console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
      }
  }

    componentDidMount()
    {
        this.fetch_treatments_list(this.get_treatments_callback, this.set_error)
    }

    async on_row_add(newData)
    {
        console.log("ooooo ", newData)

        let newTreatment = {
            patient: parseInt(newData.patient),
            ttype: parseInt(newData.ttype),
            date: newData.date,
        }


        const response = await fetch('/treatments/', {
                              method: 'POST',
                              headers: { 'Content-Type': 'application/json',
                                        'Authorization':  "Bearer " + this.cookie_token.access_token,
                                    },
                              body: JSON.stringify(newTreatment),
                                });

        let res = await response.json();
        let status = response.status;

        if (status !== 200)
        {
            alert("Error: Could not create treatment")
            console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
            return
        }

        this.setState(prevState => {

            const data = [...prevState.data];

            let refactoredData = newData;

            // refactoredData.password = "default";

            data.push(refactoredData);

            return { ...prevState, data };
            }
        );
    }

    on_row_update(new_data, old_data)
    {
        if (old_data)
        {
            this.setState(prevState => {
                const data = [...prevState.data];
                data[data.indexOf(old_data)] = new_data;

                // this.props.socket.emit("updatedUserList", data)

                // ManagementApi.edit_nurse()
                // TODO: Add a fetch here
                return { ...prevState, data };
            });
        }
    }

    async on_row_delete(old_data)
    {
        const response = await fetch('/treatments/' + old_data.rid, {
                              method: 'DELETE',
                              headers: { 'Content-Type': 'application/json',
                                        'Authorization':  "Bearer " + this.cookie_token.access_token,
                                    },
                                });

        let status = response.status;

        if (status !== 204)
        {
            alert("Error: Does not exist")
            console.error(`Request rejected with status ${status}}`)
            return
        }

        this.setState(prevState => {
            const data = [...prevState.data];
            data.splice(data.indexOf(old_data), 1);

            return { ...prevState, data };
        });
    }

    render() {
        return (
                <MuiThemeProvider theme={theme} >
                    <Paper>
                            <h1> Management section </h1>

                                <MaterialTable

                                    title = "User list"
                                    columns = {this.state.columns}
                                    data = {this.state.data}

                                    editable = {{

                                        onRowAdd: newData =>
                                            new Promise(resolve => {

                                                this.on_row_add(newData);
                                                resolve();
                                            }),

                                        onRowUpdate: (newData, oldData) =>
                                            new Promise(resolve => {
                                                resolve()
                                                this.on_row_update(newData, oldData)
                                            }),

                                        onRowDelete: oldData =>
                                            new Promise(resolve => {
                                                resolve()
                                                this.on_row_delete(oldData)
                                            }),
                                    }}
                                />
                </Paper>
            </MuiThemeProvider>
        )
    }
}
