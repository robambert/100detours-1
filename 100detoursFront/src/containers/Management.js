import React from 'react'
import Paper from "@material-ui/core/Paper";
import { MuiThemeProvider, createMuiTheme } from "@material-ui/core/styles";
import { blue } from "@material-ui/core/colors";
import MaterialTable from "material-table"

import * as ManagementApi from "../api_communication/management_api.js"

const theme = createMuiTheme({ palette: { type: "light", primary: blue } });

export default class Management extends React.Component
{
    constructor(props)
    {
        super(props);
    
        this.state = {
            columns: [
                    { title: 'Username', field: 'username' },
                    { title: 'Status', field: 'rank', lookup: { 'admin':'admin', 'nurse':'nurse' } },
                ],
            data: []
        }

    }

    componentDidMount()
    {
        ManagementApi.get_nurses_list((new_state) => this.setState(new_state), console.error);
    }

    on_row_add(new_data)
    {
        this.setState(prevState => {
            console.log("yyy ==> ewq" )
            const data = [...prevState.data];
            let refactoredData = new_data;
            refactoredData.password = "default";

            data.push(refactoredData);

            console.log("yyy ==> ", data )
            // this.props.socket.emit("updatedUserList", data)

            // TODO: Add a fetch here
            return { ...prevState, data };
        });   
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

    on_row_delete(old_data)
    {
        this.setState(prevState => {
            const data = [...prevState.data];
            data.splice(data.indexOf(old_data), 1);

            // this.props.socket.emit("updatedUserList", data)

            // ManagementApi.delete_nurse()

            // TODO: Add a fetch here
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
