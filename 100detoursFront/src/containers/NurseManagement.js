import React from 'react'
import Paper from "@material-ui/core/Paper";
import { MuiThemeProvider, createMuiTheme } from "@material-ui/core/styles";
import { blue } from "@material-ui/core/colors";
import MaterialTable from "material-table"
import Cookies from 'universal-cookie'

const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

export default class NurseManagement extends React.Component
{
    constructor(props)
    {
        super(props);
        const cookies = new Cookies();
        const cookie_token = cookies.get('token')

        this.state = {
            columns: [
                    { title: 'Username', field: 'user__join.username' },
                    { title: 'Status', field: 'user__join.usertype', lookup: { 0:'admin', 1:'nurse' } },
                ],
            data: [],
            access_token:cookie_token.access_token
        }
        this.get_nurses_callback = this.get_nurses_callback.bind(this)
        this.set_error = this.set_error.bind(this)

        this.cookie_token = new Cookies().get('token')

    }


  async fetch_nurses_list(get_nurses_callback, set_error)
  {

      const response = await fetch('/nurses/', {
                              method: 'GET',
                              headers: { 'Authorization':  "Bearer " + this.state.access_token,
                               'JoinRID': '["user"]',
                            },
      })

      let res = await response.json();
      let status = response.status;
      if (status === 200)
      {
          this.get_nurses_callback(res);
      }
      else
      {
         set_error("Could not fetch planning for nurse ")
         console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
      }
  }

    get_nurses_callback(res)
    {
      this.setState({data:res}, () =>{console.log(this.state.data)})
    }

    set_error(e)
    {
        console.error(e)
    }
    componentDidMount()
    {
        this.fetch_nurses_list(this.get_nurses_callback, this.set_error)
    }

    async on_row_add(newData)
    {

        let newNurse = {
            name: newData.user__join.username,
        }

        const response = await fetch('/nurses/', {
                              method: 'POST',
                              headers: { 'Content-Type': 'application/json',
                                        'Authorization':  "Bearer " + this.cookie_token.access_token,
                                         'JoinRID': '["user"]',

                                    },
                              body: JSON.stringify(newNurse),
                                });

        let status = response.status;

        if (status !== 200)
        {
            alert("Error: Username already exist")
            console.error(`Request rejected with status ${status}}`)
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

    on_row_update(new_data, old_data)
    {
        if (old_data)
        {
            this.setState(prevState => {
                const data = [...prevState.data];
                data[data.indexOf(old_data)] = new_data;

                return { ...prevState, data };
            });
        }
    }

    async on_row_delete(old_data)
    {
        const response = await fetch('/nurses/' + old_data.rid, {
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

                                    title = "Nurse list"
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
