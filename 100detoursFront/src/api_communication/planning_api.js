/*
 *  Utils functions for Planning 
 *
 */

export function fetch_week_planning(nurse, date, success_callback)
{

}

export async function fetch_planning(nurse, date, success_callback, set_error)
{
    const nurse_id = nurse.id;
    const token = nurse.token;


    const response = await fetch('/treatments/', {
                            method: 'GET',
                            headers: { 'Authorization':  "Bearer " + token},
    })

    let res = await response.json();
    let status = response.status;
    if (status === 200)
    {
        success_callback(res);
    }
    else
    {
       set_error("Could not fetch planning for nurse ")
       console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
    }

}


