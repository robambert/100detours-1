import React from 'react'
import Paper from "@material-ui/core/Paper";
import { MuiThemeProvider, createMuiTheme } from "@material-ui/core/styles";
import { blue } from "@material-ui/core/colors";
import MaterialTable from "material-table"
import Cookies from 'universal-cookie'

import * as ManagementApi from "../api_communication/management_api.js"

const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

export default class PatientManagement extends React.Component
{
    constructor(props)
    {
        super(props);

        this.state = {
            columns: [
                    { title: 'Patient Name', field: 'name' },
                    { title: 'Patient Id', field: 'rid' },
                    { title: 'Address', field: 'address' },
                ],
            data: []
        }
        this.get_patients_callback = this.get_patients_callback.bind(this)
        this.set_error = this.set_error.bind(this)
        this.cookie_token = new Cookies().get('token')


    }

    set_error(e)
    {
        console.error(e)
    }

    get_patients_callback(res)
    {
      this.setState({data:res}, () =>{console.log(this.state.data)})
    }

      async fetch_patients_list(get_patients_callback, set_error)
      {

          const response = await fetch('/patients/', {
                                  method: 'GET',
                                  headers: { 'Authorization':  "Bearer " + this.cookie_token.access_token,
                                },
          })

          let res = await response.json();
          let status = response.status;
          if (status === 200)
          {
              this.get_patients_callback(res);
          }
          else
          {
             console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
          }
      }

    async on_row_add(newData)
    {

        let newPatient = {
            name: newData.name,
            address: newData.address,
        }

        const response = await fetch('/patients/', {
                              method: 'POST',
                              headers: { 'Content-Type': 'application/json',
                                        'Authorization':  "Bearer " + this.cookie_token.access_token,

                                    },
                              body: JSON.stringify(newPatient),
                                });

        let status = response.status;

        if (status !== 200)
        {
            alert("Error: Patient already exist")
            console.error(`Request rejected with status ${status}`)
            return
        }

        try
        {
            let res = await response.json();
            newData.user__join.username = res.user__join.username;

            this.setState(prevState => {

                const data = [...prevState.data];

                let refactoredData = newData;

                data.push(refactoredData);

                return { ...prevState, data };
                }
            );
        }
        catch (error)
        {
            console.error(error)
        }
    }
    componentDidMount()
    {
        this.fetch_patients_list(this.get_patients_callback, this.set_error)
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
        const response = await fetch('/patients/' + old_data.rid, {
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
